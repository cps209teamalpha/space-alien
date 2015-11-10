#ifndef BOSS_H
#define BOSS_H

class Boss {
    double x, y;

public:
    Boss();
    Boss(double, double);

    double getX() const { return x; }
    double getY() const { return y; }
};

#endif // BOSS_H
