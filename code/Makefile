CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lcurl -lpigpio -lrt -lpthread -lm

HEADERS = $(wildcard *.h)
TARGETS = calibrate live_demo live_sim dragon
TESTS   = test_video test_driver test_servo test_dump test_move

# For camera testing only, does not need to be compiled on the Pi
CAMTEST = opencv_server opencv_test
CXX = g++
OPENCV_LD =  -lopencv_core -lopencv_highgui -lopencv_imgproc
MONGOOSE = mongoose/mongoose.o

.phony: all clean tests targets camtest

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ -c $<

targets: $(TARGETS)
all:     $(TARGETS) $(TESTS) $(CAMTEST)
tests:   $(TESTS)
camtest: $(CAMTEST)
clean:
	rm -f $(TARGETS) $(TESTS) $(CAMTEST) $(MONGOOSE) *.o

# standard targets
calibrate: calibrate.c driver.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

live_demo: live_demo.o ca.o move.o driver.o vid_capture.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

dragon: dragon.o move.o driver.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

#Simulates the demo movement for debugging
live_sim: live_demo.o ca.o move_fake.o vid_capture.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)


# tests
test_driver: test_driver.o driver.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test_move: test_move.o move.o driver.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test_dump: test_dump.o move.o driver.o vid_capture.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test_servo: test_servo.o driver.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test_video: test_video.o vid_capture.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# camtest - test GUIs for camera testing to compile on a normal machine
opencv_server: opencv_server.cc $(MONGOOSE) $(HEADERS)
	$(CXX) $(CFLAGS) -std=c++11 -o $@ $^ $(LDFLAGS) $(OPENCV_LD)

opencv_test: opencv_test.cc $(HEADERS)
	$(CXX) $(CFLAGS) -std=c++11 -o $@ $< $(LDFLAGS) $(OPENCV_LD)

