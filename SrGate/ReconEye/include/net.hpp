#pragma once

#include "anet_type.hpp"
#include "performance.hpp"
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/shape_predictor.h>

#include <mutex>

struct Net {
private:
  dlib::frontal_face_detector m_detector;
  dlib::shape_predictor m_shape_predictor_low;
  dlib::shape_predictor m_shape_predictor_high;
  anet_type m_anet;

  std::mutex m_detector_mtx, m_shape_predictor_mtx, m_anet_mtx;

public:
  float size = 0, padding = 0;

  Net();

  inline dlib::frontal_face_detector &detector() {
    std::lock_guard<std::mutex> guard(m_detector_mtx);
    return m_detector;
  }
  inline dlib::shape_predictor &shape_predictor() {
    std::lock_guard<std::mutex> guard(m_shape_predictor_mtx);
    float performance = Performance::getInstance().getPerformance();
    if (performance < .5f) {
      return m_shape_predictor_low;
    }

    return m_shape_predictor_high;
  }
  inline anet_type &anet() {
    std::lock_guard<std::mutex> guard(m_anet_mtx);
    return m_anet;
  }
};