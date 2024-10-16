// FunctionArea.cpp
#include <QVBoxLayout>
#include <QProcess>
#include <QDir>
#include "functionarea.h"
#include <QListWidgetItem>
#include <QDirIterator>
#include <qthread.h>

FunctionArea::FunctionArea(QWidget *parent)
        : QWidget(parent) {
    // 创建快捷方式列表
    shortcuts = new QListWidget(this);

    // 连接双击信号到槽函数
    connect(shortcuts, &QListWidget::itemDoubleClicked, this, &FunctionArea::onShortcutDoubleClicked);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(shortcuts);
    setLayout(layout);
}

void FunctionArea::showShortcuts(const QString &folderName) {
    // 清空当前快捷方式列表
    shortcuts->clear();

    // 读取指定文件夹及其子文件夹下的 exe 和 cmd 文件
    QDir toolsDir("tools/" + folderName);  // 确认此路径是否正确

    // 创建QDirIterator来递归遍历文件夹
    QDirIterator dirIterator(toolsDir.path(), QStringList() << "*.exe" << "*.cmd", QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
    while (dirIterator.hasNext()) {
        dirIterator.next();
        QFileInfo fileInfo = dirIterator.fileInfo();

        // 检查文件是否以 .exe 或 .cmd 结尾
        if (fileInfo.isFile() && (fileInfo.suffix().compare("exe", Qt::CaseInsensitive) == 0 || fileInfo.suffix().compare("cmd", Qt::CaseInsensitive) == 0)) {
            QListWidgetItem *item = new QListWidgetItem(fileInfo.completeBaseName(), shortcuts);
            item->setData(Qt::UserRole, fileInfo.absoluteFilePath());
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
