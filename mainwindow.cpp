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
#include <QTimer>
#include <QTextBrowser>
#include <QStorageInfo>
#include <QProcess>
#include <QDateTime>
#include <QProcessEnvironment>
#include <QCloseEvent>
#include <QtConcurrent>
#include <QFutureWatcher>
#include <windows.h>

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), systemMonitorDialog(nullptr), systemMonitorContentLabel(nullptr), systemMonitorTimer(nullptr),
          cpuBar(nullptr), gpuBar(nullptr), memBar(nullptr), diskBar(nullptr),
          lastIdleTime(0), lastKernelTime(0), lastUserTime(0) {
    // 初始化监控结果监听器
    connect(&monitorDataWatcher, &QFutureWatcher<MainWindow::MonitorData>::finished, this, &MainWindow::handleMonitorDataReadyV2);
    
    // 设置窗口标题和大小
    setWindowTitle("可扩展工具箱 - Expandable Toolbox");
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
    QString styleSheet = 
        "QMainWindow { background-color: #f8f9fa; }"
        "QSplitter::handle { background-color: #e9ecef; width: 1px; }"
        "QStatusBar { background-color: #ffffff; border-top: 1px solid #dee2e6; color: #495057; }"
        "QToolTip { background-color: #333333; color: #ffffff; border: none; padding: 5px; border-radius: 3px; }"
        "QDialog { background-color: #ffffff; }"
        "QPushButton { padding: 6px 12px; border-radius: 4px; border: 1px solid #ced4da; background-color: #ffffff; }"
        "QPushButton:hover { background-color: #e9ecef; }"
        "QPushButton:pressed { background-color: #dee2e6; }"
        "QLineEdit { padding: 6px; border-radius: 4px; border: 1px solid #ced4da; background-color: #ffffff; }"
        "QLineEdit:focus { border-color: #4dabf7; outline: none; }";        
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
    systemMonitorLabel = new QLabel("系统资源");
    systemMonitorLabel->setStyleSheet("padding: 2px 8px; color: #1976d2; text-decoration: underline;");
    systemMonitorLabel->setToolTip("点击查看硬件配置与实时状态");
    systemMonitorLabel->setCursor(Qt::PointingHandCursor);
    bar->addWidget(systemMonitorLabel, 1);
    
    // 将状态栏传递给功能区
    functionArea->setStatusBar(bar);
    
    // 连接欢迎标签的点击事件
    connect(welcomeLabel, &QLabel::linkActivated, this, &MainWindow::showAboutDialog);
    // 由于QLabel默认不处理鼠标事件，我们需要安装事件过滤器
    welcomeLabel->installEventFilter(this);
    systemMonitorLabel->installEventFilter(this);
    welcomeLabel->setCursor(Qt::PointingHandCursor);
    
    // 创建系统托盘图标
    createTrayIcon();
}

void MainWindow::createTrayIcon() {
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/toolbox.ico"));
    trayIcon->setToolTip("Expandable Toolbox");
    
    trayMenu = new QMenu(this);
    QAction *showAction = trayMenu->addAction("显示/隐藏");
    connect(showAction, &QAction::triggered, this, &MainWindow::toggleWindowVisibility);
    
    trayMenu->addSeparator();
    
    QAction *quitAction = trayMenu->addAction("退出");
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
    
    trayIcon->setContextMenu(trayMenu);
    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::onTrayIconActivated);
    
    trayIcon->show();
}

void MainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason) {
    if (reason == QSystemTrayIcon::Trigger) {
        toggleWindowVisibility();
    }
}

void MainWindow::toggleWindowVisibility() {
    if (isVisible()) {
        hide();
    } else {
        showNormal();
        activateWindow();
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    // 关闭时直接退出，不再拦截到托盘
    QMainWindow::closeEvent(event);
}

void MainWindow::changeEvent(QEvent *event) {
    if (event->type() == QEvent::WindowStateChange) {
        if (isMinimized()) {
            // 最小化时隐藏窗口并显示托盘提示（可选）
            hide();
            if (trayIcon && trayIcon->isVisible()) {
                trayIcon->showMessage("Expandable Toolbox", "应用已最小化到托盘", QSystemTrayIcon::Information, 2000);
            }
            event->accept();
            return;
        }
    }
    QMainWindow::changeEvent(event);
}

MainWindow::~MainWindow() {
    if (systemMonitorTimer) {
        systemMonitorTimer->stop();
    }
}

// 实现关于对话框
void MainWindow::showAboutDialog() {
    QDialog aboutDialog(this);
    aboutDialog.setWindowTitle("关于工具箱");
    aboutDialog.setFixedSize(400, 420);
    aboutDialog.setWindowFlags(aboutDialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);
    
    aboutDialog.setStyleSheet("QDialog { background-color: #f5f5f5; }");

    QVBoxLayout *mainLayout = new QVBoxLayout(&aboutDialog);
    mainLayout->setContentsMargins(25, 30, 25, 20);
    mainLayout->setSpacing(15);

    // 图标展示
    QLabel *iconLabel = new QLabel(&aboutDialog);
    QPixmap appIcon(":/toolbox.ico");
    if (appIcon.isNull()) appIcon.load("toolbox.ico");
    if (!appIcon.isNull()) {
        iconLabel->setPixmap(appIcon.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        iconLabel->setText("🧰");
        iconLabel->setStyleSheet("font-size: 40pt;");
    }
    iconLabel->setAlignment(Qt::AlignCenter);

    // 标题与版本
    QLabel *titleLabel = new QLabel("可扩展工具箱", &aboutDialog);
    titleLabel->setStyleSheet("font-size: 18pt; font-weight: bold; color: #333333;");
    titleLabel->setAlignment(Qt::AlignCenter);
    
    QLabel *versionLabel = new QLabel(QString("版本 %1").arg(APP_VERSION_STRING), &aboutDialog);
    versionLabel->setStyleSheet("font-size: 10pt; color: #666666;");
    versionLabel->setAlignment(Qt::AlignCenter);

    // 简简软件要点
    QLabel *descLabel = new QLabel(
        "一款简单、高效的可扩展工具管理平台。<br><br>"
        "• 工具分类与智能管理<br>"
        "• 异步扫描与快速启动<br>"
        "• 启动参数与权限配置", 
        &aboutDialog);
    descLabel->setWordWrap(true);
    descLabel->setStyleSheet("font-size: 10pt; color: #444444; line-height: 1.5;");
    descLabel->setTextFormat(Qt::RichText);
    descLabel->setAlignment(Qt::AlignCenter);

    // 分隔线
    QFrame *line = new QFrame(&aboutDialog);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setStyleSheet("background-color: #dddddd; max-height: 1px;");

    // 底部链接与版权
    QLabel *footerLabel = new QLabel(
        "<a href='https://github.com/LEIJM/Expandable-Toolbox' style='color: #0066cc; text-decoration: none;'>GitHub 项目主页</a><br>"
        "<span style='color: #888888; font-size: 9pt;'>© 2024-2026 Expandable Toolbox Team</span>", 
        &aboutDialog);
    footerLabel->setAlignment(Qt::AlignCenter);
    footerLabel->setOpenExternalLinks(true);
    footerLabel->setTextFormat(Qt::RichText);

    // 确定按钮
    QPushButton *okButton = new QPushButton("确 定", &aboutDialog);
    okButton->setFixedWidth(100);
    okButton->setCursor(Qt::PointingHandCursor);
    okButton->setStyleSheet(
        "QPushButton { background-color: #e1e1e1; color: #333333; border: 1px solid #adadad; padding: 6px; border-radius: 2px; }"
        "QPushButton:hover { background-color: #e5f1fb; border-color: #0078d7; }"
        "QPushButton:pressed { background-color: #cce4f7; }"
    );
    connect(okButton, &QPushButton::clicked, &aboutDialog, &QDialog::accept);

    mainLayout->addWidget(iconLabel);
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(versionLabel);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(descLabel);
    mainLayout->addStretch();
    mainLayout->addWidget(line);
    mainLayout->addWidget(footerLabel);
    mainLayout->addWidget(okButton, 0, Qt::AlignCenter);

    aboutDialog.exec();
}

void MainWindow::showSystemMonitorDialog() {
    if (!systemMonitorDialog) {
        systemMonitorDialog = new QDialog(this);
        systemMonitorDialog->setWindowTitle("系统资源监控");
        systemMonitorDialog->setFixedSize(620, 600); // 增加高度以容纳进度条
        systemMonitorDialog->setStyleSheet("QDialog { background-color: #ffffff; }");
        systemMonitorDialog->setWindowFlags(systemMonitorDialog->windowFlags() & ~Qt::WindowContextHelpButtonHint);

        QVBoxLayout *layout = new QVBoxLayout(systemMonitorDialog);
        layout->setContentsMargins(25, 25, 25, 25);
        layout->setSpacing(15);

        QLabel *titleLabel = new QLabel("实时资源状态", systemMonitorDialog);
        titleLabel->setStyleSheet("font-size: 18pt; font-weight: bold; color: #202124;");
        layout->addWidget(titleLabel);

        // 创建进度条容器
        QFrame *barsFrame = new QFrame(systemMonitorDialog);
        barsFrame->setStyleSheet("QFrame { background: transparent; border: none; padding: 0px; }");
        QGridLayout *barsGrid = new QGridLayout(barsFrame);
        barsGrid->setContentsMargins(0, 0, 0, 0);
        barsGrid->setSpacing(12);
        barsGrid->setColumnStretch(1, 1); // 进度条列自动拉伸

        auto addBarToGrid = [&](const QString &name, QProgressBar* &bar, int row) {
            QLabel *label = new QLabel(name, barsFrame);
            label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            label->setStyleSheet("font-weight: bold; color: #495057; background: transparent;");

            bar = new QProgressBar(barsFrame);
            bar->setRange(0, 100);
            bar->setTextVisible(true);
            bar->setFormat("%p%");
            bar->setStyleSheet(
                "QProgressBar { border: 1px solid #ced4da; border-radius: 4px; background: #e9ecef; text-align: center; height: 18px; color: #212529; font-weight: bold; }"
                "QProgressBar::chunk { background-color: #4dabf7; border-radius: 3px; }"
            );
            
            barsGrid->addWidget(label, row, 0);
            barsGrid->addWidget(bar, row, 1);
        };

        addBarToGrid("CPU", cpuBar, 0);
        addBarToGrid("GPU", gpuBar, 1);
        addBarToGrid("内存", memBar, 2);
        addBarToGrid("硬盘", diskBar, 3);
        layout->addWidget(barsFrame);

        systemMonitorContentLabel = new QLabel(systemMonitorDialog);
        systemMonitorContentLabel->setTextFormat(Qt::RichText);
        systemMonitorContentLabel->setWordWrap(true);
        systemMonitorContentLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        systemMonitorContentLabel->setStyleSheet(
            "QLabel {"
            "    font-family: 'Segoe UI', 'Microsoft YaHei';"
            "    font-size: 10pt;"
            "    color: #3c4043;"
            "    line-height: 1.6;"
            "}"
        );
        layout->addWidget(systemMonitorContentLabel, 1);



        QHBoxLayout *buttonLayout = new QHBoxLayout();
        buttonLayout->addStretch();
        QPushButton *closeButton = new QPushButton("关闭", systemMonitorDialog);
        closeButton->setCursor(Qt::PointingHandCursor);
        connect(closeButton, &QPushButton::clicked, systemMonitorDialog, &QDialog::close);
        buttonLayout->addWidget(closeButton);
        layout->addLayout(buttonLayout);

        connect(systemMonitorDialog, &QDialog::finished, this, [this]() {
            if (systemMonitorTimer) {
                systemMonitorTimer->stop();
            }
        });
    }

    // 立即触发一次更新，确保弹窗出来时尽快显示数据
    updateSystemMonitorInfo();

    if (!systemMonitorTimer) {
        systemMonitorTimer = new QTimer(this);
        connect(systemMonitorTimer, &QTimer::timeout, this, &MainWindow::updateSystemMonitorInfo);
        systemMonitorTimer->start(1500);
    } else {
        systemMonitorTimer->start();
    }
    
    // 设置初始加载状态，避免白屏
    if (systemMonitorContentLabel && systemMonitorContentLabel->text().isEmpty()) {
        systemMonitorContentLabel->setText("<p style='color: #5f6368;'>正在采集系统实时数据，请稍候...</p>");
    }

    systemMonitorDialog->show();
    systemMonitorDialog->raise();
    systemMonitorDialog->activateWindow();
}

void MainWindow::updateSystemMonitorInfo() {
    // 移除 !systemMonitorDialog->isVisible() 的限制，允许在弹窗显示前就开始后台采集
    if (!systemMonitorDialog || monitorDataWatcher.isRunning()) {
        return;
    }
    
    // 使用 QtConcurrent 在后台线程执行耗时的数据采集任务
    QFuture<MainWindow::MonitorData> future = QtConcurrent::run(&MainWindow::collectMonitorDataV2, this);
    monitorDataWatcher.setFuture(future);
}

void MainWindow::handleMonitorDataReadyV2() {
    MonitorData data = monitorDataWatcher.result();
    if (systemMonitorContentLabel) {
        systemMonitorContentLabel->setText(data.htmlContent);
    }
    
    // 更新进度条
    if (cpuBar) cpuBar->setValue(static_cast<int>(data.cpuUsage));
    if (gpuBar) gpuBar->setValue(static_cast<int>(data.gpuUsage));
    if (memBar) memBar->setValue(static_cast<int>(data.memUsage));
    if (diskBar) diskBar->setValue(static_cast<int>(data.diskUsage));
    
    // 根据负载动态调整进度条颜色
    auto updateBarStyle = [](QProgressBar* bar, double val) {
        if (!bar) return;
        QString color = "#4dabf7"; // 蓝色 (正常)
        if (val > 85) color = "#fa5252"; // 红色 (高负载)
        else if (val > 60) color = "#fab005"; // 黄色 (中负载)
        
        bar->setStyleSheet(
            QString("QProgressBar { border: 1px solid #ced4da; border-radius: 5px; background: #e9ecef; text-align: center; height: 20px; color: #212529; font-weight: bold; }"
                    "QProgressBar::chunk { background-color: %1; border-radius: 4px; }").arg(color)
        );
    };
    
    updateBarStyle(cpuBar, data.cpuUsage);
    updateBarStyle(gpuBar, data.gpuUsage);
    updateBarStyle(memBar, data.memUsage);
    updateBarStyle(diskBar, data.diskUsage);
}

MainWindow::MonitorData MainWindow::collectMonitorDataV2() {
    MonitorData data;
    
    QString cpuName = qEnvironmentVariable("PROCESSOR_IDENTIFIER");
    if (cpuName.isEmpty()) {
        cpuName = QSysInfo::currentCpuArchitecture();
    }

    FILETIME idleTime;
    FILETIME kernelTime;
    FILETIME userTime;
    GetSystemTimes(&idleTime, &kernelTime, &userTime);
    auto toUInt64 = [](const FILETIME &ft) -> quint64 {
        ULARGE_INTEGER uli;
        uli.LowPart = ft.dwLowDateTime;
        uli.HighPart = ft.dwHighDateTime;
        return uli.QuadPart;
    };
    quint64 currentIdle = toUInt64(idleTime);
    quint64 currentKernel = toUInt64(kernelTime);
    quint64 currentUser = toUInt64(userTime);
    
    if (lastKernelTime != 0 && lastUserTime != 0) {
        quint64 systemDelta = (currentKernel - lastKernelTime) + (currentUser - lastUserTime);
        quint64 idleDelta = currentIdle - lastIdleTime;
        if (systemDelta > 0 && systemDelta >= idleDelta) {
            data.cpuUsage = (double)(systemDelta - idleDelta) * 100.0 / (double)systemDelta;
        }
    }
    lastIdleTime = currentIdle;
    lastKernelTime = currentKernel;
    lastUserTime = currentUser;

    MEMORYSTATUSEX memStatus;
    memStatus.dwLength = sizeof(memStatus);
    GlobalMemoryStatusEx(&memStatus);
    double memTotalGB = memStatus.ullTotalPhys / 1024.0 / 1024.0 / 1024.0;
    double memAvailGB = memStatus.ullAvailPhys / 1024.0 / 1024.0 / 1024.0;
    double memUsedGB = memTotalGB - memAvailGB;
    data.memUsage = memStatus.dwMemoryLoad;

    QStorageInfo systemDisk = QStorageInfo::root();
    qint64 diskTotal = systemDisk.bytesTotal();
    qint64 diskFree = systemDisk.bytesAvailable();
    double diskTotalGB = diskTotal / 1024.0 / 1024.0 / 1024.0;
    double diskUsedGB = (diskTotal - diskFree) / 1024.0 / 1024.0 / 1024.0;
    data.diskUsage = diskTotal > 0 ? (double)(diskTotal - diskFree) * 100.0 / (double)diskTotal : 0.0;

    QString gpuName = cachedGpuName.isEmpty() ? "-" : cachedGpuName;
    
    if (cachedGpuName.isEmpty()) {
        QProcess gpuInfoProcess;
        gpuInfoProcess.start("powershell", QStringList() << "-NoProfile" << "-Command" << "Get-CimInstance Win32_VideoController | Select-Object -First 1 -ExpandProperty Name");
        if (gpuInfoProcess.waitForFinished(1000)) {
            QString output = QString::fromLocal8Bit(gpuInfoProcess.readAllStandardOutput()).trimmed();
            if (!output.isEmpty()) {
                gpuName = output;
                const_cast<MainWindow*>(this)->cachedGpuName = gpuName;
            }
        }
    }
    
    QProcess gpuUsageProcess;
    gpuUsageProcess.start("powershell", QStringList() << "-NoProfile" << "-Command" << "$v=(Get-Counter '\\GPU Engine(*)\\Utilization Percentage').CounterSamples | Measure-Object -Property CookedValue -Sum; [math]::Round($v.Sum,1)");
    if (gpuUsageProcess.waitForFinished(1000)) {
        QString output = QString::fromLocal8Bit(gpuUsageProcess.readAllStandardOutput()).trimmed();
        if (!output.isEmpty()) {
            data.gpuUsage = output.toDouble();
        }
    }

    data.htmlContent = QString(
        "<p><b>更新时间：</b>%1</p>"
        "<p><b>CPU：</b>%2</p>"
        "<p><b>GPU：</b>%3</p>"
        "<p><b>内存：</b>%4 / %5 GB</p>"
        "<p><b>硬盘（系统盘 %6）：</b>%7 / %8 GB</p>"
        "<p><b>系统：</b>%9</p>")
        .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
        .arg(cpuName.toHtmlEscaped())
        .arg(gpuName.toHtmlEscaped())
        .arg(QString::number(memUsedGB, 'f', 1))
        .arg(QString::number(memTotalGB, 'f', 1))
        .arg(systemDisk.rootPath().toHtmlEscaped())
        .arg(QString::number(diskUsedGB, 'f', 1))
        .arg(QString::number(diskTotalGB, 'f', 1))
        .arg(QSysInfo::prettyProductName().toHtmlEscaped());
        
    return data;
}

// 事件过滤器，处理标签的鼠标点击事件
bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    if (obj == welcomeLabel && event->type() == QEvent::MouseButtonRelease) {
        showAboutDialog();
        return true;
    }
    if (obj == systemMonitorLabel && event->type() == QEvent::MouseButtonRelease) {
        showSystemMonitorDialog();
        return true;
    }
    return QMainWindow::eventFilter(obj, event);
}
