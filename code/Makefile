CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lcurl -lpigpio -lrt -lpthread
OPENCV_LD =  -lopencv_core -lopencv_highgui -lopencv_imgproc

TARGETS = test_video driver_test
HEADERS = $(wildcard *.h)
MONGOOSE = mongoose/mongoose.o

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ -c $<

all: $(TARGETS)

test_video: test_video.c vid_capture.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

driver_test: driver_test.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

opencv_server: opencv_server.cc $(MONGOOSE)
	$(CXX) $(CFLAGS) -std=c++11 -o $@ $^ $(LDFLAGS) $(OPENCV_LD)

clean:
	rm -f $(TARGETS) $(MONGOOSE) opencv_server *.o
