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
    layout = new QVBoxLayout(contentWidget);
    layout->setSpacing(8);
    layout->setContentsMargins(10, 10, 10, 10);
    
    // 添加弹性空间，使按钮靠上对齐
    layout->addStretch();
    
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
}

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
    if (!selectedButton) {
        return;
    }
    
    QString oldName = selectedButton->text();
    
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
            // 刷新文件夹列表
            refreshFolders();
            
            // 选中重命名后的文件夹
            if (folderButtons.contains(newName)) {
                folderButtons[newName]->click();
            }
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
