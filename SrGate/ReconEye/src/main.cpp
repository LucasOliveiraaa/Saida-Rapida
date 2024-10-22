#include <chrono>

#include <opencv2/objdetect.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

#include <filesystem>
#include <string>
#include <thread>
#include <vector>
#include <zlib.h>
#include <zmq.h>

#include "nlohmann/json.hpp"

#include "base64.hpp"
#include "config.hpp"
#include "db.hpp"
#include "descriptor.hpp"
#include "image.hpp"

std::vector<unsigned char> gzip_compress(const std::string &data) {
  uLongf compressed_size = compressBound(data.size());
  std::vector<unsigned char> compressed_data(compressed_size);

  if (compress(compressed_data.data(), &compressed_size,
               reinterpret_cast<const Bytef *>(data.data()),
               data.size()) != Z_OK) {
    throw std::runtime_error("Error: Failed to compress data");
  }

  if (compressed_size == 0) {
    throw std::runtime_error("Error: Compression resulted in zero size");
  }

  compressed_data.resize(compressed_size);
  return compressed_data;
}

int main(int argc, char *argv[]) {
  // Open config file
  auto config = Config::get_instance().get();

  // Initialize the Net Type
  net _net = create_net();

  // Load the descriptor DB
  DB db = load_db(config["database"]["path"]);

  // Initialize the cascade model and camera
  std::filesystem::path base = config["opencv"]["models"]["directory"];
  std::filesystem::path haarcascade = config["opencv"]["models"]["haarcascade"];
  cv::CascadeClassifier cascade((base / haarcascade).string());
  cv::VideoCapture cap(config["opencv"]["camera"]["device"].get<int>());

  if (!cap.isOpened()) {
    std::cerr << "Error: Failed to open camera" << std::endl;
    return -1;
  }

  void *context = zmq_ctx_new();
  void *publisher = zmq_socket(context, ZMQ_PUB);
  int rc = zmq_bind(publisher, "tcp://*:5555");
  assert(rc == 0);

  cv::Mat frame;
  while (true) {
    // Get the frame
    cap >> frame;
    if (frame.empty()) {
      std::cerr << "Warning: Received an empty frame from the camera"
                << std::endl;
      continue;
    }

    // Run a Haarcascade to recognize faces
    std::vector<cv::Rect> faces_rects;
    cascade.detectMultiScale(frame, faces_rects, 1.1, 2,
                             0 | cv::CASCADE_DO_ROUGH_SEARCH |
                                 cv::CASCADE_SCALE_IMAGE,
                             cv::Size(30, 30));

    // Highlight all faces and crop the frame to separate them
    std::vector<cv::Mat> faces;
    for (const auto &rect : faces_rects) {
      faces.push_back(frame(rect));
      cv::rectangle(frame, rect, cv::Scalar(0, 0, 255), 1);
    }

    std::vector<nlohmann::json> matches_array;
    for (const auto &face : faces) {
      // Describe face
      Image dlib_img = to_dlib(face);
      Descriptor *desc = Descriptor::fromFace(dlib_img, _net);

      if (!desc) {
        continue;
      }

      // Match descriptor
      std::string id = get_match(db, desc);

      if (id.empty())
        continue;

      std::vector<unsigned char> binary = desc->bytes();
      std::string binary_encoded = encode_base64(binary);

      nlohmann::json match_obj;
      match_obj["id"] = id;
      match_obj["binary"] = binary_encoded;
      matches_array.push_back(match_obj);
    }

    nlohmann::json json = {{"matches", matches_array},
                           {"frame", encode_base64(frame)}};
    std::string serialized = json.dump();
    std::vector<unsigned char> data = gzip_compress(serialized);

    // Send message
    int sent_bytes = zmq_send(publisher, data.data(), data.size(), 0);
    if (sent_bytes == -1) {
      std::cerr << "Error: Failed to send data: " << zmq_strerror(zmq_errno())
                << std::endl;
    } else {
      std::cout << "Sent " << sent_bytes << " bytes" << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  zmq_close(publisher);
  zmq_ctx_destroy(context);
  cap.release();

  return 0;
}
