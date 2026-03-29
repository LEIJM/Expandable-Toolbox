// FunctionArea.cpp
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QProcess>
#include <QDir>
#include "FunctionArea.h"
#include <QListWidgetItem>
#include <QDirIterator>
#include <QThread>
#include <QLabel>
#include <QFileIconProvider>
#include <QHeaderView>
#include <QApplication>
#include <QInputDialog>
#include <QMessageBox>
#include <QMenu>
#include <QPushButton>
#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QFileDialog>
#include <QIcon>
#include <QSet>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <windows.h>
#include <shellapi.h>

FunctionArea::FunctionArea(QWidget *parent)
        : QWidget(parent), currentFolderName(""), maxProcesses(10), statusBar(nullptr) {
    // 初始化异步扫描观察者
    watcher = new QFutureWatcher<QList<ShortcutEntry>>(this);
    connect(watcher, &QFutureWatcher<QList<ShortcutEntry>>::finished, [this]() {
        onScanningFinished(currentFolderName, watcher->result());
    });
    
    // 初始化缓存清理定时器
    cacheCleanupTimer = new QTimer(this);
    cacheCleanupTimer->setInterval(30 * 60 * 1000); // 30分钟清理一次缓存
    connect(cacheCleanupTimer, &QTimer::timeout, this, &FunctionArea::cleanupCache);
    cacheCleanupTimer->start();
    
    // 初始化搜索延迟定时器
    searchTimer = new QTimer(this);
    searchTimer->setSingleShot(true);
    searchTimer->setInterval(300); // 300毫秒延迟
    connect(searchTimer, &QTimer::timeout, this, &FunctionArea::performSearch);
    // 加载支持的文件后缀
    loadFileExtensions();
    // 创建标题标签
    QLabel *titleLabel = new QLabel("可用工具", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-weight: bold; font-size: 12pt; padding: 8px; color: #2c3e50; background-color: #ecf0f1; border-bottom: 1px solid #bdc3c7;");
    
    // 创建搜索框
    QHBoxLayout *searchLayout = new QHBoxLayout();
    searchLayout->setContentsMargins(8, 8, 8, 8);
    
    searchBox = new QLineEdit(this);
    searchBox->setPlaceholderText("搜索工具名称或描述...");
    searchBox->setClearButtonEnabled(true);
    searchBox->setStyleSheet(
        "QLineEdit {"
        "    border: 1px solid #bdc3c7;"
        "    border-radius: 4px;"
        "    padding: 6px;"
        "    background-color: #ffffff;"
        "}"
        "QLineEdit:focus {"
        "    border: 1px solid #3498db;"
        "}");
    
    clearButton = new QPushButton("清除", this);
    clearButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #ecf0f1;"
        "    border: 1px solid #bdc3c7;"
        "    border-radius: 4px;"
        "    padding: 6px;"
        "    color: #7f8c8d;"
        "}"
        "QPushButton:hover {"
        "    background-color: #e0e0e0;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #d0d0d0;"
        "}");
    clearButton->setVisible(false); // 初始隐藏

    viewModeCombo = new QComboBox(this);
    viewModeCombo->addItem("列表");
    viewModeCombo->addItem("图标");
    viewModeCombo->addItem("详情");
    viewModeCombo->setStyleSheet(
        "QComboBox {"
        "    border: 1px solid #bdc3c7;"
        "    border-radius: 4px;"
        "    padding: 4px 8px;"
        "    background-color: #ffffff;"
        "    min-width: 90px;"
        "}"
        "QComboBox::drop-down { border: none; }"
        "QComboBox QAbstractItemView {"
        "    border: 1px solid #bdc3c7;"
        "    background-color: #ffffff;"
        "    selection-background-color: #e3f2fd;"
        "    selection-color: #1976d2;"
        "    padding: 4px;"
        "    outline: none;"
        "}");
    
    searchLayout->addWidget(searchBox);
    searchLayout->addWidget(clearButton);
    searchLayout->addWidget(viewModeCombo);
    
    // 连接搜索框信号
    connect(searchBox, &QLineEdit::textChanged, this, [this]() {
        searchTimer->start(); // 启动延迟搜索
    });
    connect(clearButton, &QPushButton::clicked, this, &FunctionArea::clearSearch);
    connect(viewModeCombo, &QComboBox::currentTextChanged, this, &FunctionArea::onViewModeChanged);
    
    // 创建快捷方式列表
    shortcuts = new QListWidget(this);
    
    // 加载视图模式
    loadViewMode();
    
    shortcuts->setStyleSheet(
        "QListWidget {"
        "    background-color: transparent;"
        "    border: none;"
        "    outline: none;"
        "    padding: 10px;"
        "}"
        "QListWidget::item {"
        "    background-color: #ffffff;"
        "    border: 1px solid #e9ecef;"
        "    border-radius: 8px;"
        "    margin-bottom: 4px;"
        "    padding: 6px 12px;"
        "    color: #212529;"
        "}"
        "QListWidget::item:hover {"
        "    background-color: #f8f9fa;"
        "    border-color: #4dabf7;"
        "}"
        "QListWidget::item:selected {"
        "    background-color: #e7f5ff;"
        "    border-color: #339af0;"
        "    color: #1c7ed6;"
        "}");
    
    // 设置列表项布局
    shortcuts->setIconSize(QSize(24, 24));
    shortcuts->setTextElideMode(Qt::ElideNone);
    shortcuts->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    shortcuts->setSpacing(2);
    shortcuts->setViewMode(QListView::ListMode);
    shortcuts->setFlow(QListView::TopToBottom);
    shortcuts->setMovement(QListView::Static);
    shortcuts->setResizeMode(QListView::Adjust);
    shortcuts->setWordWrap(true);
    
    // 启用拖放功能
    shortcuts->setDragEnabled(true);
    shortcuts->setAcceptDrops(true);
    shortcuts->setDropIndicatorShown(true);
    shortcuts->setDragDropMode(QAbstractItemView::InternalMove);
    
    // 连接拖放完成信号
    connect(shortcuts->model(), &QAbstractItemModel::rowsMoved, this, &FunctionArea::onShortcutsReordered);
    
    // 连接双击信号到槽函数
    connect(shortcuts, &QListWidget::itemDoubleClicked, this, &FunctionArea::onShortcutDoubleClicked);
    
    // 设置右键菜单
    shortcuts->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(shortcuts, &QListWidget::customContextMenuRequested, this, &FunctionArea::onContextMenuRequested);
    loadViewMode();
    
    // 创建提示标签
    QLabel *hintLabel = new QLabel("双击项目启动工具", this);
    hintLabel->setAlignment(Qt::AlignCenter);
    hintLabel->setStyleSheet("color: #757575; padding: 5px; font-size: 9pt;");
    
    // 创建布局
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(titleLabel);
    layout->addLayout(searchLayout);
    layout->addWidget(shortcuts);
    layout->addWidget(hintLabel);
    setLayout(layout);
}

QListWidgetItem* FunctionArea::getSelectedItem() const {
    QList<QListWidgetItem*> selectedItems = shortcuts->selectedItems();
    if (selectedItems.isEmpty()) {
        return nullptr;
    }
    return selectedItems.first();
}

QString FunctionArea::currentViewMode() const {
    if (!viewModeCombo) {
        return "列表";
    }
    return viewModeCombo->currentText();
}

void FunctionArea::loadViewMode() {
    QString mode = "列表";
    QFile file("tools/view_mode.cfg");
    if (file.exists() && file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString stored = QTextStream(&file).readAll().trimmed();
        file.close();
        if (stored == "列表" || stored == "图标" || stored == "详情") {
            mode = stored;
        }
    }
    viewModeCombo->setCurrentText(mode);
    applyViewMode(mode);
}

void FunctionArea::saveViewMode(const QString &mode) const {
    QDir toolsDir("tools");
    if (!toolsDir.exists()) {
        toolsDir.mkpath(".");
    }
    QFile file("tools/view_mode.cfg");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << mode;
        file.close();
    }
}

void FunctionArea::applyViewMode(const QString &mode) {
    if (mode == "图标") {
        shortcuts->setViewMode(QListView::IconMode);
        shortcuts->setFlow(QListView::LeftToRight);
        shortcuts->setWrapping(true);
        shortcuts->setResizeMode(QListView::Adjust);
        shortcuts->setGridSize(QSize(180, 110));
        shortcuts->setIconSize(QSize(40, 40));
        shortcuts->setWordWrap(true);
        refreshItemsForViewMode();
        return;
    }
    shortcuts->setViewMode(QListView::ListMode);
    shortcuts->setFlow(QListView::TopToBottom);
    shortcuts->setWrapping(false);
    shortcuts->setResizeMode(QListView::Fixed);
    shortcuts->setGridSize(QSize());
    shortcuts->setIconSize(QSize(24, 24));
    shortcuts->setWordWrap(true);
    refreshItemsForViewMode();
}

void FunctionArea::onViewModeChanged(const QString &mode) {
    applyViewMode(mode);
    saveViewMode(mode);
}

void FunctionArea::refreshItemsForViewMode() {
    QString mode = currentViewMode();
    int detailWidth = qMax(320, shortcuts->viewport()->width() - 20);
    auto applyForItem = [mode, detailWidth](QListWidgetItem *item) {
        QString displayName = item->data(Qt::UserRole + 2).toString();
        if (displayName.isEmpty()) {
            displayName = item->text();
            item->setData(Qt::UserRole + 2, displayName);
        }
        if (mode == "图标") {
            item->setSizeHint(QSize(170, 96));
        } else if (mode == "详情") {
            item->setSizeHint(QSize(detailWidth, 68));
        } else {
            item->setSizeHint(QSize(detailWidth, 42));
        }
        if (mode == "详情") {
            QString description = item->data(Qt::UserRole + 1).toString();
            QString filePath = item->data(Qt::UserRole).toString();
            QString detail = description.isEmpty() ? filePath : description;
            item->setText(displayName + "\n" + detail);
        } else {
            item->setText(displayName);
        }
    };
    for (int i = 0; i < shortcuts->count(); i++) {
        applyForItem(shortcuts->item(i));
    }
    for (int i = 0; i < allItems.size(); i++) {
        applyForItem(allItems[i]);
    }
}

// 加载支持的文件后缀
void FunctionArea::loadFileExtensions() {
    // 默认支持的文件后缀
    supportedExtensions = QStringList() << "exe" << "cmd";
    
    // 从配置文件加载自定义后缀
    QFile file("tools/extensions.cfg");
    if (file.exists() && file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString content = in.readAll().trimmed();
        file.close();
        
        if (!content.isEmpty()) {
            supportedExtensions = content.split(";", Qt::SkipEmptyParts);
        }
    }
}

// 保存支持的文件后缀
void FunctionArea::saveFileExtensions(const QStringList &extensions) {
    supportedExtensions = extensions;
    
    // 保存到配置文件
    QFile file("tools/extensions.cfg");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << extensions.join(";");
        file.close();
    }
}

// 获取文件过滤器
QStringList FunctionArea::getFileFilters() const {
    QStringList filters;
    for (const QString &ext : supportedExtensions) {
        filters << "*." + ext;
    }
    return filters;
}

// 析构函数，清理资源
FunctionArea::~FunctionArea() {
    // 停止定时器
    cacheCleanupTimer->stop();
    activeProcesses.clear();
    
    // 清理 allItems 中的项目
    qDeleteAll(allItems);
    allItems.clear();
}

// 检查缓存是否有效
bool FunctionArea::isCacheValid(const QString &folderName) const {
    auto it = folderCache.constFind(folderName);
    if (it == folderCache.constEnd()) {
        return false;
    }
    
    QFileInfo dirInfo("tools/" + folderName);
    QDateTime lastModified = dirInfo.lastModified();
    
    // 如果文件夹修改时间比缓存新，缓存无效
    return lastModified <= it.value().dirLastModified;
}

// 更新缓存
void FunctionArea::updateCache(const QString &folderName, const QList<ShortcutEntry> &files) {
    FileCache cache;
    cache.dirLastModified = QFileInfo("tools/" + folderName).lastModified();
    cache.lastAccessedAt = QDateTime::currentDateTime();
    cache.files = files;
    folderCache[folderName] = cache;
}

// 清理过期缓存
void FunctionArea::cleanupCache() {
    // 清理超过1小时未使用的缓存
    QDateTime now = QDateTime::currentDateTime();
    QList<QString> keysToRemove;
    
    for (auto it = folderCache.begin(); it != folderCache.end(); ++it) {
        if (it.value().lastAccessedAt.secsTo(now) > 3600) { // 1小时 = 3600秒
            keysToRemove.append(it.key());
        }
    }
    
    for (const QString &key : keysToRemove) {
        folderCache.remove(key);
    }
}

// 管理进程
void FunctionArea::manageProcesses() {
    // 清理已结束的进程
    cleanupProcesses();
    
    // 如果活动进程数超过最大值，终止最早启动的进程
    while (activeProcesses.size() >= maxProcesses) {
        if (!activeProcesses.isEmpty()) {
            auto oldestProcess = activeProcesses.takeFirst();
            if (oldestProcess && oldestProcess->state() == QProcess::Running) {
                oldestProcess->terminate();
            }
        }
    }
}

// 清理已结束的进程
void FunctionArea::cleanupProcesses() {
    QList<QSharedPointer<QProcess>> runningProcesses;
    
    for (auto &process : activeProcesses) {
        if (process && process->state() == QProcess::Running) {
            runningProcesses.append(process);
        }
    }
    
    activeProcesses = runningProcesses;
}

// 启动进程
bool FunctionArea::startProcess(const QString &exeFilePath, const QStringList &args) {
    QFileInfo fileInfo(exeFilePath);
    bool started = QProcess::startDetached(exeFilePath, args, fileInfo.absolutePath());
    if (started && statusBar) {
        statusBar->showMessage(QString("已启动工具: %1").arg(fileInfo.fileName()), 3000);
    }
    return started;
}

// 创建单个快捷方式项
void FunctionArea::createShortcutItem(const ShortcutEntry &entry, int count) {
    // 创建列表项
    QListWidgetItem *item = new QListWidgetItem(shortcuts);
    
    item->setText(entry.displayName);
    item->setData(Qt::UserRole + 2, entry.displayName);
    
    // 设置工具提示 - 如果有描述则显示描述，否则显示文件路径
    if (!entry.description.isEmpty()) {
        item->setToolTip(entry.description);
        // 存储描述信息
        item->setData(Qt::UserRole + 1, entry.description);
    } else {
        item->setToolTip(entry.filePath);
    }
    
    // 设置图标
    item->setIcon(fileIconProvider.icon(QFileInfo(entry.filePath)));
    
    // 存储文件路径
    item->setData(Qt::UserRole, entry.filePath);
    item->setData(Qt::UserRole + 3, entry.arguments);
    item->setData(Qt::UserRole + 4, entry.runAsAdmin);
    item->setData(Qt::UserRole + 5, entry.isFavorite);
    
    // 设置交替背景色
    if (count % 2 == 0) {
        item->setBackground(QColor(252, 252, 252));
    } else {
        item->setBackground(QColor(248, 248, 248));
    }
}

// 应用自定义排序
QList<FunctionArea::ShortcutEntry> FunctionArea::applyCustomOrder(const QString &folderName, QList<ShortcutEntry> files) {
    // 检查是否有保存的顺序文件
    QString orderFilePath = "tools/" + folderName + "/order/shortcuts.order";
    if (!QFile::exists(orderFilePath)) {
        return files; // 没有自定义顺序，直接返回原列表
    }
    
    QFile orderFile(orderFilePath);
    if (!orderFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return files; // 无法打开文件，直接返回原列表
    }
    
    QTextStream in(&orderFile);
    QStringList orderedPaths;
    
    // 读取保存的顺序
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty()) {
            orderedPaths.append(line);
        }
    }
    orderFile.close();
    
    // 如果有保存的顺序，按照顺序重新排列文件列表
    if (orderedPaths.isEmpty()) {
        return files; // 没有有效的顺序信息，直接返回原列表
    }
    
    QList<ShortcutEntry> newOrderedFiles;
    
    // 首先添加有序的文件
    foreach (const QString &path, orderedPaths) {
        for (int i = 0; i < files.size(); i++) {
            if (files[i].filePath == path) {
                newOrderedFiles.append(files[i]);
                files.removeAt(i);
                break;
            }
        }
    }
    
    // 然后添加剩余的文件（新添加的或未排序的）
    newOrderedFiles.append(files);
    return newOrderedFiles;
}

// 加载缓存的快捷方式
bool FunctionArea::loadCachedShortcuts(const QString &folderName) {
    if (!isCacheValid(folderName)) {
        return false; // 缓存无效
    }
    
    auto it = folderCache.find(folderName);
    if (it == folderCache.end()) {
        return false;
    }
    it.value().lastAccessedAt = QDateTime::currentDateTime();
    QList<ShortcutEntry> orderedFiles = it.value().files;
    
    // 应用自定义排序
    orderedFiles = applyCustomOrder(folderName, orderedFiles);
    
    // 显示快捷方式
    int count = 0;
    for (const auto &entry : orderedFiles) {
        createShortcutItem(entry, count++);
    }
    
    // 应用当前视图模式的样式
    refreshItemsForViewMode();
    
    return true;
}

// 执行搜索操作
void FunctionArea::performSearch() {
    QString text = searchBox->text();
    onSearchTextChanged(text);
}

QJsonObject FunctionArea::loadMetadata(const QString &folderName) {
    QFile file("tools/" + folderName + "/metadata.json");
    if (file.open(QIODevice::ReadOnly)) {
        return QJsonDocument::fromJson(file.readAll()).object();
    }
    return QJsonObject();
}

void FunctionArea::saveMetadata(const QString &folderName, const QJsonObject &metadata) {
    QDir dir("tools/" + folderName);
    if (!dir.exists()) dir.mkpath(".");
    
    QFile file("tools/" + folderName + "/metadata.json");
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(metadata).toJson());
    }
}

void FunctionArea::updateShortcutMetadata(const QString &folderName, const QString &fileName, const QJsonObject &data) {
    QJsonObject metadata = loadMetadata(folderName);
    QJsonObject shortcuts = metadata["shortcuts"].toObject();
    
    QJsonObject shortcut = shortcuts[fileName].toObject();
    for (auto it = data.begin(); it != data.end(); ++it) {
        shortcut[it.key()] = it.value();
    }
    
    shortcuts[fileName] = shortcut;
    metadata["shortcuts"] = shortcuts;
    saveMetadata(folderName, metadata);
}

// 处理搜索文本变化
void FunctionArea::onSearchTextChanged(const QString &text) {
    // 如果搜索文本为空，显示所有项目
    if (text.isEmpty()) {
        clearSearch();
        return;
    }
    
    // 显示清除按钮
    clearButton->setVisible(true);
    
    // 清空当前列表
    shortcuts->clear();
    
    // 搜索匹配项并添加到列表
    for (int i = 0; i < allItems.size(); i++) {
        QListWidgetItem* item = allItems[i];
        QString itemText = item->text();
        QString itemToolTip = item->toolTip();
        
        // 使用Qt::CaseInsensitive标志进行不区分大小写的比较
        if (itemText.contains(text, Qt::CaseInsensitive) || 
            itemToolTip.contains(text, Qt::CaseInsensitive)) {
            // 使用克隆方式创建新项目
            QListWidgetItem* newItem = item->clone();
            shortcuts->addItem(newItem);
        }
    }
    
    // 如果没有匹配项，显示提示
    if (shortcuts->count() == 0) {
        QListWidgetItem* noResultItem = new QListWidgetItem("没有找到匹配的工具");
        noResultItem->setFlags(noResultItem->flags() & ~Qt::ItemIsEnabled);
        noResultItem->setTextAlignment(Qt::AlignCenter);
        shortcuts->addItem(noResultItem);
    }
}

// 清除搜索
void FunctionArea::clearSearch() {
    // 清空搜索框
    searchBox->clear();
    
    // 隐藏清除按钮
    clearButton->setVisible(false);
    
    // 清空当前列表
    shortcuts->clear();
    
    // 恢复所有项目（使用克隆方式创建新项目）
    for (int i = 0; i < allItems.size(); i++) {
        QListWidgetItem* item = allItems[i];
        QListWidgetItem* newItem = item->clone();
        shortcuts->addItem(newItem);
    }
}

// 扫描文件夹获取快捷方式
void FunctionArea::scanFolderForShortcuts(const QString &folderName) {
    if (watcher->isRunning()) {
        watcher->cancel();
    }
    
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (statusBar) statusBar->showMessage("正在扫描工具...", 0);
    
    QFuture<QList<ShortcutEntry>> future = QtConcurrent::run(&FunctionArea::scanFolderWorker, this, folderName, getFileFilters());
    watcher->setFuture(future);
}

QList<FunctionArea::ShortcutEntry> FunctionArea::scanFolderWorker(const QString &folderName, const QStringList &filters) {
    QDir toolsDir("tools/" + folderName);
    QJsonObject metadata = loadMetadata(folderName);
    QJsonObject shortcutsJson = metadata["shortcuts"].toObject();

    QList<ShortcutEntry> scannedFiles;
    
    // 首先扫描所有文件夹中的main.cfg文件
    QMap<QString, QStringList> mainConfigFiles; 
    QSet<QString> foldersWithMainCfg; 
    QDirIterator configIterator(toolsDir.path(), QStringList() << "main.cfg", QDir::Files, QDirIterator::Subdirectories);
    
    while (configIterator.hasNext()) {
        configIterator.next();
        QFileInfo configFileInfo = configIterator.fileInfo();
        QString dirPath = configFileInfo.absolutePath();
        foldersWithMainCfg.insert(dirPath);
        
        QFile configFile(configFileInfo.absoluteFilePath());
        if (configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&configFile);
            QStringList exePaths;
            while (!in.atEnd()) {
                QString line = in.readLine().trimmed();
                if (!line.isEmpty()) {
                    if (QFileInfo(line).isRelative()) {
                        line = dirPath + "/" + line;
                    }
                    exePaths.append(line);
                }
            }
            configFile.close();
            if (!exePaths.isEmpty()) {
                mainConfigFiles[dirPath] = exePaths;
            }
        }
    }
    
    QDirIterator dirIterator(toolsDir.path(), filters, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
    
    while (dirIterator.hasNext()) {
        dirIterator.next();
        QFileInfo fileInfo = dirIterator.fileInfo();
        QString dirPath = fileInfo.absolutePath();
        
        bool skipDueToParentFolder = false;
        foreach (const QString &folderWithCfg, foldersWithMainCfg) {
            if (dirPath != folderWithCfg && dirPath.startsWith(folderWithCfg)) {
                skipDueToParentFolder = true;
                break;
            }
        }
        
        if (skipDueToParentFolder) continue;
        
        if (fileInfo.isFile() && filters.contains("*." + fileInfo.suffix().toLower())) {
            if (mainConfigFiles.contains(dirPath)) {
                if (!mainConfigFiles[dirPath].contains(fileInfo.absoluteFilePath())) {
                    continue; 
                }
            }
            
            QString fileName = fileInfo.fileName();
            QJsonObject itemMeta = shortcutsJson[fileName].toObject();

            ShortcutEntry entry;
            entry.filePath = fileInfo.absoluteFilePath();
            entry.displayName = itemMeta.contains("displayName") ? itemMeta["displayName"].toString() : fileInfo.completeBaseName();
            entry.description = itemMeta.contains("description") ? itemMeta["description"].toString() : "";
            entry.arguments = itemMeta.contains("arguments") ? itemMeta["arguments"].toString() : "";
            entry.runAsAdmin = itemMeta.contains("runAsAdmin") ? itemMeta["runAsAdmin"].toBool() : false;
            entry.isFavorite = itemMeta.contains("isFavorite") ? itemMeta["isFavorite"].toBool() : false;
            
            scannedFiles.append(entry);
        }
    }
    
    return scannedFiles;
}

void FunctionArea::onScanningFinished(const QString &folderName, const QList<ShortcutEntry> &files) {
    QApplication::restoreOverrideCursor();
    if (statusBar) statusBar->clearMessage();
    
    // 如果返回的文件夹名称与当前正在查看的不一致，说明用户切换了文件夹，直接返回
    if (folderName != currentFolderName) {
        return;
    }
    
    // 先清空列表，防止重复
    shortcuts->clear();
    
    // 应用自定义排序
    QList<ShortcutEntry> orderedFiles = applyCustomOrder(folderName, files);
    
    // 显示快捷方式
    int count = 0;
    for (const auto &entry : orderedFiles) {
        createShortcutItem(entry, count++);
    }
    
    // 更新视图样式（应用高度和间距）
    refreshItemsForViewMode();
    
    // 更新缓存
    updateCache(folderName, files);
    
    // 成功加载后，保存所有项目用于搜索
    qDeleteAll(allItems);
    allItems.clear();
    for (int i = 0; i < shortcuts->count(); i++) {
        QListWidgetItem* originalItem = shortcuts->item(i);
        QListWidgetItem* clonedItem = originalItem->clone();
        allItems.append(clonedItem);
    }
}

void FunctionArea::showShortcuts(const QString &folderName) {
    // 保存当前文件夹名称
    currentFolderName = folderName;
    
    // 如果有正在运行的扫描，先停止
    if (watcher->isRunning()) {
        watcher->cancel();
    }

    // 清空当前快捷方式列表和存储的所有项目
    shortcuts->clear();
    
    // 清空搜索框
    searchBox->clear();
    clearButton->setVisible(false);
    
    // 尝试加载缓存的快捷方式
    if (loadCachedShortcuts(folderName)) {
        // 成功加载缓存后，保存所有项目用于搜索（使用深拷贝）
        qDeleteAll(allItems);
        allItems.clear();
        for (int i = 0; i < shortcuts->count(); i++) {
            QListWidgetItem* originalItem = shortcuts->item(i);
            QListWidgetItem* clonedItem = originalItem->clone();
            allItems.append(clonedItem);
        }
        return; // 成功加载缓存，直接返回
    }
    
    // 缓存无效，需要重新扫描
    scanFolderForShortcuts(folderName);
}

void FunctionArea::onContextMenuRequested(const QPoint &pos) {
    QListWidgetItem *item = shortcuts->itemAt(pos);
    
    // 创建右键菜单
    QMenu menu(this);
    
    if (item) {
        // 选中当前项
        shortcuts->setCurrentItem(item);
        
        // 添加重命名选项
        QAction *renameAction = menu.addAction("重命名工具");
        connect(renameAction, &QAction::triggered, this, &FunctionArea::onRenameShortcut);
        
        // 添加编辑描述选项
        QAction *descriptionAction = menu.addAction("编辑描述");
        connect(descriptionAction, &QAction::triggered, this, &FunctionArea::onEditDescription);
        
        // 添加启动参数选项
        QAction *argsAction = menu.addAction("启动参数");
        connect(argsAction, &QAction::triggered, this, &FunctionArea::onEditArguments);
        
        // 添加管理员权限选项
        bool isAdmin = item->data(Qt::UserRole + 4).toBool();
        QAction *adminAction = menu.addAction(isAdmin ? "取消管理员权限" : "以管理员权限运行");
        connect(adminAction, &QAction::triggered, this, &FunctionArea::onToggleAdmin);
        
        menu.addSeparator();
        
        QAction *openInExplorerAction = menu.addAction("在资源管理器中打开");
        connect(openInExplorerAction, &QAction::triggered, this, &FunctionArea::onOpenShortcutInExplorer);
        
        menu.addSeparator();
        
        // 添加删除选项
        QAction *deleteAction = menu.addAction("删除工具");
        deleteAction->setIcon(QIcon::fromTheme("edit-delete", QIcon(":/icons/delete.png")));
        connect(deleteAction, &QAction::triggered, this, &FunctionArea::onDeleteTool);
        
        menu.addSeparator();
    }
    
    // 添加工具选项
    QAction *addToolAction = menu.addAction("添加工具");
    addToolAction->setIcon(QIcon::fromTheme("list-add", QIcon(":/icons/add.png")));
    connect(addToolAction, &QAction::triggered, this, &FunctionArea::onAddTool);
    
    // 添加管理文件后缀选项
    QAction *manageExtensionsAction = menu.addAction("管理支持的文件类型");
    connect(manageExtensionsAction, &QAction::triggered, this, &FunctionArea::onManageFileExtensions);
    
    // 显示菜单
    menu.exec(shortcuts->mapToGlobal(pos));
}

void FunctionArea::onEditArguments() {
    QListWidgetItem *item = getSelectedItem();
    if (!item) return;
    
    QString filePath = item->data(Qt::UserRole).toString();
    QFileInfo fileInfo(filePath);
    QString currentArgs = item->data(Qt::UserRole + 3).toString();
    
    // 创建自定义对话框
    QDialog dialog(this);
    dialog.setWindowTitle("编辑启动参数");
    
    // 创建输入框和标签
    QLineEdit *lineEdit = new QLineEdit(&dialog);
    lineEdit->setText(currentArgs);
    lineEdit->setPlaceholderText("例如: -port 8080 --debug");
    QLabel *label = new QLabel("请输入启动参数 (用空格分隔):", &dialog);
    
    // 创建按钮
    QPushButton *saveButton = new QPushButton("保存", &dialog);
    QPushButton *cancelButton = new QPushButton("取消", &dialog);
    
    // 设置按钮角色
    saveButton->setDefault(true);
    dialog.connect(saveButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    dialog.connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);
    
    // 创建布局
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(label);
    mainLayout->addWidget(lineEdit);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    dialog.setLayout(mainLayout);
    
    // 显示对话框
    if (dialog.exec() == QDialog::Accepted) {
        QString newArgs = lineEdit->text();
        QJsonObject data;
        data["arguments"] = newArgs;
        updateShortcutMetadata(currentFolderName, fileInfo.fileName(), data);
        item->setData(Qt::UserRole + 3, newArgs);
    }
}

void FunctionArea::onToggleAdmin() {
    QListWidgetItem *item = getSelectedItem();
    if (!item) return;
    
    QString filePath = item->data(Qt::UserRole).toString();
    QFileInfo fileInfo(filePath);
    bool currentAdmin = item->data(Qt::UserRole + 4).toBool();
    bool newAdmin = !currentAdmin;
    
    QJsonObject data;
    data["runAsAdmin"] = newAdmin;
    updateShortcutMetadata(currentFolderName, fileInfo.fileName(), data);
    item->setData(Qt::UserRole + 4, newAdmin);
    
    // 如果是以管理员身份运行，可以在名称后面加个小标记或者改下颜色
    refreshItemsForViewMode();
}

void FunctionArea::onAddTool() {
    if (currentFolderName.isEmpty()) {
        QMessageBox::warning(this, "添加工具", "请先选择一个分类。");
        return;
    }
    
    QString filePath = QFileDialog::getOpenFileName(this, "选择要添加的工具", "", 
                                                   "可执行文件 (*.exe *.cmd *.bat);;所有文件 (*.*)");
    if (filePath.isEmpty()) return;
    
    QFileInfo fileInfo(filePath);
    QString destDir = "tools/" + currentFolderName;
    QString destPath = destDir + "/" + fileInfo.fileName();
    
    // 如果文件已存在，提示用户
    if (QFile::exists(destPath)) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("添加工具");
        msgBox.setText("工具已存在，是否覆盖？");
        msgBox.setIcon(QMessageBox::Question);
        QPushButton *yesButton = msgBox.addButton("是", QMessageBox::YesRole);
        QPushButton *noButton = msgBox.addButton("否", QMessageBox::NoRole);
        msgBox.setDefaultButton(noButton);
        
        msgBox.exec();
        if (msgBox.clickedButton() == noButton) return;
        QFile::remove(destPath);
    }
    
    // 复制文件
    if (QFile::copy(filePath, destPath)) {
        // 刷新列表
        scanFolderForShortcuts(currentFolderName);
    } else {
        QMessageBox::critical(this, "添加工具", "无法复制文件到工具目录。");
    }
}

void FunctionArea::onDeleteTool() {
    QListWidgetItem *item = getSelectedItem();
    if (!item) return;
    
    QString filePath = item->data(Qt::UserRole).toString();
    QFileInfo fileInfo(filePath);
    
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("删除工具");
    msgBox.setText(QString("确定从工具箱中删除 '%1' 吗？\n注意：这会从磁盘上永久删除该文件。").arg(item->text()));
    msgBox.setIcon(QMessageBox::Warning);
    QPushButton *deleteButton = msgBox.addButton("删除", QMessageBox::AcceptRole);
    QPushButton *cancelButton = msgBox.addButton("取消", QMessageBox::RejectRole);
    msgBox.setDefaultButton(cancelButton);
    
    msgBox.exec();
    
    if (msgBox.clickedButton() == deleteButton) {
        if (QFile::remove(filePath)) {
            // 从元数据中移除
            QJsonObject metadata = loadMetadata(currentFolderName);
            QJsonObject shortcutsJson = metadata["shortcuts"].toObject();
            shortcutsJson.remove(fileInfo.fileName());
            metadata["shortcuts"] = shortcutsJson;
            saveMetadata(currentFolderName, metadata);
            
            // 刷新列表
            scanFolderForShortcuts(currentFolderName);
        } else {
            QMessageBox::critical(this, "删除工具", "无法删除文件，请检查文件是否正在运行。");
        }
    }
}

void FunctionArea::onOpenShortcutInExplorer() {
    QListWidgetItem *item = getSelectedItem();
    if (!item) {
        return;
    }

    QString filePath = QDir::toNativeSeparators(item->data(Qt::UserRole).toString());
    bool opened = QProcess::startDetached("explorer.exe", QStringList() << "/select," << filePath);
    if (!opened) {
        QMessageBox::warning(this, "打开工具位置", "无法打开资源管理器。");
    }
}

void FunctionArea::onRenameShortcut() {
    QListWidgetItem *item = getSelectedItem();
    if (!item) {
        return;
    }
    
    // 获取当前名称和文件路径
    QString currentName = item->text();
    QString filePath = item->data(Qt::UserRole).toString();
    QFileInfo fileInfo(filePath);
    
    // 创建自定义对话框以便修改按钮文本
    QDialog dialog(this);
    dialog.setWindowTitle("重命名工具");
    
    // 创建输入框和标签
    QLineEdit *lineEdit = new QLineEdit(&dialog);
    lineEdit->setText(currentName);
    QLabel *label = new QLabel("请输入新的工具名称:", &dialog);
    
    // 创建按钮
    QPushButton *saveButton = new QPushButton("保存", &dialog);
    QPushButton *cancelButton = new QPushButton("取消", &dialog);
    
    // 设置按钮角色
    saveButton->setDefault(true);
    dialog.connect(saveButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    dialog.connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);
    
    // 创建布局
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(label);
    mainLayout->addWidget(lineEdit);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    dialog.setLayout(mainLayout);
    
    // 显示对话框
    bool ok = (dialog.exec() == QDialog::Accepted);
    QString newName = lineEdit->text();
    
    if (ok && !newName.isEmpty() && newName != currentName) {
        // 更新元数据
        QJsonObject data;
        data["displayName"] = newName;
        updateShortcutMetadata(currentFolderName, fileInfo.fileName(), data);
        
        // 更新列表项
        item->setData(Qt::UserRole + 2, newName);
        refreshItemsForViewMode();
    }
}

void FunctionArea::onEditDescription() {
    QListWidgetItem *item = getSelectedItem();
    if (!item) {
        return;
    }
    
    // 获取文件路径
    QString filePath = item->data(Qt::UserRole).toString();
    QFileInfo fileInfo(filePath);
    
    // 获取当前描述
    QString currentDescription = item->data(Qt::UserRole + 1).toString();
    
    // 创建自定义对话框
    QDialog dialog(this);
    dialog.setWindowTitle("编辑工具描述");
    dialog.setMinimumWidth(400);
    
    // 创建输入框和标签
    QTextEdit *textEdit = new QTextEdit(&dialog);
    textEdit->setText(currentDescription);
    textEdit->setPlaceholderText("请输入工具描述...");
    QLabel *label = new QLabel("请输入工具描述:", &dialog);
    
    // 创建按钮
    QPushButton *saveButton = new QPushButton("保存", &dialog);
    QPushButton *cancelButton = new QPushButton("取消", &dialog);
    
    // 设置按钮角色
    saveButton->setDefault(true);
    dialog.connect(saveButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    dialog.connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);
    
    // 创建布局
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(label);
    mainLayout->addWidget(textEdit);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    dialog.setLayout(mainLayout);
    
    // 显示对话框
    bool ok = (dialog.exec() == QDialog::Accepted);
    QString newDescription = textEdit->toPlainText().trimmed();
    
    if (ok) {
        // 更新元数据
        QJsonObject data;
        data["description"] = newDescription;
        updateShortcutMetadata(currentFolderName, fileInfo.fileName(), data);
        
        // 更新列表项
        item->setToolTip(newDescription.isEmpty() ? filePath : newDescription);
        item->setData(Qt::UserRole + 1, newDescription);
        refreshItemsForViewMode();
    }
}

void FunctionArea::onManageFileExtensions() {
    // 创建自定义对话框以便修改按钮文本
    QDialog dialog(this);
    dialog.setWindowTitle("管理支持的文件类型");
    
    // 创建输入框和标签
    QLineEdit *lineEdit = new QLineEdit(&dialog);
    lineEdit->setText(supportedExtensions.join(";"));
    QLabel *label = new QLabel("请输入支持的文件后缀，用分号(;)分隔：", &dialog);
    
    // 创建按钮
    QPushButton *saveButton = new QPushButton("保存", &dialog);
    QPushButton *cancelButton = new QPushButton("取消", &dialog);
    
    // 设置按钮角色
    saveButton->setDefault(true);
    dialog.connect(saveButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    dialog.connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);
    
    // 创建布局
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(label);
    mainLayout->addWidget(lineEdit);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    dialog.setLayout(mainLayout);
    
    // 显示对话框
    bool ok = (dialog.exec() == QDialog::Accepted);
    QString extensionsStr = lineEdit->text();
    
    if (ok && !extensionsStr.isEmpty()) {
        // 解析输入的后缀
        QStringList newExtensions = extensionsStr.split(";", Qt::SkipEmptyParts);
        
        // 移除空白字符并转为小写
        for (int i = 0; i < newExtensions.size(); ++i) {
            newExtensions[i] = newExtensions[i].trimmed().toLower();
        }
        
        // 保存新的后缀列表
        saveFileExtensions(newExtensions);
        
        // 清除所有文件夹的缓存，确保下次访问时重新扫描
        folderCache.clear();
        
        // 刷新当前显示的快捷方式
        if (!currentFolderName.isEmpty()) {
            scanFolderForShortcuts(currentFolderName);
            if (statusBar) {
                statusBar->showMessage("文件类型已更新并刷新显示", 3000);
            }
        } else {
            if (statusBar) {
                statusBar->showMessage("文件类型已更新，请选择一个文件夹以查看更改", 3000);
            }
        }
    }
}

// 处理工具列表重新排序
void FunctionArea::onShortcutsReordered() {
    // 如果没有选中文件夹，则不处理
    if (currentFolderName.isEmpty()) {
        return;
    }
    
    // 创建顺序配置文件目录
    QString orderDir = "tools/" + currentFolderName + "/order";
    QDir dir;
    if (!dir.exists(orderDir)) {
        dir.mkpath(orderDir);
    }
    
    // 保存当前顺序到配置文件
    QString orderFilePath = orderDir + "/shortcuts.order";
    QFile orderFile(orderFilePath);
    
    QList<ShortcutEntry> orderedEntries;
    if (orderFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&orderFile);
        
        for (int i = 0; i < shortcuts->count(); i++) {
            QListWidgetItem *item = shortcuts->item(i);
            QString filePath = item->data(Qt::UserRole).toString();
            if (filePath.isEmpty()) {
                continue;
            }
            out << filePath << "\n";
            ShortcutEntry entry;
            entry.filePath = filePath;
            entry.displayName = item->data(Qt::UserRole + 2).toString();
            if (entry.displayName.isEmpty()) {
                entry.displayName = item->text();
            }
            entry.description = item->data(Qt::UserRole + 1).toString();
            orderedEntries.append(entry);
        }
        
        orderFile.close();
        if (!orderedEntries.isEmpty()) {
            updateCache(currentFolderName, orderedEntries);
            qDeleteAll(allItems);
            allItems.clear();
            for (int i = 0; i < shortcuts->count(); i++) {
                QListWidgetItem *item = shortcuts->item(i);
                if (item->data(Qt::UserRole).toString().isEmpty()) {
                    continue;
                }
                allItems.append(item->clone());
            }
        }
        qDebug() << "工具顺序已保存到" << orderFilePath;
    } else {
        qDebug() << "无法保存工具顺序到" << orderFilePath;
    }
}

void FunctionArea::onShortcutDoubleClicked(QListWidgetItem *item) {
    if (!item) {
        return;
    }

    QString exeFilePath = item->data(Qt::UserRole).toString();

    // 检查文件是否存在
    if (!QFile::exists(exeFilePath)) {
        QMessageBox::warning(this, "启动工具", "文件不存在: " + exeFilePath);
        return;
    }
    
    // 获取文件信息
    QFileInfo fileInfo(exeFilePath);
    
    // 从数据角色获取启动参数
    QString argumentsStr = item->data(Qt::UserRole + 3).toString();
    QStringList args = QProcess::splitCommand(argumentsStr);
    
    // 是否以管理员权限运行
    bool runAsAdmin = item->data(Qt::UserRole + 4).toBool();
    
    if (runAsAdmin) {
        // 在 Windows 上使用 ShellExecuteEx 以管理员身份启动
        std::wstring wExePath = exeFilePath.toStdWString();
        std::wstring wArgs = argumentsStr.toStdWString();
        
        SHELLEXECUTEINFOW sei = { sizeof(sei) };
        sei.lpVerb = L"runas";
        sei.lpFile = wExePath.c_str();
        sei.lpParameters = wArgs.c_str();
        sei.nShow = SW_SHOWNORMAL;
        
        if (!ShellExecuteExW(&sei)) {
            QMessageBox::critical(this, "启动工具", "无法以管理员身份启动工具。");
            return;
        }
    } else {
        // 使用现有进程管理逻辑启动
        if (!startProcess(exeFilePath, args)) {
            QMessageBox::critical(this, "启动工具", 
                QString("无法启动工具: %1\n\n请检查文件权限或是否被其他程序占用。").arg(fileInfo.fileName()));
        } else {
            emit shortcutSelected(fileInfo.fileName());
        }
    }
}
