#include "descriptor.hpp"

Descriptor *Descriptor::fromFace(Image &img, net &_net) {
  std::vector<dlib::rectangle> dets = _net.detector(img);
  if (dets.empty())
    return nullptr;

  dlib::full_object_detection shape = _net.sp(img, dets[0]);

  Image face_chip;
  dlib::extract_image_chip(img, dlib::get_face_chip_details(shape, 150, 0.25),
                           face_chip);

  return new Descriptor(_net.net(face_chip));
}

std::vector<Descriptor> Descriptor::fromFaces(std::vector<Image> &imgs,
                                              net &_net) {
  std::vector<Image> faces;

  for (auto &img : imgs) {
    for (auto face : _net.detector(img)) {
      auto shape = _net.sp(img, face);

      Image face_chip;
      dlib::extract_image_chip(
          img, dlib::get_face_chip_details(shape, 150, 0.25), face_chip);

      faces.push_back(face_chip);
    }
  }

  std::vector<Descriptor> descs;
  std::vector<DescType> pure_desctriptors = _net.net(faces);

  for (auto &desc : pure_desctriptors) {
    descs.push_back(Descriptor(desc));
  }

  return descs;
}