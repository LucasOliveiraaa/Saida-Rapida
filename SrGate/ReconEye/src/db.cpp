#include "db.hpp"

namespace fs = std::filesystem;

DB load_db(const std::string &path) {
  DB data;
  for (const auto &dir : fs::directory_iterator(path)) {
    if (!dir.is_directory())
      continue;

    std::string name = dir.path().filename().string();
    data[name] = std::vector<Descriptor>();

    for (const auto &file : fs::directory_iterator(dir.path())) {
      if (!file.is_regular_file())
        continue;

      Descriptor desc;
      desc.load(file.path().string());
      data[name].push_back(desc);
    }
  }
  return data;
} // namespace std::filesystem

std::string get_match(const DB &data, Descriptor *desc) {
  if (!desc) {
    std::cerr << "Descriptor is null!" << std::endl;
    return "";
  }

  for (auto descriptors : data) {
    for (auto sample : descriptors.second) {
      if (length(sample.getDesc() - desc->getDesc()) < 0.65) {
        return descriptors.first;
      }
    }
  }

  return "";
}