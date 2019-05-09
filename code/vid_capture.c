#include "vid_capture.h"
#include <linux/videodev2.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <time.h>

#include <pthread.h>


static int xioctl(int fh, int request, void *arg)
{
  int r;
  do { r = ioctl(fh, request, arg);}
  while (-1 == r && EINTR == errno);
  return r;
}

typedef struct buffer {
        void   *start;
        size_t  length;
} buf_t;

buf_t * buffers = NULL;
unsigned  buf_c = 0;
int         cfd = 0;
frame_callback fcb = NULL;
void * udata = NULL;

int run = 0;
int max_frames = 0;

pthread_t thread = {0};

int set_format(const int fd){
  struct v4l2_format fmt = {0};
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  fmt.fmt.pix.width       = 640;
  fmt.fmt.pix.height      = 480;
  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUV420;
  if (xioctl(fd,  VIDIOC_S_FMT, &fmt) == -1){
    perror("ioctl video format");
    return -1;
  }
  return 0;
}

int init_buffers(const int fd){
  struct v4l2_requestbuffers req = {0};
  req.count = 4;
  req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory = V4L2_MEMORY_MMAP;

  if (xioctl(fd, VIDIOC_REQBUFS,&req) == -1){
    perror("`reqbufs");
    return -1;
  }

  buffers = calloc(req.count, sizeof(*buffers));

  for (buf_c = 0; buf_c < req.count; buf_c ++){
    struct v4l2_buffer buf = {0};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = buf_c;
    if (xioctl(fd, VIDIOC_QUERYBUF,&buf) == -1){
      perror("querybuf");
      return -1;
    }
    buffers[buf_c].length = buf.length;
    buffers[buf_c].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE,
                                MAP_SHARED, fd, buf.m.offset);
    if (MAP_FAILED == buffers[buf_c].start){
      perror("mmap");
      return -1;
    }
  }
  for (unsigned i = 0; i < buf_c; i++){
    struct v4l2_buffer buf = {0};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = i;
    if (xioctl(fd, VIDIOC_QBUF,&buf) == -1){
      perror("queue buf");
      return -1;
    }
  }
  return 0;
}

int get_frame(int fd){
  struct v4l2_buffer buf = {.type=V4L2_BUF_TYPE_VIDEO_CAPTURE,
                            .memory = V4L2_MEMORY_MMAP, {0}};

  if (xioctl(fd, VIDIOC_DQBUF,&buf) == -1){
    perror("get_frame_DQ");
    return -1;
  }
  int requeue = 1;
  if (fcb)
    if ((*fcb)(buffers[buf.index].start, buf.bytesused, udata))
      requeue = 0;

  if (requeue){
    if (xioctl(fd, VIDIOC_QBUF,&buf) == -1){
      perror("get_frame_Q");
      return -1;
    }
  }
  return 0;
}

int requeue_buffer(void * data){
  for (unsigned i = 0; i < buf_c; i++)
    if (buffers[i].start == data){
      struct v4l2_buffer buf = {.index = i,
                                .type=V4L2_BUF_TYPE_VIDEO_CAPTURE,
                                .memory = V4L2_MEMORY_MMAP, {0}};
      if (xioctl(cfd, VIDIOC_QBUF,&buf) == -1){
        perror("queue buf");
        return -1;
      }
      return 0;

    }

  return -1;
}

void* main_loop(void * args){
  (void)args;
  enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (xioctl(cfd, VIDIOC_STREAMON, &type)){
    perror("stream on");
    return NULL;
  }
  int count = 0;
  while (run){
    struct timeval tv = {.tv_sec = 2, .tv_usec = 0};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(cfd, &fds);
    int r = select(cfd + 1, &fds, NULL, NULL, &tv);
    if (r == -1){
      perror("select");
      break;
    }
    else if (r == 0){
      perror("select timeout");
      continue;
    }

    if (get_frame(cfd))
      break;

    count++;
    if (count == max_frames)
      break;
  }
  if (xioctl(cfd, VIDIOC_STREAMOFF, &type)){
    perror("stream off");
    return NULL;
  }
  return NULL;
}

void clean_up(){
  if (cfd > 0){
    close(cfd);
  }
  if (buffers){
    for (unsigned i = 0; i < buf_c; i++)
      munmap(buffers[i].start, buffers[i].length);
    free(buffers);
  }
}


static inline int set_control(int id, int value){
  struct v4l2_control control = {0};
  control.id = id;
  control.value = value;

  if (xioctl(cfd, VIDIOC_S_CTRL, &control)){
    perror("Set control");
    return -1;
  }
  return 0;
}

int set_controls(int value){
  if (set_control(0x00980914, value)) return -1;
  if (set_control(0x00980915, value)) return -1;
  if (xioctl(cfd, VIDIOC_OVERLAY, &value)){
   perror("Enable overlay");
    return -1;
  }
  return 0;
}

int init_capture(const char * filename, frame_callback callback, void *ud){
  cfd = open(filename, O_RDWR | O_NONBLOCK);
  if (cfd < 0){
    perror("open");
    return -1;
  }

  if (set_format(cfd))
    goto clean;

  if (init_buffers(cfd))
    goto clean;

  if(set_controls(0))
    goto clean;


  fcb = callback;
  udata = ud;

  return 0;
clean:
  clean_up();
  return -1;
}

void start_capture(const int max_frames_arg){
  run = 1;
  max_frames = max_frames_arg;
  if (pthread_create(&thread, NULL, &main_loop, NULL))
    perror("pthread_create");
}

void stop_capture(){
  if (run){
    run = 0;
    pthread_join(thread, NULL);
    clean_up();
  }
}