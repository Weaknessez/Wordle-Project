#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <string>

int main()
{
    std::ifstream words_file("words.txt");

    if (!words_file.is_open())
    {
        std::cerr << "Couldn't open file!" << std::endl;
        return 1;
    }

    std::vector<std::string> words;

    std::string word;
    while (words_file >> word)
    {
        if (word.length() == 5)
        {
            words.push_back(word);
        }
    }

    std::cout << "Succesfully loaded " << words.size() << " words!" << std::endl;

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<size_t> distr(0, words.size() - 1);

    size_t random_index = distr(gen);
    std::string random_item = words[random_index];

    std::cout << "Randomly chose " << random_item << " as a word!" << std::endl;

    words_file.close();
    return 0;
}