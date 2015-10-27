#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QKeyEvent>
#include <QDebug>
#include <QLabel>
#include <QPixmap>
#include <QTransform>

#include "game.h"
#include "player.h"
#include "enemyspawn.h"

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

int random_int(int min, int max) {
    static std::default_random_engine engine { std::random_device{}() };
    std::uniform_int_distribution<int> distro{min, max};
    return distro(engine);
}

void PlayerLabel::rotate(int angle)
{
    QTransform rotate_label;

    rotate_label.translate(((orig_pixmap->width() - width()) / 2), ((orig_pixmap->width() - height()) / 2));
    rotate_label.rotate(angle, Qt::ZAxis);

    QPixmap pixmap;
    pixmap = orig_pixmap->transformed(rotate_label, Qt::SmoothTransformation);
    setPixmap(pixmap);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    QObjectList objList = ui->centralWidget->children();
    for (QObject *lbl : objList)
    {
        PlayerLabel *lblPlayer = dynamic_cast<PlayerLabel *>(lbl);
        if (lblPlayer != nullptr)
        {
            switch (event->key())
            {
            case Qt::Key_Up:
                Game::instance()->getPlayer()->accelerate();
                if ((Game::instance()->getPlayer()->getX() >= 0) && (Game::instance()->getPlayer()->getY() >= 0))
                {
                    lblPlayer->move(Game::instance()->getPlayer()->getX(), Game::instance()->getPlayer()->getY());
                }
                break;
            case Qt::Key_Down:
                qDebug() << "Ship at (" << Game::instance()->getPlayer()->getX() << "," << Game::instance()->getPlayer()->getY() << ") traveling at " << Game::instance()->getPlayer()->getSpeed() << " upt at an angle of " << Game::instance()->getPlayer()->getAngle() << " rotated " << Game::instance()->getPlayer()->getRot() << " degrees." << endl;
                break;
            case Qt::Key_Left:
                Game::instance()->getPlayer()->turnLeft();
                lblPlayer->rotate(lblPlayer->getPlayer()->getRot());
                break;
            case Qt::Key_Right:
                Game::instance()->getPlayer()->turnRight();
                lblPlayer->rotate(lblPlayer->getPlayer()->getRot());
                break;
            default:
                break;
            }
        }
    }
}
// Matt: Brethen, doest thou participate in the act of the separation of large mass from the earth against its gravitational influence, for the ideal purpose of increasing muscle volume and the burning of the chubs?

void MainWindow::on_btnPlay_clicked()
{
    // Initialize game
    Game::instance()->newGame();

    // Hide Menu GUI
    ui->lblMove->deleteLater();
    ui->lblTitle->hide();
    ui->btnPlay->hide();

    // Enemy set-up
    int num_enemy = 5;

    for (int i = 0; i < num_enemy; ++i)
    {
        auto label_left = random_int(0, this->geometry().width() - 32);
        auto label_top = random_int(ui->btnPlay->geometry().bottom(),
                       this->geometry().height() - 32);

        Enemy *alien = new Enemy(ui->centralWidget);
        QPixmap evil(":/images/asteroid.png");
        alien->setPixmap(evil);
        alien->setGeometry(QRect(label_left, label_top, 32, 32));
        alien->setScaledContents(true);
        alien->show();
    }

    // Player set-up
    PlayerLabel *lblPlayer = new PlayerLabel(ui->centralWidget);
    lblPlayer->setPlayer(Game::instance()->getPlayer());
    QPixmap pixmap(":/images/spaceship.png");
    lblPlayer->setGeometry(380, 190, 41, 41);
    lblPlayer->setPixmap(pixmap);
    lblPlayer->setOrigPixmap(pixmap);
    lblPlayer->setScaledContents(true);
    lblPlayer->show();
    lblPlayer->setFocus();
}
