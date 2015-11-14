#ifndef PLAYER_H
#define PLAYER_H

#include <QLabel>
#include <QString>
#include <QDebug>
#include <QtMath>

#include <string>
#include <math.h>

// The player class. In mp mode, there
// will be multiple player objects, so
// we may as well make it an object from
// the start.

class Player
{
private:
    int rotation; // The direction the ship is pointing (360 degrees)
    int x, y; // x, y = current ship loc - int, to get proper movement. Should be rounded to ints for graphics purposes
    int speed; // the current speed the ship is traveling at (may be rewritten)
    int angle; // the angle the ship is drifting at
    QString peerName; // The name of the network client controlling this one
    QString pixmapName; // The chosen pixmap for the ship. Used to transfer across networks

    bool isImmune;
    int immuneTimer;
public:
    Player(int startX, int startY, QString name, int imTime): x(startX), y(startY), peerName(name), rotation(0), speed(0), angle(0), isImmune(true), immuneTimer(imTime) { }

    // Accessor methods (mostly for debugging)
    int getX() { return x; }
    int getY() { return y; }
    int getRot() { return rotation; }
    int getAngle() { return angle; }
    int getSpeed() { return speed; }
    QString getPeerName() { return peerName; }
    QString getPixmapName() { return pixmapName; }

    void setRot(int rot) { rotation = rot; }
    void setAngle(int ang) { angle = ang; }
    void setSpeed(int spd) { speed = spd; }
    void setPixmapName(QString name) { pixmapName = name; }

    std::string getSave();

    // Updates the player's position based on speed and angle
    void move();
    // Rotates the ship to the left
    void turnLeft();
    // Rotates the ship to the right
    void turnRight();
    // Increases the ship's speed
    void accelerate();
    // Decreases the ship's speed
    void decelerate();

    // Save/load methods. Again, not
    // sure how appropriate this is
    // to our game concept.
    //void load();
    //void save();
    // Could not make Player inherit from SavableObject.
    // Kept complaining about 'undefined reference to vtable'
    // Because SavableObject has no constructor that takes
    // two ints.
    // How to fix this???
};

class Phaser : public QLabel
{

private:


public:
    double x, y, dx, dy, rad, speed, angle;
    bool firing;

    explicit Phaser(QWidget *parent,double init_angle, double initx, double inity);
    void updatePhaser(Phaser *lblPew);
    void phaserGen(Phaser *pew, QPixmap bullet);

    double getX() { return x; }
    double getY() { return y; }
    double getDX() { return dx; }
    double getDY() { return dy; }
    double getRad() { return rad; }
    double getSpeed() { return speed; }

    void setX(double initx) { x = initx; }
    void setY(double inity) { y = inity; }
    void setAngle(double init_angle) { angle = init_angle; }

};

#endif // PLAYER_H
