#include <vector>
#include "game.h"

using namespace std;

// Constructor for Game:
// initializes highscore list and initial player object
Game::Game()
{
    highscores = new Highscores();
    players.push_back(new Player(380, 190));
}

// Wipes the current game and reinstantiates
// all the member variables
void Game::newGame()
{

}

// Updates player & enemies, for use with a timer
void Game::updateField()
{

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

// Delete any pointers
Game::~Game()
{
    delete highscores;
    for (size_t i = 0; i < players.size(); i++)
    {
        delete players[i];
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
