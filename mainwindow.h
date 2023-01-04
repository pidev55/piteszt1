#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QListWidget* logListWidget();

private slots:
    void on_pushButton1_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
