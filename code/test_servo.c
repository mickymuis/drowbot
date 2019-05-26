#include <stdio.h>
#include "driver.h"


int main(){
  if (drv_init(1, 1))
    return -1;

  drv_servo_testing();

  drv_term();
  return 0;
}

