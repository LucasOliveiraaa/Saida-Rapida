#include "net.hpp"
#include "config.hpp"
#include <dlib/pixel.h>

Net::Net() {
  auto models = Config::getInstance().get()["dlib-models"];

  size = models["chip-extractor"]["size"].get<float>();
  padding = models["chip-extractor"]["padding"].get<float>();

  m_detector = dlib::get_frontal_face_detector();

  auto sp_paths = models["shape-predictor"]["path"];
  dlib::deserialize(sp_paths["low"]) >> m_shape_predictor_low;
  dlib::deserialize(sp_paths["high"]) >> m_shape_predictor_high;

  auto anet_path = models["face-recognition"]["path"];
  dlib::deserialize(anet_path) >> m_anet;
}