CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lcurl -lpigpio -lrt -lpthread -lm
OPENCV_LD =  -lopencv_core -lopencv_highgui -lopencv_imgproc

TARGETS = calibrate live_demo live_sim
TESTS   = test_video test_driver test_servo test_dump test_move
CAMTEST = opencv_server opencv_test
HEADERS = $(wildcard *.h)
MONGOOSE = mongoose/mongoose.o

.phony: all clean tests targets camtest


%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ -c $<

targets: $(TARGETS)
all:     $(TARGETS) $(TESTS) $(CAMTEST)
tests:   $(TESTS)
camtest: $(CAMTEST)

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
	$(CXX) $(CFLAGS) -std=c++11 -o $@ $< $(LDFLAGS) $(OPENCV_LD)

clean:
	rm -f $(TARGETS) $(TESTS) $(CAMTEST) $(MONGOOSE) *.o
