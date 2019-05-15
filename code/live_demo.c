#include "move.h"
#include "vid_capture.h"
#include "ca.h"
#include <stdio.h>
#include <stdlib.h>
#include "imgproc.h"
#include <getopt.h>
#include <time.h>


#define COL_COUNT_MAX    255
#define CAM_DELAY  900 * 1000UL * 1000UL
#define MAX_X   2000
#define MAX_Y   1200

// #define MOVE_TO(x,y,draw) move_to(x,y,draw);
#define MOVE_TO(x,y,draw) move_to_s(x,y,draw);

int col_count = 16;
int box_size  = 100;
int base_row  = 1100;
int row_len   = 0;
int max_its   = 0;

//Drawing stuff
void draw_hline(const int height, const int length, const int reverse){
if (reverse){
  MOVE_TO(length,height, 0);
  MOVE_TO(0, height, 1);
}
else {
  MOVE_TO(0,height, 0);
  MOVE_TO(length, height, 1);
}
  // int steps = (length + 1)/box_size;
  // if (reverse)
  //   MOVE_TO(length, height, 0);
  // else
  //   MOVE_TO(0, height, 0);

  // for (int i = 0; i <= steps; i++){
  //   if (reverse)
  //     MOVE_TO(length - i * box_size, height, 1);
  //   else
  //     MOVE_TO(i * box_size, height, 1);
  // }
}

void draw_initial_boxes(){
  fprintf(stderr, "Moving to base row\n");
  MOVE_TO(0, base_row, 0);
  fprintf(stderr, "Drawing line\n");
  draw_hline(base_row, row_len, 0);
  fprintf(stderr, "Drawing box sides\n");
  for (int i = col_count; i >= 0; i--){
    MOVE_TO(i * box_size, base_row, 0);
    MOVE_TO(i * box_size, base_row + box_size, 1);
  }
  fprintf(stderr, "Moving out of the way\n");
  MOVE_TO(0, base_row - 400, 0);
}


inline void draw_cell(const int row, const int col){
  MOVE_TO(col*box_size, base_row - box_size * row, 0);
  MOVE_TO(col*box_size, base_row - (box_size * (row-1)),1);
  MOVE_TO((col + 1)*box_size, base_row - (box_size *row),1);
  MOVE_TO((col + 1)*box_size, base_row - (box_size *(row -1)),1);
}


int draw_ca(){
  MOVE_TO(0, base_row, 0);
  int * state = NULL;
  for (int i = 0; i < max_its; i++){
    state = ca_next_state();
    for (int j = 0; j < col_count; j++){
      if (state[j]){
        fprintf(stderr, "Drawing box %d %d\n", i,j);
        draw_cell(i + 1,j);
      }
    }
    // draw_hline(base_row - i * box_size, row_len, 1);
  }
  return 0;
}

//Camera stuff
char * cam_name = "/dev/video0";
int cam_capture_img = 0;
void * img_dat = NULL;
size_t img_len = 0;

int cam_get_state(){
  int state = -1;
  fprintf(stderr, "Getting state\n");
  cam_capture_img = 1;
  while (img_dat == NULL){}
  fprintf(stderr, "Processing img\n");
  state = process_image_verbose(img_dat);
  fprintf(stderr, "Reset capture\n");
  img_dat = NULL;
  cam_capture_img = 0;
  return state;
}

int cam_callback(void * data, size_t len, void * udat){
  (void)udat;
  if (cam_capture_img > 0){
    fprintf(stderr, "Camera image captured\n");
    img_dat = data;
    img_len = len;
    cam_capture_img = -1;
    return 1;
  }
  else return 0;
}

int cam_setup(){
  if (init_capture(cam_name, &cam_callback, NULL))
    return -1;
  start_capture(-1);
  return 0;
}

int init_state[COL_COUNT_MAX] = {0};

int read_initial_state(){
  fprintf(stderr, "Reading initial state\n");
  MOVE_TO(0, base_row, 0);

  for (int i = 0; i < col_count; i++){
    MOVE_TO(i * box_size + (box_size/2), base_row, 0);
    if (CAM_DELAY > 0){
      struct timespec sl = {.tv_sec = 0, .tv_nsec = CAM_DELAY};
      nanosleep(&sl, NULL);
    }
    int s = cam_get_state();
    fprintf(stderr, "%2d Got state? %d\n", i, s);
    if (s < 0)
      return -1;
    if (ca_set_cell(i, s))
      return -1;
    init_state[i] = s;
  }

  fprintf(stderr, "Finished reading state\n  0|");
  for (int i = 0; i < col_count; i++)
    putc(init_state[i]?'#':' ', stderr);
  fprintf(stderr, "|\n");
  MOVE_TO(0,base_row, 0);
  return 0;
}

//Cellular Automaton stuff
int sim_output(const int rule, const int cols, const int its, const int seed){
  ca_init(rule, cols);

  if (seed){
    int init[cols];
    srand(seed);
    for (int i = 0; i < cols; i++)
      init[i] = rand()&1;
    ca_set_state(init);
  } else
    ca_set_state(init_state);

  int * state = ca_get_state();

  char out[its][cols];
  for (int i = 0; i < its; i++){
    for (int j = 0; j < cols; j++)
      out[i][j] = state[j]?'#':' ';

    state = ca_next_state();
  }
  fprintf(stderr, "Output preview:\n");
  for (int i = its - 1; i >= 0; i--)
    fprintf(stderr,"%3d|%.*s|\n", i, cols,out[i]);
  return 0;
}

int pick_rule_no(){
  int pick = 0;
  int done = 0;
  while(! done){
    fprintf(stderr, "Enter a wolfram code number[0-255]> ");
    if (scanf("%d", &pick) != 1){
      perror("scanf");
      continue;
    }
    if (pick < 0 || pick > 255){
      fprintf(stderr, "Error: number out of range\n");
      continue;
    }
    fprintf(stderr, "Simulated output for %d\n", pick);
    sim_output(pick, col_count, max_its, 0);
    fprintf(stderr, "Is this what you want? [y/n]> ");
    while (1){
      char x = getchar();
      if (x == 'y'){
        done = 1;
        break;
      } else if (x == 'n')
        break;
    }
  }
  return pick;
}


int main(int argc, char ** argv){
  int c = 0;
  int draw_boxes = 0,
      read_cam = 0,
      ca_rule = -1,
      seed = 0,
      ask_coord = 0,
      do_draw_ca = 0;

  while ((c = getopt(argc, argv, "hbc:Cs:r:dax:B:y:o:")) != -1){
    switch (c){
      case 'b': draw_boxes = 1; break;
      case 'c': read_cam = 1; cam_name = optarg; break;
      case 'C': read_cam = 1; break;
      case 's': seed = atoi(optarg); break;
      case 'r': ca_rule = atoi(optarg); break;
      case 'd': do_draw_ca = 1; break;
      case 'a': ask_coord = 1; break;
      case 'x': col_count = atoi(optarg); break;
      case 'y': max_its   = atoi(optarg); break;
      case 'B': box_size  = atoi(optarg); break;
      case 'o': base_row  = atoi(optarg); break;
      case 'h': fprintf(stderr, "Usage: %s [options]\n"
" -b        - Draw boxes\n"
" -c <path> - Use camera specified in path\n"
" -C        - Use camera (/dev/video0)\n"
" -s        - Specify random seed for state initialisation\n"
" -r        - Specify rule for cellular automaton\n"
" -d        - Actually draw the cellular automaton with Drowbot\n"
" -a        - We will ask for alternative start coordinates (not 0,0)\n"
" -x <int>  - Column count for CA\n"
" -y <int>  - Iteration (row) count for CA\n"
" -B <int>  - Box size in steps\n"
" -o <int>  - Base row (bottom) under which initial boxes are drawn\n"
" -h        - Show this help text\n", argv[0]);
              return -1;
              break;

      default: break;
    }
  }

  row_len   = box_size * col_count;
  if (max_its == 0)
    max_its = base_row/box_size;

  if (row_len > MAX_X){
    fprintf(stderr, "Row length too long %d > %d\n", row_len, MAX_X);
    return -1;
  }
  if (base_row + box_size > MAX_Y){
    fprintf(stderr, "Base row too low %d + %d > %d\n",
                    base_row, box_size, MAX_Y);
    return -1;
  }
  if (max_its * box_size > base_row){
    fprintf(stderr, "Base row too low %d * %d > %d\n",
                    max_its, box_size, base_row);
    return -1;
  }

  if (move_init())
    return -1;
  if (ask_coord)
    move_ask_for_coords();
  if (draw_boxes)
    draw_initial_boxes();

  if (read_cam){
    fprintf(stderr, "Draw the initial state and enter c to continue> ");
    while (getchar() != 'c'){}
    if (cam_setup())
      goto end;

    if (read_initial_state())
      goto end;

    stop_capture();
  } else {
    fprintf(stderr, "Using random initial state (seed: %d)\n   |", seed);
    srand(seed);
    for (int i = 0; i < col_count; i++){
      init_state[i] = rand()&1;
      putc(init_state[i]?'#':' ', stderr);
    }
    fprintf(stderr, "|\n");
  }


  if (ca_rule == -1){
    ca_rule = pick_rule_no();
  }
  if (do_draw_ca){
    ca_init(ca_rule, col_count);
    ca_set_state(init_state);
    fprintf(stderr, "Ready to draw, enter c to continue> ");
    while (getchar() != 'c'){}
    draw_ca();
  } else
    sim_output(ca_rule, col_count, max_its, 0);

end:
  move_term();
  if (read_cam)
    stop_capture();
  return 0;
}