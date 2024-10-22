#include "base64.hpp"

std::string &base64_encode(const std::vector<unsigned char> &data) {
  static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                          "abcdefghijklmnopqrstuvwxyz"
                                          "0123456789+/";

  std::string base64_str;
  int i = 0, val = 0, valb = -6;
  for (unsigned char c : data) {
    val = (val << 8) + c;
    valb += 8;
    while (valb >= 0) {
      base64_str.push_back(base64_chars[(val >> valb) & 0x3F]);
      valb -= 6;
    }
  }
  if (valb > -6)
    base64_str.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
  while (base64_str.size() % 4)
    base64_str.push_back('=');
  return base64_str;
}

std::string &base64_encode(const cv::Mat &data) {
  std::vector<unsigned char> buf;
  cv::imencode(".png", data, buf);

  std::string encoded = base64_encode(buf);
  return encoded;
}