// FunctionArea.cpp
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QProcess>
#include <QDir>
#include "functionarea.h"
#include <QListWidgetItem>
#include <QDirIterator>
#include <QThread>
#include <QLabel>
#include <QFileIconProvider>
#include <QHeaderView>
#include <QApplication>
#include <QInputDialog>
#include <QMessageBox>
#include <QMenu>
#include <QPushButton>
#include <QDialog>
#include <QLineEdit>

FunctionArea::FunctionArea(QWidget *parent)
        : QWidget(parent), currentFolderName("") {
    // 加载支持的文件后缀
    loadFileExtensions();
    // 创建标题标签
    QLabel *titleLabel = new QLabel("可用工具", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-weight: bold; font-size: 12pt; padding: 8px; color: #2c3e50; background-color: #ecf0f1; border-bottom: 1px solid #bdc3c7;");
    
    // 创建快捷方式列表
    shortcuts = new QListWidget(this);
    shortcuts->setStyleSheet(
        "QListWidget {"
        "    background-color: #ffffff;"
        "    border: none;"
        "    outline: none;"
        "}"
        "QListWidget::item {"
        "    padding: 8px;"
        "    border-bottom: 1px solid #eeeeee;"
        "}"
        "QListWidget::item:selected {"
        "    background-color: #e3f2fd;"
        "    color: #1976d2;"
        "}"
        "QListWidget::item:hover {"
        "    background-color: #f5f5f5;"
        "}");
    
    // 设置图标大小
    shortcuts->setIconSize(QSize(24, 24));
    
    // 设置列表项高度
    shortcuts->setStyleSheet(shortcuts->styleSheet() + "QListWidget::item { height: 36px; }");
    
    // 连接双击信号到槽函数
    connect(shortcuts, &QListWidget::itemDoubleClicked, this, &FunctionArea::onShortcutDoubleClicked);
    
    // 设置右键菜单
    shortcuts->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(shortcuts, &QListWidget::customContextMenuRequested, this, &FunctionArea::onContextMenuRequested);
    
    // 创建提示标签
    QLabel *hintLabel = new QLabel("双击项目启动工具", this);
    hintLabel->setAlignment(Qt::AlignCenter);
    hintLabel->setStyleSheet("color: #757575; padding: 5px; font-size: 9pt;");
    
    // 创建布局
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(titleLabel);
    layout->addWidget(shortcuts);
    layout->addWidget(hintLabel);
    setLayout(layout);
}

QListWidgetItem* FunctionArea::getSelectedItem() const {
    QList<QListWidgetItem*> selectedItems = shortcuts->selectedItems();
    if (selectedItems.isEmpty()) {
        return nullptr;
    }
    return selectedItems.first();
}

// 加载支持的文件后缀
void FunctionArea::loadFileExtensions() {
    // 默认支持的文件后缀
    supportedExtensions = QStringList() << "exe" << "cmd";
    
    // 从配置文件加载自定义后缀
    QFile file("tools/extensions.cfg");
    if (file.exists() && file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString content = in.readAll().trimmed();
        file.close();
        
        if (!content.isEmpty()) {
            supportedExtensions = content.split(";", Qt::SkipEmptyParts);
        }
    }
}

// 保存支持的文件后缀
void FunctionArea::saveFileExtensions(const QStringList &extensions) {
    supportedExtensions = extensions;
    
    // 保存到配置文件
    QFile file("tools/extensions.cfg");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << extensions.join(";");
        file.close();
    }
}

// 获取文件过滤器
QStringList FunctionArea::getFileFilters() const {
    QStringList filters;
    for (const QString &ext : supportedExtensions) {
        filters << "*." + ext;
    }
    return filters;
}

void FunctionArea::showShortcuts(const QString &folderName) {
    // 保存当前文件夹名称
    currentFolderName = folderName;
    
    // 清空当前快捷方式列表
    shortcuts->clear();
    
    // 设置鼠标等待光标
    QApplication::setOverrideCursor(Qt::WaitCursor);
    
    // 读取指定文件夹及其子文件夹下的支持的文件类型
    QDir toolsDir("tools/" + folderName);
    
    // 创建文件图标提供器
    QFileIconProvider iconProvider;
    
    // 首先扫描所有文件夹中的main.cfg文件
    QMap<QString, QStringList> mainConfigFiles; // 文件夹路径 -> 主要可执行文件路径列表
    QSet<QString> foldersWithMainCfg; // 存储包含main.cfg的文件夹及其子文件夹路径
    QDirIterator configIterator(toolsDir.path(), QStringList() << "main.cfg", QDir::Files, QDirIterator::Subdirectories);
    
    while (configIterator.hasNext()) {
        configIterator.next();
        QFileInfo configFileInfo = configIterator.fileInfo();
        QString dirPath = configFileInfo.absolutePath();
        
        // 将该文件夹添加到包含main.cfg的文件夹集合中
        foldersWithMainCfg.insert(dirPath);
        
        QFile configFile(configFileInfo.absoluteFilePath());
        if (configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&configFile);
            QStringList exePaths;
            
            // 逐行读取main.cfg文件中的路径
            while (!in.atEnd()) {
                QString line = in.readLine().trimmed();
                if (!line.isEmpty()) {
                    // 如果是相对路径，转换为绝对路径
                    if (QFileInfo(line).isRelative()) {
                        line = dirPath + "/" + line;
                    }
                    exePaths.append(line);
                }
            }
            configFile.close();
            
            if (!exePaths.isEmpty()) {
                mainConfigFiles[dirPath] = exePaths;
            }
        }
    }
    
    // 创建QDirIterator来递归遍历文件夹
    QDirIterator dirIterator(toolsDir.path(), getFileFilters(), QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
    
    // 计数器，用于交替背景色
    int count = 0;
    
    while (dirIterator.hasNext()) {
        dirIterator.next();
        QFileInfo fileInfo = dirIterator.fileInfo();
        QString dirPath = fileInfo.absolutePath();
        
        // 检查文件所在路径是否是某个包含main.cfg的文件夹的子文件夹
        bool skipDueToParentFolder = false;
        foreach (const QString &folderWithCfg, foldersWithMainCfg) {
            // 如果当前文件所在目录是包含main.cfg的文件夹的子文件夹（但不是同一个文件夹）
            if (dirPath != folderWithCfg && dirPath.startsWith(folderWithCfg)) {
                skipDueToParentFolder = true;
                break;
            }
        }
        
        // 如果是子文件夹中的文件，跳过
        if (skipDueToParentFolder) {
            continue;
        }
        
        // 检查文件是否是支持的类型
        if (fileInfo.isFile() && supportedExtensions.contains(fileInfo.suffix().toLower())) {
            // 检查该文件所在目录是否有main.cfg配置
            if (mainConfigFiles.contains(dirPath)) {
                // 如果有main.cfg，只显示指定的文件列表中的文件
                if (!mainConfigFiles[dirPath].contains(fileInfo.absoluteFilePath())) {
                    continue; // 跳过非指定文件
                }
            }
            
            // 创建列表项
            QListWidgetItem *item = new QListWidgetItem(shortcuts);
            
            // 检查是否有重命名配置
            QString displayName = fileInfo.completeBaseName();
            QString renameConfigPath = "tools/" + folderName + "/shortcuts/" + fileInfo.fileName() + ".rename";
            QFile renameFile(renameConfigPath);
            if (renameFile.exists() && renameFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&renameFile);
                QString customName = in.readAll().trimmed();
                if (!customName.isEmpty()) {
                    displayName = customName;
                }
                renameFile.close();
            }
            
            // 设置文本和工具提示
            item->setText(displayName);
            item->setToolTip(fileInfo.absoluteFilePath());
            
            // 设置图标
            item->setIcon(iconProvider.icon(fileInfo));
            
            // 存储文件路径
            item->setData(Qt::UserRole, fileInfo.absoluteFilePath());
            
            // 设置交替背景色
            if (count % 2 == 0) {
                item->setBackground(QColor(252, 252, 252));
            } else {
                item->setBackground(QColor(248, 248, 248));
            }
            
            count++;
        }
    }
    
    // 恢复鼠标光标
    QApplication::restoreOverrideCursor();
}

void FunctionArea::onContextMenuRequested(const QPoint &pos) {
    QListWidgetItem *item = shortcuts->itemAt(pos);
    
    // 创建右键菜单
    QMenu menu(this);
    
    if (item) {
        // 选中当前项
        shortcuts->setCurrentItem(item);
        
        // 添加重命名选项
        QAction *renameAction = menu.addAction("重命名工具");
        connect(renameAction, &QAction::triggered, this, &FunctionArea::onRenameShortcut);
        
        menu.addSeparator();
    }
    
    // 添加管理文件后缀选项
    QAction *manageExtensionsAction = menu.addAction("管理支持的文件类型");
    connect(manageExtensionsAction, &QAction::triggered, this, &FunctionArea::onManageFileExtensions);
    
    // 显示菜单
    menu.exec(shortcuts->mapToGlobal(pos));
}

void FunctionArea::onRenameShortcut() {
    QListWidgetItem *item = getSelectedItem();
    if (!item) {
        return;
    }
    
    // 获取当前名称和文件路径
    QString currentName = item->text();
    QString filePath = item->data(Qt::UserRole).toString();
    QFileInfo fileInfo(filePath);
    
    // 创建自定义对话框以便修改按钮文本
    QDialog dialog(this);
    dialog.setWindowTitle("重命名工具");
    
    // 创建输入框和标签
    QLineEdit *lineEdit = new QLineEdit(&dialog);
    lineEdit->setText(currentName);
    QLabel *label = new QLabel("请输入新的工具名称:", &dialog);
    
    // 创建按钮
    QPushButton *saveButton = new QPushButton("保存", &dialog);
    QPushButton *cancelButton = new QPushButton("取消", &dialog);
    
    // 设置按钮角色
    saveButton->setDefault(true);
    dialog.connect(saveButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    dialog.connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);
    
    // 创建布局
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(label);
    mainLayout->addWidget(lineEdit);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    dialog.setLayout(mainLayout);
    
    // 显示对话框
    bool ok = (dialog.exec() == QDialog::Accepted);
    QString newName = lineEdit->text();
    
    if (ok && !newName.isEmpty() && newName != currentName) {
        // 创建快捷方式文件（.lnk或.desktop）来实现重命名
        // 在Windows上，我们可以创建一个.lnk文件
        QString shortcutDir = "tools/" + currentFolderName + "/shortcuts";
        QDir dir;
        if (!dir.exists(shortcutDir)) {
            dir.mkpath(shortcutDir);
        }
        
        // 创建配置文件来存储重命名信息
        QString configPath = shortcutDir + "/" + fileInfo.fileName() + ".rename";
        QFile configFile(configPath);
        if (configFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&configFile);
            out << newName;
            configFile.close();
            
            // 更新列表项
            item->setText(newName);
        } else {
            QMessageBox::warning(this, "重命名工具", "无法保存重命名信息。");
        }
    }
}

void FunctionArea::onManageFileExtensions() {
    // 创建自定义对话框以便修改按钮文本
    QDialog dialog(this);
    dialog.setWindowTitle("管理支持的文件类型");
    
    // 创建输入框和标签
    QLineEdit *lineEdit = new QLineEdit(&dialog);
    lineEdit->setText(supportedExtensions.join(";"));
    QLabel *label = new QLabel("请输入支持的文件后缀，用分号(;)分隔：", &dialog);
    
    // 创建按钮
    QPushButton *saveButton = new QPushButton("保存", &dialog);
    QPushButton *cancelButton = new QPushButton("取消", &dialog);
    
    // 设置按钮角色
    saveButton->setDefault(true);
    dialog.connect(saveButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    dialog.connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);
    
    // 创建布局
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(label);
    mainLayout->addWidget(lineEdit);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    dialog.setLayout(mainLayout);
    
    // 显示对话框
    bool ok = (dialog.exec() == QDialog::Accepted);
    QString extensionsStr = lineEdit->text();
    
    if (ok && !extensionsStr.isEmpty()) {
        // 解析输入的后缀
        QStringList newExtensions = extensionsStr.split(";", Qt::SkipEmptyParts);
        
        // 移除空白字符并转为小写
        for (int i = 0; i < newExtensions.size(); ++i) {
            newExtensions[i] = newExtensions[i].trimmed().toLower();
        }
        
        // 保存新的后缀列表
        saveFileExtensions(newExtensions);
        
        // 刷新当前显示的快捷方式
        if (!currentFolderName.isEmpty()) {
            showShortcuts(currentFolderName);
        }
    }
}

void FunctionArea::onShortcutDoubleClicked(QListWidgetItem *item) {
    if (!item) {
        return;
    }

    QString exeFilePath = item->data(Qt::UserRole).toString();

    // 检查文件是否存在
    if (!QFile::exists(exeFilePath)) {
        qDebug() << "文件不存在: " << exeFilePath;
        return;
    }

    // 创建新进程对象，使用智能指针自动管理资源
    QSharedPointer<QProcess> process(new QProcess);

    // 设置进程参数
    process->setProcessChannelMode(QProcess::MergedChannels);
    process->setProgram(exeFilePath);

    // 为避免阻塞GUI，连接结束信号来安全地处理进程和线程的清理
    connect(process.get(), QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [process](int exitCode, QProcess::ExitStatus exitStatus) {
                qDebug() << "Process exited with code " << exitCode << " and status " << exitStatus;
            });

    // 在不阻塞GUI的情况下启动进程
    process->start();
    if (!process->waitForStarted()) {
        qDebug() << "无法启动进程: " << exeFilePath;
    }
}
