#include <vector>
#include "game.h"

#ifndef M_PI
#define M_PI (atan(1) * 4)
#endif

using namespace std;

// Constructor for Game:
// initializes highscore list and initial player object
Game::Game()
{
    highscores = new Highscores();
    players.push_back(new Player(380, 190));
    nextShot = 0;
    shotTimer = 50;
    untrackedShots = 0;
}

void Game::addAlien(int rotation)
{
    int startX, startY;
    switch(rotation)
    {
    case 90:
        startX = 740;
        startY = 5;
        break;
    case 180:
        startX = 740;
        startY = 527;
        break;
    case 270:
        startX = 5;
        startY = 527;
        break;
    default:
        startX = 5;
        startY = 5;
        break;
    }

    aliens.push_back(new Alien(startX, startY, rotation));
}

void Game::addBoss() {
    bosses.emplace_back(new Boss());
}

void Game::addBoss(double X, double Y) {
    bosses.emplace_back(new Boss(X, Y));
}

// Wipes the current game and reinstantiates
// all the member variables
void Game::newGame()
{

}

// Updates player & enemies, for use with a timer
void Game::updateField()
{
    for (size_t i = 0; i < players.size(); i++)
    {
        players[i]->move();
    }
    for (size_t i = 0; i < aliens.size(); i++)
    {
        aliens[i]->move();
        aliens[i]->shoot();
    }
    for (size_t i = 0; i < shots.size(); i++)
    {
        shots[i]->move();
    }
}

// Methods to save/load game.
// Talk to Mr. J about necessity of these
// Reads file and instantiates member variables with the info
void Game::load()
{

}

// Writes game to file
void Game::save()
{

}

void Game::deleteShot(int shotID)
{
    int index = -1;
    for (size_t i = 0; i < shots.size(); i++)
    {
        if (shots[i]->getID() == shotID)
        {
            index = i;
        }
    }
    if (index >= 0)
    {
        delete shots[index];
        shots.erase(shots.begin() + index);
    }
}

// Delete any pointers
Game::~Game()
{
    delete highscores;
    for (size_t i = 0; i < players.size(); i++)
    {
        delete players[i];
    }
    for (size_t i = 0; i < aliens.size(); i++)
    {
        delete aliens[i];
    }
    for (size_t i = 0; i < shots.size(); i++)
    {
        delete shots[i];
    }
}

// Singleton implementation
Game *Game::instance_ = nullptr;
Game *Game::instance()
{
    if (instance_ == nullptr)
    {
        instance_ = new Game();
    }
    return instance_;
}

void Shot::move()
{
    double yInc;
    if (angle >= 90 && angle <= 270)
    {
        yInc = sqrt(30 + (30 * sin(angle * M_PI / 180)));
    }
    else
    {
        yInc = -1 * sqrt(30 - (30 * sin(angle * M_PI / 180)));
    }
    double xInc = 30 * sin(angle * M_PI / 180);
    x += xInc;
    y += yInc;
}

// Default constructor for Highscore object
// Loads the highscores from a file.
// If the file is not found, creates
// new highscore list.
Highscores::Highscores()
{

}

// Tests <score> to see if it's higher than
// the last score in the list. If so, it will
// be inserted at the proper spot and the last
// score in the list will be deleted.
// In case it's needed, the method will return
// whether the score was added to the list or
// not. If it's unused, change the return type
// to void.
bool Highscores::addScore(int score)
{
    return true;
}

// Load the highscores from a file.
// Normally called by the constructor itself,
// but can be forced when necessary.
void Highscores::load()
{

}

// Save the highscores to a file.
void Highscores::save()
{

}
