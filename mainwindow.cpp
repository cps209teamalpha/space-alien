#include "mainwindow.h"
#include "ui_mainwindow.h"

#ifndef M_PI
#define M_PI (atan(1) * 4)
#endif

void ConnectionNames::addRecord(QString name, QString address, quint16 port)
{
    names.push_back(name);
    addresses.push_back(address);
    ports.push_back(port);
}

void ConnectionNames::deleteRecord(QString name)
{
    int index = -1;
    for (size_t i = 0; i < names.size(); i++)
    {
        if (names[i] == name)
        {
            index = i;
        }
    }
    if (index >= 0)
    {
        names.erase(names.begin() + index);
        addresses.erase(addresses.begin() + index);
        ports.erase(ports.begin() + index);
    }
}

QString ConnectionNames::getName(QString address, quint16 port)
{
    for (size_t i = 0; i < addresses.size(); i++)
    {
        if ((addresses[i] == address) && (ports[i] == port))
        {
            return names[i];
        }
    }
    return "";
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Initialize timer:
    timer->setInterval(50);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerHit()));
    connectionNames = new ConnectionNames();
}

MainWindow::~MainWindow()
{
    delete connectionNames;
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
    Player *player = Game::instance()->getPlayer(initData[0]);
    if (player == nullptr)
    {
        qDebug() << "Client '" << initData[0] << "' connected from " << sock->peerAddress().toString() << endl;
        connect(sock, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
        connect(sock, SIGNAL(readyRead()), this, SLOT(dataReceived()));
        int immunity;
        if (ui->cbCheatMode->isChecked())
        {
            immunity = -1;
        }
        else
        {
            immunity = 40;
        }
        Game::instance()->addPlayer(380, 190, initData[0], immunity);
        PlayerLabel *lblPlayer = new PlayerLabel(ui->centralWidget);
        lblPlayer->setPlayer(Game::instance()->getPlayer(initData[0]));
        lblPlayer->getPlayer()->setPixmapName(":" + initData[1]);
        lblPlayer->playerGen();
        connectionNames->addRecord(initData[0], sock->peerAddress().toString(), sock->peerPort());
        sendGameData(sock);
    }
    else
    {
        sock->disconnectFromHost();
    }
}

void MainWindow::clientDisconnected()
{
    QTcpSocket *sock = dynamic_cast<QTcpSocket*>(sender());
    QString clientName = connectionNames->getName(sock->peerAddress().toString(), sock->peerPort());
    PlayerLabel *lblPlayer = nullptr;
    QObjectList objList = ui->centralWidget->children();
    for (QObject *lbl : objList)
    {
        PlayerLabel *test = dynamic_cast<PlayerLabel*>(lbl);
        if (test != nullptr)
        {
            if (test->getPlayer()->getPeerName() == clientName)
            {
                lblPlayer = test;
            }
        }
    }
    if (lblPlayer != nullptr)
    {
        Game::instance()->deletePlayer(clientName);
        lblPlayer->deleteLater();
    }
    qDebug() << "Client disconnected: " << clientName << endl;
    sock->deleteLater();
}

void MainWindow::serverDisconnected()
{
    QMessageBox::information(this, "Error", "Server disconnected.");
    gotoMenu();
}

void MainWindow::dataReceived()
{
    QTcpSocket *sock = dynamic_cast<QTcpSocket*>(sender());
    QString str = sock->readLine();
    if (str == "SYNCH\n")
    {
        while (str != "SYNCHEND\n")
        {
            if (str == "SYNCH\n")
            {
                Game::instance()->newGame();
            }
            else
            {
                string line = str.toStdString();
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
                    QString name = QString::fromStdString(data[5]).simplified();
                    QString pixmapName = QString::fromStdString(data[6]).simplified();
                    int immunity = stoi(data[7]);
                    Game::instance()->addPlayer(x, y, name, immunity);
                    Game::instance()->getPlayer(name)->setRot(rotation);
                    Game::instance()->getPlayer(name)->setSpeed(speed);
                    Game::instance()->getPlayer(name)->setAngle(angle);
                    Game::instance()->getPlayer(name)->setPixmapName(pixmapName);
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
                    bool isAlienShot;
                    if (data[4] == "1")
                    {
                        isAlienShot = true;
                    }
                    else
                    {
                        isAlienShot = false;
                    }
                    Game::instance()->addOldShot(x, y, angle, id, isAlienShot);
                    break;
                }
                case 'E':
                {
                    line = line.substr(1, line.size() - 1);
                    vector<string> data = splitString(line, ',');
                    int x = stoi(data[0]);
                    int y = stoi(data[1]);
                    int dX = stoi(data[2]);
                    int dY = stoi(data[3]);
                    int id = stoi(data[4]);
                    Game::instance()->addOldEnemy(x, y, dX, dY, id);
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
        vector<QString> cmdList = splitQString(str.simplified(), '/');
        for (size_t i = 0; i < cmdList.size(); i++)
        {

        QString newStr = cmdList[i];
        vector<QString> data = splitQString(newStr.simplified(), ':');
        QObjectList objList = ui->centralWidget->children();
        for (QObject *lbl : objList)
        {
            PlayerLabel *lblPlayer = dynamic_cast<PlayerLabel *>(lbl);
            if (lblPlayer != nullptr)
            {
                if ((data[0] == "ACC") && (lblPlayer->getPlayer()->getPeerName() == data[1]))
                {
                    lblPlayer->getPlayer()->accelerate();
                }
                if ((data[0] == "DEC") && (lblPlayer->getPlayer()->getPeerName() == data[1]))
                {
                    lblPlayer->getPlayer()->decelerate();
                }
                if ((data[0] == "RTL") && (lblPlayer->getPlayer()->getPeerName() == data[1]))
                {
                    lblPlayer->getPlayer()->turnLeft();
                    lblPlayer->rotate(lblPlayer->getPlayer()->getRot());
                }
                if ((data[0] == "RTR") && (lblPlayer->getPlayer()->getPeerName() == data[1]))
                {
                    lblPlayer->getPlayer()->turnRight();
                    lblPlayer->rotate(lblPlayer->getPlayer()->getRot());
                }
                if ((data[0] == "SHT") && (lblPlayer->getPlayer()->getPeerName() == data[1]))
                {
                    double shotX = lblPlayer->x() + (lblPlayer->width() / 2) - 10;
                    double shotY = lblPlayer->y() + (lblPlayer->height() / 2) - 10;
                    updateCoords(shotX, shotY, 30, lblPlayer->getPlayer()->getRot());
                    Game::instance()->addShot(shotX, shotY, lblPlayer->getPlayer()->getRot(), false);

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

    ui->lblScore->setText(0);
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
                        EnemyLabel *lblEnemy = dynamic_cast<EnemyLabel *>(lbl);
                        if (lblEnemy != nullptr)
                        {
                            lblEnemy->deleteLater();
                        }
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
        lblPlayer->playerGen();
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
    vector<Enemy*> enemies = Game::instance()->getEnemies();
    for (size_t i = 0; i < enemies.size(); i++)
    {
        EnemyLabel *lblEnemy = new EnemyLabel(ui->centralWidget);
        lblEnemy->setEnemy(enemies[i]);
        lblEnemy->enemyGen();
    }
    vector<Boss*> bosses = Game::instance()->getBosses();
    for (size_t i = 0; i < bosses.size(); i++) {
        BossLabel *lblBoss = new BossLabel(ui->centralWidget);
        lblBoss->setBoss(bosses.at(i));
        QPixmap pixmap(":/images/mrj.png");
        lblBoss->bossGen(pixmap);
    }
}

void MainWindow::hideGUI()
{
    // Hide Menu GUI
    ui->lblSpaceship1->hide();
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
    ui->lblSound->hide();
    ui->lblCheatMode->hide();
    ui->cbCheatMode->hide();
}

void MainWindow::showGUI()
{
    // Show Menu GUI
    ui->lblSpaceship1->show();
    ui->lblTitle->show();
    ui->btnPlay->show();
    ui->cbSound->show();
    ui->lblSpaceship2->show();
    ui->lblSpaceship3->show();
    ui->lblSpaceship4->show();
    ui->lblSpaceship5->show();
    ui->radioButton->show();
    ui->radioButton_2->show();
    ui->radioButton_3->show();
    ui->radioButton_4->show();
    ui->radioButton_5->show();
    ui->networkSelect->show();
    ui->lblHostname->show();
    ui->lnHostname->show();
    ui->lblPeerName->show();
    ui->lnPeerName->show();
    ui->lblSound->show();
    ui->lblCheatMode->show();
    ui->cbCheatMode->show();
    ui->btnPlay->setFocus();
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
        return ":/images/spaceship.png";
}

//Generates player onto the window
void PlayerLabel::playerGen()
{
    QPixmap pixmap(getPlayer()->getPixmapName());
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

// Generate X number of asteroids on the screen
void MainWindow::makeEnemies(int NumEnemy) {
    for (int i = 0; i <= NumEnemy; ++i)
    {
        auto label_left = random_int(0, this->geometry().width() - 32);
        auto label_top = random_int(ui->btnPlay->geometry().bottom(),
                       this->geometry().height() - 32);

        Game::instance()->addEnemy(label_left, label_top, random_int(-1, 1), random_int(-1, 1));
        EnemyLabel *lblEnemy = new EnemyLabel(ui->centralWidget);
        lblEnemy->setEnemy(Game::instance()->getLastEnemy());
        lblEnemy->enemyGen();
    }
}

// If no enemies are left, return true
bool MainWindow::noEnemiesLeft() {
    if   (Game::instance()->getCurrentEnemies() <= 0) return true;
    else                                              return false;
}

void MainWindow::showCongrats()
{
    //Setting up a label to inform the user that he has advanced to the next level. It will disappear after 3 seconds.
        congratsLabel = new QLabel(this);
    congratsLabel->setText("<font color='red'>Congratulations! You've advanced to the next level!</font>");
        congratsLabel->adjustSize();
        congratsLabel->move(230, 270);
    congratsLabel->show();

    connect(congratsLabelTimer, SIGNAL(timeout()),this, SLOT(hideMessage()));
    congratsLabelTimer->start(3000);
}

// lazy level implementation
void MainWindow::advanceLevel() {
    //Plays a sound before level advances
    if (ui->cbSound->isChecked() && Game::instance()->CurrentLevel() < 5)
    {
    levelUp->play();
    }

    showCongrats();

    vector<Player*> players = Game::instance()->getPlayers();
    for (size_t i = 0; i < players.size(); i++)
    {
        if (players[i]->getImmuneTimer() >= 0)
        {
            players[i]->setImmuneTimer(40);
        }
    }

    ++Game::instance()->CurrentLevel();
    //int lvl = Game::instance()->CurrentLevel();

    // consider moving the following lines into a function?
    // does the same thing as init

    makeEnemies(Game::instance()->NumEnemy() * Game::instance()->CurrentLevel());
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

    if (ui->rbServer->isChecked())
    {
        for (QObject *obj : server->children()) {
            QTcpSocket *anotherSock = dynamic_cast<QTcpSocket*>(obj);
            if (anotherSock != nullptr) {
                sendGameData(anotherSock);
            }
        }
    }
}

void MainWindow::hideMessage()
{
    //slot to hide the msg and stop the timer.
    congratsLabel->hide();
    congratsLabelTimer->stop();
}

void MainWindow::gotoMenu()
{
    timer->stop();
    if (ui->rbServer->isChecked())
    {
        for (QObject *obj : server->children()) {
            QTcpSocket *anotherSock = dynamic_cast<QTcpSocket*>(obj);
            if (anotherSock != nullptr) {
                anotherSock->disconnectFromHost();
            }
        }
        server->deleteLater();
    }
    if (ui->rbClient->isChecked())
    {
        socket->deleteLater();
    }
    upKeyPressed = false;
    downKeyPressed = false;
    leftKeyPressed = false;
    rightKeyPressed = false;
    spacebarKeyPressed = false;
    Game::instance()->newGame();
    resetGUI();
    showGUI();
}

void MainWindow::updateScore() {
    int score = ui->lblScore->text().toInt();
    score += Game::instance()->Score();
    ui->lblScore->setText(QString::number(score));
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
                    serverMsgs.push_back(msg);
                }
                else if (ui->rbServer->isChecked() && (lblPlayer->getPlayer()->getPeerName() == "serverPlayer"))
                {
                    lblPlayer->getPlayer()->accelerate();
                    QString msg = "ACC:serverPlayer\n";
                    serverMsgs.push_back(msg);
                }
                else if (ui->rbSingleplayer->isChecked())
                {
                    lblPlayer->getPlayer()->accelerate();
                }
            }
            if (downKeyPressed)
            {
                if (ui->rbClient->isChecked() && (lblPlayer->getPlayer()->getPeerName() == ui->lnPeerName->text()))
                {
                    QString msg = "DEC:" + ui->lnPeerName->text();
                    serverMsgs.push_back(msg);
                }
                else if (ui->rbServer->isChecked() && (lblPlayer->getPlayer()->getPeerName() == "serverPlayer"))
                {
                    lblPlayer->getPlayer()->decelerate();
                    QString msg = "DEC:serverPlayer\n";
                    serverMsgs.push_back(msg);
                }
                else if (ui->rbSingleplayer->isChecked())
                {
                    lblPlayer->getPlayer()->decelerate();
                }
            }
            if (leftKeyPressed)
            {
                if (ui->rbClient->isChecked() && (lblPlayer->getPlayer()->getPeerName() == ui->lnPeerName->text()))
                {
                    QString msg = "RTL:" + ui->lnPeerName->text();
                    serverMsgs.push_back(msg);
                }
                else if (ui->rbServer->isChecked() && (lblPlayer->getPlayer()->getPeerName() == "serverPlayer"))
                {
                    lblPlayer->getPlayer()->turnLeft();
                    lblPlayer->rotate(lblPlayer->getPlayer()->getRot());
                    QString msg = "RTL:serverPlayer\n";
                    serverMsgs.push_back(msg);
                }
                else if (ui->rbSingleplayer->isChecked())
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
                    serverMsgs.push_back(msg);
                }
                else if (ui->rbServer->isChecked() && (lblPlayer->getPlayer()->getPeerName() == "serverPlayer"))
                {
                    lblPlayer->getPlayer()->turnRight();
                    lblPlayer->rotate(lblPlayer->getPlayer()->getRot());
                    QString msg = "RTR:serverPlayer\n";
                    serverMsgs.push_back(msg);
                }
                else if (ui->rbSingleplayer->isChecked())
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
                    serverMsgs.push_back(msg);
                }
                else if (ui->rbServer->isChecked() && (lblPlayer->getPlayer()->getPeerName() == "serverPlayer"))
                {
                    //setting the shot as false (player shot)
                    double shotX = lblPlayer->x() + (lblPlayer->width() / 2) - 10;
                    double shotY = lblPlayer->y() + (lblPlayer->height() / 2) - 10;
                    updateCoords(shotX, shotY, 30, lblPlayer->getPlayer()->getRot());
                    Game::instance()->addShot(shotX, shotY, lblPlayer->getPlayer()->getRot(), false);

                    ShotLabel *lblShot = new ShotLabel(ui->centralWidget);

                    lblShot->setShot(Game::instance()->getLastShot());
                    lblShot->shotGen();
                    if (ui->cbSound->isChecked())
                    {
                        pewSound->play();
                    }
                    QString msg = "SHT:serverPlayer\n";
                    serverMsgs.push_back(msg);
                }
                else if (ui->rbSingleplayer->isChecked())
                {
                    //setting the shot as false (player shot)
                    double shotX = lblPlayer->x() + (lblPlayer->width() / 2) - 10;
                    double shotY = lblPlayer->y() + (lblPlayer->height() / 2) - 10;
                    updateCoords(shotX, shotY, 30, lblPlayer->getPlayer()->getRot());
                    Game::instance()->addShot(shotX, shotY, lblPlayer->getPlayer()->getRot(), false);

                    ShotLabel *lblShot = new ShotLabel(ui->centralWidget);

                    lblShot->setShot(Game::instance()->getLastShot());
                    lblShot->shotGen();
                    if (ui->cbSound->isChecked())
                    {
                        pewSound->play();
                    }
                }
            }

            // PLAYER START Collision
            //2D unit collision algorithm from: https://developer.mozilla.org/en-US/docs/Games/Techniques/2D_collision_detection
            for (int i = 0; i < objList.size(); i++)
            {

                EnemyLabel *test = dynamic_cast<EnemyLabel *>(objList[i]);
                if (test != nullptr && lblPlayer->getPlayer()->getImmunity() == false)
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
                       if (ui->rbClient->isChecked() && (lblPlayer->getPlayer()->getPeerName() == ui->lnPeerName->text()))
                       {
                            QMessageBox::information(this, "", "You have been DESTROYED!");
                            gotoMenu();
                       }
                       else if (ui->rbServer->isChecked() && (lblPlayer->getPlayer()->getPeerName() == "serverPlayer"))
                       {
                           QMessageBox::information(this, "", "You have been DESTROYED!");
                           gotoMenu();
                       }
                       else if (ui->rbSingleplayer->isChecked())
                       {
                           QMessageBox::information(this, "", "You have been DESTROYED!");
                           gotoMenu();
                       }
                   }
                }
                //Collision for the player when hit by alien lasers. Only dies from alien shots.
                 ShotLabel *lblShot = dynamic_cast<ShotLabel *>(objList[i]);

                 if (lblShot != nullptr && lblShot->getShot()->getIsAlienShot() == true && lblPlayer->getPlayer()->getImmunity() == false)
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
                         if (ui->rbClient->isChecked() && (lblPlayer->getPlayer()->getPeerName() == ui->lnPeerName->text()))
                         {
                              QMessageBox::information(this, "", "You have been DESTROYED!");
                              gotoMenu();
                         }
                         else if (ui->rbServer->isChecked() && (lblPlayer->getPlayer()->getPeerName() == "serverPlayer"))
                         {
                             QMessageBox::information(this, "", "You have been DESTROYED!");
                             gotoMenu();
                         }
                         else if (ui->rbSingleplayer->isChecked())
                         {
                             QMessageBox::information(this, "", "You have been DESTROYED!");
                             gotoMenu();
                         }
                         //PLAYER COLLISION END
                     }
                 }
                AlienLabel *lblAliensu = dynamic_cast<AlienLabel *>(objList[i]);
                if (lblAliensu != nullptr && lblPlayer->getPlayer()->getImmunity() == false)
                {
                    if (lblPlayer->x() < (lblAliensu->x() + (lblAliensu->width() / 2)) &&
                            (lblPlayer->x() + lblPlayer->width()) > lblAliensu->x() &&
                            lblPlayer->y() < (lblAliensu->y() + (lblAliensu->height() / 2)) &&
                            ((lblPlayer->height() / 2) + lblPlayer->y()) > lblAliensu->y())
                    {
                        if (ui->cbSound->isChecked())
                        {
                            riperinoPlayerino->play();
                        }
                        if (ui->rbClient->isChecked() && (lblPlayer->getPlayer()->getPeerName() == ui->lnPeerName->text()))
                        {
                             QMessageBox::information(this, "", "You have been DESTROYED!");
                             gotoMenu();
                        }
                        else if (ui->rbServer->isChecked() && (lblPlayer->getPlayer()->getPeerName() == "serverPlayer"))
                        {
                            QMessageBox::information(this, "", "You have been DESTROYED!");
                            gotoMenu();
                        }
                        else if (ui->rbSingleplayer->isChecked())
                        {
                            QMessageBox::information(this, "", "You have been DESTROYED!");
                            gotoMenu();
                        }
                        //PLAYER COLLISION END
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
                EnemyLabel *test = dynamic_cast<EnemyLabel *>(objList[i]);
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
                       Game::instance()->Score() += 100;
                       updateScore();
                       Game::instance()->deleteEnemy(test->getEnemy()->getID());
                       test->deleteLater();
                       Game::instance()->deleteShot(lblShot->getShot()->getID());
                       lblShot->deleteLater();
                       if(noEnemiesLeft()) {
                           advanceLevel();
                       }
                   }
                }
                if (!lblShot->getShot()->getIsAlienShot())
                {
                AlienLabel *alienTest = dynamic_cast<AlienLabel *>(objList[i]);
                if (alienTest != nullptr)
                {
                        if (lblShot->x() < (alienTest->x() + (alienTest->width() / 2)) &&
                           (lblShot->x() + lblShot->width()) > alienTest->x() &&
                           (lblShot->y() < (alienTest->y()) + (alienTest->height() / 2)) &&
                           ((lblShot->height() / 2) + lblShot->y()) > alienTest->y())

                        {
                            if (ui->cbSound->isChecked())
                            {
                                ripAsteroid->play();
                            }
                            Game::instance()->Score() += 100;
                            updateScore();
                            Game::instance()->deleteAlien(alienTest->getAlien()->getID());
                            alienTest->deleteLater();
                            Game::instance()->deleteShot(lblShot->getShot()->getID());
                            lblShot->deleteLater();
                            if(noEnemiesLeft()) {
                                advanceLevel();
                            }
                        }
                    }
                }
            }
        }
        //Updates an Enemy's position
        EnemyLabel *lblEnemy = dynamic_cast<EnemyLabel *>(lbl);
        if (lblEnemy != nullptr)
        {
            lblEnemy->move(lblEnemy->getEnemy()->getX(), lblEnemy->getEnemy()->getY());
        }
    }
    QString msg = "";
    for (size_t i = 0; i < serverMsgs.size(); i++)
    {
        msg += serverMsgs[i].simplified() + "/";
    }
    serverMsgs.clear();
    if (msg != "")
    {
        if (ui->rbServer->isChecked())
        {
            for (QObject *obj : server->children()) {
                QTcpSocket *anotherSock = dynamic_cast<QTcpSocket*>(obj);
                if (anotherSock != nullptr) {
                    anotherSock->write(msg.toLocal8Bit());
                }
            }
        }
        else if (ui->rbClient->isChecked())
        {
            socket->write(msg.toLocal8Bit());
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
    case Qt::Key_Escape:
        gotoMenu();
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
    vector<Enemy*> enemies = Game::instance()->getEnemies();
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
    for (size_t i = 0; i < enemies.size(); i++)
    {
        msg = QString::fromStdString(enemies[i]->getSave());
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
        // Initialize server:
        server = new QTcpServer(this);
        connect(server, SIGNAL(newConnection()), this, SLOT(clientConnected()));
        if (!server->listen(QHostAddress::Any, 5000)) {
            QMessageBox::critical(this, "Uh oh", "Cannot start socket.");
            return;
        }
        // Player set-up
        int immunity;
        if (ui->cbCheatMode->isChecked())
        {
            immunity = -1;
        }
        else
        {
            immunity = 40;
        }
        Game::instance()->addPlayer(380, 190, "serverPlayer", immunity);
        Game::instance()->getPlayer("serverPlayer")->setPixmapName(MainWindow::shipSelect());
        PlayerLabel *lblPlayer = new PlayerLabel(ui->centralWidget);
        lblPlayer->setPlayer(Game::instance()->getPlayer("serverPlayer"));
        lblPlayer->playerGen();
    }
    else if (ui->rbClient->isChecked())
    {
        QString userName = ui->lnPeerName->text();
        if ((userName.simplified() == "serverPlayer") || userName.contains(','))
        {
            QMessageBox::critical(this, "Error", "Invalid username (cannot contain commas and cannot be 'serverPlayer').");
            return;
        }
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
        int immunity;
        if (ui->cbCheatMode->isChecked())
        {
            immunity = -1;
        }
        else
        {
            immunity = 40;
        }
        Game::instance()->addPlayer(380, 190, ui->lnPeerName->text(), immunity);
        Game::instance()->getPlayer(ui->lnPeerName->text())->setPixmapName(MainWindow::shipSelect());
        PlayerLabel *lblPlayer = new PlayerLabel(ui->centralWidget);
        lblPlayer->setPlayer(Game::instance()->getPlayer(ui->lnPeerName->text()));
        lblPlayer->playerGen();
        QString shipSelect = MainWindow::shipSelect();
        socket->write(userName.toLocal8Bit());
        socket->write(shipSelect.toLocal8Bit());
    }
    else
    {
        // Player set-up
        int immunity;
        if (ui->cbCheatMode->isChecked())
        {
            immunity = -1;
        }
        else
        {
            immunity = 40;
        }
        Game::instance()->addPlayer(380, 190, "localPlayer", immunity);
        Game::instance()->getPlayer("localPlayer")->setPixmapName(MainWindow::shipSelect());
        PlayerLabel *lblPlayer = new PlayerLabel(ui->centralWidget);
        lblPlayer->setPlayer(Game::instance()->getPlayer("localPlayer"));
        lblPlayer->playerGen();
    }

    // Hide Menu GUI
    MainWindow::hideGUI();

    // Show score labels
    ui->lblHighscoreText->show();
    ui->lblHighScore->show();
    ui->lblScore->show();

    // Enemy set-up
    Game::instance()->NumEnemy() = 5; //This amount for level 1 and PoC purposes
    makeEnemies(Game::instance()->NumEnemy());

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

    timer->start();
}
