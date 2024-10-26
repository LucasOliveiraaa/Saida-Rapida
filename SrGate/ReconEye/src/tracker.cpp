#include "tracker.hpp"
#include "config.hpp"
#include "console.hpp"
#include "future.hpp"
#include <mutex>
#include <opencv2/core/cvstd_wrapper.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/tracking/tracking.hpp>
#include <opencv2/video/tracking.hpp>
#include <stdexcept>

Tracker::Tracker() {
  auto config = Config::getInstance().get();
  auto models = config["opencv-models"];

  if (!frontal.load(models["frontal-detector"]["path"])) {
    console::error("tracker",
                   "Error: Failed to load the frontal face detector.");
    throw std::runtime_error(
        "Error: Failed to load the frontal face detector.");
  }

  if (!profile.load(models["profile-detector"]["path"])) {
    console::error("tracker",
                   "Error: Failed to load the profile face detector.");
    throw std::runtime_error(
        "Error: Failed to load the profile face detector.");
  }
}

std::vector<cv::Rect> Tracker::searchFaces(const cv::Mat &frame) {
  Future future;
  std::vector<cv::Rect> frontal, profile;
  std::mutex mtx_frontal, mtx_profile;

  future.push([&]() {
    auto config =
        Config::getInstance().get()["opencv-models"]["frontal-detector"];

    std::vector<cv::Rect> faces;
    this->frontal.detectMultiScale(
        frame, faces, config["scale-factor"], config["min-neighbors"],
        cv::CASCADE_DO_ROUGH_SEARCH | cv::CASCADE_SCALE_IMAGE,
        cv::Size(50, 50));

    std::lock_guard<std::mutex> lock(mtx_frontal);
    frontal = std::move(faces);
  });

  future.push([&]() {
    auto config =
        Config::getInstance().get()["opencv-models"]["profile-detector"];

    std::vector<cv::Rect> faces;
    this->profile.detectMultiScale(
        frame, faces, config["scale-factor"], config["min-neighbors"],
        cv::CASCADE_DO_ROUGH_SEARCH | cv::CASCADE_SCALE_IMAGE,
        cv::Size(50, 50));

    std::lock_guard<std::mutex> lock(mtx_profile);
    profile = std::move(faces);
  });

  future.await();

  std::vector<cv::Rect> result;
  result.reserve(frontal.size() + profile.size());
  result.insert(result.end(), frontal.begin(), frontal.end());
  result.insert(result.end(), profile.begin(), profile.end());

  return result;
}

void Tracker::updateTrackers(const cv::Mat &frame) {
  std::vector<cv::Rect> faces = this->searchFaces(frame);

  std::unique_lock<std::mutex> lock(mutex);
  trackers.clear();
  bounding_boxes.clear();
  for (auto &face : faces) {
    cv::Ptr<cv::Tracker> tracker = cv::TrackerKCF::create();
    tracker->init(frame, face);

    trackers.push_back(tracker);
    bounding_boxes.push_back(face);
  }
}

std::vector<cv::Rect> Tracker::getFaces(const cv::Mat &frame) {
  std::vector<cv::Rect> faces;

  std::unique_lock<std::mutex> lock(mutex);
  for (size_t i = 0; i < trackers.size(); i++) {
    bool ok = trackers[i]->update(frame, bounding_boxes[i]);
    if (ok) {
      faces.push_back(bounding_boxes[i]);
    } else {
      trackers.erase(trackers.begin() + i);
      bounding_boxes.erase(bounding_boxes.begin() + i);
      --i;
    }
  }

  return faces;
}