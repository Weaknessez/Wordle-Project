#include "WordleGame.h"
#include "WordleUtils.h"

#include <algorithm>

WordleGame::WordleGame(
    const std::vector<std::string>& words,
    const std::string& secretWord,
    std::size_t maxGuesses
) : words_(words), secretWord_(secretWord), maxGuesses_(maxGuesses) { }


GuessStatus WordleGame::submitGuess(const std::string& guess) {
    if (isGameOver()) {
        return GuessStatus::GameOver;
    }

    if (guess.length() != secretWord_.length()) {
        return GuessStatus::InvalidLength;
    }

    auto it = std::find_if(words_.begin(), words_.end(), [&](const std::string& word) {
            return icompare(word, guess);
        });

    if (it == words_.end())
    {
        return GuessStatus::NotInDictionary;
    }

    guessHistory_.push_back({guess, evaluateGuess(guess, secretWord_)});

    if (icompare(guess, secretWord_)) {
        won_ = true;
        return GuessStatus::Won;
    }

    return GuessStatus::Accepted;
}


bool WordleGame::hasWon() const {
    return won_;
}

bool WordleGame::isGameOver() const {
    return hasWon() || getGuessesUsed() >= maxGuesses_;
}

std::size_t WordleGame::getMaxGuesses() const {
    return maxGuesses_;
}

std::size_t WordleGame::getGuessesUsed() const {
    return guessHistory_.size();
}

std::size_t WordleGame::getLetterCount() const {
    return secretWord_.length();
}

const std::vector<GuessRecord>& WordleGame::getHistory() const {
    return guessHistory_;
}