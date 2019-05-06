#include <stdio.h>
#include "driver.h"
#include "move.h"

int main(){
  if (move_init())
    return -1;

  int x,y,draw;
  while (1){
    fprintf(stderr, "%5d %5d left right draw?>",
                     get_x(), get_y());
    if (scanf("%d%d%d",&x, &y,&draw) !=3 )
      break;
    if (x <  0 || y < 0)
      break;
    move_to(x, y, draw);
  }

  move_term();
  return 0;
}

