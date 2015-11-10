#include "enemyspawn.h"

using namespace std;

//Generates a new enemy
void Enemy::enemyGen(QPixmap evil, Enemy *alien, int label_left, int label_top)
{
    alien->setPixmap(evil);
    alien->setGeometry(QRect(label_left, label_top, 32, 32));
    alien->setScaledContents(true);
    alien->setAttribute(Qt::WA_TranslucentBackground, true);
    alien->show();
}

//Updates an Enemy's current position
void Enemy::updateEnemy(Enemy *lblEnemy)
{
    int deltaX = lblEnemy->getDeltaX();
    int deltaY = lblEnemy->getDeltaY();
    lblEnemy->move(lblEnemy->x() + deltaX, lblEnemy->y() + deltaY);

    //Bounds Checking
    if (lblEnemy->x() == 800)
    {
        lblEnemy->move(lblEnemy->x() - 800, lblEnemy->y() + 0);
    }
    else if (lblEnemy->x() == 0)
    {
        lblEnemy->move(lblEnemy->x() + 800, lblEnemy->y() + 0);
    }

    if (lblEnemy->y() == 573)
    {
        lblEnemy->move(lblEnemy->x() + 0, lblEnemy->y() - 573);
    }
    else if (lblEnemy->y() == 0)
    {
        lblEnemy->move(lblEnemy->x() + 0, lblEnemy->y() + 573);
    }
}
