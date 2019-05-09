#ifndef __CA_H__
#define __CA_H__
void ca_init(const int code_nr, const int cols);
int  ca_set_cell(const int col, const int state);
int  ca_set_state(const int * state);
int *ca_get_state();
int* ca_next_state();
#endif