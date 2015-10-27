#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "enemyspawn.h"

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
//Is it not really?

 int random_int(int min, int max) {
    static std::default_random_engine engine { std::random_device{}() };
    std::uniform_int_distribution<int> distro{min, max};
    return distro(engine);
}

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

void MainWindow::on_pushButton_clicked()
{
    auto label_left = random_int(0, this->geometry().width() - 32);
    auto label_top = random_int(ui->pushButton->geometry().bottom(),
                   this->geometry().height() - 32);

    Enemy *alien = new Enemy(ui->centralWidget);
    QPixmap evil(":/images/asteroid.png");
    alien->setPixmap(evil);
    alien->setGeometry(QRect(label_left, label_top, 32, 32));
    alien->setScaledContents(true);
    alien->show();
}
