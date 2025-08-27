// main.cpp
#include <QApplication>
#include <QIcon>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 设置应用程序图标
    // app.setWindowIcon(QIcon("toolbox.ico"));

    MainWindow window;
    window.show();

    return app.exec();
}