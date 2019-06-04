#include <stdint.h>
#include <stdio.h>
#include "arm_math.h"
#include "vanilla_weight.h"
#include "math.h"
void per_processing(q7_t *image_data , q7_t* img_buffer){
  int mean_data[3] = {125,123,114};
  unsigned int scale_data[3] = {8,8,8};
  for(int i = 0 ; i < 32*32*3 ; i += 3){
    img_buffer[i] = (q7_t)__SSAT( ((((int)image_data[i] - mean_data[0])<<7) + (0x1<<(scale_data[0]-1))) >> scale_data[0], 8);
    img_buffer[i+1] = (q7_t)__SSAT( ((((int)image_data[i+1] - mean_data[1])<<7) + (0x1<<(scale_data[1]-1))) >> scale_data[1], 8);
    img_buffer[i+2] = (q7_t)__SSAT( ((((int)image_data[i+2] - mean_data[2])<<7) + (0x1<<(scale_data[2]-1))) >> scale_data[2], 8);
  }

}
