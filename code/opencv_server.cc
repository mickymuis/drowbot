#include "mongoose/mongoose.h"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <string>
#include <cstdint>
#include <vector>
#include "imgproc.h"

//Will run a Mongoose webserver on localhost:8080 that can receive
//raw images and display them while also processing the images with
//the filter in imgproc.h
//Run test_video on the Raspberry pi after setting the correct server
//URL and this program will receive and display them.
//q exits, but this might cause weird race conditions on the Pi side
//so it might either crash here or on the pi if you are not careful

#define IMG_W 640
#define IMG_H 480
#define IMGSIZE IMG_W * (IMG_H + IMG_H/2)

const std::string gui_name = "drowbot remote gui";
cv::Mat rgbimg;

static const struct mg_str post_str = MG_MK_STR("POST");
static const char *s_http_port = "8080";
int ws_run = 1;

static void handle_img(const void * data){
  void * tmp = malloc(IMGSIZE);
  std::memcpy(tmp, data, IMGSIZE);
  process_image_verbose(tmp);
  cv::Mat imgbuf(IMG_H + IMG_H/2, IMG_W, CV_8UC1, tmp);
  rgbimg = cv::Mat(IMG_H, IMG_W, CV_8UC3);
  cv::cvtColor(imgbuf, rgbimg, cv::COLOR_YUV2BGR_I420);
  cv::imshow(gui_name, rgbimg);
  free(tmp);
}

static void ev_handler(struct mg_connection *c, int ev, void *data) {
  if (ev == MG_EV_HTTP_REQUEST) {
    struct http_message *hm = (struct http_message *) data;
    if (mg_strcmp(hm->method, post_str) == 0){
      handle_img(hm->body.p);
      mg_send_head(c, 200, 0, "Content-Type: text/plain");
    }
  }
}


void server_start(){
  struct mg_mgr mgr;
  struct mg_connection *c;

  mg_mgr_init(&mgr, NULL);
  c = mg_bind(&mgr, s_http_port, ev_handler);
  mg_set_protocol_http_websocket(c);
  printf("Listening on: http://localhost:%s/\n", s_http_port);

  while (ws_run) {
    mg_mgr_poll(&mgr, 1000);
    if (cv::waitKey(10) == 'q')
      break;
  }
  mg_mgr_free(&mgr);
}


int main(){
  cv::namedWindow(gui_name, cv::WINDOW_AUTOSIZE);
  server_start();
  cv::destroyWindow(gui_name);
  return 0;
}
