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
  int pen;
} turtle_t;

void 
turtle_penDown( turtle_t *t, int down ) {
  t->pen =down;
}

void
turtle_turn( turtle_t *t, int deg ) {
  deg /= 45;
  t->dir = (t->dir + deg) % 8;
  if( t->dir < 0 ) t->dir += 8;
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

void
dragon_curveRecursive(turtle_t *t, int order, int length, int sign ){
  static const float ROOT_HALF = sqrt( .5f );
  
  if( order == 0 ) {
    turtle_line( t, length );
  } else {
    dragon_curveRecursive( order - 1, (float)length * ROOT_HALF, 1);
    turtle_turn( t, sign * -90 );
    dragon_curveRecursive( order - 1, (float)length * ROOT_HALF, -1);
  }
}

void
dragon_curve( turtle_t *t, int order, int length) {
  turtle_turn( t, order * 45 );
  dragon_curveRecursive(order, length, 1);
}

int main(){
  if (move_init())
    return -1;
    
  int cx = MAX_X /2; int cy = MAX_Y /2;

  turtle_t t;
  t->x = cx; t->y =cy; t->pen =1; t->dir =N;
  
  move_to_s( cx, cy, 0 );
  
  dragon_curve( &t, 4, LINE_LENGTH );

  move_term();
  return 0;
}
