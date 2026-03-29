// FunctionArea.h
#pragma once
#include <QWidget>
#include <QListWidget>
#include <QFileIconProvider>
#include <QMenu>
#include <QAction>
#include <QStringList>
#include <QHash>
#include <QSharedPointer>
#include <QProcess>
#include <QTimer>
#include <QStatusBar>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDateTime>

class QListWidgetItem;

class FunctionArea : public QWidget {
Q_OBJECT

public:
    FunctionArea(QWidget *parent = nullptr);
    ~FunctionArea();
    
    // 获取当前选中的工具项
    QListWidgetItem* getSelectedItem() const;
    
    // 设置状态栏
    void setStatusBar(QStatusBar *bar) { statusBar = bar; }

signals:
    void shortcutSelected(const QString &fileName);

public slots:
    void showShortcuts(const QString &folderName);
    void onShortcutDoubleClicked(QListWidgetItem *item);
    void onRenameShortcut();
    void onEditDescription(); // 编辑工具描述
    void onContextMenuRequested(const QPoint &pos);
    void onManageFileExtensions();
    void saveFileExtensions(const QStringList &extensions);
    void onShortcutsReordered(); // 处理工具列表重新排序
    void onSearchTextChanged(const QString &text); // 处理搜索文本变化
    void clearSearch(); // 清除搜索框

private:
    struct ShortcutEntry {
        QString filePath;
        QString displayName;
        QString description;
    };

    QListWidget *shortcuts;
    QLineEdit *searchBox; // 搜索框
    QPushButton *clearButton; // 清除搜索按钮
    QString currentFolderName; // 当前显示的文件夹名称
    QStringList supportedExtensions; // 支持的文件后缀列表
    QList<QListWidgetItem*> allItems; // 存储所有项目，用于搜索功能
    QTimer *searchTimer; // 搜索延迟定时器
    
    // 执行搜索操作
    void performSearch();
    
    // 文件缓存相关
    struct FileCache {
        QDateTime dirLastModified;
        QDateTime lastAccessedAt;
        QList<ShortcutEntry> files;
    };
    QHash<QString, FileCache> folderCache;  // 文件夹缓存
    QTimer *cacheCleanupTimer;  // 缓存清理定时器
    
    // 进程管理相关
    QList<QSharedPointer<QProcess>> activeProcesses;  // 活动进程列表
    int maxProcesses;  // 最大进程数
    
    // 状态栏指针
    QStatusBar *statusBar;
    QFileIconProvider fileIconProvider;
    
    // 加载支持的文件后缀
    void loadFileExtensions();
    // 获取文件过滤器
    QStringList getFileFilters() const;
    // 检查缓存是否有效
    bool isCacheValid(const QString &folderName) const;
    // 更新缓存
    void updateCache(const QString &folderName, const QList<ShortcutEntry> &files);
    // 清理过期缓存
    void cleanupCache();
    // 管理进程
    void manageProcesses();
    // 启动进程
    bool startProcess(const QString &exeFilePath, const QStringList &args = QStringList());
    // 清理已结束的进程
    void cleanupProcesses();
    
    // 创建单个快捷方式项
    void createShortcutItem(const ShortcutEntry &entry, int count);
    // 应用自定义排序
    QList<ShortcutEntry> applyCustomOrder(const QString &folderName, QList<ShortcutEntry> files);
    // 加载缓存的快捷方式
    bool loadCachedShortcuts(const QString &folderName);
    // 扫描文件夹获取快捷方式
    void scanFolderForShortcuts(const QString &folderName);
};
