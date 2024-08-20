// FunctionArea.cpp
#include <QVBoxLayout>
#include <QProcess>
#include <QDir>
#include "functionarea.h"
#include <QListWidgetItem>
#include <QDirIterator>

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

    // 读取指定文件夹及其子文件夹下的 exe 文件快捷方式
    QDir toolsDir("tools/" + folderName);  // 确认此路径是否正确

    // 创建QDirIterator来递归遍历文件夹
    QDirIterator dirIterator(toolsDir.path(), QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
    while (dirIterator.hasNext()) {
        dirIterator.next();
        QString filePath = dirIterator.filePath();
        QFileInfo fileInfo = dirIterator.fileInfo();

        // 检查文件是否以.exe结尾
        if (fileInfo.isFile() && fileInfo.suffix() == "exe") {
            QListWidgetItem *item = new QListWidgetItem(fileInfo.completeBaseName(), shortcuts);
            item->setData(Qt::UserRole, filePath);
        }
    }
}


void FunctionArea::onShortcutDoubleClicked(QListWidgetItem *item) {
    if (item) {
        QString exeFilePath = item->data(Qt::UserRole).toString();

        // 检查文件是否存在
        if (!QFile::exists(exeFilePath)) {
            return;
        }

        // 尝试启动程序
        QProcess process;
        process.start(exeFilePath);

        // 等待程序启动
        if (process.waitForStarted()) {
            // 等待程序结束
            process.waitForFinished();
        }
    }
}
