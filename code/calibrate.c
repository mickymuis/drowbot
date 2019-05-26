#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "driver.h"

//This file attempts to guide the user through the calibration
//process and will output the final calibration data to a provided
//filename or - for stdout
//Should be mostly self-explanatory but the basic steps are:
//1. Determine the movement direction of the chains versus steppers
//2. Ask to move to the top left corner of the field
//3. Ask to measure chain lengths/top length in mm
//4. Ask to move the left chain a few hundred steps
//5. Ask for a new chain length measurement to determine steps/mm
//6. Calculate all the necessary constants and writes them to a file
//7. Returns the drowbot ot the top left

#define MAX_MOVE   800
#define MIN_STEPS    1

//Maybe use 1/10 mm here, this is prolly good enough
int   mm_top = 0,
     mm_left = 0,
    mm_right = 0,
    step_mcm = 0,
    dir_left = 0,
   dir_right = 0,
       mm_sx = 0,
       mm_sy = 0;

inline int mm_to_step(const int mm){
  if (step_mcm < 1)
    return 0;
  return (mm * 1000)/step_mcm;
}

void user_movement(){
  fprintf(stderr, "=== User controlled momvemen\n");
  fprintf(stderr, "    Manually enter step counts as \"left right\"\n");
  fprintf(stderr, "    both 0 = done, -x = step backwards\n");
  while (1){
    fprintf(stderr, "    %5d %5d left right>", drv_lpos(), drv_rpos());
    int left,right;
    if (scanf("%d%d",&left, &right) != 2)
      break;
    if (left == 0 && right == 0)
      break;
    if (abs(left) > MAX_MOVE || abs(right) > MAX_MOVE)
      fprintf(stderr, "    Moving > %d at once is not allowed\n", MAX_MOVE);
    else
      drv_step(left, right, 0);
  }
  fprintf(stderr, "=== Done\n");
}

int test_direction(int dir){
  const char * dirname = (dir)?"right":"left";
  int * out = (dir)? &dir_right: &dir_left;
  fprintf(stderr, "We will be testing the %s chain\n", dirname);
  fprintf(stderr, "Make sure it has some room to move in either direction\n");
  fprintf(stderr, "We will turn one rotation after you press enter\n");
  (void)getchar();
  while (getchar()!='\n'){};
  if (dir)
    drv_step(0, 400, 0);
  else
    drv_step(400, 0, 0);
  fprintf(stderr, "If the chain got longer type 1, or -1 otherwise: \n");
  while (1){
    if (scanf("%d", out) != 1){
      perror("scanf");
      return -1;
    } else if(*out != -1 && *out != 1)
      fprintf(stderr, "Please enter -1 or 1 >\n");
    else
      break;
  }
  return 0;
}

int set_direction(){
  int known = 0;
  fprintf(stderr, "Let's start by determining the step direction\n");
  fprintf(stderr, "If you know them type: 1 <dir_left> <dir_right>\n");
  fprintf(stderr, "Or some other number to determine them experimentally\n");
  if (scanf("%d", &known) != 1){
    perror("scanf");
    return -1;
  }
  if (known == 1){
    while (1){
      if (scanf("%d%d", &dir_left, &dir_right) != 2){
        perror("scanf");
        return -1;
      }
      if (drv_set_dir(dir_left, dir_right)){
        fprintf(stderr, "Enter the two directions again>\n");
      }
      else
        return 0;
    }
    return -1;
  }
  if (test_direction(0))
    return -1;
  if (test_direction(1))
    return -1;
  fprintf(stderr, "Done setting the direction\n");
  return 0;
}

int start_calibration(){
  fprintf(stderr, "Starting calibration\n");
  fprintf(stderr, "Move centre platform to the upper left corner\n");
  user_movement();
  fprintf(stderr, "Now measure and enter \"top left right\" in mm\n");
  while (1){
    if (scanf("%d%d%d", &mm_top, &mm_left, &mm_right) != 3){
      perror("scanf");
      return -1;
    }
    if (mm_top < 1 || mm_left < 1 || mm_right < 1)
      fprintf(stderr, "Please enter positive numbers\n");
    else if ((mm_right + mm_left < mm_top) ||
             (mm_right + mm_top  < mm_left) ||
             (mm_left  + mm_top  < mm_right))
      fprintf(stderr, "Please adhere to the laws of physics\n");
    else
      break;
    fprintf(stderr, "Try entering numbers again>\n");
  }
  drv_reset_pos();
  return 0;
}

int calculate_mm(){
  int known = 0;
  fprintf(stderr, "If mcm/step is known enter it or 0 to find it>");
  if (scanf("%d",&known) != 1){
    perror("scanf");
    return -1;
  }
  if (known > 0){
    step_mcm = known;
    return 0;
  }
  int newlen = 0;
  while (1){
    fprintf(stderr, "Move to a point where the left chain is a lot longer\n");
    fprintf(stderr, "You want at least %d steps\n", MIN_STEPS);
    user_movement();
    if (abs(drv_lpos()) < MIN_STEPS){
      fprintf(stderr, "We asked for at least %d steps, not %d\n",
                       MIN_STEPS, abs(drv_lpos()));
      continue;
    }
    fprintf(stderr, "Now measure and enter the new length in mm> ");
    if (scanf("%d", &newlen) != 1){
      perror("scanf");
      continue;
    }
    if (newlen < mm_left){
      printf("Maybe make it longer next time\n");
    } else
      break;
  }
  int mm_diff = newlen - mm_left;
  int steps_moved = abs(drv_lpos());
  step_mcm = (mm_diff * 1000)/steps_moved;
  fprintf(stderr, "New micrometer/step: %d\n", step_mcm);
  fprintf(stderr, "Returning to top left\n");
  drv_step_to(0,0,0);
  return 0;
}

int write_file(const char * file){
  FILE * f = NULL;
  if (strcmp(file, "-") == 0){
    f = stdout;
  } else {
    f = fopen(file, "w");
  }
  if (f == NULL){
    fprintf(stderr, "Error opening %s\n", file);
    return -1;
  }
  fprintf(f, "//Necessary constants\n");
  fprintf(f, "#define DIR_L     %7d\n", dir_left);
  fprintf(f, "#define DIR_R     %7d\n", dir_right);
  fprintf(f, "#define LEN_TOP   %7d\n", mm_to_step(mm_top));
  fprintf(f, "#define LEN_LEFT  %7d\n", mm_to_step(mm_left));
  fprintf(f, "#define LEN_RIGHT %7d\n", mm_to_step(mm_right));
  fprintf(f, "#define SX        %7d\n", mm_to_step(mm_sx));
  fprintf(f, "#define SY        %7d\n", mm_to_step(mm_sy));
  fprintf(f, "//Some of the source values\n");
  fprintf(f, "#define STEP_MCM  %7d\n", step_mcm);
  fprintf(f, "#define MM_TOP    %7d\n", mm_top);
  fprintf(f, "#define MM_LEFT   %7d\n", mm_left);
  fprintf(f, "#define MM_RIGHT  %7d\n", mm_right);
  fprintf(f, "#define MM_SX     %7d\n", mm_sx);
  fprintf(f, "#define MM_SY     %7d\n", mm_sy);

  if (f != stdout)
    fclose(f);
  return 0;
}

//Heron's for area and solve area for height + pythagoras
void calculate_startcoord(){
  double a = mm_top, b = mm_left, c = mm_right;
  double s = (double)(a + b + c)/2.0;
  double A = sqrt(s * (s-a) * (s-b)* (s-c));
  double sy = (2 * A)/a;
  double sx = sqrt(b * b - sy * sy);
  mm_sx = (int)round(sx);
  mm_sy = (int)round(sy);
  fprintf(stderr, "s: %f, A: %f, sx: %f, sy: %f, %d, %d\n",
                   s, A, sx, sy, mm_sx, mm_sy);
}


int main(int argc, char ** argv){
  if (argc < 2){
    fprintf(stderr, "Usage: %s <output_file>\n", argv[0]);
    fprintf(stderr, "  output_file - file name or '-'' for stdout\n");
  }

  drv_init(1, 1);
  if (set_direction())
    goto err;

  if(start_calibration())
    goto err;

  if (calculate_mm())
    goto err;

  calculate_startcoord();

  if (write_file(argv[1]))
    goto err;

  drv_term();
  return 0;
err:
  drv_term();
  return -1;
}