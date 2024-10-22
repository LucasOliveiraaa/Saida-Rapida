#pragma once

#include <dlib/image_transforms/assign_image.h>
#include <dlib/image_transforms/interpolation.h>
#include <dlib/matrix/matrix.h>
#include <dlib/opencv/cv_image.h>
#include <dlib/pixel.h>

#include <opencv2/core/mat.hpp>

typedef dlib::matrix<dlib::rgb_pixel> Image;

Image to_dlib(const cv::Mat &mat);