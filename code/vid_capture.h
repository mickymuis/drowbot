#include <stdlib.h>

typedef void (* frame_callback) (void *, size_t, void *);

int init_capture(const char * devname, frame_callback callback, void * ud);

void start_capture(const int max_frames);
void stop_capture();
