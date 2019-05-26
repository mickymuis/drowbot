#include <stdlib.h>

//Does all camera handling in the background

#ifndef __VID_CAPTURE_H__
#define __VID_CAPTURE_H__

//A callback function called for every frame with (img_data, img_size, userdata)
//If the return values is nonzero the provided buffer will not be requeued and
//should be requeued manually by passing the pointer to requeue_buffer
typedef int (* frame_callback) (void *, size_t, void *);

//Initialise the capture with a /dev/video* path, the callback, and user data
int init_capture(const char * devname, frame_callback callback, void * ud);
//Requeue a buffer that was passed via the frame_callback
int requeue_buffer(void * data);

//Starts/stops the capture thread with max_frames > 0 limiting the capture time
void start_capture(const int max_frames);
void stop_capture();

#endif