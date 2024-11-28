#pragma once

#include <chrono>

class Timer {
    using clock = std::chrono::high_resolution_clock;
public:
    void start() {
        if (!start_time) {
            start_time = clock::now();
        }
    }

    void stop() {
        if (start_time) {
            total_time += clock::now() - *start_time;
            start_time.reset();
        }
    }

    void reset() {
        start_time.reset();
        total_time = std::chrono::nanoseconds{0};
    }

    bool is_running() const {
        return start_time.has_value();
    }

    template<typename Duration = std::chrono::duration<float, std::ratio<1>>>
    Duration elapsed() const {
        return total_time + (start_time ? clock::now() - *start_time : std::chrono::nanoseconds{0});
    }

private:
    std::optional<clock::time_point> start_time;
    std::chrono::nanoseconds total_time{0};
};