#pragma once

#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/shape_predictor.h>

#include <vector>

#include "image.hpp"
#include "net.hpp"
#include "serialization.hpp"

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

  static Descriptor *fromFace(Image &img, Net &net);
  static std::vector<Descriptor> fromFaces(std::vector<Image> &imgs, Net &net);

  inline void save(const std::string &path) {
    dlib::serialize(path) << desc;
    valid = true;
  }
  inline void load(const std::string &path) {
    dlib::deserialize(path) >> desc;
    valid = true;
  }
};