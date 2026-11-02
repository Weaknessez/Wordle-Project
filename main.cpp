#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <string>
#include <algorithm>
#include <cctype>

// Constants
const int MAX_GUESSES = 6;
const int LETTER_COUNT = 5;
const std::string TEXT_FILE_NAME = "words_alpha.txt";

// ANSI Color codes
//const std::string RESET = "\033[0m";
const std::string BLACK = "\033[40m";
const std::string YELLOW = "\033[43m";
const std::string GREEN = "\033[42m";

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

bool playGame(const std::vector<std::string>& words, const std::string& random_word) {
    std::string user_guess;
    std::vector<std::string> colors(LETTER_COUNT, BLACK);
    std::vector<bool> used(LETTER_COUNT, false);
    int guesses_count = 0;

    // For testing/debugging; prints the random word
    std::cout << random_word << std::endl;
    std::cout << "\n\033[1mWordle by Yoosuuuff\e[0m\n\n";

    while (guesses_count < MAX_GUESSES)
    {
        std::cout << "Guesses: " << guesses_count << "/" << MAX_GUESSES << "\n\n";

        std::cout << "Enter in a 5 letter guess: ";
        std::cin >> user_guess;

        if (user_guess.length() != LETTER_COUNT) {
            std::cout << std::endl << "Guess not 5 letters long! Please try again" << std::endl;
            continue;
        }

        auto it = std::find_if(words.begin(), words.end(), [&](const std::string& word) {
            return icompare(word, user_guess);
        });

        if (it == words.end())
        {
            std::cout << std::endl << "Guess is not in \"" << TEXT_FILE_NAME << "\". Please try again" << std::endl;
            continue;
        }

        std::string random_word_lower = lowerString(random_word);
        std::string user_guess_lower = lowerString(user_guess);

        for (size_t i = 0; i < LETTER_COUNT; i++) {
            // apple and ppppp
            colors[i] = BLACK;
            used[i] = false;

            if (random_word_lower[i] == user_guess_lower[i]) {
                colors[i] = GREEN;
                used[i] = true;
            }
        }

        for (size_t i = 0; i < LETTER_COUNT; i++) {
            if (colors[i] == GREEN) { continue; }

            for (size_t x = 0; x < LETTER_COUNT; x++) {
                if (user_guess_lower[i] == random_word_lower[x] && !used[x]) {
                    colors[i] = YELLOW;
                    used[x] = true;
                    break;
                }
            }
        }

        std::cout << '\n';

        std::cout << "\n\nCurrent word: ";
        for (size_t i = 0; i < user_guess.length(); i++) {
            printColoredLetter(user_guess[i], colors[i]);
        }
        std::cout << "\n\n";

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
    std::vector<std::string> words = loadWords(TEXT_FILE_NAME);

    if (words.size() <= 0) {
        std::cerr << "No 5 letter words found in \"" << TEXT_FILE_NAME << "\"" << std::endl;
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