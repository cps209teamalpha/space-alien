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
#include <QMessageBox>
#include <QSound>

#ifndef M_PI
#define M_PI (atan(1) * 4)
#endif


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

//Random integer generator
int random_int(int min, int max) {
    static std::default_random_engine engine { std::random_device{}() };
    std::uniform_int_distribution<int> distro{min, max};
    return distro(engine);
}

//Generates player onto the window
void PlayerLabel::playerGen(QPixmap pixmap)
{
    setGeometry(getPlayer()->getX(), getPlayer()->getY(), 41, 41);
    setPixmap(pixmap);
    setOrigPixmap(pixmap);
    setScaledContents(true);
    setAttribute(Qt::WA_TranslucentBackground, true);

    show();
    setFocus();
}

void AlienLabel::alienGen(QPixmap pixmap)
{
    QTransform rotater;
    switch(getAlien()->getRot())
    {
    case 90:
        setGeometry(x(), y(), 55, 41);
        rotater.rotate(90);
        break;
    case 180:
        setGeometry(x(), y(), 41, 55);
        rotater.rotate(180);
        break;
    case 270:
        setGeometry(x(), y(), 55, 41);
        rotater.rotate(270);
        break;
    default:
        setGeometry(x(), y(), 41, 55);
        break;
    }

    pixmap = pixmap.transformed(rotater);

    setPixmap(pixmap);
    setScaledContents(true);
    setAttribute(Qt::WA_TranslucentBackground, true);

    show();
}

void ShotLabel::shotGen()
{
    QPixmap pixmap(":/images/pew.png");

    setGeometry(getShot()->getX(), getShot()->getY(), 20, 20);
    setPixmap(pixmap);
    setScaledContents(true);
    setAttribute(Qt::WA_TranslucentBackground, true);

    show();
}

//Rotates the player
void PlayerLabel::rotate(int angle)
{
    QTransform rotate_label;

    rotate_label.rotate(angle);

    QPixmap pixmap;
    pixmap = orig_pixmap->transformed(rotate_label, Qt::SmoothTransformation);

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

    int newX = round(myPlayer->getX() - (((41 * scale) - 41) / 2));
    int newY = round(myPlayer->getY() - (((41 * scale) - 41) / 2));

    offsetX = myPlayer->getX() - newX;
    offsetY = myPlayer->getY() - newY;

    setGeometry((myPlayer->getX() - offsetX), (getPlayer()->getY() - offsetY), 41 * scale, 41 * scale);
    setPixmap(pixmap);
}

//Performs different operations on each timer event
void MainWindow::timerHit()
{
    Game::instance()->updateField();
    QObjectList objList = ui->centralWidget->children();
    for (QObject *lbl : objList)
    {
        PlayerLabel *lblPlayer = dynamic_cast<PlayerLabel *>(lbl);
        if (lblPlayer != nullptr)
        {
            lblPlayer->move((lblPlayer->getPlayer()->getX() - lblPlayer->getOffsetX()), (lblPlayer->getPlayer()->getY() - lblPlayer->getOffsetY()));
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
            if (spacebarKeyPressed)
            {
                Game::instance()->addShot((lblPlayer->x() + 42), (lblPlayer->y() + 42), lblPlayer->getPlayer()->getRot());

                ShotLabel *lblShot = new ShotLabel(ui->centralWidget);

                lblShot->setShot(Game::instance()->getLastShot());
                lblShot->shotGen();
                if (ui->cbSound->isChecked())
                {
                    pewSound->play();
                }
            }

            //Collision
            //2D unit collision algorithm from: https://developer.mozilla.org/en-US/docs/Games/Techniques/2D_collision_detection
            for (int i = 0; i < objList.size(); i++)
            {
                Enemy *test = dynamic_cast<Enemy *>(objList[i]);
                if (test != nullptr)
                {
                   if (lblPlayer->x() < (test->x() + (test->width() / 2)) &&
                           (lblPlayer->x() + lblPlayer->width()) > test->x() &&
                           lblPlayer->y() < (test->y() + (test->height() / 2)) &&
                           ((lblPlayer->height() / 2) + lblPlayer->y()) > test->y())
                   {
                       if (ui->cbSound->isChecked())
                       {
                           riperinoPlayerino->play();
                       }
                       QMessageBox::information(this, "", "You have been DESTROYED!");
                       QApplication::quit();
                   }
                }
            }

        }
        if (Game::instance()->getUntrackedShots() > 0)
        {
            vector<Shot*> shots = Game::instance()->getShots();
            for (size_t i = ((shots.size() - Game::instance()->getUntrackedShots()) - 1); i < shots.size(); i++)
            {
                ShotLabel *lblShot = new ShotLabel(ui->centralWidget);

                lblShot->setShot(shots[i]);
                lblShot->shotGen();
                if (ui->cbSound->isChecked())
                {
                    pewSound->play();
                }
            }
            Game::instance()->setUntrackedShots(0);
        }
        AlienLabel *lblAlien = dynamic_cast<AlienLabel *>(lbl);
        if (lblAlien != nullptr)
        {
            lblAlien->move(lblAlien->getAlien()->getX(), lblAlien->getAlien()->getY());
        }
        ShotLabel *lblShot = dynamic_cast<ShotLabel *>(lbl);
        if (lblShot != nullptr)
        {
            lblShot->move(lblShot->getShot()->getX(), lblShot->getShot()->getY());
            if ((lblShot->getShot()->getX() >= 800) ||
                (lblShot->getShot()->getX() <= 0) ||
                (lblShot->getShot()->getY() >= 573) ||
                (lblShot->getShot()->getY() <= 0))
            {
                // For some reason deleting the label causes the program to crash.
                // Deleting the shot will cause the label to remain on-screen.
                // Keeping it around means it's still flying off-screen: could
                // cause some performance issues (or unexpected bugs)
                //Game::instance()->deleteShot(lblShot->getShot()->getID());
                //lblShot->deleteLater();
            }
            for (int i = 0; i < objList.size(); i++)
            {
                Enemy *test = dynamic_cast<Enemy *>(objList[i]);
                if (test != nullptr)
                {
                   if (lblShot->x() < (test->x() + (test->width() / 2)) &&
                           (lblShot->x() + lblShot->width()) > test->x() &&
                           lblShot->y() < (test->y() + (test->height() / 2)) &&
                           ((lblShot->height() / 2) + lblShot->y()) > test->y())

                   {
                       if (ui->cbSound->isChecked())
                       {
                           ripAsteroid->play();
                       }
                       test->deleteLater();
                       // Causes errors - see above
                       //Game::instance()->deleteShot(lblShot->getShot()->getID());
                       //lblShot->deleteLater();
                   }
                }
            }
        }
        //Updates an Enemy's position
        Enemy *lblEnemy = dynamic_cast<Enemy *>(lbl);
        if (lblEnemy != nullptr)
        {
            lblEnemy->updateEnemy(lblEnemy); //Don't ask, you can fix this if you like lol
        }
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    //Sets a key variable true if pressed
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
                break;
            case Qt::Key_Left:
                leftKeyPressed = true;
                break;
            case Qt::Key_Right:
                rightKeyPressed = true;
                break;
            case Qt::Key_Space:
                spacebarKeyPressed = true;
                break;
            default:
                break;
            }
        }
    }
}

//Sets a key variable false if released
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
    case Qt::Key_Space:
        spacebarKeyPressed = false;
        break;
    default:
        break;
    }
}

void MainWindow::on_btnPlay_clicked()
{
    // Initialize game
    Game::instance()->newGame();

    // Hide Menu GUI
    ui->lblMove->deleteLater();
    ui->lblTitle->hide();
    ui->btnPlay->hide();
    ui->cbSound->hide();

    // Enemy set-up
    int num_enemy = 5; //This amount for level 1 and PoC purposes

    for (int i = 0; i < num_enemy; ++i)
    {
        auto label_left = random_int(0, this->geometry().width() - 32);
        auto label_top = random_int(ui->btnPlay->geometry().bottom(),
                       this->geometry().height() - 32);

        Enemy *alien = new Enemy(ui->centralWidget, random_int(-1,1), random_int(-1,1));
        QPixmap evil(":/images/asteroid.png");
        alien->enemyGen(evil, alien, label_left, label_top);
    }

    Game::instance()->addAlien(0);
    Game::instance()->addAlien(90);
    Game::instance()->addAlien(180);
    Game::instance()->addAlien(270);

    vector<Alien*> aliens = Game::instance()->getAliens();

    for (size_t i = 0; i < aliens.size(); i++)
    {
        AlienLabel *lblAlien = new AlienLabel(ui->centralWidget);
        lblAlien->setAlien(aliens[i]);
        QPixmap pixmap(":/images/alien1.png");
        lblAlien->alienGen(pixmap);
    }

    // Player set-up
    PlayerLabel *lblPlayer = new PlayerLabel(ui->centralWidget);
    lblPlayer->setPlayer(Game::instance()->getPlayer());
    QPixmap pixmap(":/images/spaceship.png");
    lblPlayer->playerGen(pixmap);

    // Initialize timer:
    timer->setInterval(50);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerHit()));
    timer->start();
}
