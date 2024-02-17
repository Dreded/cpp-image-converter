# cpp-image-converter

Simple command line tool to convert images from one format to another

## Run

`image_convert filename`  
Allows for any image type opencv recognizes, opencv examines the file by contents.
So if the extension is wrong it doesn't matter.so if the extension is wrong it
doesn't matter.

Written in C++ uses opencv2

## TODO

* Make work on wildcard list of files.
* * for now just use find exec `find *.webp -exec image_convert {} \;`
* allow file with list as input
* allow to specify output name, if list append ### to end of name
* allow specify output extension currently outputs only JPG
* allow to specify output resolution(resize)

## Build

`clang++-17 main.cpp -lopencv_core -lopencv_imgcodecs -o image_convert`
