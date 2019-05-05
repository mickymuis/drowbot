#include <pigpio.h>
#include <stdlib.h>
#include <stdio.h>

#define L_STEP       5
#define L_DIR       12
#define R_STEP       6
#define R_DIR       13

#define SERVO_PWM   19
#define SERVO_MIN 1000
#define SERVO_MAX 2000
#define SERVO_MID 1500


#define STEP_FREQ  800
#define STEP_WAIT (((1000 * 1000)/2)/STEP_FREQ)

int pos[2] = {0};

int handle_steps(const int left, const int right, int freq){
  const int dir_l = (left < 0) ? 1 : -1,
            dir_r = (right < 0)? 1 : -1;
  int step_l = abs(left),
      step_r = abs(right);

  gpioWrite(L_DIR, (dir_l < 0)? 1 : 0);
  gpioWrite(R_DIR, (dir_r < 0)? 1 : 0);
  gpioDelay(1000);

  const int wait_len = (freq > 0) ? (((1000 * 1000)/2)/freq) : STEP_WAIT;

  while (step_l > 0 || step_r > 0){
    if (step_l > 0) gpioWrite(L_STEP, 1);
    if (step_r > 0) gpioWrite(R_STEP, 1);
    gpioDelay(wait_len);
    if (step_l-- > 0){
      gpioWrite(L_STEP, 0);
      pos[0] += dir_l;
    }
    if (step_r-- > 0){
      gpioWrite(R_STEP, 0);
      pos[1] +=dir_r;
    }
    gpioDelay(wait_len);
  }

  gpioWrite(R_STEP, 0);
  gpioWrite(L_STEP, 0);
  gpioWrite(R_DIR,  0);
  gpioWrite(L_DIR,  0);

  return 0;
}

int handle_servo(const int pos){
  const int realpos = SERVO_MIN + pos;
  if (realpos < SERVO_MAX && realpos > SERVO_MIN){
    gpioServo(SERVO_PWM, realpos);
  } else {
    gpioServo(SERVO_PWM, 0);
    return -1;
  }
  return 0;
}

int main(int argc, char ** argv){
  (void)argc;
  (void)argv;


  if (gpioInitialise() < 0)
    return -1;

  gpioSetMode(L_DIR,  PI_OUTPUT);
  gpioSetMode(R_DIR,  PI_OUTPUT);
  gpioSetMode(L_STEP, PI_OUTPUT);
  gpioSetMode(R_STEP, PI_OUTPUT);
  // gpioServo(SERVO_PWM, SERVO_MID);

  int left, right, freq;

  while (1){
    fprintf(stderr, "%5d %5d left right>", pos[0], pos[1]);
    if (scanf("%d%d%d",&left, &right,&freq) !=3 )
      break;
    if (left == 0 && right == 0)
      break;
    handle_steps(left, right, freq);
  }
  gpioWrite(R_STEP, 0);
  gpioWrite(L_STEP, 0);
  gpioWrite(R_DIR, 0);
  gpioWrite(L_DIR, 0);
  // gpioWrite(SERVO_PWM, 0);

  gpioTerminate();
  return 0;
}

