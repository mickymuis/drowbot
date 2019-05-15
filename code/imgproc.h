#include <stdint.h>
#include <stdio.h>

#define IP_W 640
#define IP_H 480
#define IP_SIZE 100
#define IP_SX 320
#define IP_SY 240

#define THRESHOLD 100
#define MIN_HIGH 1000
// #define IP_EX IP_W-IP_SX
// #define IP_EY IP_H-IP_SY


inline uint8_t* pix(void * data, const int x, const int y){
  return &((uint8_t*)data)[y * IP_W + x];
}

inline int process_image(const uint8_t * data){
  int high = 0;
  const uint8_t *dat = data;
  for (int y = IP_SY; y < IP_SY + IP_SIZE; y++)
    for (int x = IP_SX; x < IP_SX + IP_SIZE; x++)
      if (dat[y * IP_W + x] > THRESHOLD)
        high++;
  return (high > MIN_HIGH)? 1 : 0;
}

inline int process_image_verbose(void * data){
  uint32_t sum = 0, count = 0, max = 0, min = 255, high = 0;
  for (int y = IP_SY; y < IP_SY + IP_SIZE; y++)
    for (int x = IP_SX; x < IP_SX + IP_SIZE; x++){
      uint8_t p = *pix(data, x, y);
      sum += p;
      if (p > max)
        max = p;
      if (p < min)
        min = p;
      count++;
      if (p > THRESHOLD)
        high++;
      *pix(data, x, y) = 128;
    }
  fprintf(stderr, "Avg: %f, Min %u, Max %u, high: %d (%f)\n",
                  (double)sum/count, min, max, high, (double)high/count);
  return (high > MIN_HIGH)? 1 : 0;;
}
