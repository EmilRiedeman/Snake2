#include "snake.hpp"

#include <algorithm>
#include <random>

SnakeGrid::SnakeGrid(std::size_t width, std::size_t height)
    : flat_grid(width * height), apple{height / 2, width - 3}, width(width), height(height), empty_cells(width * height - 1) {}

void SnakeGrid::set_snake_body(Position position, bool value) {
    if (flat_grid[position.row * width + position.col] == value) return;
    flat_grid[position.row * width + position.col] = value;
    empty_cells += value ? -1 : 1;
}

bool SnakeGrid::is_snake_body(Position position) const {
    return flat_grid[position.row * width + position.col];
}

std::optional<Position> SnakeGrid::move_head(Position head, Direction direction) const {
    switch (direction) {
    case Direction::RIGHT:
        if (head.col + 1 < width) {
            return Position{head.row, head.col + 1};
        }
        break;
    case Direction::DOWN:
        if (head.row + 1 < height) {
            return Position{head.row + 1, head.col};
        }
        break;
    case Direction::LEFT:
        if (head.col > 0) {
            return Position{head.row, head.col - 1};
        }
        break;
    case Direction::UP:
        if (head.row > 0) {
            return Position{head.row - 1, head.col};
        }
        break;
    }
    return std::nullopt;
}

static std::mt19937 rng{std::random_device{}()};

void SnakeGrid::shuffle_apple() {
    if (empty_cells == 0) return;

    std::uniform_int_distribution<std::size_t> dist(0, empty_cells - 1);
    std::size_t index = dist(rng);
    for (std::size_t i = 0; i < flat_grid.size(); ++i) {
        if (!flat_grid[i]) {
            if (index == 0) {
                apple = {i / width, i % width};
                break;
            }
            --index;
        }
    }
}

Snake::Snake(SnakeGrid &grid, const Position &position) : last_direction(Direction::RIGHT) {
    const std::size_t row = position.row;
    const std::size_t col = position.col;
    for (std::size_t i = 0; i < 4; ++i) {
        body.push_back(Position{row, col - i});
        grid.set_snake_body(body.back(), true);
    }
    previous_tail_position = body.back();
}

bool Snake::update(SnakeGrid &grid) {
    if (!has_state<AliveSnake>())
        return false;

    AliveSnake &alive_state = std::get<AliveSnake>(state);

    const Direction direction = get_next_direction();

    if (alive_state.next_direction) {
        if (alive_state.next_direction->second) {
            alive_state.next_direction = {*alive_state.next_direction->second, std::nullopt};
        } else {
            alive_state.next_direction.reset();
        }
    }

    if (auto new_position = grid.move_head(body.front(), direction)) {
        if (grid.is_snake_body(*new_position) && *new_position != body.back()) {
            state = DeadSnake{};
            return false;
        }
        bool eaten_apple = false;
        if (*new_position == grid.get_apple_position()) {
            body.push_back(body.back());
            eaten_apple = true;
        }
        last_direction = direction;

        grid.set_snake_body(body.back(), eaten_apple);
        previous_tail_position = body.back();
        for (std::size_t i = body.size() - 1; i > 0; --i) {
            body[i] = body[i - 1];
        }
        body.front() = *new_position;
        grid.set_snake_body(body.front(), true);
        if (eaten_apple) {
            grid.shuffle_apple();
            return true;
        }
    } else {
        state = DeadSnake{};
    }
    return false;
}

bool is_opposite(Direction a, Direction b) {
    return (a == Direction::RIGHT && b == Direction::LEFT) ||
           (a == Direction::LEFT && b == Direction::RIGHT) ||
           (a == Direction::UP && b == Direction::DOWN) ||
           (a == Direction::DOWN && b == Direction::UP);
}

void Snake::push_direction(Direction direction) {
    visit_state([this, direction](auto &visited_state) {
        using State = std::decay_t<decltype(visited_state)>;
        using std::is_same_v;

        if constexpr (is_same_v<State, AliveSnake>) {
            if (visited_state.next_direction) {
                if (!visited_state.next_direction->second && !is_opposite(visited_state.next_direction->first, direction) && visited_state.next_direction->first != direction) {
                    visited_state.next_direction->second = direction;
                }
            } else if (!is_opposite(last_direction, direction) && last_direction != direction) {
                visited_state.next_direction = {direction, std::nullopt};
            }
        } else if constexpr (is_same_v<State, PreStartSnake>) {
            if (!is_opposite(last_direction, direction)) {
                state = AliveSnake{std::pair{direction, std::optional<Direction>{}}};
            }
        }
    });
}

Direction AliveSnake::get_next_direction(Direction last_direction) const {
    return next_direction ? next_direction->first : last_direction;
}

Direction Snake::get_next_direction() const {
    if (has_state<AliveSnake>()) {
        return std::get<AliveSnake>(state).get_next_direction(last_direction);
    } else {
        return last_direction;
    }
}