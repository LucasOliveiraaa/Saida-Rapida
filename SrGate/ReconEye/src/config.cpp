#include "config.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>

Config::Config() {
  std::ifstream file("/home/lucas/SaidaRapida/SrGate/srgate.conf.json");

  if (!file.is_open()) {
    std::cerr << "Error: Failed to open config file." << std::endl;
    throw std::runtime_error("Error: Failed to open config file.");
  }

  file >> this->data;

  file.close();
}

json &Config::get() { return this->data; }