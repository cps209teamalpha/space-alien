#include "enemyspawn.h"

using namespace std;

void EnemyLabel::enemyGen()
{
    QPixmap pixmap(":/images/asteroid.png");

    setGeometry(getEnemy()->getX(), getEnemy()->getY(), 32, 32);
    setPixmap(pixmap);
    setScaledContents(true);
    setAttribute(Qt::WA_TranslucentBackground, true);
    show();
}

//Updates an Enemy's current position
void Enemy::move()
{
    x += dX;
    y += dY;

    //Bounds Checking
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

string Enemy::getSave()
{
    string result = "E";
    result += to_string(x);
    result += ",";
    result += to_string(y);
    result += ",";
    result += to_string(dX);
    result += ",";
    result += to_string(dY);
    result += ",";
    result += to_string(id);
    result += "\n";
    return result;
}
