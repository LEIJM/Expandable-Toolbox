// MainWindow.cpp
#include <QApplication>
#include "mainwindow.h"
#include "directoryarea.h"
#include "functionarea.h"
#include <QLabel>
#include <QFile>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent) {
    // 设置窗口标题和大小
    setWindowTitle("Expandable Toolbox");
    resize(800, 600);

    // 创建目录区和功能区
    directoryArea = new DirectoryArea(this);
    functionArea = new FunctionArea(this);

    // 设置布局
    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(directoryArea);
    layout->addWidget(functionArea);

    QWidget *centralWidget = new QWidget();
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    // 设置全局字体大小
    QFont font("微软雅黑", 12); // 这里设置字体和大小
    QApplication::setFont(font); // 应用全局字体设置

    // 连接信号和槽
    connect(directoryArea, &DirectoryArea::folderSelected, functionArea, &FunctionArea::showShortcuts);

    // 创建 QLabel 用于显示版本信息
    versionLabel = new QLabel(this);

    // 读取 data/version 文件
    QString versionText;
    QFile versionFile("data/version");
    if (versionFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&versionFile);
        versionText = in.readAll();
        versionFile.close();
    } else {
        versionText = "无法读取版本信息";
    }

    // 设置 QLabel 的文本
    versionLabel->setText(versionText);

    // 将 QLabel 添加到布局中
    layout->addWidget(versionLabel, 0, Qt::AlignBottom | Qt::AlignLeft);
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 设置应用程序图标
    app.setWindowIcon(QIcon("data/toolbox.ico"));

    MainWindow window;
    window.show();

    return app.exec();
}
