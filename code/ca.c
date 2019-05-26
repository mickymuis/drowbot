//Cellular automaton

#define CA_STATES   8
#define CA_MAXCOLS 32

int ca_it   = -1;
int ca_code = -1;
int ca_cols = -1;
int ca_state[CA_MAXCOLS] = {0};
int ca_next[CA_STATES]={0};

void ca_init(const int code_nr, const int cols){
  //initialise next state LUT
  for (int i = 0; i < CA_STATES; i++)
    ca_next[i] = (code_nr >> i & 1);
  ca_it = 0;
  ca_code = code_nr;
  ca_cols = cols;
  for (int i = 0; i < CA_MAXCOLS; i++)
    ca_state[i] = 0;
}

//overwrite cell state in the current state
int ca_set_cell(const int col, const int state){
  if (col < 0 || col >= ca_cols)
    return -1;
  if (state != 0 && state != 1)
    return -1;
  ca_state[col] = state;
  return 0;
}

//Overwrite current state
int  ca_set_state(const int * state){
  for (int i = 0; i < ca_cols; i++)
    if (ca_set_cell(i, state[i]))
      return -1;
  return 0;
}


//Determine the next state based on neigbourhood (using a LUT)
int next_state(const int l, const int c, const int r){
  return ca_next[l << 2 | c << 1 | r];
}


//Return the state
int * ca_get_state(){
  return ca_state;
}

//Advance a generation (done in place)
int * ca_next_state(){
  ca_it++;
  int last = 0;
  for (int i = 0; i < ca_cols; i++){
    int cur = ca_state[i];
    int next = (ca_cols - 1 == i)? 0: ca_state[i + 1];
    ca_state[i] = next_state(last, cur, next);
    last = cur;
  }
  return ca_state;
}
