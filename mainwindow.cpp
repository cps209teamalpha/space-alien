#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QKeyEvent>

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
//this is a delete
//Is it really??????????????????????

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Up:
        if (ui->lblMove->y() >= 5)
        {
            ui->lblMove->move((ui->lblMove->x()), (ui->lblMove->y() - 5));
        }
        break;
    case Qt::Key_Down:
        if (ui->lblMove->y() <= (ui->centralWidget->height() - 20))
        {
            ui->lblMove->move((ui->lblMove->x()), (ui->lblMove->y() + 5));
        }
        break;
    case Qt::Key_Left:
        if (ui->lblMove->x() >= 5)
        {
            ui->lblMove->move((ui->lblMove->x() - 5), (ui->lblMove->y()));
        }
        break;
    case Qt::Key_Right:
        if (ui->lblMove->x() <= (ui->centralWidget->width() - 70))
        {
            ui->lblMove->move((ui->lblMove->x() + 5), (ui->lblMove->y()));
        }
        break;
    default:
        break;
    }
}
    // Matt: Brethen, doest thou participate in the act of the separation of large mass from the earth against its gravitational influence, for the ideal purpose of increasing muscle volume and the burning of the chubs?
