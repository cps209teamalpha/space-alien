#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "enemyspawn.h"

#include <QKeyEvent>
#include <QDebug>
#include <QLabel>
#include <QPixmap>
#include <QTransform>
#include <QMatrix>

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

    rotate_label.rotate(angle);

    QPixmap pixmap;
    pixmap = orig_pixmap->transformed(rotate_label, Qt::SmoothTransformation);

    // Working on rotation: 10:55-11:25 am
    double scale;

    if ((angle > 90) && (angle < 180)) // Between 90 and 180 degrees
    {
        angle -= 90;
    }
    else if ((angle > 180) && (angle < 270)) // Between 180 and 270 degrees
    {
        angle -= 180;
    }
    else if ((angle > 270) && (angle < 360)) // Between 270 and 360 degrees
    {
        angle -= 270;
    }

    scale = ((41 * abs(sin(angle * M_PI / 180))) + (41 * abs(sin((90 - angle) * M_PI / 180)))) / 41;

    int newX = round(this->x() - (((41 * scale) - 41) / 2));
    int newY = round(this->y() - (((41 * scale) - 41) / 2));

    setGeometry(newX, newY, 41 * scale, 41 * scale);
    setPixmap(pixmap);
}

void MainWindow::timerHit()
{
    Game::instance()->updateField();
    QObjectList objList = ui->centralWidget->children();
    for (QObject *lbl : objList)
    {
        PlayerLabel *lblPlayer = dynamic_cast<PlayerLabel *>(lbl);
        if (lblPlayer != nullptr)
        {
            lblPlayer->move(lblPlayer->getPlayer()->getX(), lblPlayer->getPlayer()->getY());
            if (upKeyPressed)
            {
                lblPlayer->getPlayer()->accelerate();
            }
            if (downKeyPressed)
            {
                lblPlayer->getPlayer()->decelerate();
            }
            if (leftKeyPressed)
            {
                Game::instance()->getPlayer()->turnLeft();
                lblPlayer->rotate(lblPlayer->getPlayer()->getRot());
            }
            if (rightKeyPressed)
            {
                Game::instance()->getPlayer()->turnRight();
                lblPlayer->rotate(lblPlayer->getPlayer()->getRot());
            }
        }
        Enemy *lblEnemy = dynamic_cast<Enemy *>(lbl);
        if (lblEnemy != nullptr)
        {
            int deltaX = lblEnemy->getDeltaX();
            int deltaY = lblEnemy->getDeltaY();
            lblEnemy->move(lblEnemy->x() + deltaX, lblEnemy->y() + deltaY);
        }
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    // Get rid of these unnecessary functions
    // once we remove the qDebug() message.
    QObjectList objList = ui->centralWidget->children();
    for (QObject *lbl : objList)
    {
        PlayerLabel *lblPlayer = dynamic_cast<PlayerLabel *>(lbl);
        if (lblPlayer != nullptr)
        {
            switch (event->key())
            {
            case Qt::Key_Up:
                upKeyPressed = true;
                break;
            case Qt::Key_Down:
                downKeyPressed = true;
                qDebug() << "Ship at (" << Game::instance()->getPlayer()->getX() << "," << Game::instance()->getPlayer()->getY() << ") traveling at " << Game::instance()->getPlayer()->getSpeed() << " upt at an angle of " << Game::instance()->getPlayer()->getAngle() << " rotated " << Game::instance()->getPlayer()->getRot() << " degrees." << endl;
                break;
            case Qt::Key_Left:
                leftKeyPressed = true;
                break;
            case Qt::Key_Right:
                rightKeyPressed = true;
                break;
            default:
                break;
            }
        }
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Up:
        upKeyPressed = false;
        break;
    case Qt::Key_Down:
        downKeyPressed = false;
        break;
    case Qt::Key_Left:
        leftKeyPressed = false;
        break;
    case Qt::Key_Right:
        rightKeyPressed = false;
        break;
    default:
        break;
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
    int num_enemy = 5; //This amount for level 1 and PoC purposes

    for (int i = 0; i < num_enemy; ++i)
    {
        auto label_left = random_int(0, this->geometry().width() - 32);
        auto label_top = random_int(ui->btnPlay->geometry().bottom(),
                       this->geometry().height() - 32);

        Enemy *alien = new Enemy(ui->centralWidget, random_int(-1,1), random_int(-1,1));
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

    // Initialize timer:
    timer->setInterval(50);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerHit()));
    timer->start();
}
