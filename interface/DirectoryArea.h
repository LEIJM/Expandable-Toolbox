// DirectoryArea.h
#include <QWidget>
#include <QVBoxLayout>


class QPushButton;

class DirectoryArea : public QWidget {
Q_OBJECT

signals:
    void folderSelected(const QString &folderName);

public:
    DirectoryArea(QWidget *parent = nullptr);
private slots:
    void onFolderButtonClicked();
private:
    QVBoxLayout *layout;
};
