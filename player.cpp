#include <math.h>

#include "player.h"

#include <QDebug>

using namespace std;

// Updates the player's position based on speed and direction
void Player::move()
{

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

    // This code (minus testing bits) will actually go in the move() method
    // in the final game.
    int tempSpeed = 5; // A sample test speed
    int theta = rotation;
    double yInc;
    if (rotation >= 90 && rotation <= 270)
    {
        yInc = sqrt(tempSpeed + (tempSpeed * sin(theta * M_PI / 180)));
    }
    else
    {
        yInc = -1 * sqrt(tempSpeed - (tempSpeed * sin(theta * M_PI / 180)));
    }
    double xInc = tempSpeed * sin(theta * M_PI / 180);
    // qDebug() << "Theta: " << theta << "(sin " << sin(theta * M_PI / 180) << ") yInc: " << yInc << "xInc: " << xInc << endl;
    x += xInc;
    y += yInc;
}

// Decreases the ship's speed
void Player::decelerate()
{

}
