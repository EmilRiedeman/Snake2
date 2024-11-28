#pragma once

#include "game.hpp"

#include <variant>

struct App;

struct MenuState;

struct MainMenu {
    void render(MenuState &menu_state, App &app);
};

struct SinglePlayerMenu {
    SinglePlayerSettings settings;

    void render(MenuState &menu_state, App &app);
};

struct MenuState {
    void render(App &app);

    std::variant<MainMenu, SinglePlayerMenu> state = MainMenu{};
};

