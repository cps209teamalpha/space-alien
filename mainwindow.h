#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTimer>

#include "player.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QTimer *timer = new QTimer(this);

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

    Player *myPlayer;

    QPixmap *orig_pixmap;

public:
    explicit PlayerLabel(QWidget *parent): QLabel(parent) {
        if (this->pixmap() != nullptr)
        {
            orig_pixmap = new QPixmap(*this->pixmap());
        }
    }

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
