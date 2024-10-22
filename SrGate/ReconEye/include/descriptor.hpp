#pragma once

#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/shape_predictor.h>

#include <vector>

#include "config.hpp"
#include "image.hpp"
#include "serialization.hpp"

#include "anet_type.hpp"

struct net {
  dlib::frontal_face_detector detector;
  dlib::shape_predictor sp;
  anet_type net;
};

inline net create_net() {
  struct net _net;

  auto conf = Config::get_instance().get();
  std::filesystem::path base = conf["dlib"]["models"]["directory"];
  std::filesystem::path sp = base / conf["dlib"]["models"]["shape_predictor"];
  std::filesystem::path anet = base / conf["dlib"]["models"]["anet"];

  _net.detector = dlib::get_frontal_face_detector();

  dlib::deserialize(sp.string()) >> _net.sp;
  dlib::deserialize(anet.string()) >> _net.net;

  return _net;
}

struct Descriptor {
public:
  using DescType = dlib::matrix<float, 0, 1>;

private:
  DescType desc;
  bool valid;

public:
  Descriptor() : valid(false) {}

  Descriptor(DescType desc) : desc(desc), valid(true) {}

  inline DescType &getDesc() { return desc; }

  inline operator DescType() { return desc; }

  inline bool isValid() { return valid; }

  inline std::vector<unsigned char> bytes() { return serialize_matrix(desc); }

  static Descriptor *fromFace(Image &img, net &_net);
  static std::vector<Descriptor> fromFaces(std::vector<Image> &imgs, net &_net);

  inline void save(const std::string &path) {
    dlib::serialize(path) << desc;
    valid = true;
  }
  inline void load(const std::string &path) {
    dlib::deserialize(path) >> desc;
    valid = true;
  }
};