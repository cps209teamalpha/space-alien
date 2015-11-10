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

void MainWindow::clientConnected()
{
    QTcpSocket *sock = server->nextPendingConnection();
    sock->waitForReadyRead();
    QString str = sock->readLine();
    vector<QString> initData = splitQString(str, ':');
    qDebug() << "Client connected: " << initData[0] << endl;
    connect(sock, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
    connect(sock, SIGNAL(readyRead()), this, SLOT(dataReceived()));
    Game::instance()->addPlayer(380, 190, initData[0]);
    PlayerLabel *lblPlayer = new PlayerLabel(ui->centralWidget);
    lblPlayer->setPlayer(Game::instance()->getPlayer(initData[0]));
    QPixmap pixmap(":" + initData[1]);
    lblPlayer->playerGen(pixmap);
    sendGameData(sock);
}

void MainWindow::clientDisconnected()
{
    QTcpSocket *sock = dynamic_cast<QTcpSocket*>(sender());
    PlayerLabel *lblPlayer = nullptr;
    QObjectList objList = ui->centralWidget->children();
    for (QObject *lbl : objList)
    {
        PlayerLabel *test = dynamic_cast<PlayerLabel*>(lbl);
        if (test != nullptr)
        {
            if (test->getPlayer()->getPeerName() == sock->peerName())
            {
                lblPlayer = test;
            }
        }
    }
    if (lblPlayer != nullptr)
    {
        Game::instance()->deletePlayer(lblPlayer->getPlayer()->getPeerName());
        lblPlayer->deleteLater();
    }
    qDebug() << "Client disconnected." << endl;
    sock->deleteLater();
}

void MainWindow::serverDisconnected()
{
    QMessageBox::information(this, "Error", "Server disconnected.");
    QApplication::quit();
}

void MainWindow::dataReceived()
{
    QTcpSocket *sock = dynamic_cast<QTcpSocket*>(sender());
    QString str = sock->readLine();
    if (str == "SYNCH\n")//(ui->rbClient->isChecked() && (synching || (str == "SYNCH")))
    {
        while (str != "SYNCHEND\n")
        {
            if (str == "SYNCH\n")
            {
                Game::instance()->newGame();
                synching = true;
            }
            else if (str == "SYNCHEND\n")
            {
                synching = false;
                resetGUI();
            }
            else
            {
                string line = str.toStdString();
                qDebug() << str << endl;
                switch (line[0])
                {
                case 'P':
                {
                    line = line.substr(1, line.size() - 1);
                    vector<string> data = splitString(line, ',');
                    int x = stoi(data[0]);
                    int y = stoi(data[1]);
                    int rotation = stoi(data[2]);
                    int speed = stoi(data[3]);
                    int angle = stoi(data[4]);
                    QString name = QString::fromStdString(data[5]);
                    Game::instance()->addPlayer(x, y, name);
                    Game::instance()->getPlayer(name)->setRot(rotation);
                    Game::instance()->getPlayer(name)->setSpeed(speed);
                    Game::instance()->getPlayer(name)->setAngle(angle);
                    break;
                }
                case 'A':
                {
                    line = line.substr(1, line.size() - 1);
                    vector<string> data = splitString(line, ',');
                    int x = stoi(data[0]);
                    int y = stoi(data[1]);
                    int id = stoi(data[2]);
                    int rotation = stoi(data[3]);
                    int timedShot = stoi(data[4]);
                    Game::instance()->addNewAlien(x, y, id, rotation);
                    Game::instance()->getAlien(id)->setTimedShot(timedShot);
                    break;
                }
                case 'S':
                {
                    line = line.substr(1, line.size() - 1);
                    vector<string> data = splitString(line, ',');
                    int x = stoi(data[0]);
                    int y = stoi(data[1]);
                    int angle = stoi(data[2]);
                    int id = stoi(data[3]);
                    Game::instance()->addOldShot(x, y, angle, id);
                    break;
                }
                }
            }
            str = sock->readLine();
        }
        resetGUI();
    }
    else
    {
        vector<QString> data = splitQString(str, ':');
        QObjectList objList = ui->centralWidget->children();
        for (QObject *lbl : objList)
        {
            PlayerLabel *lblPlayer = dynamic_cast<PlayerLabel *>(lbl);
            if (lblPlayer != nullptr)
            {
                if ((data[0] == "ACC") && (lblPlayer->getPlayer()->getPeerName() == data[1]) && (!ui->rbClient->isChecked() || (data[1] != ui->lnPeerName->text())))
                {
                    lblPlayer->getPlayer()->accelerate();
                }
                if ((data[0] == "DEC") && (lblPlayer->getPlayer()->getPeerName() == data[1]) && (!ui->rbClient->isChecked() || (data[1] != ui->lnPeerName->text())))
                {
                    lblPlayer->getPlayer()->decelerate();
                }
                if ((data[0] == "RTL") && (lblPlayer->getPlayer()->getPeerName() == data[1]) && (!ui->rbClient->isChecked() || (data[1] != ui->lnPeerName->text())))
                {
                    lblPlayer->getPlayer()->turnLeft();
                    lblPlayer->rotate(lblPlayer->getPlayer()->getRot());
                }
                if ((data[0] == "RTR") && (lblPlayer->getPlayer()->getPeerName() == data[1]) && (!ui->rbClient->isChecked() || (data[1] != ui->lnPeerName->text())))
                {
                    lblPlayer->getPlayer()->turnRight();
                    lblPlayer->rotate(lblPlayer->getPlayer()->getRot());
                }
                if ((data[0] == "SHT") && (lblPlayer->getPlayer()->getPeerName() == data[1]) && (!ui->rbClient->isChecked() || (data[1] != ui->lnPeerName->text())))
                {
                    Game::instance()->addShot((lblPlayer->x() + 42), (lblPlayer->y() + 42), lblPlayer->getPlayer()->getRot(), false);

                    ShotLabel *lblShot = new ShotLabel(ui->centralWidget);

                    lblShot->setShot(Game::instance()->getLastShot());
                    lblShot->shotGen();
                    if (ui->cbSound->isChecked())
                    {
                        pewSound->play();
                    }
                }
            }
        }
        if (ui->rbServer->isChecked())
        {
            // send data to all connected clients
            for (QObject *obj : server->children()) {
                QTcpSocket *anotherSock = dynamic_cast<QTcpSocket*>(obj);
                if (anotherSock != nullptr) {
                    anotherSock->write(str.toLocal8Bit());
                }
            }
        }
    }
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
        lblPlayer->setPlayer(players[i]);
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
    ui->networkSelect->hide();
    ui->lblHostname->hide();
    ui->lnHostname->hide();
    ui->lblPeerName->hide();
    ui->lnPeerName->hide();
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
        Game::instance()->CurrentEnemies() += 1;
    }
}

// If no enemies are left, return true
bool MainWindow::noEnemiesLeft() {
    if (Game::instance()->CurrentEnemies() == 0) return true;
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


    ++Game::instance()->CurrentLevel();
    int lvl = Game::instance()->CurrentLevel();

    // consider moving the following lines into a function?
    // does the same thing as init

    if(Game::instance()->CurrentLevel() == 5) {
        Game::instance()->addBoss(50, 50);

        vector<Boss*> bosses = Game::instance()->getBosses();

        for (size_t i = 0; i < bosses.size(); i++) {
            BossLabel *lblBoss = new BossLabel(ui->centralWidget);
            lblBoss->setBoss(bosses.at(i));
            QPixmap pixmap(":/images/mrj.png");
            lblBoss->bossGen(pixmap);
        }
    }
    else {
        makeEnemies(Game::instance()->Num_enemy() * Game::instance()->CurrentLevel());
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
    }

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
                if (ui->rbClient->isChecked() && (lblPlayer->getPlayer()->getPeerName() == ui->lnPeerName->text()))
                {
                    QString msg = "ACC:" + ui->lnPeerName->text();
                    socket->write(msg.toLocal8Bit());
                    lblPlayer->getPlayer()->accelerate();
                }
                else if (ui->rbServer->isChecked())
                {
                    lblPlayer->getPlayer()->accelerate();
                    QString msg = "ACC:serverPlayer";
                    for (QObject *obj : server->children()) {
                        QTcpSocket *anotherSock = dynamic_cast<QTcpSocket*>(obj);
                        if (anotherSock != nullptr) {
                            anotherSock->write(msg.toLocal8Bit());
                        }
                    }
                }
                else
                {
                    lblPlayer->getPlayer()->accelerate();
                }
            }
            if (downKeyPressed)
            {
                if (ui->rbClient->isChecked() && (lblPlayer->getPlayer()->getPeerName() == ui->lnPeerName->text()))
                {
                    QString msg = "DEC:" + ui->lnPeerName->text();
                    socket->write(msg.toLocal8Bit());
                    lblPlayer->getPlayer()->decelerate();
                }
                else if (ui->rbServer->isChecked())
                {
                    lblPlayer->getPlayer()->decelerate();
                    QString msg = "DEC:serverPlayer";
                    for (QObject *obj : server->children()) {
                        QTcpSocket *anotherSock = dynamic_cast<QTcpSocket*>(obj);
                        if (anotherSock != nullptr) {
                            anotherSock->write(msg.toLocal8Bit());
                        }
                    }
                }
                else
                {
                    lblPlayer->getPlayer()->decelerate();
                }
            }
            if (leftKeyPressed)
            {
                if (ui->rbClient->isChecked() && (lblPlayer->getPlayer()->getPeerName() == ui->lnPeerName->text()))
                {
                    QString msg = "RTL:" + ui->lnPeerName->text();
                    socket->write(msg.toLocal8Bit());
                    lblPlayer->getPlayer()->turnLeft();
                    lblPlayer->rotate(lblPlayer->getPlayer()->getRot());
                }
                else if (ui->rbServer->isChecked())
                {
                    lblPlayer->getPlayer()->turnLeft();
                    lblPlayer->rotate(lblPlayer->getPlayer()->getRot());
                    QString msg = "RTL:serverPlayer";
                    for (QObject *obj : server->children()) {
                        QTcpSocket *anotherSock = dynamic_cast<QTcpSocket*>(obj);
                        if (anotherSock != nullptr) {
                            anotherSock->write(msg.toLocal8Bit());
                        }
                    }
                }
                else
                {
                    lblPlayer->getPlayer()->turnLeft();
                    lblPlayer->rotate(lblPlayer->getPlayer()->getRot());
                }
            }
            if (rightKeyPressed)
            {
                if (ui->rbClient->isChecked() && (lblPlayer->getPlayer()->getPeerName() == ui->lnPeerName->text()))
                {
                    QString msg = "RTR:" + ui->lnPeerName->text();
                    socket->write(msg.toLocal8Bit());
                    lblPlayer->getPlayer()->turnRight();
                    lblPlayer->rotate(lblPlayer->getPlayer()->getRot());
                }
                else if (ui->rbServer->isChecked())
                {
                    lblPlayer->getPlayer()->turnRight();
                    lblPlayer->rotate(lblPlayer->getPlayer()->getRot());
                    QString msg = "RTR:serverPlayer";
                    for (QObject *obj : server->children()) {
                        QTcpSocket *anotherSock = dynamic_cast<QTcpSocket*>(obj);
                        if (anotherSock != nullptr) {
                            anotherSock->write(msg.toLocal8Bit());
                        }
                    }
                }
                else
                {
                    lblPlayer->getPlayer()->turnRight();
                    lblPlayer->rotate(lblPlayer->getPlayer()->getRot());
                }
            }
            if (spacebarKeyPressed)
            {
                if (ui->rbClient->isChecked() && (lblPlayer->getPlayer()->getPeerName() == ui->lnPeerName->text()))
                {
                    QString msg = "SHT:" + ui->lnPeerName->text();
                    socket->write(msg.toLocal8Bit());
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
                else if (ui->rbServer->isChecked())
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
                    QString msg = "SHT:serverPlayer";
                    for (QObject *obj : server->children()) {
                        QTcpSocket *anotherSock = dynamic_cast<QTcpSocket*>(obj);
                        if (anotherSock != nullptr) {
                            anotherSock->write(msg.toLocal8Bit());
                        }
                    }
                }
                else
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
                       --Game::instance()->CurrentEnemies();
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
                       --Game::instance()->CurrentEnemies();
                       if(noEnemiesLeft()) {
                           advanceLevel();
                       }
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

// Sends out the game data to all clients
void MainWindow::sendGameData(QTcpSocket *sock)
{
        if (sock != nullptr) {
    QString msg = "SYNCH\n";
    sock->write(msg.toLocal8Bit());
    vector<Player*> players = Game::instance()->getPlayers();
    vector<Shot*> shots = Game::instance()->getShots();
    vector<Alien*> aliens = Game::instance()->getAliens();
    for (size_t i = 0; i < players.size(); i++)
    {
        msg = QString::fromStdString(players[i]->getSave());
        sock->write(msg.toLocal8Bit());
    }
    for (size_t i = 0; i < shots.size(); i++)
    {
        msg = QString::fromStdString(shots[i]->getSave());
        sock->write(msg.toLocal8Bit());
    }
    for (size_t i = 0; i < aliens.size(); i++)
    {
        msg = QString::fromStdString(aliens[i]->getSave());
        sock->write(msg.toLocal8Bit());
    }
    msg = "SYNCHEND\n";
    sock->write(msg.toLocal8Bit());
        }
}

void MainWindow::on_btnPlay_clicked()
{
    // Initialize game
    Game::instance()->newGame();
    if (ui->rbServer->isChecked())
    {
        server = new QTcpServer(this);
        connect(server, SIGNAL(newConnection()), this, SLOT(clientConnected()));
        if (!server->listen(QHostAddress::Any, 5000)) {
            QMessageBox::critical(this, "Uh oh", "Cannot start socket.");
            return;
        }
        // Player set-up
        Game::instance()->addPlayer(380, 190, "localPlayer");
        PlayerLabel *lblPlayer = new PlayerLabel(ui->centralWidget);
        lblPlayer->setPlayer(Game::instance()->getPlayer("localPlayer"));
        QPixmap pixmap(MainWindow::shipSelect());
        lblPlayer->playerGen(pixmap);
    }
    else if (ui->rbClient->isChecked())
    {
        socket = new QTcpSocket(this);
        connect(socket, SIGNAL(readyRead()), this, SLOT(dataReceived()));
        connect(socket, SIGNAL(disconnected()), this, SLOT(serverDisconnected()));
        QString hostname = ui->lnHostname->text();
        if (hostname.size() == 0) {
            QMessageBox::critical(this, "Error", "Please specify name of server.");
            return;
        }
        socket->connectToHost(hostname, 5000);
        if (!socket->waitForConnected())  {
            QMessageBox::critical(this, "Error", "Unable to connect to server.");
            return;
        }
        // Player set-up
        Game::instance()->addPlayer(380, 190, ui->lnPeerName->text());
        PlayerLabel *lblPlayer = new PlayerLabel(ui->centralWidget);
        lblPlayer->setPlayer(Game::instance()->getPlayer(ui->lnPeerName->text()));
        QPixmap pixmap(MainWindow::shipSelect());
        lblPlayer->playerGen(pixmap);
        QString userName = ui->lnPeerName->text();
        QString shipSelect = MainWindow::shipSelect();
        socket->write(userName.toLocal8Bit());
        socket->write(shipSelect.toLocal8Bit());
    }
    else
    {
        // Player set-up
        Game::instance()->addPlayer(380, 190, "localPlayer");
        PlayerLabel *lblPlayer = new PlayerLabel(ui->centralWidget);
        lblPlayer->setPlayer(Game::instance()->getPlayer("localPlayer"));
        QPixmap pixmap(MainWindow::shipSelect());
        lblPlayer->playerGen(pixmap);
    }

    // Hide Menu GUI
    MainWindow::hideGUI();

    // Enemy set-up
    Game::instance()->Num_enemy() = 5; //This amount for level 1 and PoC purposes
    makeEnemies(Game::instance()->Num_enemy());

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

    // Initialize timer:
    timer->setInterval(50);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerHit()));
    timer->start();
}
