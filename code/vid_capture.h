#include <stdlib.h>

#ifndef __VID_CAPTURE_H__
#define __VID_CAPTURE_H__
typedef int (* frame_callback) (void *, size_t, void *);

int init_capture(const char * devname, frame_callback callback, void * ud);
int requeue_buffer(void * data);

void start_capture(const int max_frames);
void stop_capture();

#endif