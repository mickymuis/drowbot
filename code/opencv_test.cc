#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <string>
#include <cstdint>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include "imgproc.h"

#define IMG_W 640
#define IMG_H 480
#define IMGSIZE IMG_W * (IMG_H + IMG_H/2)

const std::string gui_name = "drowbot remote gui";
cv::Mat rgbimg;


static void handle_img(void * data, const size_t len){
  process_image(data);
  cv::Mat imgbuf(IMG_H + IMG_H/2, IMG_W, CV_8UC1, data);
  rgbimg = cv::Mat(IMG_H, IMG_W, CV_8UC3);
  // fprintf(stderr, "bufsize %lu\n", imgbuf.total());
  // fprintf(stderr, "bufsize %lu\n", rgbimg.total());
  // fprintf(stderr, "%d %d\n", rgbimg.rows, rgbimg.cols);
  cv::cvtColor(imgbuf, rgbimg, cv::COLOR_YUV2BGR_I420);
  // fprintf(stderr, "%d %d\n", rgbimg.rows, rgbimg.cols);
  cv::imshow(gui_name, rgbimg);
}

void load_file(const char * filename){
  void * mem = malloc(IMGSIZE);
  fprintf(stderr, "Loading %s\n", filename);
  int f = open(filename, O_RDONLY);
  if (f > -1)
    if (read(f, mem, IMGSIZE) != IMGSIZE)
      perror("write");
  close(f);
  handle_img(mem, IMGSIZE);
  free(mem);
}


int main(int argc, char ** argv){
  if (argc < 2)
    return 1;
  cv::namedWindow(gui_name, cv::WINDOW_AUTOSIZE);
  int key = 0;
  int loop = 1;
  int img = 0;
  const int lastimg = argc - 1;
  load_file(argv[img + 1]);

  while (loop){
    key = cv::waitKey();
    switch (key){
      case 'x':
      case 'q': loop = 0; break;
      case 'n':
        if (++img > lastimg)
          img = 1;
        load_file(argv[img]);
        break;
      case 'p':
      case 'b':
        if (--img == 0)
          img = lastimg;
        load_file(argv[img]);
        break;
      default:
        fprintf(stderr, "%c %d\n",key, key);
        break;
    }
  }
  cv::destroyWindow(gui_name);
  return 0;
}