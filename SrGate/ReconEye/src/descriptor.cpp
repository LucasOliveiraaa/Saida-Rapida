#include "descriptor.hpp"

Descriptor *Descriptor::fromFace(Image &img, Net &net) {
  std::vector<dlib::rectangle> dets = net.detector()(img);
  if (dets.empty())
    return nullptr;

  dlib::full_object_detection shape = net.shape_predictor()(img, dets[0]);

  Image face_chip;
  dlib::extract_image_chip(
      img, dlib::get_face_chip_details(shape, net.size, net.padding),
      face_chip);

  return new Descriptor(net.anet()(face_chip));
}

std::vector<Descriptor> Descriptor::fromFaces(std::vector<Image> &imgs,
                                              Net &net) {
  std::vector<Image> faces;

  for (auto &img : imgs) {
    for (auto face : net.detector()(img)) {
      auto shape = net.shape_predictor()(img, face);

      Image face_chip;
      dlib::extract_image_chip(
          img, dlib::get_face_chip_details(shape, net.size, net.padding),
          face_chip);

      faces.push_back(face_chip);
    }
  }

  std::vector<Descriptor> descs;
  std::vector<DescType> pure_desctriptors = net.anet()(faces);

  for (auto &desc : pure_desctriptors) {
    descs.push_back(Descriptor(desc));
  }

  return descs;
}