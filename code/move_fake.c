#include <stdio.h>
#include <math.h>
#include "move.h"
#include "calibration.h"

#define X_MAX LEN_TOP - (SX * 2)
#define Y_MAX (1300 * 1000)/STEP_MCM

int cx = 0, cy = 0;

int get_x(){return cx;}
int get_y(){return cy;}
int get_x_max(){return X_MAX;}
int get_y_max(){return Y_MAX;}

void calc_target(const int x, const int y, int * left, int * right){
  int tx = SX + x;
  int ty = SY + y;
  int ta = LEN_TOP - tx;
  double b = sqrt(tx * tx + ty * ty);
  double c = sqrt(ta * ta + ty * ty);
  *left  = (int)round(b);
  *right = (int)round(c);
}


int move_to(const int x, const int y, const int draw){
  if (x < 0 || x > X_MAX || y < 0 || y > Y_MAX){
    fprintf(stderr, "Error: target coordinates out of bounds %d %d\n",
      x,y);
    _exit(1);
    return -1;
  }
  int left = 0, right = 0;
  calc_target(x,y,&left,&right);

  fprintf(stderr, "%s %5d %5d->%5d %5d\n",
                 draw?"draw":"move", cx, cy, x, y);
  cx = x;
  cy = y;
  return 0;
}


int move_init(){
  return 0;
}

void move_term(){
  move_to(0,0,0);
}
