#pragma once

#include "game.hpp"
#include "menu.hpp"

#include <variant>

struct App {
    App();
    ~App();

    void run();

    std::variant<MenuState, GameContext> state = MenuState{};
    bool running = false;
};
