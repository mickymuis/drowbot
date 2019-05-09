#include <stdio.h>
#include "driver.h"


int main(){
  if (drv_init(1, 1))
    return -1;

  drv_servo_stuff();

  // int left, right, draw;
  // while (1){
  //   fprintf(stderr, "%5d %5d %d left right draw?>",
  //                    drv_lpos(), drv_rpos(), drv_spos() == SRV_DRAW);
  //   if (scanf("%d%d%d",&left, &right,&draw) !=3 )
  //     break;
  //   if (left == 0 && right == 0)
  //     break;
  //   drv_step(left, right, draw);
  // }

  drv_term();
  return 0;
}

