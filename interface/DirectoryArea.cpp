// DirectoryArea.cpp
#include <QVBoxLayout>
#include <QPushButton>
#include <QDir>
#include "directoryarea.h"

DirectoryArea::DirectoryArea(QWidget *parent)
        : QWidget(parent) {
    // 创建布局
    layout = new QVBoxLayout();

    // 读取 tools 文件夹下的子文件夹
    QDir toolsDir("tools");
    QStringList folders = toolsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString &folder : folders) {
        QPushButton *button = new QPushButton(folder, this);
        layout->addWidget(button);
        connect(button, &QPushButton::clicked, this, &DirectoryArea::onFolderButtonClicked);
    }
    setLayout(layout);
}

void DirectoryArea::onFolderButtonClicked() {
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (button) {
        emit folderSelected(button->text());
    }
}
