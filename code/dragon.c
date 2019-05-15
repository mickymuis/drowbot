#include <stdio.h>
#include <math.h>
#include "driver.h"
#include "move.h"

// Limits
#define MIN_X 0
#define MAX_X 2000
#define MIN_Y 0
#define MAX_Y 1200
#define LINE_LEN 100

typedef enum {
  N,NW,W,SW,S,SE,E,NE
} dir_t;

static const int DIR_COEFF[][2] = {
  {0, -1}, // N
  {1, -1}, // NW
  {1,  0}, // W
  {1,  1}, // SW
  {0,  1}, // S
  {-1, 1}, // SE
  {-1, 0}, // E
  {-1,-1} // NE  
};

typedef struct {
  int x, y;
  dir_t dir;  
} turtle_t;

void
turtle_turn( turtle_t* t, int deg ) {
  deg /= 45;
  t->dir += deg;
  if( t->dir < 0 ) t->dir += 8;
  else t->dir = t->dir % 8;
}

int
turtle_line( turtle_t* t, int len ) {
  int x,y;
  if( t->dir % 2 ) { // 45 degree angle
    x =y =sqrt(pow(len, 2)/2);
  } else {
    x =y =len;
  }
  x *= DIR_COEFF[t->dir][0];
  y *= DIR_COEFF[t->dir][1];
  
  if( x < MIN_X || x > MAX_X || y < MIN_Y || y > MAX_Y ) {
    fprintf( stderr, "Illegal move to %5d, %5d\n", x, y );
    return -1;
  }
  
  move_to_s( x, y, t->pen );
  t->x += x; t->y += y;
  return 0;
}

int main(){
  if (move_init())
    return -1;

  int x,y,draw;

  if (getchar()=='s')
    move_ask_for_coords();


  while (1){
    fprintf(stderr, "%5d %5d left right draw?>",
                     get_x(), get_y());
    if (scanf("%d%d%d",&x, &y,&draw) !=3 )
      break;
    if (x <  0 || y < 0)
      break;
    // move_to(x, y, draw);
    move_to_s(x, y, draw);
  }

  move_term();
  return 0;
}
