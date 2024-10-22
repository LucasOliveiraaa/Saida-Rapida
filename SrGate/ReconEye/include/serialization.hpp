#pragma once

#include <cstring>
#include <dlib/matrix.h>
#include <vector>

inline std::vector<unsigned char>
serialize_matrix(const dlib::matrix<float, 0, 1> &matrix) {
  std::vector<unsigned char> buffer;

  long size = matrix.size();

  buffer.resize(size * sizeof(float));

  std::memcpy(buffer.data(), matrix.begin(), size * sizeof(float));

  return buffer;
}

inline dlib::matrix<float, 0, 1>
deserialize_matrix(const std::vector<unsigned char> &buffer) {
  long size = buffer.size() / sizeof(float);

  dlib::matrix<float, 0, 1> matrix(size);

  std::memcpy(matrix.begin(), buffer.data(), buffer.size());

  return matrix;
}