CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lcurl -lpigpio -lrt -lpthread -lm
OPENCV_LD =  -lopencv_core -lopencv_highgui -lopencv_imgproc

TARGETS = test_video test_driver calibrate test_move
HEADERS = $(wildcard *.h)
MONGOOSE = mongoose/mongoose.o

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ -c $<

all: $(TARGETS)

test_video: test_video.o vid_capture.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test_driver: test_driver.o driver.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test_move: test_move.o move.o driver.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)


calibrate: calibrate.c driver.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

opencv_server: opencv_server.cc $(MONGOOSE)
	$(CXX) $(CFLAGS) -std=c++11 -o $@ $^ $(LDFLAGS) $(OPENCV_LD)

clean:
	rm -f $(TARGETS) $(MONGOOSE) opencv_server *.o
