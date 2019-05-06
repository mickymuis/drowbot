#ifndef __MOVE_H__
#define __MOVE_H__

int get_x();
int get_y();

int move_init();
int move_to(const int x, const int y, const int draw);
void move_term();

#endif