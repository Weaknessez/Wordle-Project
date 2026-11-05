cmake_minimum_required(VERSION 3.20)
project(WordlePlusPlus VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# A static MSVC runtime reduces the chance Windows players need to
# install a separate Visual C++ redistributable for this project.
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set(USE_MSVC_RUNTIME_LIBRARY_DLL OFF CACHE BOOL "" FORCE)
endif()

option(WORDLE_BUILD_GUI "Build the raylib GUI" ON)
option(WORDLE_USE_SYSTEM_RAYLIB "Use installed raylib via pkg-config" OFF)

add_library(wordle_core STATIC
    WordleGame.cpp
    WordleUtils.cpp
)
target_include_directories(wordle_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(wordle_terminal main.cpp)
target_link_libraries(wordle_terminal PRIVATE wordle_core)
install(TARGETS wordle_terminal RUNTIME DESTINATION .)

if(WORDLE_BUILD_GUI)
    if(WORDLE_USE_SYSTEM_RAYLIB)
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(RAYLIB REQUIRED IMPORTED_TARGET raylib)
        set(WORDLE_RAYLIB_TARGET PkgConfig::RAYLIB)
    else()
        include(FetchContent)
        set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
        if(UNIX AND NOT APPLE)
            set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
            set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)
        endif()
        FetchContent_Declare(raylib
            GIT_REPOSITORY https://github.com/raysan5/raylib.git
            GIT_TAG 6.2
            GIT_SHALLOW TRUE
        )
        FetchContent_MakeAvailable(raylib)
        set(WORDLE_RAYLIB_TARGET raylib)
    endif()

    if(APPLE)
        add_executable(wordle_gui MACOSX_BUNDLE gui/wordle_app_gui.cpp)
        set_target_properties(wordle_gui PROPERTIES
            MACOSX_BUNDLE_BUNDLE_NAME "Wordle++"
            MACOSX_BUNDLE_GUI_IDENTIFIER "com.example.wordleplusplus"
        )
    else()
        add_executable(wordle_gui gui/wordle_app_gui.cpp)
    endif()

    target_link_libraries(wordle_gui PRIVATE wordle_core ${WORDLE_RAYLIB_TARGET})
    install(TARGETS wordle_gui RUNTIME DESTINATION . BUNDLE DESTINATION .)
endif()

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/WordTextCollections" DESTINATION .)

if(APPLE)
    if(WORDLE_BUILD_GUI)
        install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/WordTextCollections"
            DESTINATION "wordle_gui.app/Contents/Resources")
    endif()
    install(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/launchers/Play-Terminal.command" DESTINATION .)
elseif(WIN32)
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/launchers/Play-Terminal.bat" DESTINATION .)
else()
    install(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/launchers/Play-Terminal.sh" DESTINATION .)
endif()
