// DirectoryArea.h
#include <QWidget>
#include <QVBoxLayout>
#include <QMap>
#include <QPushButton>
#include <QScrollArea>
#include <QWidget>

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
    void onAddFolderClicked();
    void onRenameFolderClicked();
    void onContextMenuRequested(const QPoint &pos);
private:
    QVBoxLayout *layout;
    QWidget *contentWidget;
    QScrollArea *scrollArea;
    QMap<QString, QPushButton*> folderButtons; // 存储文件夹名称和对应的按钮
    QPushButton *selectedButton; // 当前选中的按钮
};
