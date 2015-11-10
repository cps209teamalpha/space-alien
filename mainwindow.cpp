#include "mainwindow.h"
#include "ui_mainwindow.h"

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

void MainWindow::resetGUI()
{
    // Clear playing field of UI elements:
    QObjectList objList = ui->centralWidget->children();
    for (QObject *lbl : objList)
    {
        PlayerLabel *lblPlayer = dynamic_cast<PlayerLabel *>(lbl);
        if (lblPlayer != nullptr)
        {
            lblPlayer->deleteLater();
        }
        else
        {
            ShotLabel *lblShot = dynamic_cast<ShotLabel *>(lbl);
            if (lblShot != nullptr)
            {
                lblShot->deleteLater();
            }
            else
            {
                AlienLabel *lblAlien = dynamic_cast<AlienLabel *>(lbl);
                if (lblAlien != nullptr)
                {
                    lblAlien->deleteLater();
                }
                else
                {
                    PlayerLabel *lblPlayer = dynamic_cast<PlayerLabel *>(lbl);
                    if (lblPlayer != nullptr)
                    {
                        lblPlayer->deleteLater();
                    }
                    else
                    {
                        // Should delete Enemies here, but they aren't
                        // yet capable of being reloaded.
                    }
                }
            }
        }
    }

    // Generate new UI elements:
    vector<Player*> players = Game::instance()->getPlayers();
    for (size_t i = 0; i < players.size(); i++)
    {
        PlayerLabel *lblPlayer = new PlayerLabel(ui->centralWidget);
        lblPlayer->setPlayer(Game::instance()->getPlayer());
        QPixmap pixmap(MainWindow::shipSelect());
        lblPlayer->playerGen(pixmap);
        lblPlayer->rotate(lblPlayer->getPlayer()->getRot());
    }
    vector<Alien*> aliens = Game::instance()->getAliens();
    for (size_t i = 0; i < aliens.size(); i++)
    {
        AlienLabel *lblAlien = new AlienLabel(ui->centralWidget);
        lblAlien->setAlien(aliens[i]);
        QPixmap pixmap(":/images/alien1.png");
        lblAlien->alienGen(pixmap);
    }
    vector<Shot*> shots = Game::instance()->getShots();
    for (size_t i = 0; i < shots.size(); i++)
    {
        ShotLabel *lblShot = new ShotLabel(ui->centralWidget);
        lblShot->setShot(shots[i]);
        lblShot->shotGen();
    }
}

void MainWindow::hideGUI()
{
    // Hide Menu GUI
    ui->lblMove->deleteLater();
    ui->lblTitle->hide();
    ui->btnPlay->hide();
    ui->cbSound->hide();
    ui->lblSpaceship2->hide();
    ui->lblSpaceship3->hide();
    ui->lblSpaceship4->hide();
    ui->lblSpaceship5->hide();
    ui->radioButton->hide();
    ui->radioButton_2->hide();
    ui->radioButton_3->hide();
    ui->radioButton_4->hide();
    ui->radioButton_5->hide();
}

QString MainWindow::shipSelect()
{
    //Allows the user to select a ship of their selection (BONUS)
        if (ui->radioButton->isChecked())
        {
            return ":/images/spaceship.png";
        }
        else if (ui->radioButton_2->isChecked())
        {
            return ":/images/spaceship2.png";
        }
        else if (ui->radioButton_3->isChecked())
        {
            return ":/images/spaceship3.png";
        }
        else if (ui->radioButton_4->isChecked())
        {
            return ":/images/spaceship4.png";
        }
        else if (ui->radioButton_5->isChecked())
        {
            return ":/images/spaceship5.png";
        }
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

//Generates Mr. Jueckstock onto the window
void BossLabel::bossGen(QPixmap pixmap)
{
    setGeometry(getBoss()->getX(), getBoss()->getY(), 139, 212);
    setPixmap(pixmap);
    setScaledContents(true);
    setAttribute(Qt::WA_TranslucentBackground, true);

    show();
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

// Generate X number of enemies on the screen
void MainWindow::makeEnemies(int num_enemy) {
    for (size_t i = 0; i < num_enemy; ++i)
    {
        auto label_left = random_int(0, this->geometry().width() - 32);
        auto label_top = random_int(ui->btnPlay->geometry().bottom(),
                       this->geometry().height() - 32);

        Enemy *alien = new Enemy(ui->centralWidget, random_int(-1,1), random_int(-1,1));
        QPixmap evil(":/images/asteroid.png");
        alien->enemyGen(evil, alien, label_left, label_top);
        ++currentEnemies;
    }
}

// If no enemies are left, return true
bool MainWindow::noEnemiesLeft() {
    if (currentEnemies == 0) return true;
    else                     return false;
}

// lazy level implementation
void MainWindow::advanceLevel() {
    //Plays a sound before level advances
    levelUp->play();

    //Setting up a label to inform the user that he has advanced to the next level. It will disappear after 3 seconds.
        congratsLabel = new QLabel(this);
    congratsLabel->setText("<font color='red'>Congratulations! You've advanced to the next level!</font>");
        congratsLabel->adjustSize();
        congratsLabel->move(230, 270);
    congratsLabel->show();

    connect(congratsLabelTimer, SIGNAL(timeout()),this, SLOT(hideMessage()));
    congratsLabelTimer->start(3000);


    ++currentLevel;

    makeEnemies(num_enemy * currentLevel);
}

void MainWindow::hideMessage()
{
    //slot to hide the msg and stop the timer.
    congratsLabel->hide();
    congratsLabelTimer->stop();

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
                //setting the shot as false (player shot)
                Game::instance()->addShot((lblPlayer->x() + 42), (lblPlayer->y() + 42), lblPlayer->getPlayer()->getRot(), false);


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
                //Collision for the player when hit by alien lasers. Only dies from alien shots.
                 ShotLabel *lblShot = dynamic_cast<ShotLabel *>(objList[i]);

                 if (lblShot != nullptr && lblShot->getShot()->getIsAlienShot() == true)
                 {
                     if (lblPlayer->x() < (lblShot->x() + (lblShot->width() / 2)) &&
                             (lblPlayer->x() + lblPlayer->width()) > lblShot->x() &&
                             lblPlayer->y() < (lblShot->y() + (lblShot->height() / 2)) &&
                             ((lblPlayer->height() / 2) + lblPlayer->y()) > lblShot->y())
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
                Game::instance()->deleteShot(lblShot->getShot()->getID());
                lblShot->deleteLater();
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
                       Game::instance()->deleteShot(lblShot->getShot()->getID());
                       lblShot->deleteLater();
                       --currentEnemies;
                       if(noEnemiesLeft()) {
                           advanceLevel();
                       }
                   }
                }
                AlienLabel *alienTest = dynamic_cast<AlienLabel *>(objList[i]);
                if (alienTest != nullptr)
                {
                   if (lblShot->x() < (alienTest->x() + (alienTest->width() / 2)) &&
                           (lblShot->x() + lblShot->width()) > alienTest->x() &&
                           lblShot->y() < (alienTest->y() + (alienTest->height() / 2)) &&
                           ((lblShot->height() / 2) + lblShot->y()) > alienTest->y())

                   {
                       if (ui->cbSound->isChecked())
                       {
                           ripAsteroid->play();
                       }
                       Game::instance()->deleteAlien(alienTest->getAlien()->getID());
                       alienTest->deleteLater();
                       Game::instance()->deleteShot(lblShot->getShot()->getID());
                       lblShot->deleteLater();
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
    case Qt::Key_S:
        Game::instance()->save();
        break;
    case Qt::Key_L:
        Game::instance()->load();
        resetGUI();
        break;
    default:
        break;
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
    MainWindow::hideGUI();

    // Enemy set-up
    num_enemy = 5; //This amount for level 1 and PoC purposes
    makeEnemies(num_enemy);

    Game::instance()->addAlien(0);
    Game::instance()->addAlien(90);
    Game::instance()->addAlien(180);
    Game::instance()->addAlien(270);

    // Summons Mr. Jueckstock to the battlefield
    //Game::instance()->addBoss();

    vector<Boss*> bosses = Game::instance()->getBosses();

    for (size_t i = 0; i < bosses.size(); i++) {
        BossLabel *lblBoss = new BossLabel(ui->centralWidget);
        lblBoss->setBoss(bosses.at(i));
        QPixmap pixmap(":/images/mrj.png");
        lblBoss->bossGen(pixmap);
    }


    vector<Alien*> aliens = Game::instance()->getAliens();

    for (size_t i = 0; i < aliens.size(); i++)
    {
        AlienLabel *lblAlien = new AlienLabel(ui->centralWidget);
        lblAlien->setAlien(aliens[i]);
        QPixmap pixmap(":/images/alien1.png");
        lblAlien->alienGen(pixmap);
    }

    Game::instance()->addPlayer(380, 190);

    // Player set-up
    PlayerLabel *lblPlayer = new PlayerLabel(ui->centralWidget);
    lblPlayer->setPlayer(Game::instance()->getPlayer());
    QPixmap pixmap(MainWindow::shipSelect());
    lblPlayer->playerGen(pixmap);

    // Initialize timer:
    timer->setInterval(50);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerHit()));
    timer->start();
}
