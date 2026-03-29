// MainWindow.h
#pragma once
#include <QMainWindow>
#include <QLabel>
#include <QtGlobal>
#include <QSystemTrayIcon>
#include <QMenu>

#include <QFutureWatcher>
#include <QProgressBar>

class DirectoryArea;
class FunctionArea;
class QDialog;
class QTimer;

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
    // 重写事件过滤器
    bool eventFilter(QObject *obj, QEvent *event) override;

protected:
    void closeEvent(QCloseEvent *event) override;
    void changeEvent(QEvent *event) override;

private slots:
    void showAboutDialog();
    void showSystemMonitorDialog();
    void updateSystemMonitorInfo();
    void toggleWindowVisibility();
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    DirectoryArea *directoryArea;
    FunctionArea *functionArea;
    
    // 系统托盘相关
    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;
    void createTrayIcon();
    QLabel *welcomeLabel;
    QLabel *systemMonitorLabel;
    QDialog *systemMonitorDialog;
    QLabel *systemMonitorContentLabel;
    
    // 进度条组件
    QProgressBar *cpuBar;
    QProgressBar *gpuBar;
    QProgressBar *memBar;
    QProgressBar *diskBar;
    
    QTimer *systemMonitorTimer;
    quint64 lastIdleTime;
    quint64 lastKernelTime;
    quint64 lastUserTime;
    QString cachedGpuName;
    
    struct MonitorData {
        double cpuUsage = 0;
        double gpuUsage = 0;
        double memUsage = 0;
        double diskUsage = 0;
        QString htmlContent;
    };
    QFutureWatcher<MonitorData> monitorDataWatcher;
    MonitorData collectMonitorDataV2();
    void handleMonitorDataReadyV2();
};
