#include <iostream>
#include <filesystem>
#include <array>
#include <chrono>
#include <math.h>
#include <vector>
#include <algorithm>
#include <opencv2/opencv.hpp>

using namespace std;
namespace fs = filesystem;

// Config
namespace config {
  const int TARGET_WIDTH = 2000;
  const int TARGET_HEIGHT = 500;
  const string WORK_DIR = ".temp";
}

// Can't leave out those ✨fancy✨ console output colours
namespace console_colours {
  const string WARNING = "\033[93m";
  const string ERROR = "\033[91m";
  const string NORMAL = "\033[0m";
}

// Executes a command by opening a new shell using popen() and
// pipes the output to the given string.
int execute(string cmd, string& output) {
  const int bufsize=128;
  array<char, bufsize> buffer;

  auto pipe = popen(cmd.c_str(), "r");
  if (!pipe) throw runtime_error("popen() failed!");

  size_t count;
  do {
    if ((count = fread(buffer.data(), 1, bufsize, pipe)) > 0) {
      output.insert(output.end(), begin(buffer), next(begin(buffer), count));
    }
  } while(count > 0);

  return pclose(pipe);
}

// Returns the number of milliseconds passed since the UNIX epoch.
long current_timestamp() {
  using namespace chrono;
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

// Main function
int main(int argc, char* argv[]) {

  // Defining input and output
  const string input_file_path = argv[1];
  const string output_file_path = (argc > 2) ? argv[2] : string(argv[1]) + ".png";

  // Emptying work directory
  if (fs::is_directory(config::WORK_DIR)) {
    fs::remove_all(config::WORK_DIR);
  }
  fs::create_directory(config::WORK_DIR);

  // Checking input file existance
  if (!fs::exists(input_file_path)) {
    cout << console_colours::ERROR << "[ERROR] Input file does not exist" << 
    console_colours::NORMAL << endl;
    return 1;
  } else {
    cout << "[INFO] Input file: " << input_file_path << endl;
  }

  // Getting duration and calculating extraction rate
  float duration, rate;
  {
    const string probe_cmd = "ffprobe -i " + input_file_path +
    " -show_format -loglevel panic | grep duration";

    string d_str; execute(probe_cmd, d_str);
    duration = stof(d_str.substr(9));
    rate = duration / config::TARGET_WIDTH;
  }

  // Extracting frames
  const long start_time = current_timestamp();
  string ffmpeg_output;
  {
    const string ffmpeg_cmd = "ffmpeg -loglevel fatal -i " + input_file_path +
    " -s 100x100 -r 1/" + to_string(rate) + " " + config::WORK_DIR + "/frame%04d.bmp";
    execute(ffmpeg_cmd, ffmpeg_output);
  }
  if (!ffmpeg_output.empty()) {
    cout << ffmpeg_output << endl;
  }
  const int work_time = round((current_timestamp() - start_time) / 1000);

  // Collecting extracted frames and sorting them
  vector<string> extracted_files;
  for (const auto& entry : fs::directory_iterator(config::WORK_DIR)) {
    extracted_files.push_back(entry.path().generic_string());
  }
  sort(extracted_files.begin(), extracted_files.end());

  // Calculating average colour in each of the frames
  cout << "[INFO] Calculating colours";
  vector<cv::Vec3b> colours;
  for (const auto& filename : extracted_files) {
    const cv::Mat3b img = cv::imread(filename);
    cv::Mat3b img_b;
    cv::GaussianBlur(img, img_b, cv::Size(99, 99), 0);

    colours.push_back(img_b.at<cv::Vec3b>(cv::Point(50, 50)));

    const int progress_percent = (
      (double_t)size(colours) / (double_t)size(extracted_files)
    ) * 100.0;
    cout << "\r[INFO] Calculating colours (" << progress_percent << "%)";
  }
  cout << endl;

  // Creating output picture based on colour data
  const int actual_width = size(colours); // should be equal to TARGET_WIDTH, but you never know
  cv::Mat3b out_img = cv::Mat::zeros(cv::Size(actual_width, config::TARGET_HEIGHT), CV_8UC1);

  for (int x = 0; x < out_img.rows; x++) {
    for (int y = 0; y < out_img.cols; y++) {
      out_img.at<cv::Vec3b>(x, y) = colours[y];
    }
  }

  cv::imwrite(output_file_path, out_img);
  cout << "[INFO] Done! Work time: " << work_time << " seconds" << endl;
  cout << "[INFO] Output file: " << output_file_path << endl;

  // Removing work directory
  fs::remove_all(config::WORK_DIR);

  return 0;
}
