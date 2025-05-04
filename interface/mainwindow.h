// MainWindow.h
#include <QMainWindow>
#include <QLabel>

class DirectoryArea;
class FunctionArea;

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    
    // 重写事件过滤器
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void showAboutDialog();

private:
    DirectoryArea *directoryArea;
    FunctionArea *functionArea;
    QLabel *welcomeLabel;
};
