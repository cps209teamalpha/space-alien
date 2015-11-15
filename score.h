#ifndef SCORE_H
#define SCORE_H

#include <fstream>
#include <string>

class Score
{
    static std::size_t value;
    static std::size_t hsValue;

public:
    Score();

    // TODO: Italo is waiting on difficulty levels
    // flesh out highscore class finally.

    static void reset();
    static void load();
    static void load(std::ifstream&);
    static void load(std::string);

    static std::size_t &Value()          { return value;   }
    static std::size_t &HighscoreValue() { return hsValue; }

};

#endif // SCORE_H
