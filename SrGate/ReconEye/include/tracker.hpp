#pragma once

#include <opencv2/core/cvstd_wrapper.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/video/tracking.hpp>
#include <vector>
struct Tracker {
private:
  cv::CascadeClassifier frontal, profile;
  std::vector<cv::Ptr<cv::Tracker>> trackers;
  std::vector<cv::Rect> bounding_boxes;

  std::mutex mutex;

  inline bool isOverlapping(const cv::Rect2d &rect1, const cv::Rect2d &rect2) {
    return (rect1 & rect2).area() > 0;
  }

  inline cv::Rect2d mergeBoundingBoxes(const cv::Rect2d &rect1,
                                       const cv::Rect2d &rect2) {
    return rect1.area() > rect2.area() ? rect1 : rect2;
  }

public:
  Tracker();

  inline bool haveTrackers() { return trackers.size() > 0; }

  // Search faces in the frame using frontal and profile algorithms
  std::vector<cv::Rect> searchFaces(const cv::Mat &frame);

  // Update all trackers saved
  void updateTrackers(const cv::Mat &frame);

  // Get the faces using the saved trackers
  std::vector<cv::Rect> getFaces(const cv::Mat &frame);
};