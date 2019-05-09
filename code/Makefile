CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lcurl -lpigpio -lrt -lpthread -lm
OPENCV_LD =  -lopencv_core -lopencv_highgui -lopencv_imgproc

TARGETS = test_video test_driver calibrate test_move test_servo test_dump \
					live_demo live_sim
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

test_dump: test_dump.o move.o driver.o vid_capture.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test_servo: test_servo.o driver.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

calibrate: calibrate.c driver.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

live_demo: live_demo.o ca.o move.o driver.o vid_capture.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

live_sim: live_demo.o ca.o move_fake.o vid_capture.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)


opencv_server: opencv_server.cc $(MONGOOSE) $(HEADERS)
	$(CXX) $(CFLAGS) -std=c++11 -o $@ $^ $(LDFLAGS) $(OPENCV_LD)

opencv_test: opencv_test.cc $(HEADERS)
	$(CXX) $(CFLAGS) -std=c++11 -o $@ $^ $(LDFLAGS) $(OPENCV_LD)

clean:
	rm -f $(TARGETS) $(MONGOOSE) opencv_server opencv_test *.o
