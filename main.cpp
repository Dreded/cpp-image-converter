#include "lib/argparse.hpp"
#include <filesystem>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <string>

struct MyArgs : public argparse::Args {
  //    std::string &anonymous = arg("an anonymous positional string argument");
  //    std::string &src_path  = arg("src_path", "a positional string
  //    argument"); int &k                 = kwarg("k", "A keyworded integer
  //    value"); float &alpha           = kwarg("a,alpha", "An optional float
  //    value").set_default(0.5f); bool &verbose          = flag("v,verbose", "A
  //    flag to toggle verbose");
  std::filesystem::path &fullpath =
      arg("img", "Path to the source image to convert");
};

void quit(std::string message, int exit_status = 0) {
  std::cout << message << std::endl;
  exit(exit_status);
}

void print(std::string message) { std::cout << message << std::endl; }

using namespace cv;
int main(int argc, char *argv[]) {
  auto args = argparse::parse<MyArgs>(argc, argv);

  if (!std::filesystem::exists(args.fullpath)) {
    quit("Input file does not exist, check the name.", 1);
  }
  std::filesystem::path ext = args.fullpath.extension();
  std::filesystem::path filename = args.fullpath.filename();
  std::filesystem::path parent = args.fullpath.parent_path();

  std::filesystem::path output_file =
      parent / filename.replace_extension(".jpg");

  Mat img = imread(args.fullpath, IMREAD_COLOR);

  imwrite(output_file, img);
  std::string save_message = "Saved To: " + output_file.string();
  print(save_message);
  return 0;
}
