#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "descriptor.hpp"

typedef std::unordered_map<std::string, std::vector<Descriptor>> DB;
DB load_db(const std::string &path);
std::string get_match(const DB &data, Descriptor *desc);