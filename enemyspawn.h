#ifndef ENEMYSPAWN_H
#define ENEMYSPAWN_H

#include <QString>
#include <QLabel>
#include <QDebug>
#include <string>
#include <random>

using namespace std;

class Enemy : public QLabel
{
    Q_OBJECT

public:

    int dX, dY;

    explicit Enemy(QWidget *parent, int initX, int initY): QLabel(parent) {

       dX = initX;
       dY = initY;
    }
    void enemyGen(QPixmap evil, Enemy *alien, int label_left, int label_top);
    void updateEnemy(Enemy *lblEnemy);


 int getDeltaX() { return dX; }
 int getDeltaY() { return dY; }

};
//Enemy Class


#endif
