// MainWindow.h
#pragma once
#include <QMainWindow>
#include <QLabel>
#include <QtGlobal>

class DirectoryArea;
class FunctionArea;
class QDialog;
class QTimer;

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    
    // 重写事件过滤器
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void showAboutDialog();
    void showSystemMonitorDialog();
    void updateSystemMonitorInfo();

private:
    DirectoryArea *directoryArea;
    FunctionArea *functionArea;
    QLabel *welcomeLabel;
    QLabel *systemMonitorLabel;
    QDialog *systemMonitorDialog;
    QLabel *systemMonitorContentLabel;
    QTimer *systemMonitorTimer;
    quint64 lastIdleTime;
    quint64 lastKernelTime;
    quint64 lastUserTime;
};
