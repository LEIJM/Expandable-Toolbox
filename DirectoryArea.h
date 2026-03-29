// DirectoryArea.h
#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QMap>
#include <QPushButton>
#include <QScrollArea>
#include <QListWidget>
#include <QFile>
#include <QTextStream>
#include <QDebug>

class DirectoryArea : public QWidget {
Q_OBJECT

signals:
    void folderSelected(const QString &folderName);

public:
    DirectoryArea(QWidget *parent = nullptr);
    
    // 刷新分类列表
    void refreshFolders();
private slots:
    void onFolderButtonClicked();
    void onFolderItemClicked(QListWidgetItem *item);
    void onAddFolderClicked();
    void onRenameFolderClicked();
    void onContextMenuRequested(const QPoint &pos);
    void onFolderContextMenuRequested(const QPoint &pos);
    void onFoldersReordered();
private:
    QWidget *contentWidget;
    QScrollArea *scrollArea;
    QListWidget *folderListWidget; // 用于显示和排序文件夹
    QMap<QString, QPushButton*> folderButtons; // 存储文件夹名称和对应的按钮（兼容现有代码）
    QPushButton *selectedButton; // 当前选中的按钮
};
