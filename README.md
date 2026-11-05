<div align="center">

# Wordle++

**A customizable Wordle-inspired game built with C++.**  
Play with a simple frontend gui or in a terminal interface.

[Download a release](https://github.com/Weaknessez/Wordle-Project/releases) · [View source](https://github.com/Weaknessez/Wordle-Project) · [Report an issue](https://github.com/Weaknessez/Wordle-Project/issues)

</div>

---

## About

Wordle++ is a personal C++ project that I started as small learning experience using the terminal as the frontend and slightly grew into a graphical desktop application using [raylib](https://www.raylib.com/). Both versions share the same game logic.

The goal is to guess the hidden word before you run out of attempts. Each submitted guess reveals which letters are in the correct position, in the wrong position, or not matched by the answer.

## Features

- **Two ways to play:** a colorful terminal game and a desktop GUI.
- **Custom word lengths:** choose from **3–7 letters**.
- **Adjustable attempts:** **1–10** in the GUI or **1–20** in the terminal version.

### Letter colors

Green -> Correct letter, correct position |
Yellow -> Letter appears in the word, but not in that position |
Dark gray -> No unmatched occurrence of that letter remains in the answer |

## Download and play

Check the **[Releases page](https://github.com/Weaknessez/Wordle-Project/releases)** for packaged downloads.

Windows (64-bit) -> `Wordle-windows-x64.zip` |
macOS (Intel) -> `Wordle-macos-intel.zip` |
macOS (Apple Silicon) -> `Wordle-macos-apple-silicon.zip` |
Linux (64-bit) -> `Wordle-linux-x64.zip` |

1. Download the ZIP for your operating system, then extract it.
2. **GUI:** launch `wordle_gui.exe` on Windows, `wordle_gui.app` on macOS, or `wordle_gui` on Linux.
3. **Terminal:** use the included `Play-Terminal` launcher for your operating system.
4. Keep the included `WordTextCollections` files with the game; they're used to select and validate words.

> **Note:** Releases are published separately from the source code. macOS and Windows may display an unfamiliar-developer warning because these builds are not code-signed. Linux systems may need compatible graphics/system libraries.

## Build from source

**Requirements:**
- A compiler with C++20 support
- [CMake](https://cmake.org/) **3.20+**
- Git and an internet connection for the default raylib source download
- Platform-specific development libraries as required by raylib (particularly on Linux)

Clone the project:

```bash
git clone https://github.com/Weaknessez/Wordle-Project.git
cd Wordle-Project
```

Configure and compile both versions:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

Assemble a playable distribution folder:

```bash
cmake --install build --config Release --prefix dist/Wordle-local
```

The resulting `dist/Wordle-local` folder contains the built applications and word list. Build output filenames and directory layouts vary slightly by operating system.

**Already have raylib installed through MacPorts or another `pkg-config`-compatible setup?** Configure with `-DWORDLE_USE_SYSTEM_RAYLIB=ON` to use that installation instead of downloading raylib's source. You can also build just the terminal version with `-DWORDLE_BUILD_GUI=OFF`.

## Project notes

Wordle++ is a project focused on learning C++, object-oriented concepts, separating logic from presentation, and cross-platform builds. The GUI is built with raylib; the terminal version uses standard C++ and ANSI terminal colors.

Built by [Weaknessez](https://github.com/Weaknessez).
