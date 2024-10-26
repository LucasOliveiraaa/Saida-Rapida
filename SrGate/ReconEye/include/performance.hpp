#pragma once

#include <algorithm>
#include <chrono>

struct Performance {
public:
  inline static Performance &getInstance() {
    static Performance instance;
    return instance;
  }

  void setIdealFPS(float fps) { ideal_fps = fps; }

  inline void addPoint() {
    auto now = std::chrono::high_resolution_clock::now();

    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - last_point)
            .count();

    if (duration > 0) {
      fps = 1000.0f / duration;
    }

    last_point = now;
  }

  inline float getPerformance() const {
    if (ideal_fps == 0)
      return 0.0f;
    return std::min(1.0f, fps / ideal_fps);
  }

private:
  Performance()
      : fps(0), ideal_fps(10.0f),
        last_point(std::chrono::high_resolution_clock::now()) {}

  std::chrono::high_resolution_clock::time_point last_point;

  float fps;
  float ideal_fps;

public:
  Performance(Performance const &) = delete;
  void operator=(Performance const &) = delete;
};
