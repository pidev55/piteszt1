#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDateTime>

extern int i;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton1_clicked()
{
    auto ts = QDateTime().currentDateTime().toString("yyyy.MM.dd. HH:mm:ss.zzz");
    //QString ts= t.toString();
    auto a = QString(QStringLiteral("Megnyomva %1x: %2").arg(++i).arg(ts));

    ui->listWidget1->addItem(a);
}

QListWidget* MainWindow::logListWidget(){
    return ui->listWidget1;
}
