#include "player.h"

using namespace std;

// Returns a string to be saved to the save file
string Player::getSave()
{
    string result = "P";
    result += to_string(x);
    result += ",";
    result += to_string(y);
    result += ",";
    result += to_string(rotation);
    result += ",";
    result += to_string(speed);
    result += ",";
    result += to_string(angle);
    result += ",";
    result += peerName.toStdString();
    result += ",";
    result += pixmapName.toStdString();
    result += ",";
    result += to_string(immuneTimer);
    result += "\n";
    return result;
}

// Updates the player's position based on speed and direction
void Player::move()
{
    if (immuneTimer > 0)
    {
        immuneTimer--;
    }
    else if (immuneTimer == 0) // Deliberately '==' instead of '<='. If immuneTimer < 0, you stay immune forever.
    {
        isImmune = false;
    }

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

//Creates a phaser
Phaser::Phaser(QWidget *parent, double init_angle, double initx, double inity): QLabel(parent) {

    x = initx;
    y = inity;

    angle = init_angle;
    rad = qDegreesToRadians(angle);
    speed = 20;

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

    dx = xInc * 5;
    dy = yInc * 5;

}

//Generate phaser on window
void Phaser::phaserGen(Phaser *pew, QPixmap bullet)
{
    pew->setPixmap(bullet);
    pew->setGeometry(QRect(pew->getX(), pew->getY(), 20, 20));
    pew->setScaledContents(true);
    pew->setAttribute(Qt::WA_TranslucentBackground, true);
    pew->show();
}

//Updates the current phaser's position
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

