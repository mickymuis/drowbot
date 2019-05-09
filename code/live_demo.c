#include "move.h"
#include "vid_capture.h"
#include "ca.h"
#include <stdio.h>
#include <stdlib.h>
#include "imgproc.h"


#define BOX_SIZE     100
#define BASE_ROW    1200
#define COL_COUNT     16
#define ROW_LEN  BOX_SIZE * COL_COUNT
#define MAX_ITS BASE_ROW/BOX_SIZE


//Drawing stuff

void draw_hline(const int height, const int length, const int reverse){
  if (reverse){
    move_to(length, height, 0);
    move_to(0,height,1);
  } else{
    move_to(0, height, 0);
    move_to(length, height, 1);
  }
}

void draw_initial_boxes(){
  fprintf(stderr, "Moving to base row\n");
  move_to(0, BASE_ROW, 0);
  fprintf(stderr, "Drawing line\n");
  draw_hline(BASE_ROW, ROW_LEN, 0);
  fprintf(stderr, "Drawing box sides\n");
  for (int i = COL_COUNT; i >= 0; i--){
    move_to(i * BOX_SIZE, BASE_ROW, 0);
    move_to(i * BOX_SIZE, BASE_ROW + BOX_SIZE, 1);
  }
  fprintf(stderr, "Moving out of the way\n");
  move_to(0, BASE_ROW - 400, 0);
}


inline void draw_cell(const int row, const int col){
  move_to(col*BOX_SIZE, BASE_ROW - BOX_SIZE * row, 0);
  move_to(col*BOX_SIZE, BASE_ROW - (BOX_SIZE * (row-1)),1);
  move_to((col + 1)*BOX_SIZE, BASE_ROW - (BOX_SIZE *row),1);
  move_to((col + 1)*BOX_SIZE, BASE_ROW - (BOX_SIZE *(row -1)),1);
}


int draw_ca(){
  move_to(0, BASE_ROW, 0);
  int * state = NULL;
  for (int i = 0; i < MAX_ITS; i++){
    state = ca_next_state();
    for (int j = 0; j < COL_COUNT; j++){
      if (state[j])
        draw_cell(i + 1,j);
    }
    draw_hline(BASE_ROW - i * BOX_SIZE, ROW_LEN, 1);
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
  cam_capture_img = 1;
  while (img_dat == NULL){}
  state = process_image(img_dat);
  img_dat = NULL;
  cam_capture_img = 0;
  return state;
}

int cam_callback(void * data, size_t len, void * udat){
  (void)udat;
  if (cam_capture_img > 0){
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

int init_state[COL_COUNT] = {0};

int read_initial_state(){
  fprintf(stderr, "Reading initial state\n");
  move_to(0, BASE_ROW, 0);

  for (int i = 0; i < COL_COUNT; i++){
    move_to(i * BOX_SIZE + (BOX_SIZE/2), BASE_ROW, 0);
    int s = cam_get_state();
    if (s < 0)
      return -1;
    if (ca_set_cell(i, s))
      return -1;
    init_state[i] = s;
  }

  fprintf(stderr, "Finished reading state\n  0|");
  for (int i = 0; i < COL_COUNT; i++)
    putc(init_state[i]?'#':' ', stderr);
  fprintf(stderr, "|\n");
  move_to(0,BASE_ROW, 0);
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
      fprintf(stderr, "Errpr: number out of range\n");
      continue;
    }
    fprintf(stderr, "Simulated output for %d\n", pick);
    sim_output(pick, COL_COUNT, MAX_ITS, 0);
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
  if (argc == 2)
    cam_name = argv[1];
  else if (argc == 3){
    srand(atoi(argv[1]));
    for (int i = 0; i < COL_COUNT; i++)
      init_state[i] = rand()&1;
    pick_rule_no();
    return 0;
  } else if (argc > 4){
    sim_output(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
    return 0;
  }

  if (move_init())
    return -1;
  draw_initial_boxes();

  fprintf(stderr, "Draw the initial state and enter c to continue> ");
  while (getchar() != 'c'){}


  srand(atoi(argv[1]));
  for (int i = 0; i < COL_COUNT; i++)
    init_state[i] = rand()&1;


  // if (cam_setup())
  //   goto end;

  // if (read_initial_state())
  //   goto end;

  // stop_capture();

  ca_init(pick_rule_no(), COL_COUNT);
  ca_set_state(init_state);
  fprintf(stderr, "Ready to draw, enter c to continue> ");
  while (getchar() != 'c'){}
  draw_ca();


end:
  move_term();
  stop_capture();
  return 0;
}