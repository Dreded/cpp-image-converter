#include "lib/argparse.hpp"
#include <filesystem>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv4/opencv2/core/base.hpp>
#include <opencv4/opencv2/core/types.hpp>
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
  int &output_height =
      kwarg("h,height",
            "Resize to output height(if no width then maintain aspect ratio)")
          .set_default(0);
  int &output_width =
      kwarg("w,width",
            "Resize to output width(if no height then maintain aspect ratio)")
          .set_default(0);
  bool &add_border = flag(
      "b,border", "Specify both width and height but still maintain aspect "
                  "ratio(resizes to width and add border to height)");

  // TODO: Sanity check the text input for the extension, OpenCV already
  // does this.. but it throws an ugly error.
  std::string &extension =
      kwarg("e,extension",
            "Specify output file ext(saves as that type) defaults to '.jpg'")
          .set_default(".jpg");
};

void quit(std::string message, int exit_status = 0) {
  std::cout << message << std::endl;
  exit(exit_status);
}

void print(std::string m) { std::cout << m << std::endl; }
void print(std::string m, int x) { std::cout << m << x << std::endl; }

// cv::INTER_AREA   = best for shrinking
// cv::INTER_CUBIC  = best for growing
// cv::INTER_LINEAR = best general purpose

auto add_border(cv::Mat &img, cv::Size dim, cv::Size out_res) {
  int b_tb = 0, b_lr = 0;
  //  print("In add border");

  //  print("Out_res: " + std::to_string(out_res.width) + "x" +
  //  std::to_string(out_res.height)); print("Dim: " + std::to_string(dim.width)
  //  + "x" + std::to_string(dim.height));
  if (out_res.height > dim.height)
    b_tb = (out_res.height - dim.height) / 2;
  if (out_res.width > dim.width)
    b_lr = (out_res.width - dim.width) / 2;

  //  print("b_tb = ", b_tb);
  //  print("b_lr = ", b_lr);

  cv::Mat border_added;
  cv::copyMakeBorder(img, border_added, b_tb, b_tb, b_lr, b_lr,
                     cv::BORDER_CONSTANT);
  return border_added;
}

auto resize(cv::Mat &img, cv::Size out_res, bool border, int inter_type = -1) {
  cv::Size dim;
  cv::Size size = img.size();
  //  print("Orig_Width: ", size.width);
  //  print("Orig_Height: ", size.height);

  float r = 0.0;

  if (out_res.height == 0 and out_res.width == 0)
    return img;

  if (out_res.width == 0 || (out_res.width > size.width && border)) {
    r = (float)out_res.height / (float)size.height;
    dim = cv::Size(size.width * r, out_res.height);
  } else {
    r = (float)out_res.width / (float)size.width;
    dim = cv::Size(out_res.width, size.height * r);
  }

  if (inter_type < 0) {
    if (r < 1)
      inter_type = cv::INTER_AREA;
    else
      inter_type = cv::INTER_CUBIC;
  }

  cv::Mat resized;
  cv::resize(img, resized, dim, 0, 0, inter_type);

  if (border)
    resized = add_border(resized, dim, out_res);

  return resized;
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

int main(int argc, char *argv[]) {
  auto args = argparse::parse<MyArgs>(argc, argv);

  if (!std::filesystem::exists(args.fullpath)) {
    // TODO: Can we read it?
    quit("Input file does not exist, check the name.", 1);
  }
  auto ext = args.fullpath.extension();
  auto filename = args.fullpath.filename();
  auto parent = args.fullpath.parent_path();
  auto output_file = parent / filename.replace_extension(args.extension);
  auto img = imread(args.fullpath, cv::IMREAD_COLOR);
  auto out_res = cv::Size(args.output_width, args.output_height);
  img = resize(img, out_res, args.add_border);
  imwrite(output_file, img);

  completed_message += "\tSaved to: " + output_file.string();

  if (args.delete_file) {
    deleteFile(args.fullpath);
  }

  print(completed_message);
  return 0;
}
