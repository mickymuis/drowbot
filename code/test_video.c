#include "vid_capture.h"
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <curl/curl.h>
#include <time.h>
#include <fcntl.h>


static int fcount = 0;
static uint8_t * img_dat = NULL;
static size_t img_len = 0;
static int run_imgloop = 1;

void cb(void * data, size_t len, void * udat){
  (void)udat;
  fprintf(stderr, "%d: %ub (%p)\n", fcount, len, data);
  fcount++;
  if (img_dat == NULL){
    img_dat = malloc(len);
    if (img_dat == NULL)
      return;
    img_len = len;
    memcpy(img_dat, data, len);
  }
}

void send_img_dat(){
  CURL *handle = curl_easy_init();

  if (handle){
    struct curl_slist *headers=NULL;
    headers = curl_slist_append(headers, "Content-Type: application/octet-stream");
    curl_easy_setopt(handle, CURLOPT_POSTFIELDS,    img_dat);
    curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, img_len);
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER,    headers);
    curl_easy_setopt(handle, CURLOPT_URL, "http://192.168.43.36:8080/");
    CURLcode res = curl_easy_perform(handle);
    if (res){
      fprintf(stderr, "Error in CURL perform\n");
    }
    curl_slist_free_all(headers);
    curl_easy_cleanup(handle);
  }
}

void save_img_dat(){
  int now = time(NULL);
  char fn[128];
  snprintf(fn, 128,"%d.YV12", now);
  int f = open(fn, O_WRONLY | O_CREAT, 00666);
  if (f > -1)
    if (write(f, img_dat, img_len) != (int)img_len)
      perror("write");
  close(f);
}


void * imgsend_loop(void * data){
  (void)data;
  run_imgloop = 1;

  while (run_imgloop){
    if (img_dat != NULL){
      fprintf(stderr, "Sending image\n");
      send_img_dat();
      // save_img_dat();
      free(img_dat);
      img_dat = NULL;
    }
    struct timespec tmp = {.tv_sec=0,.tv_nsec = 10 * 1000 * 1000};
    nanosleep(&tmp, NULL);
  }
  if (img_dat)
    free(img_dat);
  return NULL;
}



// size_t curlread(char *bufptr, size_t size, size_t nitems, void *userp){

  // "application/octet-stream";
// }

int main(int argc, char ** argv){
  if (argc < 2){
    fprintf(stderr, "Error, expecting the video device as argument\n");
    return -1;
  }
  if (init_capture(argv[1], &cb, NULL))
    return 1;

  start_capture(-1);
  pthread_t thread;
  pthread_create(&thread, NULL, &imgsend_loop, NULL);

  while (getchar()!='x');

  stop_capture();
  run_imgloop = 0;
  pthread_join(thread, NULL);

  return 0;
}