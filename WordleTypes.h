#pragma once

#include <string>
#include <vector>

enum class LetterResult {
    Correct,
    Present,
    Absent
};

struct GuessRecord {
    std::string word;
    std::vector<LetterResult> results;
};

enum class GameResult {
    Won,
    Lost,
    Quit
};

enum class GuessStatus {
    Accepted,
    Won,
    InvalidLength,
    NotInDictionary,
    GameOver
};