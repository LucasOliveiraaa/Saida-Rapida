#include "config.hpp"

Config::Config() {
  std::ifstream file("/home/lucas/SaidaRapida/SrGate/srgate.conf.json");

  if (!file.is_open()) {
    std::cerr << "Error: Failed to open config file." << std::endl;
    return;
  }

  file >> this->data;

  file.close();
}

json &Config::get() { return this->data; }