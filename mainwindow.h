#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include <QSound>

#include "player.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    int currentLevel = 1; // don't know if this is the right place to put it
    int num_enemy = 5;
    int currentEnemies = 0;

    QTimer *timer = new QTimer(this);
    //creating these now so they don't lag up the game to death
    QSound *pewSound = new QSound(":/images/pew.wav");
    QSound *riperinoPlayerino = new QSound(":/images/ripplayer.wav");
     QSound *ripAsteroid = new QSound(":/images/asteroidexlpode.wav");

    bool upKeyPressed = false;
    bool downKeyPressed = false;
    bool rightKeyPressed = false;
    bool leftKeyPressed = false;
    bool spacebarKeyPressed = false;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    void makeEnemies(int);
    bool noEnemiesLeft();
    void advanceLevel();

private slots:
    void timerHit();

    void on_btnPlay_clicked();

private:
    Ui::MainWindow *ui;
};

// A basic QLabel that also contains a pointer to
// a player object and an original pixmap (for
// rotation purposes)
class PlayerLabel : public QLabel
{
    Q_OBJECT

    int offsetX, offsetY; // The offset (based on image rotation and scaling) between the image's coordinates and the player's coordinates

    Player *myPlayer;

    QPixmap *orig_pixmap;

public:
    explicit PlayerLabel(QWidget *parent): QLabel(parent) {
        if (this->pixmap() != nullptr)
        {
            orig_pixmap = new QPixmap(*this->pixmap());
        }
        offsetX = 0;
        offsetY = 0;
    }
    void playerGen(QPixmap pixmap, PlayerLabel *lblPlayer);

    int getOffsetX() { return offsetX; }
    int getOffsetY() { return offsetY; }
    Player *getPlayer() { return myPlayer; }
    void setPlayer(Player *player) { myPlayer = player; }
    void setOrigPixmap(QPixmap &pixmap) { orig_pixmap = new QPixmap(pixmap); }

    ~PlayerLabel()
    {
        delete myPlayer;
        delete orig_pixmap;
    }

    void rotate(int angle);

};

#endif // MAINWINDOW_H
