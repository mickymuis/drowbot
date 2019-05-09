#include <pigpio.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef __DRIVER_H__
#define __DRIVER_H__

#define L_STEP       5
#define L_DIR       12
#define R_STEP       6
#define R_DIR       13

#define SRV_PWM     19
#define SRV_DRAW  1200
#define SRV_SKIP   500

#define SRV_MIN   500
#define SRV_MAX   1500
#define SRV_MID   1000


#define T_MS      1000
#define T_S       (T_MS * 1000)

#define SRV_WAIT  T_S/2

#define STEP_FREQ  100
#define STEP_WAIT (T_S/(STEP_FREQ*2))


int   drv_init(const int dir_left, const int dir_right);
int   drv_set_dir(const int dir_left, const int dir_right);
void  drv_term();
void  drv_stop();

int   drv_step_to(const int left, const int right, const int draw);
int   drv_step(const int left, const int right, const int draw);

int   drv_lpos();
int   drv_rpos();
int   drv_spos();

void  drv_set_pos(const int left, const int right);
void  drv_reset_pos();

void drv_servo_stuff();

#endif
