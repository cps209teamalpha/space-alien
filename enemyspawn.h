#ifndef ENEMYSPAWN_H
#define ENEMYSPAWN_H

#include <QString>
#include <QLabel>
#include <QDebug>
#include <string>
#include <random>

#include <string>

class Enemy
{
private:
    int id; // The unique id of the enemy
    int x, y; // x, y = current location
    int dX, dY; // deltaX, deltaY = the increase/decrease every tick
public:
    Enemy(int startX, int startY, int deltaX, int deltaY, int newId): x(startX), y(startY), dX(deltaX), dY(deltaY), id(newId) { }

    // Accessor methods
    int getX() { return x; }
    int getY() { return y; }
    int getDeltaX() { return dX; }
    int getDeltaY() { return dY; }
    int getID() { return id; }

    std::string getSave();

    // Updates the enemy's position
    void move();
};

class EnemyLabel : public QLabel
{
    Q_OBJECT

    Enemy *myEnemy;

public:

    explicit EnemyLabel(QWidget *parent): QLabel(parent) { }
    Enemy *getEnemy() { return myEnemy; }
    void setEnemy(Enemy *newEnemy) { myEnemy = newEnemy; }

    void enemyGen();

    ~EnemyLabel() { }
};
//EnemyLabel Class


#endif
