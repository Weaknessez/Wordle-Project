#ifndef WORDLEGAME_H
#define WORDLEGAME_H

#include <string>
#include <vector>
#include <cstddef>

#include "WordleTypes.h"

class WordleGame {
private:
    const std::vector<std::string>& words_;
    std::string secretWord_;
    std::vector<GuessRecord> guessHistory_;
    std::size_t maxGuesses_;
    bool won_ = false;
public:
    WordleGame(
        const std::vector<std::string>& words,
        const std::string& secretWord,
        std::size_t maxGuesses
    );

    GuessStatus submitGuess(const std::string& guess);

    bool hasWon() const;
    bool isGameOver() const;

    std::size_t getMaxGuesses() const;
    std::size_t getGuessesUsed() const;
    std::size_t getLetterCount() const;

    const std::vector<GuessRecord>& getHistory() const;
};

#endif