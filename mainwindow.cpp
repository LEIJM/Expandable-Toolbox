// MainWindow.cpp
#include <QApplication>
#include "mainwindow.h"
#include "DirectoryArea.h"
#include "FunctionArea.h"
#include "AppVersion.h"
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
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QSysInfo>
#include <QFrame>

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
    directoryArea->selectFirstFolder();

    // 创建状态栏
    QStatusBar* bar = statusBar();
    bar->setSizeGripEnabled(false); // 禁用右下角的大小控制点
    
    // 创建永久消息区域（左侧）
    welcomeLabel = new QLabel("欢迎使用工具箱");
    welcomeLabel->setStyleSheet("padding: 2px 8px; color: #1976d2; font-weight: bold; text-decoration: underline; cursor: pointer;");
    welcomeLabel->setToolTip("点击查看关于信息");
    bar->addPermanentWidget(welcomeLabel, 0); // 0表示不伸展
    
    // 创建临时消息区域（右侧）
    QLabel* messageLabel = new QLabel("就绪");
    messageLabel->setStyleSheet("padding: 2px 8px; color: #555555;");
    bar->addWidget(messageLabel, 1); // 1表示伸展填充可用空间
    
    // 将状态栏传递给功能区
    functionArea->setStatusBar(bar);
    
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
    aboutDialog->setFixedSize(450, 380); // 增加对话框尺寸，提供更多空间
    // 设置对话框标志，移除上下文帮助按钮，添加固定大小标志以防止闪烁
    aboutDialog->setWindowFlags((aboutDialog->windowFlags() & ~Qt::WindowContextHelpButtonHint) | Qt::MSWindowsFixedSizeDialogHint);
    aboutDialog->setAttribute(Qt::WA_DeleteOnClose);
    // 确保对话框使用正常的背景绘制，添加圆角和阴影效果
    aboutDialog->setStyleSheet(
        "QDialog { "
        "    background-color: #f5f5f5; "
        "    border-radius: 8px; "
        "}");
    aboutDialog->setModal(true); // 设置为模态对话框
    aboutDialog->setWindowModality(Qt::ApplicationModal); // 应用程序级别的模态
    
    QVBoxLayout *layout = new QVBoxLayout(aboutDialog);
    layout->setContentsMargins(25, 25, 25, 20); // 增加边距
    layout->setSpacing(12); // 增加间距
    
    // 创建顶部水平布局，包含图标和标题
    QHBoxLayout *headerLayout = new QHBoxLayout();
    
    // 添加应用图标
    QLabel *iconLabel = new QLabel(aboutDialog);
    QPixmap appIcon(":/toolbox.ico"); // 使用应用图标资源
    if (appIcon.isNull()) {
        // 如果无法加载图标，尝试从文件加载
        appIcon.load("toolbox.ico");
    }
    
    if (!appIcon.isNull()) {
        // 调整图标大小并设置
        appIcon = appIcon.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        iconLabel->setPixmap(appIcon);
        iconLabel->setFixedSize(64, 64);
    } else {
        // 如果仍然无法加载，使用文本替代
        iconLabel->setText("🧰");
        iconLabel->setStyleSheet("font-size: 36pt; color: #3498db;");
        iconLabel->setFixedSize(64, 64);
        iconLabel->setAlignment(Qt::AlignCenter);
    }
    
    // 创建标题和版本的垂直布局
    QVBoxLayout *titleLayout = new QVBoxLayout();
    
    // 应用名称
    QLabel *titleLabel = new QLabel("Expandable Toolbox", aboutDialog);
    titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    titleLabel->setStyleSheet("font-size: 20pt; font-weight: bold; color: #2c3e50;");
    
    // 获取应用程序版本信息
    QString versionText = QString("版本: %1").arg(APP_VERSION_STRING);
    QLabel *versionInfoLabel = new QLabel(versionText, aboutDialog);
    versionInfoLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    versionInfoLabel->setStyleSheet("font-size: 10pt; color: #7f8c8d;");
    
    // 添加到标题布局
    titleLayout->addWidget(titleLabel);
    titleLayout->addWidget(versionInfoLabel);
    titleLayout->setSpacing(2); // 减小标题和版本之间的间距
    
    // 添加到顶部布局
    headerLayout->addWidget(iconLabel);
    headerLayout->addLayout(titleLayout, 1);
    headerLayout->setSpacing(15); // 设置图标和标题之间的间距
    
    // 添加分隔线
    QFrame *line = new QFrame(aboutDialog);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setStyleSheet("background-color: #e0e0e0; max-height: 1px;");
    
    // 应用描述 - 使用更丰富的HTML格式
    QLabel *descriptionLabel = new QLabel(
        "<p style='margin-top:0;'>Expandable Toolbox是一个可扩展的工具箱应用程序，允许用户和组织快速访问各种工具。</p>"
        "<p style='margin-bottom:5px;'><b>主要功能：</b></p>"
        "<ul style='margin-top:0;'>"
        "<li>工具分类管理 - 按类别整理您的工具</li>"
        "<li>工具重命名 - 自定义工具名称以便于识别</li>"
        "<li>支持多种文件类型 - 可执行文件、文档、链接等</li>"
        "<li>现代化界面设计 - 简洁直观的用户体验</li>"
        "<li>可自定义布局 - 根据您的需求调整界面</li>"
        "</ul>"
        "<p>感谢您使用本软件！</p>", 
        aboutDialog);
    descriptionLabel->setWordWrap(true);
    descriptionLabel->setAlignment(Qt::AlignLeft);
    descriptionLabel->setStyleSheet("font-size: 10pt; color: #34495e; line-height: 150%;");
    descriptionLabel->setMinimumHeight(180); // 增加最小高度
    descriptionLabel->setTextFormat(Qt::RichText); // 使用富文本格式
    
    // 添加系统信息
    QString systemInfo = QString("<p style='margin-bottom:5px;'><b>系统信息：</b></p>"
                               "<p style='margin-top:0;'>Qt 版本: %1<br>"
                               "操作系统: %2</p>")
                            .arg(QT_VERSION_STR)
                            .arg(QSysInfo::prettyProductName());
    
    QLabel *systemInfoLabel = new QLabel(systemInfo, aboutDialog);
    systemInfoLabel->setWordWrap(true);
    systemInfoLabel->setAlignment(Qt::AlignLeft);
    systemInfoLabel->setStyleSheet("font-size: 9pt; color: #555555;");
    systemInfoLabel->setTextFormat(Qt::RichText);
    
    // 版权信息 - 添加可点击链接
    QLabel *copyrightLabel = new QLabel(
        "<div style='text-align:center;'>"
        "<a href=\"https://github.com/LEIJM/Expandable-Toolbox\" style=\"color: #3498db; text-decoration: underline;\">© 2024-2025 Expandable Toolbox Team</a><br>"
        "保留所有权利"
        "</div>", 
        aboutDialog);
    copyrightLabel->setAlignment(Qt::AlignCenter);
    copyrightLabel->setStyleSheet("font-size: 9pt; margin-top: 10px;");
    copyrightLabel->setTextFormat(Qt::RichText);
    copyrightLabel->setOpenExternalLinks(true); // 允许打开外部链接
    copyrightLabel->setTextInteractionFlags(Qt::TextBrowserInteraction); // 允许文本交互
    copyrightLabel->setToolTip("点击访问项目主页"); // 添加提示信息
    copyrightLabel->setCursor(Qt::PointingHandCursor); // 设置鼠标指针为手型
    
    // 添加到布局
    layout->addLayout(headerLayout);
    layout->addWidget(line);
    layout->addWidget(descriptionLabel);
    layout->addWidget(systemInfoLabel);
    layout->addStretch();
    layout->addWidget(copyrightLabel);
    
    // 添加确定按钮
    QPushButton *okButton = new QPushButton("确定", aboutDialog);
    okButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #3498db;"
        "    color: white;"
        "    border: none;"
        "    padding: 8px 24px;"
        "    border-radius: 4px;"
        "    font-weight: bold;"
        "    min-width: 100px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #2980b9;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #1c6ea4;"
        "}");
    okButton->setDefault(true);
    okButton->setCursor(Qt::PointingHandCursor); // 设置鼠标指针为手型
    connect(okButton, &QPushButton::clicked, aboutDialog, &QDialog::accept);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addStretch();
    
    layout->addLayout(buttonLayout);
    
    // 添加淡入效果
    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(aboutDialog);
    aboutDialog->setGraphicsEffect(effect);
    
    QPropertyAnimation *animation = new QPropertyAnimation(effect, "opacity");
    animation->setDuration(300); // 300毫秒的动画
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->start(QPropertyAnimation::DeleteWhenStopped);
    
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
