#pragma once

#include <raylib.h>

constexpr inline float FONT_SIZE = 20;
constexpr inline float ITEM_WIDTH = 300;
constexpr inline float ITEM_HEIGHT = 50;
constexpr inline float PADDING = 20;
constexpr inline float GAP = 10;

constexpr Rectangle centered(const Rectangle &parent, float width, float height) {
    return {parent.x + (parent.width - width) / 2, parent.y + (parent.height - height) / 2, width, height};
}

constexpr Rectangle relative(const Rectangle &parent, const Rectangle &child) {
    return {parent.x + child.x, parent.y + child.y, child.width, child.height};
}

constexpr Rectangle with_padding(const Rectangle &rect, float padding) {
    return Rectangle{rect.x - padding, rect.y - padding, rect.width + 2 * padding, rect.height + 2 * padding};
}

inline Rectangle get_screen_rect() {
    return Rectangle{0, 0, static_cast<float>(GetRenderWidth()), static_cast<float>(GetRenderHeight())};
}