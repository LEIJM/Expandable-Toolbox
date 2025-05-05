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
<<<<<<< HEAD
#include <QTextEdit>
#include <QIcon>

FunctionArea::FunctionArea(QWidget *parent)
        : QWidget(parent), currentFolderName(""), maxProcesses(10) {
    // 初始化缓存清理定时器
    cacheCleanupTimer = new QTimer(this);
    cacheCleanupTimer->setInterval(30 * 60 * 1000); // 30分钟清理一次缓存
    connect(cacheCleanupTimer, &QTimer::timeout, this, &FunctionArea::cleanupCache);
    cacheCleanupTimer->start();
    
    // 初始化搜索延迟定时器
    searchTimer = new QTimer(this);
    searchTimer->setSingleShot(true);
    searchTimer->setInterval(300); // 300毫秒延迟
    connect(searchTimer, &QTimer::timeout, this, &FunctionArea::performSearch);
=======

FunctionArea::FunctionArea(QWidget *parent)
        : QWidget(parent), currentFolderName("") {
>>>>>>> 24e5c4dcb27d9756890814ce87fc620fc05fe1cf
    // 加载支持的文件后缀
    loadFileExtensions();
    // 创建标题标签
    QLabel *titleLabel = new QLabel("可用工具", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-weight: bold; font-size: 12pt; padding: 8px; color: #2c3e50; background-color: #ecf0f1; border-bottom: 1px solid #bdc3c7;");
    
<<<<<<< HEAD
    // 创建搜索框
    QHBoxLayout *searchLayout = new QHBoxLayout();
    searchLayout->setContentsMargins(8, 8, 8, 8);
    
    searchBox = new QLineEdit(this);
    searchBox->setPlaceholderText("搜索工具名称或描述...");
    searchBox->setClearButtonEnabled(true);
    searchBox->setStyleSheet(
        "QLineEdit {"
        "    border: 1px solid #bdc3c7;"
        "    border-radius: 4px;"
        "    padding: 6px;"
        "    background-color: #ffffff;"
        "}"
        "QLineEdit:focus {"
        "    border: 1px solid #3498db;"
        "}");
    
    clearButton = new QPushButton("清除", this);
    clearButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #ecf0f1;"
        "    border: 1px solid #bdc3c7;"
        "    border-radius: 4px;"
        "    padding: 6px;"
        "    color: #7f8c8d;"
        "}"
        "QPushButton:hover {"
        "    background-color: #e0e0e0;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #d0d0d0;"
        "}");
    clearButton->setVisible(false); // 初始隐藏
    
    searchLayout->addWidget(searchBox);
    searchLayout->addWidget(clearButton);
    
    // 连接搜索框信号
    connect(searchBox, &QLineEdit::textChanged, this, [this]() {
        searchTimer->start(); // 启动延迟搜索
    });
    connect(clearButton, &QPushButton::clicked, this, &FunctionArea::clearSearch);
    
=======
>>>>>>> 24e5c4dcb27d9756890814ce87fc620fc05fe1cf
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
    
<<<<<<< HEAD
    // 启用拖放功能
    shortcuts->setDragEnabled(true);
    shortcuts->setAcceptDrops(true);
    shortcuts->setDropIndicatorShown(true);
    shortcuts->setDragDropMode(QAbstractItemView::InternalMove);
    
    // 连接拖放完成信号
    connect(shortcuts->model(), &QAbstractItemModel::rowsMoved, this, &FunctionArea::onShortcutsReordered);
    
=======
>>>>>>> 24e5c4dcb27d9756890814ce87fc620fc05fe1cf
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
<<<<<<< HEAD
    layout->addLayout(searchLayout);
=======
>>>>>>> 24e5c4dcb27d9756890814ce87fc620fc05fe1cf
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

<<<<<<< HEAD
// 析构函数，清理资源
FunctionArea::~FunctionArea() {
    // 停止定时器
    cacheCleanupTimer->stop();
    
    // 清理所有活动进程
    for (auto &process : activeProcesses) {
        if (process && process->state() == QProcess::Running) {
            process->terminate();
            if (!process->waitForFinished(1000)) { // 等待1秒
                process->kill(); // 如果无法正常终止，强制结束
            }
        }
    }
    activeProcesses.clear();
    
    // 清理 allItems 中的项目
    qDeleteAll(allItems);
    allItems.clear();
}

// 检查缓存是否有效
bool FunctionArea::isCacheValid(const QString &folderName) const {
    if (!folderCache.contains(folderName)) {
        return false;
    }
    
    QFileInfo dirInfo("tools/" + folderName);
    QDateTime lastModified = dirInfo.lastModified();
    
    // 如果文件夹修改时间比缓存新，缓存无效
    return lastModified <= folderCache[folderName].lastModified;
}

// 更新缓存
void FunctionArea::updateCache(const QString &folderName, const QList<QPair<QString, QString>> &files) {
    FileCache cache;
    cache.lastModified = QFileInfo("tools/" + folderName).lastModified();
    cache.files = files;
    folderCache[folderName] = cache;
}

// 清理过期缓存
void FunctionArea::cleanupCache() {
    // 清理超过1小时未使用的缓存
    QDateTime now = QDateTime::currentDateTime();
    QList<QString> keysToRemove;
    
    for (auto it = folderCache.begin(); it != folderCache.end(); ++it) {
        if (it.value().lastModified.secsTo(now) > 3600) { // 1小时 = 3600秒
            keysToRemove.append(it.key());
        }
    }
    
    for (const QString &key : keysToRemove) {
        folderCache.remove(key);
    }
}

// 管理进程
void FunctionArea::manageProcesses() {
    // 清理已结束的进程
    cleanupProcesses();
    
    // 如果活动进程数超过最大值，终止最早启动的进程
    while (activeProcesses.size() > maxProcesses) {
        if (!activeProcesses.isEmpty()) {
            auto oldestProcess = activeProcesses.takeFirst();
            if (oldestProcess && oldestProcess->state() == QProcess::Running) {
                oldestProcess->terminate();
            }
        }
    }
}

// 清理已结束的进程
void FunctionArea::cleanupProcesses() {
    QList<QSharedPointer<QProcess>> runningProcesses;
    
    for (auto &process : activeProcesses) {
        if (process && process->state() == QProcess::Running) {
            runningProcesses.append(process);
        }
    }
    
    activeProcesses = runningProcesses;
}

// 启动进程
bool FunctionArea::startProcess(const QString &exeFilePath, const QStringList &args) {
    // 管理进程数量
    manageProcesses();
    
    // 创建新进程对象
    QSharedPointer<QProcess> process(new QProcess);
    
    // 设置进程参数
    process->setProcessChannelMode(QProcess::MergedChannels);
    process->setProgram(exeFilePath);
    
    // 设置工作目录为可执行文件所在目录
    QFileInfo fileInfo(exeFilePath);
    process->setWorkingDirectory(fileInfo.absolutePath());
    
    // 设置参数
    if (!args.isEmpty()) {
        process->setArguments(args);
    }
    
    // 为避免阻塞GUI，连接结束信号来安全地处理进程和线程的清理
    connect(process.get(), QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this, process](int exitCode, QProcess::ExitStatus exitStatus) {
                qDebug() << "Process exited with code " << exitCode << " and status " << exitStatus;
                cleanupProcesses(); // 清理已结束的进程
            });
    
    // 在不阻塞GUI的情况下启动进程
    process->start();
    if (!process->waitForStarted(1000)) { // 等待1秒
        qDebug() << "无法启动进程: " << exeFilePath;
        return false;
    }
    
    // 添加到活动进程列表
    activeProcesses.append(process);
    return true;
}

// 创建单个快捷方式项
void FunctionArea::createShortcutItem(const QString &filePath, const QString &displayName, int count) {
    // 创建列表项
    QListWidgetItem *item = new QListWidgetItem(shortcuts);
    
    // 设置文本
    item->setText(displayName);
    
    // 检查是否有描述文件
    QFileInfo fileInfo(filePath);
    QString descriptionPath = "tools/" + currentFolderName + "/descriptions/" + fileInfo.fileName() + ".desc";
    QString description;
    QFile descFile(descriptionPath);
    if (descFile.exists() && descFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&descFile);
        description = in.readAll().trimmed();
        descFile.close();
    }
    
    // 设置工具提示 - 如果有描述则显示描述，否则显示文件路径
    if (!description.isEmpty()) {
        item->setToolTip(description);
        // 存储描述信息
        item->setData(Qt::UserRole + 1, description);
    } else {
        item->setToolTip(filePath);
    }
    
    // 设置图标
    QFileIconProvider iconProvider;
    item->setIcon(iconProvider.icon(QFileInfo(filePath)));
    
    // 存储文件路径
    item->setData(Qt::UserRole, filePath);
    
    // 设置交替背景色
    if (count % 2 == 0) {
        item->setBackground(QColor(252, 252, 252));
    } else {
        item->setBackground(QColor(248, 248, 248));
    }
}

// 应用自定义排序
QList<QPair<QString, QString>> FunctionArea::applyCustomOrder(const QString &folderName, QList<QPair<QString, QString>> files) {
    // 检查是否有保存的顺序文件
    QString orderFilePath = "tools/" + folderName + "/order/shortcuts.order";
    if (!QFile::exists(orderFilePath)) {
        return files; // 没有自定义顺序，直接返回原列表
    }
    
    QFile orderFile(orderFilePath);
    if (!orderFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return files; // 无法打开文件，直接返回原列表
    }
    
    QTextStream in(&orderFile);
    QStringList orderedPaths;
    
    // 读取保存的顺序
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty()) {
            orderedPaths.append(line);
        }
    }
    orderFile.close();
    
    // 如果有保存的顺序，按照顺序重新排列文件列表
    if (orderedPaths.isEmpty()) {
        return files; // 没有有效的顺序信息，直接返回原列表
    }
    
    QList<QPair<QString, QString>> newOrderedFiles;
    
    // 首先添加有序的文件
    foreach (const QString &path, orderedPaths) {
        for (int i = 0; i < files.size(); i++) {
            if (files[i].first == path) {
                newOrderedFiles.append(files[i]);
                files.removeAt(i);
                break;
            }
        }
    }
    
    // 然后添加剩余的文件（新添加的或未排序的）
    newOrderedFiles.append(files);
    return newOrderedFiles;
}

// 加载缓存的快捷方式
bool FunctionArea::loadCachedShortcuts(const QString &folderName) {
    if (!isCacheValid(folderName)) {
        return false; // 缓存无效
    }
    
    // 使用缓存数据
    const FileCache &cache = folderCache[folderName];
    QList<QPair<QString, QString>> orderedFiles = cache.files;
    
    // 应用自定义排序
    orderedFiles = applyCustomOrder(folderName, orderedFiles);
    
    // 显示快捷方式
    int count = 0;
    for (const auto &pair : orderedFiles) {
        createShortcutItem(pair.first, pair.second, count++);
    }
    
    return true;
}

// 执行搜索操作
void FunctionArea::performSearch() {
    QString text = searchBox->text();
    onSearchTextChanged(text);
}

// 处理搜索文本变化
void FunctionArea::onSearchTextChanged(const QString &text) {
    // 如果搜索文本为空，显示所有项目
    if (text.isEmpty()) {
        clearSearch();
        return;
    }
    
    // 显示清除按钮
    clearButton->setVisible(true);
    
    // 清空当前列表
    shortcuts->clear();
    
    // 搜索匹配项并添加到列表
    for (int i = 0; i < allItems.size(); i++) {
        QListWidgetItem* item = allItems[i];
        QString itemText = item->text();
        QString itemToolTip = item->toolTip();
        
        // 使用Qt::CaseInsensitive标志进行不区分大小写的比较
        if (itemText.contains(text, Qt::CaseInsensitive) || 
            itemToolTip.contains(text, Qt::CaseInsensitive)) {
            // 使用克隆方式创建新项目
            QListWidgetItem* newItem = item->clone();
            shortcuts->addItem(newItem);
        }
    }
    
    // 如果没有匹配项，显示提示
    if (shortcuts->count() == 0) {
        QListWidgetItem* noResultItem = new QListWidgetItem("没有找到匹配的工具");
        noResultItem->setFlags(noResultItem->flags() & ~Qt::ItemIsEnabled);
        noResultItem->setTextAlignment(Qt::AlignCenter);
        shortcuts->addItem(noResultItem);
    }
}

// 清除搜索
void FunctionArea::clearSearch() {
    // 清空搜索框
    searchBox->clear();
    
    // 隐藏清除按钮
    clearButton->setVisible(false);
    
    // 清空当前列表
    shortcuts->clear();
    
    // 恢复所有项目（使用克隆方式创建新项目）
    for (int i = 0; i < allItems.size(); i++) {
        QListWidgetItem* item = allItems[i];
        QListWidgetItem* newItem = item->clone();
        shortcuts->addItem(newItem);
    }
}

// 扫描文件夹获取快捷方式
void FunctionArea::scanFolderForShortcuts(const QString &folderName) {
=======
void FunctionArea::showShortcuts(const QString &folderName) {
    // 保存当前文件夹名称
    currentFolderName = folderName;
    
    // 清空当前快捷方式列表
    shortcuts->clear();
    
>>>>>>> 24e5c4dcb27d9756890814ce87fc620fc05fe1cf
    // 设置鼠标等待光标
    QApplication::setOverrideCursor(Qt::WaitCursor);
    
    // 读取指定文件夹及其子文件夹下的支持的文件类型
    QDir toolsDir("tools/" + folderName);
    
<<<<<<< HEAD
=======
    // 创建文件图标提供器
    QFileIconProvider iconProvider;
    
>>>>>>> 24e5c4dcb27d9756890814ce87fc620fc05fe1cf
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
<<<<<<< HEAD
=======
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
>>>>>>> 24e5c4dcb27d9756890814ce87fc620fc05fe1cf
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
            
            // 创建列表项
            createShortcutItem(fileInfo.absoluteFilePath(), displayName, count++);
        }
    }
    
    // 恢复鼠标光标
    QApplication::restoreOverrideCursor();
    
    // 更新缓存
    QList<QPair<QString, QString>> cachedFiles;
    for (int i = 0; i < shortcuts->count(); i++) {
        QListWidgetItem *item = shortcuts->item(i);
        cachedFiles.append(qMakePair(item->data(Qt::UserRole).toString(), item->text()));
    }
    updateCache(folderName, cachedFiles);
}

void FunctionArea::showShortcuts(const QString &folderName) {
    // 保存当前文件夹名称
    currentFolderName = folderName;
    
    // 清空当前快捷方式列表和存储的所有项目
    shortcuts->clear();
    allItems.clear();
    
    // 清空搜索框
    searchBox->clear();
    clearButton->setVisible(false);
    
    // 尝试加载缓存的快捷方式
    if (loadCachedShortcuts(folderName)) {
        // 成功加载缓存后，保存所有项目用于搜索（使用深拷贝）
        for (int i = 0; i < shortcuts->count(); i++) {
            QListWidgetItem* originalItem = shortcuts->item(i);
            QListWidgetItem* clonedItem = originalItem->clone();
            allItems.append(clonedItem);
        }
        return; // 成功加载缓存，直接返回
    }
    
    // 缓存无效，需要重新扫描
    scanFolderForShortcuts(folderName);
    
    // 扫描后保存所有项目用于搜索（使用深拷贝）
    for (int i = 0; i < shortcuts->count(); i++) {
        QListWidgetItem* originalItem = shortcuts->item(i);
        QListWidgetItem* clonedItem = originalItem->clone();
        allItems.append(clonedItem);
    }
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
        
        // 添加编辑描述选项
        QAction *descriptionAction = menu.addAction("编辑描述");
        connect(descriptionAction, &QAction::triggered, this, &FunctionArea::onEditDescription);
        
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

void FunctionArea::onEditDescription() {
    QListWidgetItem *item = getSelectedItem();
    if (!item) {
        return;
    }
    
    // 获取文件路径
    QString filePath = item->data(Qt::UserRole).toString();
    QFileInfo fileInfo(filePath);
    
    // 获取当前描述
    QString currentDescription = item->data(Qt::UserRole + 1).toString();
    
    // 创建自定义对话框
    QDialog dialog(this);
    dialog.setWindowTitle("编辑工具描述");
    dialog.setMinimumWidth(400);
    
    // 创建输入框和标签
    QTextEdit *textEdit = new QTextEdit(&dialog);
    textEdit->setText(currentDescription);
    textEdit->setPlaceholderText("请输入工具描述...");
    QLabel *label = new QLabel("请输入工具描述:", &dialog);
    
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
    mainLayout->addWidget(textEdit);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    dialog.setLayout(mainLayout);
    
    // 显示对话框
    bool ok = (dialog.exec() == QDialog::Accepted);
    QString newDescription = textEdit->toPlainText().trimmed();
    
    if (ok) {
        // 创建描述文件夹
        QString descDir = "tools/" + currentFolderName + "/descriptions";
        QDir dir;
        if (!dir.exists(descDir)) {
            dir.mkpath(descDir);
        }
        
        // 创建或更新描述文件
        QString descPath = descDir + "/" + fileInfo.fileName() + ".desc";
        QFile descFile(descPath);
        
        if (newDescription.isEmpty()) {
            // 如果描述为空，删除描述文件
            if (descFile.exists()) {
                descFile.remove();
            }
            item->setToolTip(filePath); // 恢复为文件路径
            item->setData(Qt::UserRole + 1, QVariant()); // 清除描述数据
        } else {
            // 保存描述文件
            if (descFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&descFile);
                out << newDescription;
                descFile.close();
                
                // 更新工具提示和描述数据
                item->setToolTip(newDescription);
                item->setData(Qt::UserRole + 1, newDescription);
            } else {
                QMessageBox::warning(this, "编辑描述", "无法保存描述信息。");
            }
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
        
        // 清除所有文件夹的缓存，确保下次访问时重新扫描
        folderCache.clear();
        
        // 刷新当前显示的快捷方式
        if (!currentFolderName.isEmpty()) {
            showShortcuts(currentFolderName);
            qDebug() << "已更新文件类型并刷新显示";
        } else {
            qDebug() << "已更新文件类型，请选择一个文件夹以查看更改";
            // 可以在这里添加一个状态栏消息或弹出提示
            if (statusBar) {
                statusBar->showMessage("文件类型已更新，请选择一个文件夹以查看更改", 3000);
            }
        }
    }
}

// 处理工具列表重新排序
void FunctionArea::onShortcutsReordered() {
    // 如果没有选中文件夹，则不处理
    if (currentFolderName.isEmpty()) {
        return;
    }
    
    // 创建顺序配置文件目录
    QString orderDir = "tools/" + currentFolderName + "/order";
    QDir dir;
    if (!dir.exists(orderDir)) {
        dir.mkpath(orderDir);
    }
    
    // 保存当前顺序到配置文件
    QString orderFilePath = orderDir + "/shortcuts.order";
    QFile orderFile(orderFilePath);
    
    if (orderFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&orderFile);
        
        // 保存所有工具的路径，按当前显示顺序
        for (int i = 0; i < shortcuts->count(); i++) {
            QListWidgetItem *item = shortcuts->item(i);
            QString filePath = item->data(Qt::UserRole).toString();
            out << filePath << "\n";
        }
        
        orderFile.close();
        qDebug() << "工具顺序已保存到" << orderFilePath;
    } else {
        qDebug() << "无法保存工具顺序到" << orderFilePath;
    }
}

void FunctionArea::onShortcutDoubleClicked(QListWidgetItem *item) {
    if (!item) {
        return;
    }

    QString exeFilePath = item->data(Qt::UserRole).toString();

    // 检查文件是否存在
    if (!QFile::exists(exeFilePath)) {
        QMessageBox::warning(this, "启动工具", "文件不存在: " + exeFilePath);
        return;
    }
    
    // 获取文件信息
    QFileInfo fileInfo(exeFilePath);
    
    // 检查是否有参数配置文件
    QStringList args;
    QString argsConfigPath = "tools/" + currentFolderName + "/shortcuts/" + fileInfo.fileName() + ".args";
    if (QFile::exists(argsConfigPath)) {
        QFile argsFile(argsConfigPath);
        if (argsFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&argsFile);
            QString argsStr = in.readAll().trimmed();
            if (!argsStr.isEmpty()) {
                args = argsStr.split(" ", Qt::SkipEmptyParts);
            }
            argsFile.close();
        } else {
            qDebug() << "无法打开参数配置文件: " << argsConfigPath;
        }
    }
    
    // 检查活动进程数量
    if (activeProcesses.size() >= maxProcesses) {
        // 提示用户进程数量已达上限
        QMessageBox::information(this, "启动工具", 
            QString("当前已有%1个工具在运行，将关闭最早启动的工具以启动新工具。").arg(maxProcesses));
    }
    
    // 使用进程管理启动进程
    if (!startProcess(exeFilePath, args)) {
        QMessageBox::critical(this, "启动工具", 
            QString("无法启动工具: %1\n\n请检查文件权限或是否被其他程序占用。").arg(fileInfo.fileName()));
    } else {
        // 显示成功启动的提示（可选，如果不想每次都提示，可以注释掉）
        // QMessageBox::information(this, "启动工具", 
        //     QString("成功启动工具: %1\n\n当前运行工具数: %2/%3").arg(fileInfo.fileName()).arg(activeProcesses.size()).arg(maxProcesses));
        
        // 在状态栏显示提示（如果有状态栏）
        emit shortcutSelected(fileInfo.fileName());
    }
}
