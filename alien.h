#ifndef ALIEN_H
#define ALIEN_H

#include <string>

class Alien
{
private:
    int id; // The unique id of the alien
    int rotation; // The direction the alien is pointing (every 90 degrees)
    int x, y; // x, y = current location
    int timedShot;
public:
    Alien(int startX, int startY, int newId, int rot): x(startX), y(startY), id(newId), rotation(rot), timedShot(0) { }

    // Accessor methods (mostly for debugging)
    int getX() { return x; }
    int getY() { return y; }
    int getRot() { return rotation; }
    int getID() { return id; }

    void setTimedShot(int ts) { timedShot = ts; }

    std::string getSave();

    // Updates the alien's position
    void move();
    // Fires a shot towards the player
    void shoot();

};

#endif // ALIEN_H
