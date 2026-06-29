// DirectoryArea.h
#pragma once
#include <QWidget>
#include <QVBoxLayout>
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
    void folderFilterRulesRequested(const QString &folderName);

public:
    DirectoryArea(QWidget *parent = nullptr);
    
    // 刷新分类列表
    void refreshFolders();
    bool selectFirstFolder();
private slots:
    void onFolderItemClicked(QListWidgetItem *item);
    void onAddFolderClicked();
    void onRenameFolderClicked();
    void onOpenFolderInExplorer();
    void onContextMenuRequested(const QPoint &pos);
    void onFolderContextMenuRequested(const QPoint &pos);
    void onFoldersReordered();
private:
    void adjustWidthToContents();
    QWidget *contentWidget;
    QScrollArea *scrollArea;
    QListWidget *folderListWidget; // 用于显示和排序文件夹
};
