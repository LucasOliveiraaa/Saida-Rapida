#pragma once

#include "nlohmann/json.hpp"
#include <regex>

using json = nlohmann::json;

struct Config {
public:
  inline static Config &getInstance() {
    static Config instance;
    return instance;
  }

  inline static std::string env_vars(const std::string &input) {
    std::regex env_var_pattern(R"(\$([A-Za-z_][A-Za-z0-9_]*))");
    std::string result = input;

    std::smatch match;
    std::string::const_iterator search_start(result.cbegin());

    while (std::regex_search(search_start, result.cend(), match,
                             env_var_pattern)) {
      std::string env_var = match[1].str();
      const char *env_value = std::getenv(env_var.c_str());

      if (env_value) {
        result.replace(match.position(0), match.length(0), env_value);
      } else {
        result.replace(match.position(0), match.length(0), "");
      }

      search_start = result.cbegin() + match.position(0) + match.length(0);
    }

    return result;
  }

  json &get();

private:
  json data;

  Config();

public:
  Config(Config const &) = delete;
  void operator=(Config const &) = delete;
};