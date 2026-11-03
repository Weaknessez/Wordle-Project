#pragma once

#include <string>
#include <vector>
#include "WordleTypes.h"

std::vector<LetterResult> evaluateGuess(
    const std::string& guess,
    const std::string& answer
);

std::string lowerString(const std::string& text);

bool icompare(
    const std::string& str1,
    const std::string& str2
);