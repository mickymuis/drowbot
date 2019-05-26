#ifndef __MOVE_H__
#define __MOVE_H__

int get_x();
int get_y();
int get_x_max();
int get_y_max();

void move_set_coords(const int x, const int y);
int move_ask_for_coords();

int move_to(const int x, const int y, const int draw);

int move_init();
void move_term();

#endif