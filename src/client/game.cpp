#include "game.hpp"

#include "app.hpp"
#include "ui.hpp"

#include <cmath>
#include <raygui.h>
#include <raylib.h>

void GameContext::update_and_render(App &app) {
    const double elapsed = timer.elapsed().count();
    update(elapsed);
    poll_events(elapsed);

    BeginDrawing();
    render(elapsed, app);
    EndDrawing();
}

void GameContext::update(const double time) {
    if (!timer.is_running()) return;

    game.update(time);
}

void GameContext::poll_events(const double time) {
    if (timer.is_running()) game.poll_events(time);

    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_P)) {
        if (timer.is_running()) {
            timer.stop();
        } else {
            timer.start();
        }
    }
}

void GameContext::render(const double time, App &app) {
    ClearBackground(DARKGRAY);

    game.render(time);

    if (!timer.is_running()) {
        const Rectangle screen_rect = get_screen_rect();

        DrawRectangleRec(screen_rect, Fade(DARKGRAY, 0.8));

        const auto text = "Paused";
        const auto text_width = static_cast<float>(MeasureText(text, FONT_SIZE));

        float y = PADDING;

        const Rectangle text_rect    {PADDING, y, text_width, ITEM_HEIGHT}; y += ITEM_HEIGHT + GAP;
        const Rectangle resume_button{PADDING, y, ITEM_WIDTH, ITEM_HEIGHT}; y += ITEM_HEIGHT + GAP;
        const Rectangle restart_button{PADDING, y, ITEM_WIDTH, ITEM_HEIGHT}; y += ITEM_HEIGHT + GAP;
        const Rectangle back_to_menu_button{PADDING, y, ITEM_WIDTH, ITEM_HEIGHT}; y += ITEM_HEIGHT + GAP;

        const Rectangle content_rect = centered(screen_rect, ITEM_WIDTH + 2 * PADDING, y + PADDING - GAP);

        DrawText(text, static_cast<int>(content_rect.x + (content_rect.width - text_width) / 2), static_cast<int>(text_rect.y), FONT_SIZE, WHITE);

        if (GuiButton(relative(content_rect, resume_button), "Resume") || IsKeyPressed(KEY_SPACE)) {
            timer.start();
        }

        if (GuiButton(relative(content_rect, restart_button), "Restart") || IsKeyPressed(KEY_R)) {
            app.state = GameContext(settings);
        }

        if (GuiButton(relative(content_rect, back_to_menu_button), "Back to Menu") || IsKeyPressed(KEY_Q)) {
            app.state = MenuState{SinglePlayerMenu{settings}};
        }
    }

    DrawFPS(10, 10);
}

void SinglePlayerGame::update(const double time) {
    player.update(grid, time);
}

void SinglePlayerGame::poll_events(double) {
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) player.snake.push_direction(Direction::RIGHT);
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) player.snake.push_direction(Direction::DOWN);
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) player.snake.push_direction(Direction::LEFT);
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) player.snake.push_direction(Direction::UP);
}

static inline Color CHECKER_COLOR1 = ColorFromHSV(110, 0.6, 0.9);
static inline Color CHECKER_COLOR2 = ColorFromHSV(110, 0.6, 0.8);

std::pair<float, Vector2> get_offset_and_square_size(std::size_t board_width, std::size_t board_height) {
    const auto screen_width = static_cast<float>(GetRenderWidth());
    const auto screen_height = static_cast<float>(GetRenderHeight());
    const float square_size = std::min(screen_width / static_cast<float>(board_width),
                                       screen_height / static_cast<float>(board_height));

    const Vector2 offset{(screen_width - square_size * static_cast<float>(board_width)) / 2,
                         (screen_height - square_size * static_cast<float>(board_height)) / 2};

    return {square_size, offset};
}

void SinglePlayerGame::render(const double time) {
    const auto [square_size, offset] = get_offset_and_square_size(grid.get_width(), grid.get_height());

    render_checker_board(offset, grid.get_width(), grid.get_height(), square_size, CHECKER_COLOR1, CHECKER_COLOR2);
    render_fruit(grid.get_apple_position(), offset, square_size, time);

    player.render(grid, offset, square_size, time);
}
