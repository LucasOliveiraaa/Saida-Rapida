#include <opencv2/objdetect.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

#include <cstdlib>
#include <mutex>
#include <string>
#include <vector>
#include <zlib.h>
#include <zmq.h>

#include "nlohmann/json.hpp"

#include "base64.hpp"
#include "config.hpp"
#include "console.hpp"
#include "db.hpp"
#include "descriptor.hpp"
#include "future.hpp"
#include "image.hpp"
#include "performance.hpp"
#include "tracker.hpp"

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
  auto config = Config::getInstance().get();

  // Initialize the Net Type and Database
  console::log("main", "Creating Net and loading DB");
  Net net;
  DB db = load_db(Config::env_vars(config["database"]["path"]));
  std::mutex mtx_net, mtx_db;

  console::log("main", "Opening video capture");
  cv::VideoCapture cap(config["capture"]["device"].get<int>());
  if (!cap.isOpened()) {
    std::cerr << "Error: Failed to open camera" << std::endl;
    return -1;
  }

  console::log("main", "Creating Future and Tracker");
  Future future;
  Tracker tracker;

  console::log("main", "Starting ZeroMQ");
  void *context = zmq_ctx_new();
  void *publisher = zmq_socket(context, ZMQ_PUB);
  int rc = zmq_bind(publisher, "tcp://*:5555");
  assert(rc == 0);

  cv::Mat frame;
  int count = 0;
  while (true) {
    // Get the frame
    cap >> frame;
    if (frame.empty()) {
      console::warn("main", "Warning: Received an empty frame from the camera");
      continue;
    }

    count++;
    Performance::getInstance().addPoint();
    console::info("main", "Current performance: " +
                              std::to_string(
                                  Performance::getInstance().getPerformance()));

    console::log("main", "Testing tracker reload");
    if (count % config["capture"]["reload-trackers"].get<int>() == 0 ||
        !tracker.haveTrackers()) {
      console::log("main", "Updating Trackers");
      tracker.updateTrackers(frame);

      continue;
    }

    console::log("main", "Starting description");

    // Highlight all faces and descript them
    std::vector<nlohmann::json> descriptions;
    std::mutex mtx_descs;

    std::vector<cv::Rect> face_rects = tracker.getFaces(frame);
    for (size_t i = 0; i < face_rects.size(); i++) {
      cv::Rect face_rect = face_rects[i];
      bool is_main_rect = i == 0;

      if (is_main_rect) {
        console::log("main", " -> Main Face!");
        cv::rectangle(frame, face_rect, cv::Scalar(0, 255, 0), 1);
      } else {
        cv::rectangle(frame, face_rect, cv::Scalar(0, 0, 255), 1);
      }

      console::log("main", "Adding future");
      future.push([&]() {
        cv::Mat cv_face = frame(face_rect);
        Image face = to_dlib(cv_face);

        Descriptor *desc = nullptr;
        {
          std::lock_guard<std::mutex> guard(mtx_net);
          desc = Descriptor::fromFace(face, net);
        }

        if (!desc) {
          console::log("main", " -> Main: Descriptor: Anything descripted");
          return;
        }

        std::string id = "";
        {
          std::lock_guard<std::mutex> guard(mtx_db);
          id = get_match(db, desc);
        }

        console::log("main", "Creating description json");
        nlohmann::json description;
        description["main"] = is_main_rect;
        description["id"] = id;
        description["data"] = encode_base64(desc->bytes());

        std::lock_guard<std::mutex> guard(mtx_descs);
        descriptions.push_back(description);
      });
    }

    future.await();

    nlohmann::json json;
    json["frame"] = encode_base64(frame);
    json["descriptions"] = descriptions;

    std::string serialized;
    try {
      serialized = json.dump();
    } catch (const std::exception &e) {
      console::error("main", "Error: Failed to serialize JSON: " +
                                 std::string(e.what()));
    }
    std::vector<unsigned char> data = gzip_compress(serialized);

    // Send message
    int sent_bytes = zmq_send(publisher, data.data(), data.size(), 0);
    if (sent_bytes == -1) {
      console::error("main", "Error: Failed to send data: " +
                                 std::string(zmq_strerror(zmq_errno())));
    } else {
      console::success("main", "Sent " + std::to_string(sent_bytes));
    }

    count++;
  }

  console::log("main", "Releasing components");
  zmq_close(publisher);
  zmq_ctx_destroy(context);
  cap.release();

  return 0;
}
