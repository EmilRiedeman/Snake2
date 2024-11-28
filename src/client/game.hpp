#pragma once

#include "../snake.hpp"
#include "../timer.hpp"

#include "visuals.hpp"

struct App;

struct SinglePlayerSettings {
    std::size_t width = 10;
    std::size_t height = 9;
    SnakeSkin skin{};
};

class SinglePlayerGame {
public:
    explicit SinglePlayerGame(const SinglePlayerSettings &settings)
        : grid(settings.width, settings.height), player(grid, Position{grid.get_height() / 2, 4}, settings.skin) {}

    void update(double time);
    void poll_events(double time);
    void render(double time);

    const SnakeGrid &get_grid() const { return grid; }

private:
    SnakeGrid grid;
    VisualSnake player;
};

struct GameContext {
    explicit GameContext(const SinglePlayerSettings &settings)
        : game(settings), settings(settings) { timer.start(); }

    void update_and_render(App &app);

private:
    void update(double time);
    void poll_events(const double time);
    void render(double time, App &app);

    Timer timer{};
    SinglePlayerGame game;
    SinglePlayerSettings settings;
};
