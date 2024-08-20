// MainWindow.h
#include <QMainWindow>
#include <QLabel>

class DirectoryArea;
class FunctionArea;

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
private:
    DirectoryArea *directoryArea;
    FunctionArea *functionArea;
    QLabel *versionLabel;
};
