#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <string>
#include <algorithm>
#include <cctype>

const int MAX_GUESSES = 6;
const std::string TEXT_FILE_NAME = "words_alpha.txt";

bool icompare(const std::string& str1, const std::string& str2) {
    if (str1.length() != str2.length()) {
        return false;
    }
    
    return std::equal(str1.begin(), str1.end(), str2.begin(), [](char a, char b) {
        return std::tolower(static_cast<unsigned char>(a)) == 
               std::tolower(static_cast<unsigned char>(b));
    });
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

bool playGame(std::string random_word) {
    std::string user_guess;
    int guesses_count = 0;

    // For testing/debugging; prints the random word
    std::cout << random_word << std::endl;

    while (guesses_count < MAX_GUESSES)
    {
        std::cout << "Enter in a 5 letter guess: ";
        std::cin >> user_guess;


        if (user_guess.length() != 5) {
            std::cout << std::endl << "Guess not 5 letters long! Please try again" << std::endl;
            continue;
        }

        // Pass words as a parameter to see if user_guess is inside of words

        guesses_count++;

        if (icompare(user_guess, random_word)) {
            return true;
            break;
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
    
    bool game_won = playGame(random_word);

    if (game_won) {
        std::cout << std::endl << "Game won!! You successfully guessed the word!" << std::endl;
    } else {
        std::cout << std::endl << "Game lost, the word was: " << random_word << std::endl;
    }

    return 0;
}