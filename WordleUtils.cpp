#include "WordleUtils.h"

#include <algorithm>
#include <cctype>
#include <iterator>

std::vector<LetterResult> evaluateGuess(
    const std::string& guess, 
    const std::string& answer
) {
    std::string guess_lower = lowerString(guess);
    std::string answer_lower = lowerString(answer);

    std::vector<LetterResult> colors(answer.length(), LetterResult::Absent);
    std::vector<bool> used(answer.length(), false);

    // First pass; gets every Correct match with each index
    for (size_t i = 0; i < answer.length(); i++) {
        if (guess_lower[i] == answer_lower[i]) {
            colors[i] = LetterResult::Correct;
            used[i] = true;
        }
    }

    // Second pass; gets the Present letters in the word
    for (size_t i = 0; i < answer.length(); i++) {
        if (colors[i] == LetterResult::Correct) { continue; }

        for (size_t x = 0; x < answer.length(); x++) {
            if (guess_lower[i] == answer_lower[x] && !used[x]) {
                colors[i] = LetterResult::Present;
                used[x] = true;
                break;
            }
        }
    }

    return colors;
}

std::string lowerString(const std::string& text) {
    std::string word_lower;
    word_lower.reserve(text.size());

    std::transform(text.begin(), text.end(), std::back_inserter(word_lower), [](unsigned char c) {
        return std::tolower(c);
    });

    return word_lower;
}

bool icompare(
    const std::string& str1,
    const std::string& str2
) {
    if (str1.length() != str2.length()) {
        return false;
    }
    
    return std::equal(str1.begin(), str1.end(), str2.begin(), [](char a, char b) {
        return std::tolower(static_cast<unsigned char>(a)) == 
               std::tolower(static_cast<unsigned char>(b));
    });
}