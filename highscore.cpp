#include "highscore.h"

/////////////////////////////////////////

HighScore* HighScore::i = nullptr;

// called sometime before it is needed
bool HighScore::load() {
    if (!fileDoesNotExist())   // If the highscore file is not found,
    {   createDefault();     }   // Create it.

    ifstream scorefile;               // load the "scores" file
    scorefile.open("scores");

    if (scorefile) {                        // If the scores file loaded without having issues, do the following
        while(scorefile){
            unsigned int value;     scorefile >> value;         //grab all value and initial pairs in file
            std::string initials;   scorefile >> initials;

            if(initials != "" && value){
                addScore(value, initials);// and add them to the vector of scores
            }
        }
        scorefile.close();                  // Close the file
        return true;                        // aaand... done!
    } else {
        return false;
    }
}

// If file exists, return true
bool HighScore::fileDoesNotExist()
{
    const std::string SCOREFILE = "scores";
    struct stat buf;
    return stat(SCOREFILE.c_str(), &buf) != -1;
}

// Creates a file containing default high scores ONLY WHEN THE FILE DOES NOT EXIST
void HighScore::createDefault() {
    ofstream scorefile;
    scorefile.open("scores");

    const std::string DEFAULT_CONTENTS = "1000 AAA\n800 AAA\n600 AAA\n400 AAA\n200 AAA";
    scorefile << DEFAULT_CONTENTS;
    scorefile.close();
}

// Completely writes the five scores stored in the vector<Scores>, scores
// to a highscore file, aptly named 'scores'
void HighScore::save() {
    ofstream scorefile;                         // Open 'scores' file
    scorefile.open("scores");
    for(Score s: scores){
        scorefile << s.toString() <<"\n";
    }
    scorefile.close();
}

// Add score to the vector of scores, then sort in order,
// take the lowest score (last in vector) out, and save the new highscores file 'scores'
void HighScore::addScore(unsigned int newValue, std::string newInitials) {
    Score s(newValue, newInitials);
    scores.push_back(s);
    HighScore::sort();
    if (scores.size() > 5) {
        scores.pop_back();
        save();
    }

}

Score &HighScore::getScore(unsigned int pos) {
    return scores.at(pos);
}

// unit test exclusive
std::string HighScore::state() {
    std::stringstream ss;

    ifstream scorefile;                     // load the "scores" file
    scorefile.open("scores");

    for (size_t i = 0; i < scores.size(); ++i)
    {
        Score& s = scores.at(i);
        ss << s.toString() << "\n";
    }

    return ss.str();
}

HighScore::~HighScore() {

}
