#include "image.hpp"

Image to_dlib(const cv::Mat &mat) {
  dlib::cv_image<dlib::bgr_pixel> cv_img(mat);
  Image img;
  dlib::assign_image(img, cv_img);
  return img;
}