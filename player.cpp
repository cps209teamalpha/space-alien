#include <math.h>

#include "player.h"

#include <QDebug>
#include <QtMath>

using namespace std;

// Updates the player's position based on speed and direction
void Player::move()
{
    double yInc;
    if (angle >= 90 && angle <= 270)
    {
        yInc = sqrt(speed + (speed * sin(angle * M_PI / 180)));
    }
    else
    {
        yInc = -1 * sqrt(speed - (speed * sin(angle * M_PI / 180)));
    }
    double xInc = speed * sin(angle * M_PI / 180);
    // qDebug() << "Angle: " << angle << "(sin " << sin(angle * M_PI / 180) << ") yInc: " << yInc << "xInc: " << xInc << endl;
    x += xInc;
    y += yInc;

    if (x >= 800)
    {
        x -= 800;
    }
    else if (x <= 0)
    {
        x += 800;
    }

    if (y >= 573)
    {
        y -= 573;
    }
    else if (y <= 0)
    {
        y += 573;
    }
}

// Rotates the ship to the left
void Player::turnLeft()
{
    rotation -= 5;
    if (rotation < 0)
    {
        rotation = 355;
    }
}

// Rotates the ship to the right
void Player::turnRight()
{
    rotation += 5;
    if (rotation > 359)
    {
        rotation = 0;
    }
}

// Increases the ship's speed
void Player::accelerate()
{
    // A simplification of the method.
    // This will cancel the ship's velocity and
    // replace it with an accelerated velocity in
    // the direction the ship is facing.
    // The finished product will calculate a new
    // angle in between the last angle and the
    // the current rotation, factoring in how fast
    // you were traveling in the previous direction
    // and implement angle and speed accordingly.
    speed++;
    if (speed > 15)
        speed = 15;
    angle = rotation;
    /*if (speed == 0)
    {
        angle = rotation;
        speed += 1;
    }
    else
    {
        // Ship is already moving; do calculations to change the
        // angle of the drift and add speed in a new direction
    }*/
}

// Decreases the ship's speed
void Player::decelerate()
{
    // Not sure yet if this will
    // simply reduce the speed
    // variable or if it will
    // apply a stopping force
    // in the opposite direction
    // from the one the ship is facing in
    // (i.e., a ship traveling southwest and
    // facing west will end up only traveling
    // south).
    speed--;
    if (speed < 0)
        speed = 0;
}

Phaser::Phaser(QWidget *parent, double init_angle, double initx, double inity): QLabel(parent) {

    x = initx;
    y = inity;

    angle = init_angle;
    rad = qDegreesToRadians(angle);
    speed = 15;

    double yInc;
    if (angle >= 90 && angle <= 270)
    {
        yInc = sqrt(speed + (speed * sin(angle * M_PI / 180)));
    }
    else
    {
        yInc = -1 * sqrt(speed - (speed * sin(angle * M_PI / 180)));
    }
    double xInc = speed * sin(angle * M_PI / 180);

    dx = xInc * 6;
    dy = yInc * 6;

}

void Phaser::updatePhaser(Phaser *lblPew)
{
    double x = 0;
    double y = 0;
    x = lblPew->getX() + lblPew->getDX();
    y = lblPew->getY() + lblPew->getDY();
    lblPew->setX(x);
    lblPew->setY(y);
    lblPew->move(int(x), int(y));
    if (lblPew->getX() >= 800)
    {
        lblPew->deleteLater();
        qDebug() << "Phaser is deleted" << endl;
    }

    else if (lblPew->getY() >= 573)
    {
        lblPew->deleteLater();
        qDebug() << "Phaser is deleted" << endl;
    }
}

