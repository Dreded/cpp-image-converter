#include "lib/argparse.hpp"
#include <filesystem>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <string>

// String to store out final output message.
std::string completed_message = "";

struct MyArgs : public argparse::Args {

  //  std::string &anonymous  = arg("an anonymous positional string argument");
  //  std::string &src_path   = arg("src_path", "a positional string argument");
  //  int &k                  = kwarg("k", "A keyworded integer value");
  //  float &alpha            = kwarg("a,alpha", "optional").set_default(0.5f);
  //  bool &verbose           = flag("v,verbose", "Aflag to toggle verbose");

  std::filesystem::path &fullpath =
      arg("img", "Path to the source image to convert");
  bool &delete_file =
      flag("delete", "Delete the original image after conversion?");

  // TODO: Sanity check the input, OpenCV already does this.. but it throws an
  // ugly error.
  std::string &extension =
      kwarg("e,extension",
            "Specify output file ext(saves as that type) defaults to '.jpg'")
          .set_default(".jpg");
};

void quit(std::string message, int exit_status = 0) {
  std::cout << message << std::endl;
  exit(exit_status);
}

void deleteFile(const std::filesystem::path file) {
  // returns true if deleted
  bool result = std::filesystem::remove(file);
  if (!result) {
    // File not deleted!

    // TODO: Pretty much can always delete the file, check if ro and warn, if
    // the file is owned by anyone but the directory is still mine I can delete
    // the file.
    // Maybe a sain way would be to do what 'rm' does
    //
    completed_message +=
        "\n\tOriginal image not deleted, is it on a readonly filesystem?";
    return;
  }
  completed_message += "\n\tDeleted Original at: " + file.string();
  return;
}

void print(std::string message) { std::cout << message << std::endl; }

using namespace cv;
int main(int argc, char *argv[]) {
  auto args = argparse::parse<MyArgs>(argc, argv);

  if (!std::filesystem::exists(args.fullpath)) {
    // TODO: Can we read it?
    quit("Input file does not exist, check the name.", 1);
  }
  std::filesystem::path ext = args.fullpath.extension();
  std::filesystem::path filename = args.fullpath.filename();
  std::filesystem::path parent = args.fullpath.parent_path();

  std::filesystem::path output_file =
      parent / filename.replace_extension(args.extension);

  Mat img = imread(args.fullpath, IMREAD_COLOR);

  imwrite(output_file, img);
  completed_message += "\tSaved to: " + output_file.string();

  if (args.delete_file) {
    deleteFile(args.fullpath);
  }

  print(completed_message);
  return 0;
}
