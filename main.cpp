#include <filesystem>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <string>

void quit(std::string message, int exit_status) {
  std::cout << message << std::endl;
  exit(exit_status);
}

void print(std::string message) { std::cout << message << std::endl; }

using namespace cv;
int main(int argc, char *argv[]) {
  std::filesystem::path fullpath;
  if (argc > 1) {
    fullpath = argv[1];
  } else {
    quit("Second argument must be an image.", 1);
  }
  if (!std::filesystem::exists(fullpath)) {
    quit("Input file does not exist, check the name.", 1);
  }
  std::filesystem::path ext = fullpath.extension();
  std::filesystem::path filename = fullpath.filename();
  std::filesystem::path parent = fullpath.parent_path();

  std::filesystem::path output_file =
      parent / filename.replace_extension(".jpg");

  Mat img = imread(fullpath, IMREAD_COLOR);

  imwrite(output_file, img);
  std::string save_message = "Saved To: " + output_file.string();
  print(save_message);
  return 0;
}
