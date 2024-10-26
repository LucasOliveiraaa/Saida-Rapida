#pragma once

#include "config.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

inline std::vector<std::string> split_string(const std::string &input) {
  std::vector<std::string> result;
  std::stringstream ss(input);
  std::string item;

  while (std::getline(ss, item, ',')) {
    result.push_back(item);
  }

  return result;
}

inline void print(const std::string &seg1, const std::string &seg2,
                  const std::string &text) {
  auto config = Config::getInstance().get();

  std::vector<std::string> show = split_string(config["logging"][seg1]);

  if (std::find(show.begin(), show.end(), seg2) == show.end() &&
      seg2 != "all") {
    return;
  }

  if (seg2 == "error") {
    std::cerr << text << std::endl;
    return;
  }

  std::cout << text << std::endl;
}

namespace console {

inline void log(const std::string &seg, const std::string &text) {
  print(seg, "log", text);
}

inline void info(const std::string &seg, const std::string &text) {
  print(seg, "info", text);
}

inline void success(const std::string &seg, const std::string &text) {
  print(seg, "success", text);
}

inline void warn(const std::string &seg, const std::string &text) {
  print(seg, "warn", text);
}

inline void error(const std::string &seg, const std::string &text) {
  print(seg, "error", text);
}
} // namespace console