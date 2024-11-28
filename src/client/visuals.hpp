#pragma once

#include "../snake.hpp"

#include <cstddef>
#include <span>
#include <optional>

#include "raylib.h"

struct SnakeSkin {
    float body_hue = 240;
    float body_saturation = 0.6;
    float head_brightness = 0.9;
    float tail_brightness = 0.5;
    float max_body_size = 0.8;
    float min_body_size = 0.4;

    float calculate_body_size(std::size_t i, float square_size) const;
    Color calculate_body_color(std::size_t i, float interpolate_time) const;
};

class VisualSnake {
public:
    VisualSnake(SnakeGrid &grid, const Position &position, SnakeSkin skin) : snake(grid, position), skin(std::move(skin)) {}

    void render(const SnakeGrid &grid, Vector2 offset, float square_size, double time) const;

    void update(SnakeGrid &grid, double time);

    Snake snake;
private:
    SnakeSkin skin;
    double last_update = 0;
};

void render_checker_board(Vector2 offset, std::size_t width, std::size_t height, float square_size, Color color1, Color color2);

void render_fruit(const Position &fruit, Vector2 offset, float square_size, double time);

void render_snake_body(std::span<const Position> body, const SnakeSkin &skin, Vector2 offset, float square_size, double interpolate_time);

void render_snake_head(std::span<const Position> body, Direction next_direction, const SnakeSkin &skin, std::optional<Position> fruit_target, Vector2 offset, float square_size, double interpolate_time);

void render_snake_tail(std::span<const Position> body, const Position &prev_tail_position, const SnakeSkin &skin, Vector2 offset, float square_size, double interpolate_time);

inline void render_snake(std::span<const Position> body, Direction next_direction, const Position &prev_tail_position, const SnakeSkin &skin, Vector2 offset, float square_size, std::optional<Position> fruit_target = std::nullopt, double interpolate_time = 0) {
    render_snake_tail(body, prev_tail_position, skin, offset, square_size, interpolate_time);
    render_snake_body(body, skin, offset, square_size, interpolate_time);
    render_snake_head(body, next_direction, skin, fruit_target, offset, square_size, interpolate_time);
}
