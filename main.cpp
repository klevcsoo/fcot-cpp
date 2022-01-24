#include <iostream>
#include <filesystem>
#include <array>
#include <chrono>
#include <math.h>

using namespace std;
namespace fs = filesystem;

// Config
namespace Config {
  const int TARGET_WIDTH = 2000;
  const int TARGET_HEIGHT = 500;
  const string WORK_DIR = ".temp";
}

// Can't leave out those ✨fancy✨ console output colours
namespace ConsoleColours {
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
  if (fs::is_directory(Config::WORK_DIR)) {
    fs::remove_all(Config::WORK_DIR);
  }
  fs::create_directory(Config::WORK_DIR);

  // Checking input file existance
  if (!fs::exists(input_file_path)) {
    cout << ConsoleColours::ERROR << "[ERROR] Input file does not exist" << 
    ConsoleColours::NORMAL << endl;
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
    rate = duration / Config::TARGET_WIDTH;
  }

  // Extracting frames
  const long start_time = current_timestamp();
  string ffmpeg_output;
  {
    const string ffmpeg_cmd = "ffmpeg -loglevel fatal -i " + input_file_path +
    " -s 100x100 -r 1/" + to_string(rate) + " " + Config::WORK_DIR + "/frame%03d.bmp";
    execute(ffmpeg_cmd, ffmpeg_output);
  }
  if (!ffmpeg_output.empty()) {
    cout << ffmpeg_output << endl;
  }
  const int work_time = round((current_timestamp() - start_time) / 1000);

  return 0;
}
