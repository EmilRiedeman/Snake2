#include "visuals.hpp"

#include <config.h>
#include <raylib.h>
#include <raymath.h>

#include <functional>
#include <cmath>

void render_checker_board(Vector2 offset, std::size_t width, std::size_t height, float square_size, Color color1, Color color2) {
    for (std::size_t row = 0; row < height; ++row) {
        for (std::size_t col = 0; col < width; ++col) {
            DrawRectangleV(offset + Vector2{static_cast<float>(col), static_cast<float>(row)} * square_size,
                           Vector2{square_size, square_size},
                           (row + col) % 2 == 0 ? color1 : color2);
        }
    }
}

void DrawSplineSegmentBezierQuadraticInterpolated(Vector2 p1, Vector2 c2, Vector2 p3, float thick, Color color, float t_max) {
    // See DrawSplineSegmentBezierQuadratic for the original implementation.

    if (t_max <= 0.0f) return;  // Nothing to draw if t is 0 or negative.

    const float step = t_max / SPLINE_SEGMENT_DIVISIONS;  // Adjust step to t_max.

    Vector2 previous = p1;
    Vector2 current = { 0 };
    float t = 0.0f;

    Vector2 points[2 * SPLINE_SEGMENT_DIVISIONS + 2] = { 0 };

    for (int i = 1; i <= SPLINE_SEGMENT_DIVISIONS; i++) {
        t = step * i;  // Step from 0 to t_max.

        float a = powf(1.0f - t, 2);
        float b = 2.0f * (1.0f - t) * t;
        float c = powf(t, 2);

        current.x = a * p1.x + b * c2.x + c * p3.x;
        current.y = a * p1.y + b * c2.y + c * p3.y;

        float dx = current.x - previous.x;
        float dy = current.y - previous.y;
        float size = 0.5f * thick / sqrtf(dx * dx + dy * dy);

        if (i == 1) {
            points[0].x = previous.x + dy * size;
            points[0].y = previous.y - dx * size;
            points[1].x = previous.x - dy * size;
            points[1].y = previous.y + dx * size;
        }

        points[2 * i].x = current.x + dy * size;
        points[2 * i].y = current.y - dx * size;
        points[2 * i + 1].x = current.x - dy * size;
        points[2 * i + 1].y = current.y + dx * size;

        previous = current;
    }

    // Draw only the calculated part of the curve.
    DrawTriangleStrip(points, 2 * SPLINE_SEGMENT_DIVISIONS + 2, color);
}

float SnakeSkin::calculate_body_size(std::size_t i, float square_size) const {
    return std::max(max_body_size - 0.01f * static_cast<float>(i), min_body_size) * static_cast<float>(square_size);
}

Color SnakeSkin::calculate_body_color(std::size_t i, float interpolate_time) const {
    return ColorFromHSV(body_hue, body_saturation, std::max(head_brightness - 0.02f * (static_cast<float>(i) + interpolate_time), tail_brightness));
}

inline Vector2 calculate_screen_pos(Vector2 offset, float square_size, const Position &pos) {
    return offset + Vector2{static_cast<float>(pos.col), static_cast<float>(pos.row)} * square_size + Vector2{square_size / 2, square_size / 2};
}

void render_snake_body(std::span<const Position> body, const SnakeSkin &skin, Vector2 offset, float square_size, double interpolate_time) {
    auto screen_pos = std::bind_front(calculate_screen_pos, offset, square_size);

//    for (const auto &[pos_before, pos_now, pos_after] : body | std::views::adjacent<3>) {
    for (std::size_t i = 1; i < body.size() - 1; ++i) {
        const auto &[pos_before, pos_now, pos_after] = std::tuple{std::cref(body[i - 1]), std::cref(body[i]), std::cref(body[i + 1])};

        const float body_size = skin.calculate_body_size(i, square_size);
        const Color body_color = skin.calculate_body_color(i, static_cast<float>(interpolate_time));

        const Vector2 center = screen_pos(pos_now);
        const Vector2 in_pos = (center * 0.99 + screen_pos(pos_before) * 1.01) / 2;
        const Vector2 out_pos = (center * 0.99 + screen_pos(pos_after) * 1.01) / 2;

        DrawSplineSegmentBezierQuadratic(in_pos, center, out_pos, body_size, body_color);
    }
}

void render_snake_head(std::span<const Position> body, Direction next_direction, const SnakeSkin &skin, std::optional<Position> fruit_target, Vector2 offset, float square_size, double interpolate_time) {
    if (body.size() < 2) return;

    auto screen_pos = std::bind_front(calculate_screen_pos, offset, square_size);

    auto get_side_pos = [radius = static_cast<float>(square_size) / 2] (const Vector2 &mid, Direction direction){
        switch (direction) {
            case Direction::RIGHT:
                return mid + Vector2{radius, 0};
            case Direction::DOWN:
                return mid + Vector2{0, radius};
            case Direction::LEFT:
                return mid - Vector2{radius, 0};
            case Direction::UP:
                return mid - Vector2{0, radius};
        }
        return mid;
    };

    const float body_size = skin.calculate_body_size(0, square_size);
    const Color body_color = skin.calculate_body_color(0, static_cast<float>(interpolate_time));

    const Position &head = body[0];
    const Position &before_head = body[1];

    const Vector2 center = screen_pos(head);
    const Vector2 in_pos = (center + screen_pos(before_head)) / 2;
    const Vector2 out_pos = get_side_pos(center, next_direction);

    const Vector2 head_pos = GetSplinePointBezierQuad(in_pos, center, out_pos, static_cast<float>(interpolate_time));

    DrawSplineSegmentBezierQuadraticInterpolated(in_pos, center, out_pos, body_size, body_color, static_cast<float>(interpolate_time));
    DrawCircleV(head_pos, body_size / 2, body_color);

    const Vector2 snake_direction = Vector2Normalize(Vector2{head_pos.x - center.x, head_pos.y - center.y});
    const float eye_radius = body_size * 0.2f;
    const Vector2 eye_offset = Vector2{snake_direction.y, snake_direction.x} * (body_size * 0.3f);
    const Vector2 left_eye_pos = head_pos + eye_offset;
    const Vector2 right_eye_pos = head_pos - eye_offset;

    for (const auto &eye_pos : {left_eye_pos, right_eye_pos}) {
        DrawCircleV(eye_pos, eye_radius * 1.5f, body_color);
        DrawCircleV(eye_pos, eye_radius, RAYWHITE);
    }

    const float pupil_radius = eye_radius * 0.5f;
    const Color pupil_color = ColorBrightness(body_color, -0.3f);
    if (fruit_target) {
        const Vector2 target_center = screen_pos(*fruit_target);
        for (const auto &eye_pos : {left_eye_pos, right_eye_pos}) {
            const Vector2 target_direction = Vector2Normalize(target_center - eye_pos);
            DrawCircleV(eye_pos + target_direction * (eye_radius - pupil_radius), pupil_radius, pupil_color);
        }
    } else {
        for (const auto &eye_pos : {left_eye_pos, right_eye_pos}) {
            DrawCircleV(eye_pos, pupil_radius, pupil_color);
        }
    }
}

void render_snake_tail(std::span<const Position> body, const Position &prev_tail_position, const SnakeSkin &skin, Vector2 offset, float square_size, double interpolate_time) {
    if (body.size() < 2) return;

    auto screen_pos = std::bind_front(calculate_screen_pos, offset, square_size);

    const std::size_t tail_index = body.size() - 1;
    const float body_size = skin.calculate_body_size(tail_index, square_size);
    const Color body_color = skin.calculate_body_color(tail_index, static_cast<float>(interpolate_time));

    const Position &tail = body.end()[-1];
    const Position &after_tail = body.end()[-2];
    const Vector2 center = screen_pos(tail);
    const Vector2 out_pos = (screen_pos(after_tail) + center) / 2;

    if (prev_tail_position != tail) {
        const Vector2 in_pos = (screen_pos(prev_tail_position) + center) / 2;

        const Vector2 interpolated_pos = GetSplinePointBezierQuad(in_pos, center, out_pos, static_cast<float>(interpolate_time));

        DrawSplineSegmentBezierQuadraticInterpolated(out_pos, center, in_pos, body_size, body_color, 1.0f - static_cast<float>(interpolate_time));
        DrawCircleV(interpolated_pos, body_size / 2, body_color);
    } else {
        DrawCircleV(out_pos, body_size / 2, body_color);
    }
}

void VisualSnake::render(const SnakeGrid &grid, Vector2 offset, float square_size, double time) const {
    const double interpolate_time = snake.has_state<AliveSnake>() ? std::fmod(time - last_update, SPT) / SPT : 0;

    render_snake(snake.get_body(), snake.get_next_direction(), snake.get_previous_tail_position(), skin, offset, square_size, grid.get_apple_position(), interpolate_time);
}

void VisualSnake::update(SnakeGrid &grid, double time) {
    if (time - last_update < SPT) return;

    last_update = time;
    snake.update(grid);
}

void render_fruit(const Position &fruit, Vector2 offset, float square_size, const double time) {
    const Vector2 center = offset + Vector2{static_cast<float>(fruit.col), static_cast<float>(fruit.row)} * square_size + Vector2{square_size / 2, square_size / 2};
    DrawCircleV(center, square_size / 2.0f * static_cast<float>(0.7f + (((std::sin(4.0f * time) + 1.0f) / 2.0f) * 0.15)), ColorFromHSV(0, 0.6, 0.9));
}
