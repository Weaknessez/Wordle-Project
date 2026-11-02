#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <string>
#include <algorithm>
#include <cctype>

// Constants
const int MAX_GUESSES = 6;
const std::string TEXT_FILE_NAME = "words_alpha.txt";

// ANSI Color codes
//const std::string RESET = "\e[0m";
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

std::string printColoredLetter(const char& letter, const std::string& ANSIColorCode) {
    return ANSIColorCode + letter + "\e[0m";
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
        if (word.length() == 5) {
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

bool playGame(const std::vector<std::string>& words, const std::string& random_word) {
    std::string user_guess;
    int guesses_count = 0;

    // For testing/debugging; prints the random word
    std::cout << random_word << std::endl;

    while (guesses_count < MAX_GUESSES)
    {
        std::cout << "\n\033[1mWordle\e[0m\n\nGuesses: " << guesses_count << "/" << MAX_GUESSES << std::endl << std::endl;

        std::cout << "Enter in a 5 letter guess: ";
        std::cin >> user_guess;

        if (user_guess.length() != 5) {
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

        for (size_t i = 0; i < random_word.length(); ++i)
        {
            char rChar = static_cast<char>(std::tolower(static_cast<unsigned char>(random_word[i])));
            char uChar = static_cast<char>(std::tolower(static_cast<unsigned char>(user_guess[i])));

            if (rChar == uChar) {
                // TODO: I dont know lowkey, find a way to color them green or save this index at the random_word[i] to be green and output it before.
            }
        }

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