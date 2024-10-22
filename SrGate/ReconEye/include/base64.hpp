#pragma once

#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>

#include <string>
#include <vector>

std::string &encode_base64(const std::vector<unsigned char> &data);
std::string &encode_base64(const cv::Mat &data);