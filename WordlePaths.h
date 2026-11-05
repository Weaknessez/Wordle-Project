#pragma once

#include <filesystem>
#include <string>
#include <vector>

inline std::string findWordleDictionary(const char* argv0) {
    namespace fs = std::filesystem;
    std::vector<fs::path> candidates;

    if (argv0 != nullptr && argv0[0] != '\0') {
        std::error_code error;
        fs::path executable = fs::absolute(fs::path(argv0), error);
        if (!error) {
            fs::path directory = executable.parent_path();
            candidates.push_back(directory / "WordTextCollections" / "words_alpha.txt");
#ifdef __APPLE__
            candidates.push_back(directory / ".." / "Resources" / "WordTextCollections" / "words_alpha.txt");
#endif
        }
    }

    candidates.emplace_back("WordTextCollections/words_alpha.txt");

    for (const auto& path : candidates) {
        std::error_code error;
        if (fs::is_regular_file(path, error)) return path.string();
    }
    return candidates.back().string(); // Existing error message handles a missing file.
}
