#ifndef __MOVE_H__
#define __MOVE_H__

int get_x();
int get_y();
int get_x_max();
int get_y_max();

void move_set_coords(const int x, const int y);
int move_ask_for_coords();

int move_init();
int move_to(const int x, const int y, const int draw);
void move_term();

int move_fg(const int x, const int y, const int draw);
int move_bg(const int x, const int y, const int draw);
int move_done();

int move_to_s(const int x, const int y, const int draw);

#endif