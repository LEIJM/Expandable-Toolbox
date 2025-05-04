// MainWindow.cpp
#include <QApplication>
#include "mainwindow.h"
#include "directoryarea.h"
#include "functionarea.h"
#include <QLabel>
#include <QFile>
#include <QTextStream>
#include <QStatusBar>
#include <QSplitter>
#include <QStyleFactory>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QEvent>

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent) {
    // 设置窗口标题和大小
    setWindowTitle("Expandable Toolbox");
    resize(900, 650);
    
    // 应用Fusion风格，更现代的外观
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    
    // 创建目录区和功能区
    directoryArea = new DirectoryArea(this);
    functionArea = new FunctionArea(this);
    
    // 设置分割器，允许用户调整两个区域的大小
    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(directoryArea);
    splitter->addWidget(functionArea);
    splitter->setStretchFactor(0, 1); // 目录区域
    splitter->setStretchFactor(1, 3); // 功能区域（占更多空间）
    splitter->setHandleWidth(1);      // 设置分隔条宽度
    
    // 设置中央部件
    setCentralWidget(splitter);

    // 设置全局字体
    QFont font("微软雅黑", 10); // 更适合界面的字体大小
    QApplication::setFont(font); // 应用全局字体设置
    
    // 设置应用程序样式表
    QString styleSheet = ""
        "QMainWindow, QWidget { background-color: #f5f5f5; }"
        "QSplitter::handle { background-color: #cccccc; }"
        "QStatusBar { background-color: #e0e0e0; border-top: 1px solid #cccccc; }"
        "QLabel { color: #333333; }";        
    setStyleSheet(styleSheet);

    // 连接信号和槽
    connect(directoryArea, &DirectoryArea::folderSelected, functionArea, &FunctionArea::showShortcuts);

    // 创建状态栏
    statusBar()->setSizeGripEnabled(false); // 禁用右下角的大小控制点
    
    // 添加欢迎信息到状态栏
    welcomeLabel = new QLabel("欢迎使用工具箱");
    welcomeLabel->setStyleSheet("padding: 2px 8px; color: #555555; text-decoration: underline; cursor: pointer;");
    welcomeLabel->setToolTip("点击查看关于信息");
    statusBar()->addWidget(welcomeLabel);
    
    // 连接欢迎标签的点击事件
    connect(welcomeLabel, &QLabel::linkActivated, this, &MainWindow::showAboutDialog);
    // 由于QLabel默认不处理鼠标事件，我们需要安装事件过滤器
    welcomeLabel->installEventFilter(this);
    welcomeLabel->setCursor(Qt::PointingHandCursor);
}

// 实现关于对话框
void MainWindow::showAboutDialog() {
    QDialog *aboutDialog = new QDialog(this);
    aboutDialog->setWindowTitle("关于工具箱");
    aboutDialog->setFixedSize(400, 300);
    // 设置对话框标志，移除上下文帮助按钮，添加固定大小标志以防止闪烁
    aboutDialog->setWindowFlags((aboutDialog->windowFlags() & ~Qt::WindowContextHelpButtonHint) | Qt::MSWindowsFixedSizeDialogHint);
    aboutDialog->setAttribute(Qt::WA_DeleteOnClose);
    // 确保对话框使用正常的背景绘制
    aboutDialog->setStyleSheet("QDialog { background-color: #f5f5f5; }");
    aboutDialog->setModal(true); // 设置为模态对话框
    aboutDialog->setWindowModality(Qt::ApplicationModal); // 应用程序级别的模态
    
    QVBoxLayout *layout = new QVBoxLayout(aboutDialog);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(10);
    
    // 应用名称和图标
    QLabel *titleLabel = new QLabel("Expandable Toolbox", aboutDialog);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 18pt; font-weight: bold; color: #2c3e50; margin-bottom: 10px;");
    
    // 版本信息 - 使用固定版本号
    QLabel *versionInfoLabel = new QLabel("版本: 2.0.0", aboutDialog);
    versionInfoLabel->setAlignment(Qt::AlignCenter);
    versionInfoLabel->setStyleSheet("font-size: 10pt; color: #7f8c8d; margin-bottom: 20px;");
    
    // 应用描述
    QLabel *descriptionLabel = new QLabel(
        "<p>Expandable Toolbox是一个可扩展的工具箱应用程序，允许用户组织和快速访问各种工具。</p>"
        "<p>主要功能：</p>"
        "<ul>"
        "<li>工具分类管理</li>"
        "<li>工具重命名</li>"
        "<li>支持多种文件类型</li>"
        "<li>现代化界面设计</li>"
        "</ul>"
        "<p>感谢您使用本软件！</p>", 
        aboutDialog);
    descriptionLabel->setWordWrap(true);
    descriptionLabel->setAlignment(Qt::AlignLeft);
    descriptionLabel->setStyleSheet("font-size: 10pt; color: #34495e; line-height: 150%;");
    descriptionLabel->setMinimumHeight(150); // 设置最小高度
    descriptionLabel->setFixedWidth(360); // 设置固定宽度
    descriptionLabel->setTextFormat(Qt::RichText); // 使用富文本格式
    
    // 版权信息 - 添加可点击链接
    QLabel *copyrightLabel = new QLabel("<a href=\"https://github.com/LEIJM/Expandable-Toolbox\" style=\"color: #3498db; text-decoration: underline;\">© 2024-2025 Expandable Toolbox Team. 保留所有权利。</a>", aboutDialog);
    copyrightLabel->setAlignment(Qt::AlignCenter);
    copyrightLabel->setStyleSheet("font-size: 9pt; margin-top: 20px;");
    copyrightLabel->setTextFormat(Qt::RichText);
    copyrightLabel->setOpenExternalLinks(true); // 允许打开外部链接
    copyrightLabel->setTextInteractionFlags(Qt::TextBrowserInteraction); // 允许文本交互
    copyrightLabel->setToolTip("点击访问项目主页"); // 添加提示信息
    copyrightLabel->setCursor(Qt::PointingHandCursor); // 设置鼠标指针为手型
    
    // 添加到布局
    layout->addWidget(titleLabel);
    layout->addWidget(versionInfoLabel);
    layout->addWidget(descriptionLabel);
    layout->addStretch();
    layout->addWidget(copyrightLabel);
    
    // 添加确定按钮
    QPushButton *okButton = new QPushButton("确定", aboutDialog);
    okButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #3498db;"
        "    color: white;"
        "    border: none;"
        "    padding: 8px 16px;"
        "    border-radius: 4px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #2980b9;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #1c6ea4;"
        "}");
    okButton->setDefault(true);
    connect(okButton, &QPushButton::clicked, aboutDialog, &QDialog::accept);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addStretch();
    
    layout->addLayout(buttonLayout);
    
    // 显示对话框
    aboutDialog->exec();
}

// 事件过滤器，处理标签的鼠标点击事件
bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    if (obj == welcomeLabel && event->type() == QEvent::MouseButtonRelease) {
        showAboutDialog();
        return true;
    }
    return QMainWindow::eventFilter(obj, event);
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 设置应用程序图标
    app.setWindowIcon(QIcon("data/toolbox.ico"));

    MainWindow window;
    window.show();

    return app.exec();
}
