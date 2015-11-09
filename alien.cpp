#include "alien.h"
#include "game.h"

#include <string>

using namespace std;

string Alien::getSave()
{
    string result = "A";
    result += to_string(x);
    result += ",";
    result += to_string(y);
    result += ",";
    result += to_string(rotation);
    result += ",";
    result += to_string(timedShot);
    result += "\n";
    return result;
}

void Alien::move()
{
    switch(rotation)
    {
    case 90:
        y += 3;
        break;
    case 180:
        x -= 3;
        break;
    case 270:
        y -= 3;
        break;
    default:
        x += 3;
    }

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

void Alien::shoot()
{
    if (timedShot == Game::instance()->getShotTimer())
    {
        int shotAngle = rotation + 180;
        if (shotAngle >= 360) shotAngle -= 360;
        Game::instance()->addShot(x, y, shotAngle);
        timedShot = 0;
        Game::instance()->addUntrackedShot();
    }
    else
    {
        timedShot++;
    }
}
