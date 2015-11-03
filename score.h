#ifndef SCORE_H
#define SCORE_H
#include <string>

class Score
{
    unsigned int value;
    std::string initials;

public:
// CONSTRUCTORS
    Score(unsigned int initValue, std::string initInitials) :
        value    (initValue),
        initials (initInitials)
    {}

// ACCESSORS
    unsigned int getValue()          const { return value;       }
    std::string  getInitials()       const { return initials;    }

// MUTATORS
    void setValue   (unsigned int value)   { this->value    = value;    }
    void setInitials(std::string initials) { this->initials = initials; }

    std::string toString() { return std::to_string(value) + " " + initials; }

// OPERATORS FOR std::sort()    { e.g. HighScore::sort(); }
    bool operator <(const Score& that) const { return value < that.value; }
    bool operator >(const Score& that) const { return value > that.value; }

};

#endif // SCORE_H
