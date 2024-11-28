#pragma once

#include <optional>
#include <span>
#include <vector>
#include <variant>

constexpr inline double TPS = 8;
constexpr inline double SPT = 1.0 / TPS;

struct Position {
    std::size_t row;
    std::size_t col;

    bool operator==(const Position &other) const {
        return row == other.row && col == other.col;
    }
};

enum class Direction {
    RIGHT,
    DOWN,
    LEFT,
    UP,
};

class SnakeGrid {
public:
    SnakeGrid(std::size_t width, std::size_t height);

    void set_snake_body(Position position, bool value);

    bool is_snake_body(Position position) const;

    std::optional<Position> move_head(Position head, Direction direction) const;

    std::size_t get_width() const { return width; }
    std::size_t get_height() const { return height; }

    void shuffle_apple();

    const Position &get_apple_position() const { return apple; }

private:
    std::vector<bool> flat_grid;
    Position apple;
    std::size_t width;
    std::size_t height;
    std::size_t empty_cells;
};


struct PreStartSnake {};

struct AliveSnake {
    std::optional<std::pair<Direction, std::optional<Direction>>> next_direction;

    Direction get_next_direction(Direction last_direction) const;
};
struct DeadSnake {};
struct WinnerSnake {};

class Snake {
public:
    Snake(SnakeGrid &grid, const Position &position);

    bool update(SnakeGrid &grid);

    std::span<const Position> get_body() const { return body; }
    void push_direction(Direction visited_state);

    decltype(auto) visit_state(auto &&visitor) {
        return std::visit(std::forward<decltype(visitor)>(visitor), state);
    }

    template <typename T>
    bool has_state() const { return std::holds_alternative<T>(state); }

    const Position &get_previous_tail_position() const { return previous_tail_position; }

    Direction get_next_direction() const;

private:
    std::vector<Position> body{};
    Direction last_direction;
    Position previous_tail_position;
    std::variant<PreStartSnake, AliveSnake, DeadSnake, WinnerSnake> state {PreStartSnake{}};
};
