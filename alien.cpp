#include "alien.h"

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
    // Shoot a shot towards the player
}
