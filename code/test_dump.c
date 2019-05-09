#include <stdio.h>
#include "driver.h"
#include "move.h"
#include "vid_capture.h"
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>

static int fcount = 0;
static uint8_t * img_dat = NULL;
static size_t img_len = 0;
static int run_imgloop = 1;
int dump_count = 0;
int dump_imgs  = 0;

int cb(void * data, size_t len, void * udat){
  (void)udat;
  // fprintf(stderr, "%d: %ub (%p)\n", fcount, len, data);
  fcount++;
  if (img_dat == NULL && dump_imgs){
    img_dat = data;
    img_len = len;
    return 1;
  }
  return 0;
}


void save_img_dat(){
  int now = time(NULL);
  char fn[128];
  snprintf(fn, 128,"%d-%05d.YV12", now,dump_count++);
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
    if (img_dat != NULL && dump_imgs){
      // fprintf(stderr, "Sending image\n");
      // send_img_dat();
      if (dump_imgs)
        save_img_dat();
      img_dat = NULL;
    }
    struct timespec tmp = {.tv_sec=0,.tv_nsec = 10 * 1000 * 1000};
    nanosleep(&tmp, NULL);
  }
  if (img_dat)
    requeue_buffer(img_dat);

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

  if (move_init())
    return -1;
  int x,y,draw,dump = 0;
  pthread_t thread;
  while (1){
    fprintf(stderr, "%5d %5d left right -1 -1?>",
                     get_x(), get_y());
    if (scanf("%d%d%d%d",&x, &y,&draw,&dump) !=4 )
      break;
    if (dump){
      dump_imgs = 1;
      if (init_capture(argv[1], &cb, NULL))
        return 1;

      start_capture(-1);
      pthread_create(&thread, NULL, &imgsend_loop, NULL);
      // dump_imgs = 0;

    }
    // if (dump)
    if (x <  0 || y < 0)
      break;
    // if (draw)

    move_to(x, y, draw);
    if (dump){

      stop_capture();
      dump_imgs = 0;
      run_imgloop = 0;
      pthread_join(thread, NULL);
    }
  }


  move_term();
  return 0;

}