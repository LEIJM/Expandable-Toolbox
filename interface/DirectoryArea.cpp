// DirectoryArea.cpp
#include <QVBoxLayout>
#include <QPushButton>
#include <QDir>
#include <QLabel>
#include <QScrollArea>
#include <QFrame>
#include <QIcon>
#include <QMenu>
#include <QAction>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include "directoryarea.h"

DirectoryArea::DirectoryArea(QWidget *parent)
        : QWidget(parent), selectedButton(nullptr) {
    // 创建标题标签和操作按钮区域
    QWidget *headerWidget = new QWidget(this);
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(8, 8, 8, 8);
    headerLayout->setSpacing(5);
    
    QLabel *titleLabel = new QLabel("工具分类", this);
    titleLabel->setStyleSheet("font-weight: bold; font-size: 12pt; color: #2c3e50;");
    
    // 添加分类按钮
    QPushButton *addButton = new QPushButton(this);
    addButton->setIcon(QIcon::fromTheme("list-add", QIcon(":/icons/add.png")));
    addButton->setToolTip("添加新分类");
    addButton->setFixedSize(24, 24);
    addButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #ecf0f1;"
        "    border: none;"
        "    border-radius: 12px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #d6eaf8;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #aed6f1;"
        "}");
    
    connect(addButton, &QPushButton::clicked, this, &DirectoryArea::onAddFolderClicked);
    
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(addButton);
    
    // 设置标题区域样式
    headerWidget->setStyleSheet("background-color: #ecf0f1; border-bottom: 1px solid #bdc3c7;");
    
    // 创建内容区域的容器和布局
    contentWidget = new QWidget();
<<<<<<< HEAD
    
    // 使用QListWidget替代QVBoxLayout，以支持拖放排序
    folderListWidget = new QListWidget(contentWidget);
    folderListWidget->setStyleSheet(
        "QListWidget {"
        "    background-color: transparent;"
        "    border: none;"
        "    outline: none;"
        "}"
        "QListWidget::item {"
        "    padding: 8px;"
        "    border-radius: 4px;"
        "    margin-bottom: 4px;"
        "}"
        "QListWidget::item:selected {"
        "    background-color: #e3f2fd;"
        "    color: #1976d2;"
        "}"
        "QListWidget::item:hover {"
        "    background-color: #f5f5f5;"
        "}");
    
    // 启用拖放功能
    folderListWidget->setDragEnabled(true);
    folderListWidget->setAcceptDrops(true);
    folderListWidget->setDropIndicatorShown(true);
    folderListWidget->setDragDropMode(QAbstractItemView::InternalMove);
    folderListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    
    // 连接拖放完成信号
    connect(folderListWidget->model(), &QAbstractItemModel::rowsMoved, this, &DirectoryArea::onFoldersReordered);
    
    // 连接项目点击信号
    connect(folderListWidget, &QListWidget::itemClicked, this, &DirectoryArea::onFolderItemClicked);
    
    // 设置右键菜单
    folderListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(folderListWidget, &QListWidget::customContextMenuRequested, this, &DirectoryArea::onFolderContextMenuRequested);
    
    // 创建布局
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setSpacing(8);
    contentLayout->setContentsMargins(10, 10, 10, 10);
    contentLayout->addWidget(folderListWidget);
=======
    layout = new QVBoxLayout(contentWidget);
    layout->setSpacing(8);
    layout->setContentsMargins(10, 10, 10, 10);
    
    // 添加弹性空间，使按钮靠上对齐
    layout->addStretch();
>>>>>>> 24e5c4dcb27d9756890814ce87fc620fc05fe1cf
    
    // 创建滚动区域
    scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(contentWidget);
    scrollArea->setFrameShape(QFrame::NoFrame); // 移除边框
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 禁用水平滚动条
    
    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(headerWidget);
    mainLayout->addWidget(scrollArea);
    
    // 设置整体样式
    setStyleSheet("background-color: #f8f9fa; border-right: 1px solid #e0e0e0;");
    setMinimumWidth(180); // 设置最小宽度
    
    // 启用右键菜单
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QWidget::customContextMenuRequested, this, &DirectoryArea::onContextMenuRequested);
    
    // 初始加载文件夹
    refreshFolders();
<<<<<<< HEAD
=======
}

void DirectoryArea::refreshFolders() {
    // 清除现有按钮
    QLayoutItem *item;
    while ((item = layout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    
    folderButtons.clear();
    
    // 读取 tools 文件夹下的子文件夹
    QDir toolsDir("tools");
    if (!toolsDir.exists()) {
        toolsDir.mkpath("."); // 如果tools文件夹不存在，创建它
    }
    
    QStringList folders = toolsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    
    // 为每个文件夹创建一个美化的按钮
    for (const QString &folder : folders) {
        QPushButton *button = new QPushButton(folder, this);
        button->setStyleSheet(
            "QPushButton {"
            "    text-align: left;"
            "    padding: 8px 12px;"
            "    background-color: #ffffff;"
            "    border: 1px solid #e0e0e0;"
            "    border-radius: 4px;"
            "    color: #333333;"
            "}"
            "QPushButton:hover {"
            "    background-color: #e3f2fd;"
            "    border: 1px solid #bbdefb;"
            "}"
            "QPushButton:pressed, QPushButton:checked {"
            "    background-color: #bbdefb;"
            "}");
        
        // 设置按钮为可选中状态
        button->setCheckable(true);
        
        // 尝试为按钮添加图标
        QString iconPath = "tools/" + folder + "/icon.png";
        if (QFile::exists(iconPath)) {
            button->setIcon(QIcon(iconPath));
        } else {
            // 使用默认文件夹图标
            button->setIcon(QIcon::fromTheme("folder"));
        }
        
        button->setIconSize(QSize(20, 20));
        button->setContextMenuPolicy(Qt::CustomContextMenu);
        
        // 连接信号
        connect(button, &QPushButton::clicked, this, &DirectoryArea::onFolderButtonClicked);
        connect(button, &QWidget::customContextMenuRequested, this, [this, button](const QPoint &pos) {
            selectedButton = button;
            QMenu menu(this);
            QAction *renameAction = menu.addAction("重命名分类");
            connect(renameAction, &QAction::triggered, this, &DirectoryArea::onRenameFolderClicked);
            menu.exec(button->mapToGlobal(pos));
        });
        
        layout->addWidget(button);
        folderButtons[folder] = button;
    }
    
    // 添加弹性空间，使按钮靠上对齐
    layout->addStretch();
>>>>>>> 24e5c4dcb27d9756890814ce87fc620fc05fe1cf
}

void DirectoryArea::refreshFolders() {
    // 清除现有项目
    folderListWidget->clear();
    folderButtons.clear();
    
    // 读取保存的顺序
    QStringList orderedFolders;
    QString orderFilePath = "tools/folders.order";
    if (QFile::exists(orderFilePath)) {
        QFile orderFile(orderFilePath);
        if (orderFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&orderFile);
            while (!in.atEnd()) {
                QString line = in.readLine().trimmed();
                if (!line.isEmpty()) {
                    orderedFolders.append(line);
                }
            }
            orderFile.close();
        }
    }
    
    // 读取 tools 文件夹下的子文件夹
    QDir toolsDir("tools");
    if (!toolsDir.exists()) {
        toolsDir.mkpath("."); // 如果tools文件夹不存在，创建它
    }
    
    QStringList folders = toolsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    
    // 如果有保存的顺序，按照顺序排列文件夹
    if (!orderedFolders.isEmpty()) {
        QStringList orderedAndExistingFolders;
        
        // 首先添加有序的文件夹（如果它们存在）
        foreach (const QString &folder, orderedFolders) {
            if (folders.contains(folder)) {
                orderedAndExistingFolders.append(folder);
                folders.removeOne(folder);
            }
        }
        
        // 然后添加剩余的文件夹（新添加的或未排序的）
        orderedAndExistingFolders.append(folders);
        folders = orderedAndExistingFolders;
    }
    
    // 为每个文件夹创建一个列表项
    for (const QString &folder : folders) {
        QListWidgetItem *item = new QListWidgetItem(folder);
        item->setData(Qt::UserRole, folder); // 存储文件夹名称
        folderListWidget->addItem(item);
        
        // 创建按钮（用于兼容现有代码）
        QPushButton *button = new QPushButton(folder, this);
        button->setVisible(false); // 隐藏按钮，因为我们使用列表项代替
        button->setCheckable(true);
        
        // 尝试为按钮添加图标
        QString iconPath = "tools/" + folder + "/icon.png";
        if (QFile::exists(iconPath)) {
            button->setIcon(QIcon(iconPath));
            item->setIcon(QIcon(iconPath));
        } else {
            // 使用默认文件夹图标
            button->setIcon(QIcon::fromTheme("folder"));
            item->setIcon(QIcon::fromTheme("folder"));
        }
        
        button->setIconSize(QSize(20, 20));
        
        // 存储按钮（用于兼容现有代码）
        folderButtons[folder] = button;
    }
    
    // 不再需要添加弹性空间，QListWidget已经处理了项目的布局
    // layout->addStretch(); // 这行代码已被移除，因为我们使用QListWidget而不是QVBoxLayout
}

void DirectoryArea::onFolderItemClicked(QListWidgetItem *item) {
    if (item) {
        // 获取文件夹名称
        QString folderName = item->text();
        
        // 更新选中的按钮（兼容现有代码）
        if (folderButtons.contains(folderName)) {
            selectedButton = folderButtons[folderName];
            selectedButton->setChecked(true);
        }
        
        // 发出信号，通知选中了哪个文件夹
        emit folderSelected(folderName);
    }
}

// 保留此方法以兼容现有代码
void DirectoryArea::onFolderButtonClicked() {
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (button) {
        // 取消其他按钮的选中状态
        for (auto it = folderButtons.begin(); it != folderButtons.end(); ++it) {
            if (it.value() != button) {
                it.value()->setChecked(false);
            }
        }
        
        // 设置当前按钮为选中状态
        button->setChecked(true);
        selectedButton = button;
        
        emit folderSelected(button->text());
    }
}

<<<<<<< HEAD
// 处理文件夹列表重新排序
void DirectoryArea::onFoldersReordered() {
    // 保存当前顺序到配置文件
    QString orderFilePath = "tools/folders.order";
    QFile orderFile(orderFilePath);
    
    if (orderFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&orderFile);
        
        // 保存所有文件夹的名称，按当前显示顺序
        for (int i = 0; i < folderListWidget->count(); i++) {
            QListWidgetItem *item = folderListWidget->item(i);
            QString folderName = item->text();
            out << folderName << "\n";
        }
        
        orderFile.close();
        qDebug() << "文件夹顺序已保存到" << orderFilePath;
    } else {
        qDebug() << "无法保存文件夹顺序到" << orderFilePath;
    }
}

// 处理文件夹列表的右键菜单
void DirectoryArea::onFolderContextMenuRequested(const QPoint &pos) {
    QListWidgetItem *item = folderListWidget->itemAt(pos);
    if (item) {
        // 选中当前项
        folderListWidget->setCurrentItem(item);
        
        // 获取文件夹名称
        QString folderName = item->text();
        
        // 创建右键菜单
        QMenu menu(this);
        
        // 添加重命名选项
        QAction *renameAction = menu.addAction("重命名分类");
        connect(renameAction, &QAction::triggered, this, &DirectoryArea::onRenameFolderClicked);
        
        // 显示菜单
        menu.exec(folderListWidget->mapToGlobal(pos));
    }
}

=======
>>>>>>> 24e5c4dcb27d9756890814ce87fc620fc05fe1cf
void DirectoryArea::onAddFolderClicked() {
    // 创建自定义对话框以便修改按钮文本
    QDialog dialog(this);
    dialog.setWindowTitle("添加分类");
    
    // 创建输入框和标签
    QLineEdit *lineEdit = new QLineEdit(&dialog);
    QLabel *label = new QLabel("请输入新分类名称:", &dialog);
    
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
    QString folderName = lineEdit->text();
    if (ok && !folderName.isEmpty()) {
        // 检查是否已存在同名文件夹
        QDir toolsDir("tools");
        if (toolsDir.exists(folderName)) {
            QMessageBox::warning(this, "添加分类", "已存在同名分类，请使用其他名称。");
            return;
        }
        
        // 创建新文件夹
        if (toolsDir.mkdir(folderName)) {
            // 刷新文件夹列表
            refreshFolders();
            
            // 选中新创建的文件夹
            if (folderButtons.contains(folderName)) {
                folderButtons[folderName]->click();
            }
        } else {
            QMessageBox::critical(this, "添加分类", "创建分类文件夹失败。");
        }
    }
}

void DirectoryArea::onRenameFolderClicked() {
<<<<<<< HEAD
    // 获取当前选中的列表项
    QListWidgetItem *item = folderListWidget->currentItem();
    if (!item) {
        return;
    }
    
    QString oldName = item->text();
=======
    if (!selectedButton) {
        return;
    }
    
    QString oldName = selectedButton->text();
>>>>>>> 24e5c4dcb27d9756890814ce87fc620fc05fe1cf
    
    // 创建自定义对话框以便修改按钮文本
    QDialog dialog(this);
    dialog.setWindowTitle("重命名分类");
    
    // 创建输入框和标签
    QLineEdit *lineEdit = new QLineEdit(&dialog);
    lineEdit->setText(oldName);
    QLabel *label = new QLabel("请输入新的分类名称:", &dialog);
    
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
    
    if (ok && !newName.isEmpty() && newName != oldName) {
        // 检查是否已存在同名文件夹
        QDir toolsDir("tools");
        if (toolsDir.exists(newName)) {
            QMessageBox::warning(this, "重命名分类", "已存在同名分类，请使用其他名称。");
            return;
        }
        
        // 重命名文件夹
        if (toolsDir.rename(oldName, newName)) {
<<<<<<< HEAD
            // 更新列表项文本
            item->setText(newName);
            item->setData(Qt::UserRole, newName);
            
            // 更新按钮映射（兼容现有代码）
            if (folderButtons.contains(oldName)) {
                QPushButton *button = folderButtons[oldName];
                button->setText(newName);
                folderButtons.remove(oldName);
                folderButtons[newName] = button;
                selectedButton = button;
            }
            
            // 保存更新后的顺序
            onFoldersReordered();
            
            // 发出信号，通知选中了新的文件夹
            emit folderSelected(newName);
=======
            // 刷新文件夹列表
            refreshFolders();
            
            // 选中重命名后的文件夹
            if (folderButtons.contains(newName)) {
                folderButtons[newName]->click();
            }
>>>>>>> 24e5c4dcb27d9756890814ce87fc620fc05fe1cf
        } else {
            QMessageBox::critical(this, "重命名分类", "重命名分类文件夹失败。");
        }
    }
}

void DirectoryArea::onContextMenuRequested(const QPoint &pos) {
    // 只有在没有选中按钮的情况下，才显示空白区域的右键菜单
    if (!selectedButton || !selectedButton->underMouse()) {
        QMenu menu(this);
        QAction *addAction = menu.addAction("添加新分类");
        connect(addAction, &QAction::triggered, this, &DirectoryArea::onAddFolderClicked);
        menu.exec(mapToGlobal(pos));
    }
}
