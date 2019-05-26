#include <stdlib.h>
#include <stdio.h>

//Abstracts away all the GPIO and provides a simple interface to
//Move the steppers and servo
//This uses PIGPIO internally, but if you want it to use something
//else you only have to change driver.c since all other files use
//these functions and do not interact with GPIO

#ifndef __DRIVER_H__
#define __DRIVER_H__

//GPIO pin numbers
#define L_STEP       5
#define L_DIR       12
#define R_STEP       6
#define R_DIR       13

//Servo settings
#define SRV_PWM     19
#define SRV_MIN    500
#define SRV_MAX   1500
#define SRV_MID   1000

//Preset positions for the servo to start/stop drawing
#define SRV_DRAW  1200
#define SRV_SKIP   500


//Constants for timing in microseconds
#define T_MS      1000
#define T_S       (T_MS * 1000)

//Wait time to turn servo off again after starting PWM
#define SRV_WAIT  T_S/2

//Determines the wait time between steps, not actual frequency
#define STEP_FREQ  100
#define STEP_WAIT (T_S/(STEP_FREQ*2))


//Initialisation and termination functions
int   drv_init(const int dir_left, const int dir_right);
int   drv_set_dir(const int dir_left, const int dir_right);
void  drv_term();
void  drv_stop();

//Execute steps to an absolute or relative position
//These apply smoothing on step level
int   drv_step_to(const int left, const int right, const int draw);
int   drv_step(const int left, const int right, const int draw);

//Read the positions of the steppers and servo
int   drv_lpos();
int   drv_rpos();
int   drv_spos();

//Overwrite the current positions for both chains
void  drv_set_pos(const int left, const int right);
void  drv_reset_pos();

//Control the servo position separately
void  drv_set_draw(const int draw);

//Used to test the servo with some protection against moving too far
void  drv_servo_testing();

//Execute steps without smoothing
//Should really only be used for very small step counts
//Is more optimised to execute steps quickly than the smoothed ones
void drv_new_step_to(const int l, const int r);

#endif
