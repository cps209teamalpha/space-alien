#ifndef GAME_H
#define GAME_H

#include <QString>
#include <vector>
#include <fstream>

#include "player.h"
#include "alien.h"
#include "boss.h"
#include "enemyspawn.h"

using namespace std;

class SavableObject
{
public:
    virtual void load() = 0;
    virtual void save() = 0;
};

vector<string> splitString(string input, char delim);
vector<QString> splitQString(QString input, char delim);

// The game object contains all the data about player position, enemy position,
// score, etc.
// Because there will only be one game running in a single program, the
// class lends itself to being a Singleton.

class Highscores;

class Shot
{
private:
    int id;
    bool isAlienShot; //used to differentiate between player lasers and alien lasers
    int x,y;

    int angle;
public:
    Shot(int startX, int startY, int startAngle, int ID, bool alienShot): x(startX), y(startY), angle(startAngle), id(ID), isAlienShot(alienShot) { }

    int getX() { return x; }
    int getY() { return y; }
    int getID() { return id; }
    //obvious mutator and accessor methods
    void setIsAlienShot(bool set) { isAlienShot = set; }
    bool getIsAlienShot() { return isAlienShot; }
    string getSave();

    void move();
};

class Game : SavableObject
{
private:
    int currentLevel = 1; // don't know if this is the right place to put it
    int numEnemy = 5;

    Highscores *highscores;
    // All the players in the game.
    // Normally one; if network support
    // is enabled, more will be added.
    vector<Player*> players;

    vector<Alien*> aliens;

    vector<Boss*> bosses;

    vector<Shot*> shots;

    vector<Enemy*> enemies;

    int nextShot;
    int nextEnemy;
    int nextAlien; // Generates unique ids for the shots, enemies, and aliens

    int shotTimer;

    int untrackedShots;

    Game();
public:
    // Wipes the current game and reinstantiates
    // all the member variables
    void newGame();
    // Updates player & enemies, for use with a timer
    void updateField();

    int &Num_enemy() { return numEnemy; }
    int &CurrentLevel() { return currentLevel; }
    int getCurrentEnemies() { return (int)enemies.size(); }

    void addBoss();
    void addBoss(double, double);

    void addAlien(int rotation);
    void addNewAlien(int x, int y, int id, int rotation) { aliens.push_back(new Alien(x, y, id, rotation)); }
    void addOldShot(int x, int y, int angle, int id, bool alienShot) { shots.push_back(new Shot(x, y, angle, id, alienShot)); }
    void addOldEnemy(int x, int y, int deltaX, int deltaY, int id) { enemies.push_back(new Enemy(x, y, deltaX, deltaY, id)); }
    void addPlayer(int x, int y, QString name, int imTimer) { players.push_back(new Player(x, y, name, imTimer)); }

    void addUntrackedShot() { untrackedShots++; }

    void setUntrackedShots(int amount) { untrackedShots = amount; }

    int getUntrackedShots() { return untrackedShots; }

    int getShotTimer() { return shotTimer; }

    void addShot(int origX, int origY, int angle, bool isShotAlien)
    {
        shots.push_back(new Shot(origX, origY, angle, nextShot, isShotAlien));
        nextShot++;
    }

    void addEnemy(int origX, int origY, int deltaX, int deltaY)
    {
        enemies.push_back(new Enemy(origX, origY, deltaX, deltaY, nextEnemy));
        nextEnemy++;
    }

    void deleteShot(int shotID);
    void deleteEnemy(int enemyID);
    void deleteAlien(int alienID);
    void deletePlayer(QString playerName);

    Shot *getLastShot() { return shots[shots.size() - 1]; }
    Enemy *getLastEnemy() { return enemies[enemies.size() - 1]; }

    Shot *getShot(int id);
    Alien *getAlien(int id);
    Enemy *getEnemy(int id);

    vector<Player*> getPlayers() { return players; }
    vector<Shot*> getShots() { return shots; }

    Player *getPlayer(QString name);

    vector<Alien*> getAliens() { return aliens; }

    vector<Enemy*> getEnemies() { return enemies; }

    vector<Boss*> getBosses() { return bosses; }

    // Methods to save/load game.
    // Reads file and instantiates member variables with the info
    void load();
    // Writes game to file
    void save();

    // Delete any pointers
    ~Game();

    // Singleton implementation:
private:
    static Game *instance_;
public:
    static Game *instance();
};

class Highscores : SavableObject
{
private:
    vector<int> scores; // An ordered list of scores, from highest to lowest
public:
    Highscores();

    // Tests <score> to see if it's higher than
    // the last score in the list. If so, it will
    // be inserted at the proper spot and the last
    // score in the list will be deleted.
    // In case it's needed, the method will return
    // whether the score was added to the list or
    // not. If it's unused, change the return type
    // to void.
    bool addScore(int score);

    // Load the highscores from a file.
    // Normally called by the constructor itself,
    // but can be forced when necessary.
    void load();
    // Save the highscores to a file.
    void save();
};

#endif // GAME_H
