// FunctionArea.h
#include <QWidget>
#include <QListWidget>
#include <QFileIconProvider>

class QListWidgetItem;

class FunctionArea : public QWidget {
Q_OBJECT

public:
    FunctionArea(QWidget *parent = nullptr);

signals:
    void shortcutSelected(const QString &fileName);

public slots:
    void showShortcuts(const QString &folderName);
    void onShortcutDoubleClicked(QListWidgetItem *item);

private:
    QListWidget *shortcuts;
};
