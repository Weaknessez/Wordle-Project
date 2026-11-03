// Libraries
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <string>
#include <algorithm>
#include <cctype>
#include <limits>

// Files
#include "WordleGame.h"
#include "WordleTypes.h"
#include "WordleUtils.h"

// Constants
//const size_t MAX_GUESSES = 6;
const size_t LETTER_COUNT = 5;
const std::string TEXT_FILE_NAME = "words_alpha.txt";

// ANSI Color codes
const std::string RESET = "\033[0m";
const std::string BLACK = "\033[40m";
const std::string YELLOW = "\033[43m";
const std::string GREEN = "\033[42m";

// Function initialization
std::string getRandomWord(const std::vector<std::string>& words);
size_t getInputFromSizeType(const std::string& message, const size_t& minMargin, const size_t& maxMargin);
std::vector<std::string> loadWords(const std::string& fileName, const size_t& letterCount = 5);
void printColoredLetter(const char& letter, const std::string& ANSIColorCode);
void printColoredWord(const std::vector<LetterResult>& letter_results, const std::string& user_guess, const std::size_t& newLineMargins = 2);
void printAllGuessRecords(const std::vector<GuessRecord>& guess_history, const std::size_t& newLineMargins = 2);
bool playGame(WordleGame& wordle);

// Main
int main() {  
    size_t letterCount = getInputFromSizeType("Enter letter count (3-7): ", 3, 7);
    size_t guessesAmount = getInputFromSizeType("Enter amount of guesses wanted (1-20): ", 1, 20);

    std::vector<std::string> words = loadWords(TEXT_FILE_NAME, letterCount);

    if (words.size() <= 0) {
        std::cerr << "No \033[31m" << LETTER_COUNT << " letter words found in \033[33m\"" << TEXT_FILE_NAME << "\"\033[0m" << std::endl;
        return 1;
    }
    
    std::string random_word = getRandomWord(words);

    WordleGame wordle_one(words, random_word, guessesAmount);
    bool game_won = playGame(wordle_one);
    
    if (game_won) {
        std::cout << std::endl << "\033[1;32mGame won!! You successfully guessed the word!\033[0m" << std::endl;
    } else {
        std::cout << std::endl << "\033[1;31mGame lost, the word was: \e[1;33m" << random_word << "\033[0m" << std::endl;
    }

    return 0;
}

// Starts a loop to get each guess and use it in the WordleGame class
bool playGame(WordleGame& wordle) {
    std::cout << "\n\033[1mWordle by Yusuf; (Quit program: \".QUIT\")\e[0m\n\n";

    while (!wordle.isGameOver())
    {
        std::cout << "Guesses: " << wordle.getGuessesUsed() << "/" << wordle.getMaxGuesses() << "\n\n";
        std::cout << "Enter in a " << wordle.getLetterCount() << " letter guess: ";

        std::string guess;
        std::cin >> guess;

        if (guess == ".QUIT") {
            return false;
        }

        GuessStatus submittedGuess = wordle.submitGuess(guess);

        switch (submittedGuess)
        {
            case GuessStatus::Accepted:
                printAllGuessRecords(wordle.getHistory());
                break;

            case GuessStatus::Won:
                printAllGuessRecords(wordle.getHistory());
                return true;

            case GuessStatus::InvalidLength:
                std::cout << "\n\n\033[31mboiii word isn't " << wordle.getLetterCount() << " letters long boi falsity.\033[0m\n\n";
                break;

            case GuessStatus::NotInDictionary:
                std::cout << "\n\n\033[31maint even in the .txt word list try again bakaaa.\033[0m\n\n";
                break;

            case GuessStatus::GameOver:
                return false;
                break;
        }
    }

    return false;
}

// Prints a colored letter to Terminal with an ANSIColorCode
void printColoredLetter(const char& letter, const std::string& ANSIColorCode) {
    std::cout << ANSIColorCode << static_cast<char>(std::toupper(letter)) << "\033[0m";
}

// Prints a colored word to Terminal with multiple printColoredLetter() calls based on LetterResult colors
void printColoredWord(const std::vector<LetterResult>& letter_results, const std::string& user_guess, const std::size_t& newLineMargins) {
    for (size_t i = 0; i < newLineMargins; i++) { std::cout << std::endl; }

    for (size_t i = 0; i < user_guess.length(); i++) {
        if (letter_results[i] == LetterResult::Correct) {
            printColoredLetter(user_guess[i], GREEN);
        } else if (letter_results[i] == LetterResult::Present) {
            printColoredLetter(user_guess[i], YELLOW);
        } else {
            printColoredLetter(user_guess[i], BLACK);
        }
    }

    for (size_t i = 0; i < newLineMargins; i++) { std::cout << std::endl; }
}

// Prints every Guess and its respective letter colors from a GuessRecord to Terminal
void printAllGuessRecords(const std::vector<GuessRecord>& guess_history, const std::size_t& newLineMargins) {
    for (size_t i = 0; i < newLineMargins; i++) { std::cout << std::endl; }

    for (const GuessRecord& record : guess_history) {
        printColoredWord(record.results, record.word, 0);
        std::cout << std::endl;
    }

    for (size_t i = 0; i < newLineMargins; i++) { std::cout << std::endl; }
}

// Loads words from a .txt file based on its length
std::vector<std::string> loadWords(const std::string& fileName, const size_t& letterCount) {
    std::ifstream words_file(fileName);

    std::vector<std::string> words;

    if (!words_file.is_open()) {
        std::cerr << "Couldn't open file!" << std::endl;
        return words;
    }

    std::string word;
    while (words_file >> word) {
        if (word.length() == letterCount) {
            words.push_back(word);
        }
    }

    return words;
}

// Returns a random word from a vector of strings
std::string getRandomWord(const std::vector<std::string>& words) {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<size_t> distr(0, words.size() - 1);

    size_t random_index = distr(gen);

    return words[random_index];
}


size_t getInputFromSizeType(const std::string& message, const size_t& minMargin, const size_t& maxMargin) { 
    std::size_t userInput; 
    
    while (true) { 
        std::cout << message; 
        
        if (std::cin >> userInput) {
            if (userInput <= maxMargin && userInput >= minMargin) { 
                break; 
            } else {
                std::cout << "Error: Input out of bounds (" << minMargin << " - " << maxMargin << ").\n\n";
            }
        } else {
            std::cout << "Error: Invalid input. Please enter a valid number.\n\n";
            
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    } 
    return userInput; 
}