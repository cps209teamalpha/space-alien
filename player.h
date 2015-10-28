#ifndef PLAYER_H
#define PLAYER_H

// The player class. In mp mode, there
// will be multiple player objects, so
// we may as well make it an object from
// the start.

class Player
{
private:
    int rotation; // The direction the ship is pointing (360 degrees)
    double x, y; // x, y = current ship loc - double, to get proper movement. Should be rounded to ints for graphics purposes
    int speed; // the current speed the ship is traveling at (may be rewritten)
    int angle; // the angle the ship is drifting at
public:
    Player(double startX, double startY): x(startX), y(startY), rotation(0), speed(0) { }

    // Accessor methods (mostly for debugging)
    double getX() { return x; }
    double getY() { return y; }
    double getRot() { return rotation; }
    double getAngle() { return angle; }
    double getSpeed() { return speed; }

    // Updates the player's position based on speed and angle
    void move();
    // Rotates the ship to the left
    void turnLeft();
    // Rotates the ship to the right
    void turnRight();
    // Increases the ship's speed
    void accelerate();
    // Decreases the ship's speed
    void decelerate();

    // Save/load methods. Again, not
    // sure how appropriate this is
    // to our game concept.
    //void load();
    //void save();
    // Could not make Player inherit from SavableObject.
    // Kept complaining about 'undefined reference to vtable'
    // Because SavableObject has no constructor that takes
    // two ints.
    // How to fix this???
};

#endif // PLAYER_H
