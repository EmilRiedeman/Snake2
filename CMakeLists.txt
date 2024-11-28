cmake_minimum_required(VERSION 3.22)
project(SnakeOnline)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(lib/raylib)

add_executable(SnakeOnline
        main.cpp
        src/snake.cpp
        src/client/visuals.cpp
        src/client/app.cpp
        src/client/game.cpp
        src/client/menu.cpp
        lib/raygui/raygui.c
)

target_link_libraries(SnakeOnline PRIVATE raylib_static)
target_include_directories(SnakeOnline PRIVATE lib/raygui)

if (MINGW)
    target_link_libraries(SnakeOnline PRIVATE -static-libgcc -static-libstdc++)
endif()

# if release
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    # if windows, link -mwindows
    if (WIN32)
        target_link_libraries(SnakeOnline PRIVATE -mwindows)
    endif()

    # same for unix
    if (UNIX)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")  # For GUI apps on Linux (optional, platform-specific)
    endif()
endif()