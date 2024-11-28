#include "app.hpp"

#include "ui.hpp"

#include <raygui.h>

constexpr inline int DEFAULT_SCREEN_WIDTH = 800;
constexpr inline int DEFAULT_SCREEN_HEIGHT = 620;

App::App() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, "Snake!");
    SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));
    SetExitKey(0); // Disable ESC key

    GuiSetStyle(DEFAULT, TEXT_SIZE, FONT_SIZE);
}

App::~App() {
    CloseWindow();
}

void App::run() {
    running = true;
    while (running && !WindowShouldClose()) {
        if (IsKeyPressed(KEY_F11)) ToggleBorderlessWindowed();

        if (std::holds_alternative<MenuState>(state)) {
            std::get<MenuState>(state).render(*this);
        } else if (std::holds_alternative<GameContext>(state)) {
            std::get<GameContext>(state).update_and_render(*this);
        }
    }
    running = false;
}
