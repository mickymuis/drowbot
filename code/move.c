#include <stdio.h>
#include <math.h>
#include "move.h"
#include "driver.h"
#include "calibration.h"

#define X_MAX LEN_TOP - (SX * 2)
#define Y_MAX (1500 * 1000)/STEP_MCM

int cx = 0, cy = 0;

int get_x(){return cx;}
int get_y(){return cy;}

void calc_target(const int x, const int y, int * left, int * right){
  int tx = SX + x;
  int ty = SY + y;
  int ta = LEN_TOP - tx;
  double b = sqrt(tx * tx + ty * ty);
  double c = sqrt(ta * ta + ty * ty);
  *left  = (int)round(b);
  *right = (int)round(c);
}

void rebuild_coords(){
  double s = (double)(LEN_TOP + drv_lpos() + drv_rpos())/2.0;
  double A = sqrt(s * (s-LEN_TOP) * (s-drv_lpos())* (s-drv_rpos()));
  double sy = (2 * A)/LEN_TOP;
  double sx = sqrt(drv_lpos() * drv_lpos() + sy * sy);
  cx = (int)round(sx);
  cy = (int)round(sy);
}

int move_to(const int x, const int y, const int draw){
  if (x < 0 || x > X_MAX || y < 0 || y > Y_MAX){
    fprintf(stderr, "Error: target coordinates out of bounds\n");
    return -1;
  }
  int left = 0, right = 0;
  calc_target(x,y,&left,&right);

  fprintf(stderr, "Moving %d %d->%d %d (%d %d -> %d %d)\n",
                  cx, cy, x, y, drv_lpos(), drv_rpos(), left, right);

  if (drv_step_to(left, right, draw)){
    fprintf(stderr, "Warning: Positional info is probably inaccurate\n");
    rebuild_coords();
    return -1;
  }
  cx = x;
  cy = y;
  return 0;
}

int move_init(){
  if(drv_init(DIR_L, DIR_R))
    return -1;
  drv_set_pos(LEN_LEFT, LEN_RIGHT);
  return 0;
}

void move_term(){
  move_to(0,0,0);
  drv_term();
}
