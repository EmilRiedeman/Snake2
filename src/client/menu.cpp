#include "menu.hpp"

#include "app.hpp"
#include "ui.hpp"

#include <raygui.h>

#include <array>

void MainMenu::render(MenuState &menu_state, App &app) {
    const Rectangle screen_rect = get_screen_rect();

    float y = PADDING;

    const Rectangle single_player{PADDING, y, ITEM_WIDTH, ITEM_HEIGHT}; y += ITEM_HEIGHT + GAP;
    const Rectangle multiplayer  {PADDING, y, ITEM_WIDTH, ITEM_HEIGHT}; y += ITEM_HEIGHT + GAP;
    const Rectangle quit         {PADDING, y, ITEM_WIDTH, ITEM_HEIGHT}; y += ITEM_HEIGHT + GAP;

    const Rectangle content_rect = centered(screen_rect, ITEM_WIDTH + 2 * PADDING, y + PADDING - GAP);

    if (GuiButton(relative(content_rect, single_player), "Single Player") || IsKeyPressed(KEY_ONE)) {
        menu_state.state = SinglePlayerMenu{};
        return;
    }

    if (GuiButton(relative(content_rect, multiplayer), "Multiplayer") || IsKeyPressed(KEY_TWO)) {
    }

    if (GuiButton(relative(content_rect, quit), "Quit") || IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_Q)) {
        app.running = false;
        return;
    }
}

void SinglePlayerMenu::render(MenuState &menu_state, App &app) {
    // go back to main menu button
    if (GuiButton({10, 10, 100, 50}, "#118#Back") || IsKeyPressed(KEY_ESCAPE)) {
        menu_state.state = MainMenu{};
        return;
    }

    const Rectangle screen_rect = get_screen_rect();

    float y = PADDING;

    const Rectangle width_slider {PADDING, y, ITEM_WIDTH, ITEM_HEIGHT}; y += ITEM_HEIGHT + GAP;
    const Rectangle height_slider{PADDING, y, ITEM_WIDTH, ITEM_HEIGHT}; y += ITEM_HEIGHT + GAP;
    const Rectangle color_slider {PADDING, y, ITEM_WIDTH, ITEM_HEIGHT}; y += ITEM_HEIGHT + GAP;

    constexpr static std::array<Position, 4> snake_body{Position{0, 3}, Position{0, 2}, Position{0, 1}, Position{0, 0}};
    constexpr float snake_scale = ITEM_WIDTH / snake_body.size();
    const Rectangle snake_rect = {PADDING, y, ITEM_WIDTH, snake_scale}; y += snake_scale + GAP;

    const Rectangle start_button {PADDING, y, ITEM_WIDTH, ITEM_HEIGHT}; y += ITEM_HEIGHT + GAP;

    const Rectangle content_rect = centered(screen_rect, ITEM_WIDTH + 2 * PADDING, y + PADDING - GAP);

    constexpr std::size_t MIN_SIZE = 8;
    constexpr std::size_t MAX_SIZE = 40;

    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) settings.height = std::min(settings.height + 1, MAX_SIZE);
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) settings.height = std::max(settings.height - 1, MIN_SIZE);
    if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) settings.width = std::max(settings.width - 1, MIN_SIZE);
    if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) settings.width = std::min(settings.width + 1, MAX_SIZE);

    int width = static_cast<int>(settings.width);
    int height = static_cast<int>(settings.height);
    GuiSpinner(relative(content_rect, width_slider), "Width", &width, 8, 40, false);
    GuiSpinner(relative(content_rect, height_slider), "Height", &height, 8, 40, false);
    settings.width = static_cast<std::size_t>(width);
    settings.height = static_cast<std::size_t>(height);

    GuiSlider(relative(content_rect, color_slider), "Color", nullptr, &settings.skin.body_hue, 0, 360);

    {
        const Rectangle relative_rect = relative(content_rect, snake_rect);
        const Vector2 offset = {relative_rect.x, relative_rect.y};
        render_snake(snake_body, Direction::RIGHT, snake_body.back(), settings.skin, offset, snake_scale);
    }

    if (GuiButton(relative(content_rect, start_button), "Start") || IsKeyPressed(KEY_SPACE)) {
        app.state = GameContext(settings);
    }
}

void MenuState::render(App &app) {
    BeginDrawing();
    ClearBackground(WHITE);
    std::visit([&](auto &visited_state) { visited_state.render(*this, app); }, state);
    EndDrawing();
}

