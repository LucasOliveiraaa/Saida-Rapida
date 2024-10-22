#pragma once

#include "nlohmann/json.hpp"

#include <fstream>
#include <iostream>

using json = nlohmann::json;

struct Config {
public:
  inline static Config &get_instance() {
    static Config instance;
    return instance;
  }

  json &get();

private:
  json data;

  Config();

public:
  Config(Config const &) = delete;
  void operator=(Config const &) = delete;
};