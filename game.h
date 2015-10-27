#ifndef GAME_H
#define GAME_H

#include <vector>

#include "player.h"

using namespace std;

class SavableObject
{
public:
    virtual void load() = 0;
    virtual void save() = 0;
};

// The game object contains all the data about player position, enemy position,
// score, etc.
// Because there will only be one game running in a single program, the
// class lends itself to being a Singleton.

class Highscores;

class Game : SavableObject
{
private:
    Highscores *highscores;
    // All the players in the game.
    // Normally one; if network support
    // is enabled, more will be added.
    vector<Player*> players;

    Game();
public:
    // Wipes the current game and reinstantiates
    // all the member variables
    void newGame();
    // Updates player & enemies, for use with a timer
    void updateField();

    // For debug purposes only:
    Player *getPlayer() { return players[0]; }

    // Methods to save/load game.
    // Talk to Mr. J about necessity of these
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
