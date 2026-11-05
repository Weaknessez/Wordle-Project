// Wordle++ — small raylib frontend using your existing WordleGame/WordleUtils.
#include "raylib.h"
#include "WordleGame.h"
#include "WordlePaths.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <fstream>
#include <memory>
#include <random>
#include <string>
#include <vector>

constexpr Color UI_BG      = {19, 22, 29, 255};
constexpr Color UI_WHITE   = {240, 242, 247, 255};
constexpr Color UI_GRAY    = {149, 156, 169, 255};
constexpr Color UI_TILE    = {39, 44, 55, 255};
constexpr Color UI_BORDER  = {75, 82, 97, 255};
constexpr Color UI_UNUSED  = {91, 99, 112, 255};
constexpr Color UI_ABSENT  = {37, 42, 51, 255};
constexpr Color UI_GREEN   = {78, 148, 107, 255};
constexpr Color UI_YELLOW  = {181, 150, 67, 255};
constexpr Color UI_ACCENT  = {107, 202, 163, 255};
constexpr Color UI_RED     = {244, 106, 119, 255};

enum class KeyState { Unused = 0, Absent = 1, Present = 2, Correct = 3 };

std::array<KeyState, 26> getKeyboardStates(const std::vector<GuessRecord>& history) {
    std::array<KeyState, 26> keyboard;
    keyboard.fill(KeyState::Unused);

    for (const GuessRecord& record : history) {
        std::size_t length = std::min(record.word.length(), record.results.size());
        for (std::size_t i = 0; i < length; i++) {
            char letter = static_cast<char>(std::toupper(static_cast<unsigned char>(record.word[i])));
            int index = letter - 'A';
            if (index < 0 || index >= 26) continue;

            KeyState newStatus = KeyState::Unused;
            switch (record.results[i]) {
                case LetterResult::Correct: newStatus = KeyState::Correct; break;
                case LetterResult::Present: newStatus = KeyState::Present; break;
                case LetterResult::Absent:  newStatus = KeyState::Absent;  break;
            }

            if (static_cast<int>(newStatus) > static_cast<int>(keyboard[index])) {
                keyboard[index] = newStatus;
            }
        }
    }
    return keyboard;
}

struct App {
    int letters = 5;                 // Settings to apply at the next new game
    int attempts = 6;
    bool settingsOpen = false;
    bool messageIsError = false;
    std::vector<std::string> dictionary;
    std::unique_ptr<WordleGame> game;
    std::string dictionaryPath;
    std::string answer;
    std::string typing;
    std::string message;
    std::mt19937 rng{std::random_device{}()};
};

void startGame(App& app) {
    // WordleGame has a reference to dictionary; destroy game before modifying it.
    app.game.reset();
    app.dictionary.clear();
    app.typing.clear();
    app.answer.clear();
    app.message.clear();
    app.messageIsError = false;

    std::ifstream file(app.dictionaryPath);
    std::string word;
    while (file >> word) {
        if (word.size() == static_cast<std::size_t>(app.letters)) {
            app.dictionary.push_back(word);
        }
    }
    if (app.dictionary.empty()) {
        app.message = "Couldn't load words file. Please change directory path or run from project root.";
        app.messageIsError = true;
        return;
    }

    std::uniform_int_distribution<std::size_t> pick(0, app.dictionary.size() - 1);
    app.answer = app.dictionary[pick(app.rng)];
    app.game = std::make_unique<WordleGame>(app.dictionary, app.answer, app.attempts);
    app.settingsOpen = false;
}

// center=true: the provided X is the horizontal center rather than the left edge.
void drawText(Font font, const std::string& value, float x, float y,
              float size, Color color, bool center = false) {
    constexpr float spacing = 1.0f;
    if (center) x -= MeasureTextEx(font, value.c_str(), size, spacing).x / 2.0f;
    DrawTextEx(font, value.c_str(), {x, y}, size, spacing, color);
}

// Draw a clickable button; returns true only on the mouse-click frame.
bool button(Font font, Rectangle rect, const std::string& label, bool highlighted = false) {
    bool hover = CheckCollisionPointRec(GetMousePosition(), rect);
    Color fill = highlighted ? UI_ACCENT : UI_TILE;
    if (hover) fill = highlighted ? Color{140, 224, 189, 255} : UI_BORDER;
    DrawRectangleRounded(rect, 0.20f, 6, fill);

    constexpr float fontSize = 17.0f;
    Vector2 size = MeasureTextEx(font, label.c_str(), fontSize, 1.0f);
    DrawTextEx(font, label.c_str(),
               {rect.x + (rect.width - size.x) / 2.0f,
                rect.y + (rect.height - size.y) / 2.0f},
               fontSize, 1.0f, highlighted ? UI_BG : UI_WHITE);
    return hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void submitGuess(App& app) {
    if (!app.game || app.game->isGameOver()) return;

    GuessStatus result = app.game->submitGuess(app.typing);
    app.messageIsError = false;
    switch (result) {
        case GuessStatus::InvalidLength:
            app.message = "Not enough letters.";
            app.messageIsError = true;
            break;
        case GuessStatus::NotInDictionary:
            app.message = "That word isn't in the dictionary.";
            app.messageIsError = true;
            break;
        case GuessStatus::Won:
            app.message = "You won!";
            app.typing.clear();
            break;
        case GuessStatus::Accepted:
            app.typing.clear();
            app.message = app.game->isGameOver()
                ? "Out of guesses! Word: " + app.answer : "";
            break;
        case GuessStatus::GameOver:
            break;
    }
}

void handleKeyboard(App& app) {
    if (!app.game || app.game->isGameOver() || app.settingsOpen) return;

    // Only accept A-Z. The visual keyboard is informational, not clickable.
    for (int key = GetCharPressed(); key != 0; key = GetCharPressed()) {
        if (((key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z')) &&
            app.typing.size() < app.game->getLetterCount()) {
            app.typing += static_cast<char>(std::toupper(key));
            if (app.messageIsError) app.message.clear();
        }
    }
    if (IsKeyPressed(KEY_BACKSPACE) && !app.typing.empty()) {
        app.typing.pop_back();
        if (app.messageIsError) app.message.clear();
    }
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) submitGuess(app);
}

// Bottom-anchored keyboard. It's derived entirely from game history
void drawKeyboard(const App& app, Font font, float keyboardTop) {
    if (!app.game) return;
    const std::array<KeyState, 26> states = getKeyboardStates(app.game->getHistory());
    constexpr const char* rows[] = {"QWERTYUIOP", "ASDFGHJKL", "ZXCVBNM"};
    constexpr float gap = 6.0f;
    constexpr float height = 43.0f;
    float width = std::min(43.0f, (GetScreenWidth() - 48.0f - 9 * gap) / 10.0f);

    for (int row = 0; row < 3; row++) {
        std::string keys = rows[row];
        float rowWidth = keys.size() * width + (keys.size() - 1) * gap;
        float left = (GetScreenWidth() - rowWidth) / 2.0f;
        float y = keyboardTop + row * (height + gap);

        for (std::size_t i = 0; i < keys.size(); i++) {
            char letter = keys[i];
            KeyState state = states[letter - 'A'];
            Color fill = UI_UNUSED;
            switch (state) {
                case KeyState::Unused:  fill = UI_UNUSED; break;
                case KeyState::Absent:  fill = UI_ABSENT; break;
                case KeyState::Present: fill = UI_YELLOW; break;
                case KeyState::Correct: fill = UI_GREEN; break;
            }
            Rectangle key{left + i * (width + gap), y, width, height};
            DrawRectangleRounded(key, 0.14f, 5, fill);

            DrawRectangleRoundedLinesEx(key, 0.14f, 5, 1.0f, UI_BORDER);
            drawText(font, std::string(1, letter), key.x + width / 2.0f,
                     key.y + (height - 20.0f) / 2.0f, 20.0f, UI_WHITE, true);
        }
    }
}

// The board gets its own vertical region, leaving space for status,
// Play Again and the keyboard. Both axes are centered within that region.
void drawBoard(App& app, Font font, float keyboardTop) {
    if (!app.game) return;
    int cols = static_cast<int>(app.game->getLetterCount());
    int rows = static_cast<int>(app.game->getMaxGuesses());
    constexpr float gap = 6.0f;
    constexpr float regionTop = 141.0f;
    float regionBottom = keyboardTop - 92.0f;
    float availableHeight = regionBottom - regionTop;

    float tile = std::min({62.0f,
        (GetScreenWidth() - 64.0f - (cols - 1) * gap) / cols,
        (availableHeight - (rows - 1) * gap) / rows});
    float boardWidth = cols * tile + (cols - 1) * gap;
    float boardHeight = rows * tile + (rows - 1) * gap;
    float left = (GetScreenWidth() - boardWidth) / 2.0f;
    float top = regionTop + (availableHeight - boardHeight) / 2.0f;

    const auto& history = app.game->getHistory();
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            Color fill = UI_TILE;
            char letter = ' ';
            if (row < static_cast<int>(history.size())) {
                const GuessRecord& record = history[row];
                letter = record.word[col];
                switch (record.results[col]) {
                    case LetterResult::Correct: fill = UI_GREEN; break;
                    case LetterResult::Present: fill = UI_YELLOW; break;
                    case LetterResult::Absent:  fill = UI_ABSENT; break;
                }
            } else if (row == static_cast<int>(history.size()) &&
                       col < static_cast<int>(app.typing.size())) {
                letter = app.typing[col];
            }

            Rectangle rect{left + col * (tile + gap), top + row * (tile + gap), tile, tile};
            DrawRectangleRounded(rect, 0.10f, 5, fill);
            DrawRectangleRoundedLinesEx(rect, 0.10f, 5, 1.0f, UI_BORDER);
            if (letter != ' ') {
                char upper = static_cast<char>(std::toupper(static_cast<unsigned char>(letter)));
                float fontSize = std::min(30.0f, tile * 0.59f);
                drawText(font, std::string(1, upper), rect.x + tile / 2.0f,
                         rect.y + (tile - fontSize) / 2.0f, fontSize, UI_WHITE, true);
            }
        }
    }

    float messageY = top + boardHeight + 13.0f;
    Color messageColor = app.messageIsError ? UI_RED : (app.game->hasWon() ? UI_GREEN : UI_GRAY);
    drawText(font, app.message, GetScreenWidth() / 2.0f, messageY, 17.0f, messageColor, true);

    if (app.game->isGameOver() && !app.settingsOpen) {
        Rectangle playAgain{GetScreenWidth() / 2.0f - 70.0f, messageY + 30.0f, 140.0f, 37.0f};
        if (button(font, playAgain, "Play Again", true)) startGame(app);
    }
}

void drawSettings(App& app, Font font) {
    float x = GetScreenWidth() - 255.0f;
    constexpr float y = 75.0f;
    Rectangle panel{x, y, 232.0f, 221.0f};
    DrawRectangleRounded(panel, 0.10f, 8, Color{31, 35, 45, 255});
    DrawRectangleRoundedLinesEx(panel, 0.10f, 8, 1.0f, UI_BORDER);

    drawText(font, "Game settings", x + 16.0f, y + 12.0f, 20.0f, UI_WHITE);
    drawText(font, "Letters", x + 16.0f, y + 59.0f, 17.0f, UI_GRAY);
    if (button(font, {x + 122, y + 52, 30, 31}, "-")) app.letters = std::max(3, app.letters - 1);
    drawText(font, std::to_string(app.letters), x + 170, y + 57, 19, UI_WHITE, true);
    if (button(font, {x + 187, y + 52, 30, 31}, "+")) app.letters = std::min(7, app.letters + 1);

    drawText(font, "Guesses", x + 16, y + 106, 17, UI_GRAY);
    if (button(font, {x + 122, y + 99, 30, 31}, "-")) app.attempts = std::max(1, app.attempts - 1);
    drawText(font, std::to_string(app.attempts), x + 170, y + 104, 19, UI_WHITE, true);
    if (button(font, {x + 187, y + 99, 30, 31}, "+")) app.attempts = std::min(10, app.attempts + 1);

    drawText(font, "Applies to the next game", x + 16, y + 143, 14, UI_GRAY);
    if (button(font, {x + 16, y + 171, 200, 37}, "New Game", true)) startGame(app);
}

int main(int argc, char* argv[]) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI);
    InitWindow(900, 820, "Wordle++");
    SetWindowMinSize(660, 700);
    SetTargetFPS(60);

    Font font = GetFontDefault();
    bool loadedFont = false;
    const char* fontPath = "/System/Library/Fonts/Supplemental/Arial.ttf";
    if (FileExists(fontPath)) {
        font = LoadFontEx(fontPath, 36, nullptr, 0);
        loadedFont = font.texture.id != 0;
        if (loadedFont) SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
        else font = GetFontDefault();
    }

    App app;

    (void)argc;
    app.dictionaryPath = findWordleDictionary(argv[0]);
    
    startGame(app);
    while (!WindowShouldClose()) {
        handleKeyboard(app);
        BeginDrawing();
        ClearBackground(UI_BG);

        float centerX = GetScreenWidth() / 2.0f;
        drawText(font, "Wordle++", centerX, 27, 36, UI_WHITE, true);
        drawText(font, "Type a guess and hit Enter", centerX, 79, 18, UI_GRAY, true);
        if (app.game) {
            drawText(font,
                     std::to_string(app.game->getGuessesUsed()) + " / " +
                     std::to_string(app.game->getMaxGuesses()) + " guesses",
                     centerX, 108, 14, UI_GRAY, true);
        }

        if (button(font, {static_cast<float>(GetScreenWidth()) - 132, 25, 108, 37}, "Settings")) {
            app.settingsOpen = !app.settingsOpen;
        }

        float keyboardTop = GetScreenHeight() - 165.0f;
        if (app.game) {
            drawBoard(app, font, keyboardTop);
            drawKeyboard(app, font, keyboardTop);
        } else {
            drawText(font, app.message, centerX, GetScreenHeight() / 2.0f,
                     17, UI_RED, true);
        }
        if (app.settingsOpen) drawSettings(app, font);
        EndDrawing();
    }

    if (loadedFont) UnloadFont(font);
    CloseWindow();
}
