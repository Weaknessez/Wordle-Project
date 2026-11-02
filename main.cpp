#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <string>
#include <algorithm>
#include <cctype>
#include <cassert>

// Constants
const int MAX_GUESSES = 6;
const int LETTER_COUNT = 5;
const std::string TEXT_FILE_NAME = "words_alpha.txt";

// ANSI Color codes
const std::string RESET = "\033[0m";
const std::string BLACK = "\033[40m";
const std::string YELLOW = "\033[43m";
const std::string GREEN = "\033[42m";


enum class LetterResult {
    Correct,
    Present,
    Absent
};

struct GuessRecord {
    std::string word;
    std::vector<LetterResult> results;
};

bool icompare(const std::string& str1, const std::string& str2) {
    if (str1.length() != str2.length()) {
        return false;
    }
    
    return std::equal(str1.begin(), str1.end(), str2.begin(), [](char a, char b) {
        return std::tolower(static_cast<unsigned char>(a)) == 
               std::tolower(static_cast<unsigned char>(b));
    });
}

void printColoredLetter(const char& letter, const std::string& ANSIColorCode) {
    std::cout << ANSIColorCode << static_cast<char>(std::toupper(letter)) << "\033[0m";
}

void printColoredWord(const std::vector<LetterResult>& letter_results, const std::string& user_guess, const std::size_t& newLineMargins = 2) {
    for (size_t i = 0; i < newLineMargins; i++) { std::cout << std::endl; }

    for (size_t i = 0; i < LETTER_COUNT; i++) {
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

void printAllGuessRecords(const std::vector<GuessRecord>& guess_history, const std::size_t& newLineMargins = 2) {
    for (size_t i = 0; i < newLineMargins; i++) { std::cout << std::endl; }

    for (const GuessRecord& record : guess_history) {
        printColoredWord(record.results, record.word, 0);
        std::cout << std::endl;
    }

    for (size_t i = 0; i < newLineMargins; i++) { std::cout << std::endl; }
}

std::vector<std::string> loadWords(std::string fileName) {
    std::ifstream words_file(fileName);

    std::vector<std::string> words;

    if (!words_file.is_open()) {
        std::cerr << "Couldn't open file!" << std::endl;
        return words;
    }

    std::string word;
    while (words_file >> word) {
        if (word.length() == LETTER_COUNT) {
            words.push_back(word);
        }
    }

    return words;
}

std::string getRandomWord(const std::vector<std::string>& words) {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<size_t> distr(0, words.size() - 1);

    size_t random_index = distr(gen);

    return words[random_index];
}

std::string lowerString(const std::string& text)
{
    std::string word_lower;
    word_lower.reserve(text.size());

    std::transform(text.begin(), text.end(), std::back_inserter(word_lower), [](unsigned char c) {
        return std::tolower(c);
    });

    return word_lower;
}

std::vector<LetterResult> evaluateGuess(const std::string& user_guess, const std::string& word) {
    std::string word_lower = lowerString(word);
    std::string user_guess_lower = lowerString(user_guess);

    std::vector<LetterResult> colors(LETTER_COUNT, LetterResult::Absent);
    std::vector<bool> used(LETTER_COUNT, false);

    // First pass; gets every Correct match with each index
    for (size_t i = 0; i < LETTER_COUNT; i++) {
        if (word_lower[i] == user_guess_lower[i]) {
            colors[i] = LetterResult::Correct;
            used[i] = true;
        }
    }

    // Second pass; gets the Present letters in the word
    for (size_t i = 0; i < LETTER_COUNT; i++) {
        if (colors[i] == LetterResult::Correct) { continue; }

        for (size_t x = 0; x < LETTER_COUNT; x++) {
            if (user_guess_lower[i] == word_lower[x] && !used[x]) {
                colors[i] = LetterResult::Present;
                used[x] = true;
                break;
            }
        }
    }

    return colors;
}

void runTest(const std::string& guess, const std::string& word, const std::vector<LetterResult>& expected) {
    std::vector<LetterResult> result = evaluateGuess(guess, word);

    assert(result.size() == expected.size());

    for (size_t i = 0; i < expected.size(); i++)
    {
        assert(result[i] == expected[i]);
    }
}

void runAllTests() {
    using LR = LetterResult;

    runTest("PPPPP", "APPLE", { LR::Absent, LR::Correct, LR::Correct, LR::Absent, LR::Absent });
    runTest("DIVES", "SQUAD", { LR::Present, LR::Absent, LR::Absent, LR::Absent, LR::Present });
    runTest("PLATE", "PLANT", { LR::Correct, LR::Correct, LR::Correct, LR::Present, LR::Absent });
    runTest("PEEPS", "SHEEP", { LR::Present, LR::Present, LR::Correct, LR::Absent, LR::Present });
    runTest("CRANE", "CRANE", { LR::Correct, LR::Correct, LR::Correct, LR::Correct, LR::Correct });

    std::cout << "\033[1;32mAll assertions passed successfully!\033[0m\n\n";
}

bool playGame(const std::vector<std::string>& words, const std::string& random_word) {
    std::vector<GuessRecord> game_history;
    std::string user_guess;
    int guesses_count = 0;

    // For testing/debugging; prints the random word
    std::cout << random_word << std::endl;

    std::cout << "\n\033[1mWordle by Yusuf; Made using C++\e[0m\n\n";

    while (guesses_count < MAX_GUESSES)
    {
        std::cout << "Guesses: " << guesses_count << "/" << MAX_GUESSES << "\n\n";

        std::cout << "Enter in a " << LETTER_COUNT << " letter guess: ";

        if (!(std::cin >> user_guess)) {
            std::cerr << "Input failed!\n";
            continue;
        }

        if (user_guess.length() != LETTER_COUNT) {
            std::cout << "\n\033[31mGuess is not " << LETTER_COUNT << " letters long! Please try again\033[0m\n\n";
            continue;
        }

        auto it = std::find_if(words.begin(), words.end(), [&](const std::string& word) {
            return icompare(word, user_guess);
        });

        if (it == words.end())
        {
            std::cout << "\n\033[31mGuess is not in \033[33m\"" << TEXT_FILE_NAME << "\"\033[31m.\033[0m\n\n";
            continue;
        }

        std::vector<LetterResult> letter_results = evaluateGuess(user_guess, random_word);
        game_history.push_back({user_guess, letter_results});
        printAllGuessRecords(game_history);

        guesses_count++;

        if (icompare(user_guess, random_word)) {
            return true;
        }
        
        if (guesses_count >= MAX_GUESSES) {
            std::cout << std::endl << "Max guesses reached!" << std::endl;
        }
    }

    return false;
}

int main()
{   
    std::cout << "Running tests...\n";
    runAllTests();


    std::vector<std::string> words = loadWords(TEXT_FILE_NAME);

    if (words.size() <= 0) {
        std::cerr << "No \033[31m" << LETTER_COUNT << " letter words found in \033[33m\"" << TEXT_FILE_NAME << "\"\033[0m" << std::endl;
        return 1;
    }

    std::string random_word = getRandomWord(words);
    
    bool game_won = playGame(words, random_word);

    if (game_won) {
        std::cout << std::endl << "\033[1;32mGame won!! You successfully guessed the word!\033[0m" << std::endl;
    } else {
        std::cout << std::endl << "\033[1;31mGame lost, the word was: \e[1;33m" << random_word << "\033[0m" << std::endl;
    }

    return 0;
}