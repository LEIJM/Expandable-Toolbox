// FunctionArea.h
#include <QWidget>
#include <QListWidget>
#include <QFileIconProvider>
#include <QMenu>
#include <QAction>
#include <QStringList>

class QListWidgetItem;

class FunctionArea : public QWidget {
Q_OBJECT

public:
    FunctionArea(QWidget *parent = nullptr);
    
    // 获取当前选中的工具项
    QListWidgetItem* getSelectedItem() const;

signals:
    void shortcutSelected(const QString &fileName);

public slots:
    void showShortcuts(const QString &folderName);
    void onShortcutDoubleClicked(QListWidgetItem *item);
    void onRenameShortcut();
    void onContextMenuRequested(const QPoint &pos);
    void onManageFileExtensions();
    void saveFileExtensions(const QStringList &extensions);

private:
    QListWidget *shortcuts;
    QString currentFolderName; // 当前显示的文件夹名称
    QStringList supportedExtensions; // 支持的文件后缀列表
    
    // 加载支持的文件后缀
    void loadFileExtensions();
    // 获取文件过滤器
    QStringList getFileFilters() const;
};
