#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <string>
#include <algorithm>
#include <cctype>

const int MAX_GUESSES = 6;

bool icompare(const std::string& str1, const std::string& str2) {
    if (str1.length() != str2.length()) {
        return false;
    }
    
    return std::equal(str1.begin(), str1.end(), str2.begin(), [](char a, char b) {
        return std::tolower(static_cast<unsigned char>(a)) == 
               std::tolower(static_cast<unsigned char>(b));
    });
}

std::string getRandomWord(std::vector<std::string> words)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<size_t> distr(0, words.size() - 1);

    size_t random_index = distr(gen);

    return words[random_index];
}

int main()
{
    std::ifstream words_file("words.txt");

    if (!words_file.is_open()) {
        std::cerr << "Couldn't open file!" << std::endl;
        return 1;
    }

    std::vector<std::string> words;

    std::string word;
    while (words_file >> word) {
        if (word.length() == 5) {
            words.push_back(word);
        }
    }

    if (words.size() <= 0) {
        std::cerr << "Couldn't load any words! Please make sure each word is 5 letters long." << std::endl;
        return 1;
    }

    std::cout << "Succesfully loaded " << words.size() << " words!" << std::endl << std::endl;

    std::string random_word = getRandomWord(words);
    std::string user_guess;
    int guesses_count = 0;
    bool game_won = false;

    std::cout << random_word << std::endl;

    while (guesses_count < MAX_GUESSES && !game_won)
    {
        std::cout << "Enter in a 5 letter guess: ";
        std::cin >> user_guess;


        if (user_guess.length() != 5) {
            std::cout << std::endl << "Guess not 5 letters long! Please try again" << std::endl;
            continue;
        }

        guesses_count++;

        if (icompare(user_guess, random_word)) {
            game_won = true;
            break;
        }
        
        if (guesses_count >= MAX_GUESSES) {
            std::cout << std::endl << "Max guesses reached!" << std::endl;
        }
    }

    if (game_won) {
        std::cout << std::endl << "Game won!! You successfully guessed the word!" << std::endl;
    } else {
        std::cout << std::endl << "Game lost, the word was: " << random_word << std::endl;
    }

    return 0;
}