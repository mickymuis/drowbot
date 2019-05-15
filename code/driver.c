#include <pigpio.h>
#include <stdlib.h>
#include <stdio.h>
#include "driver.h"

int pos[2]      = {0};
int real_pos[2] = {0};
int serv_pos    = SRV_MID;
int dir[2]      = {0};

int drv_lpos()  {return pos[0];}
int drv_rpos()  {return pos[1];}
int drv_spos() {return serv_pos;}

void drv_set_pos(const int left, const int right){
             pos[0] = left; pos[1] = right;}

void drv_reset_pos(){ pos[0] = 0; pos[1] = 0;}


int handle_steps(const int left, const int right){
  const int dir_l = (left < 0) ? 1 : -1,
            dir_r = (right < 0)? 1 : -1;
  int step_l = abs(left),
      step_r = abs(right);

  if (step_l == 0 && step_r == 0)
    return 0;

  double lrat = 1.0, rrat = 1.0;
  if (step_l == 0 || step_r == 0){

  } else if (step_l > step_r)
    rrat = (double)step_r/step_l;
  else
    lrat = (double)step_l/step_r;

  gpioWrite(L_DIR, (dir_l < 0)? 1 : 0);
  gpioWrite(R_DIR, (dir_r < 0)? 1 : 0);
  gpioDelay(1000);

  // const int wait_len = (freq > 0) ? (((1000 * 1000)/2)/freq) : STEP_WAIT;
  const int wait_len = STEP_WAIT;
  double cl = step_l, cr = step_r;
  while (step_l > 0 || step_r > 0){
    int sl = 0, sr = 0;
    cl -= lrat;
    cr -= rrat;
    if (step_l > cl && step_l > 0){
      gpioWrite(L_STEP, 1);
      sl = 1;
    }
    if (step_r > cr && step_r > 0){
      gpioWrite(R_STEP, 1);
      sr = 1;
    }
    gpioDelay(wait_len);
    if (sl && step_l-- > 0){
      gpioWrite(L_STEP, 0);
      real_pos[0] += dir_l;
    }
    if (sr && step_r-- > 0){
      gpioWrite(R_STEP, 0);
      real_pos[1] += dir_r;
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
  if (pos == serv_pos)
    return 0;

  if (pos <= SRV_MAX && pos >= SRV_MIN){
    gpioServo(SRV_PWM, pos);
    gpioDelay(SRV_WAIT);
    gpioServo(SRV_PWM, 0);
    serv_pos = pos;
  } else {
    gpioServo(SRV_PWM, 0);
    return -1;
  }
  return 0;
}

void drv_set_draw(const int draw){
  if (draw)
    handle_servo(SRV_DRAW);
  else
    handle_servo(SRV_SKIP);
}

int drv_step_to(const int left, const int right, const int draw){
  if (draw)
    handle_servo(SRV_DRAW);
  else
    handle_servo(SRV_SKIP);

  int ldist = (left  - pos[0]) * dir[0];
  int rdist = (right - pos[1]) * dir[1];
  int lrpos = real_pos[0];
  int rrpos = real_pos[1];
  fprintf(stderr, "step from %d %d to %d %d (%d %d, %d %d)\n",
                  pos[0],pos[1], left, right, lrpos, rrpos,
                  lrpos + ldist, rrpos + rdist);

  if (handle_steps(ldist, rdist)){
    pos[0] += dir[0] * (lrpos - real_pos[0]);
    pos[1] += dir[1] * (rrpos - real_pos[1]);
    return -1;
  }
  pos[0] = left;
  pos[1] = right;

  return 0;
}

int drv_step(const int left, const int right, const int draw){
  return drv_step_to(pos[0] + left, pos[1] + right, draw);
}


void drv_stop(){
  gpioWrite(R_STEP, 0);
  gpioWrite(L_STEP, 0);
  gpioWrite(R_DIR, 0);
  gpioWrite(L_DIR, 0);
  // gpioWrite(SRV_PWM, 0);

}

int drv_set_dir(const int dir_left, const int dir_right){
  if ((dir_left != 1 && dir_left != -1) ||
      (dir_right != 1 && dir_right != -1)){
    fprintf(stderr, "Error: Invalid directions (not -1 or 1)\n");
    return -1;
  }
  dir[0] = dir_left;
  dir[1] = dir_right;
  return 0;
}

int drv_init(const int dir_left, const int dir_right){
  if (drv_set_dir(dir_left, dir_right))
    return -1;

  if (gpioInitialise() < 0)
    return -1;


  gpioSetMode(L_DIR,  PI_OUTPUT);
  gpioSetMode(R_DIR,  PI_OUTPUT);
  gpioSetMode(L_STEP, PI_OUTPUT);
  gpioSetMode(R_STEP, PI_OUTPUT);
  handle_servo(SRV_SKIP);
  // gpioServo(SRV_PWM, SRV_MID);

  drv_stop();
  return 0;
}

void drv_term(){
  drv_stop();
  gpioTerminate();
}


void drv_servo_stuff(){
  // gpioServo(SRV_PWM, SRV_MID);
  int pos = SRV_MID;
  while (1){
    fprintf(stderr, "%4d | NEW POS PLS [%d-%d]>", pos, SRV_MIN, SRV_MAX);
    if (scanf("%d", &pos) != 1){
      perror("scanf");
      continue;
    }
    if (pos == 0)
      break;
    if (pos < SRV_MIN || pos > SRV_MAX){
      fprintf(stderr, "BAD\n");
      continue;
    }
    gpioServo(SRV_PWM, pos);
    gpioDelay(SRV_WAIT);
    gpioServo(SRV_PWM, 0);
  }
}

int cur_dir[2]  = {0, 0};


void drv_new_set_dir(const int l_dir, const int r_dir){
  int wait = 0;
  if (l_dir != cur_dir[0] && l_dir){
    fprintf(stderr, "Change l %d %d\n",l_dir,  l_dir * dir[0]);
    gpioWrite(L_DIR, (l_dir * dir[0] > 0) ? 1 : 0);
    cur_dir[0] = l_dir;
    wait = 1;
  }
  if (r_dir != cur_dir[1] && r_dir){
    fprintf(stderr, "Change r %d %d\n", r_dir, r_dir * dir[1]);
    gpioWrite(R_DIR, (r_dir * dir[1] > 0)? 1 : 0);
    cur_dir[1] = r_dir;
    wait = 1;
  }
  if (wait)
    gpioDelay(1000);
}

void drv_new_step(const int l, const int r){
  if (l == 0 && r == 0)
    return;

  if (l)  gpioWrite(L_STEP, 1);
  if (r)  gpioWrite(R_STEP, 1);
  gpioDelay(STEP_WAIT);

  if (l){ gpioWrite(L_STEP, 0); pos[0] += cur_dir[0]; }
  if (r){ gpioWrite(R_STEP, 0); pos[1] += cur_dir[1]; }
  gpioDelay(STEP_WAIT);
}

void drv_new_step_to(const int l, const int r){
  int ldist = (l - pos[0]);
  int rdist = (r - pos[1]);
  if (ldist == 0 && rdist == 0)
    return;

  int l_dir = 0;
  if (ldist != 0)
    l_dir = (ldist > 0) ? 1 : -1;
  int r_dir = 0;
  if (rdist != 0)
  r_dir = (rdist > 0) ? 1 : -1;
  drv_new_set_dir(l_dir, r_dir);
  int lc = abs(ldist), rc = abs(rdist);
  // fprintf(stderr, "Stepping %d %d\n", l_dir, r_dir);

  while (lc > 0 || rc > 0)
    drv_new_step(lc-- > 0, rc-- > 0);

  if (pos[0] != l || pos[1] != r){
    fprintf(stderr, "Error: Steps counts don't match %d %d != %d %d\n",
                    l,r,pos[0],pos[1]);
  }
}


