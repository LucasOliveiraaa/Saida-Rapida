#pragma once

#include <future>
#include <mutex>
#include <vector>

struct Future {
private:
  std::vector<std::future<void>> futures;
  std::mutex mutex;

public:
  // Push a task and starts it asynchronously
  template <typename F> void push(F &&task) {
    std::lock_guard<std::mutex> lock(mutex);
    futures.emplace_back(std::async(std::launch::async, std::forward<F>(task)));
  }

  // Wait until all tasks have been done
  void await() {
    for (auto &f : futures) {
      f.get();
    }
    futures.clear();
  }
};