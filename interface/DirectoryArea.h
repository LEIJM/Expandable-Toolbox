// DirectoryArea.h
#include <QWidget>
#include <QVBoxLayout>
#include <QMap>
#include <QPushButton>
#include <QScrollArea>
#include <QWidget>
<<<<<<< HEAD
#include <QListWidget>
#include <QFile>
#include <QTextStream>
#include <QDebug>
=======
>>>>>>> 24e5c4dcb27d9756890814ce87fc620fc05fe1cf

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
<<<<<<< HEAD
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
=======
    void onAddFolderClicked();
    void onRenameFolderClicked();
    void onContextMenuRequested(const QPoint &pos);
private:
    QVBoxLayout *layout;
    QWidget *contentWidget;
    QScrollArea *scrollArea;
    QMap<QString, QPushButton*> folderButtons; // 存储文件夹名称和对应的按钮
>>>>>>> 24e5c4dcb27d9756890814ce87fc620fc05fe1cf
    QPushButton *selectedButton; // 当前选中的按钮
};
