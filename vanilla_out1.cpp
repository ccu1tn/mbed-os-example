#include <stdint.h>
#include <stdio.h>
#include "arm_math.h"
#include "vanilla_weight.h"
#include "arm_nnfunctions.h"
#ifdef _RTE_
#include "RTE_Components.h"
#ifdef RTE_Compiler_EventRecorder
#include "EventRecorder.h"
#endif
#endif

#include "vanilla_input_per_proc.h"
#include "vanilla_out.h"
#include "matmul.h"
#include "mbed.h"

//Serial pc( USBTX , USBRX );
static q7_t conv1_wt[1*5*5*8] = CONV1_WT;
static q7_t conv1_bias[8] = CONV1_BIAS;
/*
static q7_t add1_wt[8] = ADD1;

static q7_t conv2_wt[8*5*5*16] = CONV2_WT;
static q7_t conv2_bias[16] = CONV2_BIAS;

static q7_t add2_wt[16] = ADD2;

static q7_t matmul1_wt[2560] = MATMUL_WEIGHT1;

static q7_t shape1_wt[2] = SHAPE1;
static q7_t shape2_wt[2] = SHAPE2;
static q7_t add3_wt[10] = ADD3;
*/
q7_t output_data[10];
q7_t col_buffer[2*5*5*32*2];
q7_t scratch_buffer[8*28*28];
q7_t scratch_buffer2[8*28*28];

  //q7_t image_data[28*28] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 84, 185, 159, 151, 60, 36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 222, 254, 254, 254, 254, 241, 198, 198, 198, 198, 198, 198, 198, 198, 170, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 67, 114, 72, 114, 163, 227, 254, 225, 254, 254, 254, 250, 229, 254, 254, 140, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 17, 66, 14, 67, 67, 67, 59, 21, 236, 254, 106, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 83, 253, 209, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 233, 255, 83, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 129, 254, 238, 44, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 59, 249, 254, 62, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 133, 254, 187, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 205, 248, 58, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 126, 254, 182, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 75, 251, 240, 57, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 221, 254, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 203, 254, 219, 35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 254, 254, 77, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 224, 254, 115, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 133, 254, 254, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 61, 242, 254, 254, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 121, 254, 254, 219, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 121, 254, 207, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

q7_t* vanilla_main(bool input_pre_proc, q7_t* image_data){
  #ifdef RTE_Compiler_EventRecorder
    EventRecorderInitialize (EventRecordAll, 1);
  #endif

  printf("start execution\n");
  q7_t *img_buffer1 = scratch_buffer;
  q7_t *img_buffer2 = scratch_buffer;

  if(input_pre_proc){
    per_processing(image_data , img_buffer2);
  }
  arm_convolve_HWC_q7_fast( img_buffer2,28,1,conv1_wt,8,5,0,1,conv1_bias,0,9,img_buffer1,28,(q15_t *)col_buffer,NULL );

  /*Add(img_buffer1,add1_wt);

  arm_relu_q7( img_buffer2,8 * 28 * 28 );

  arm_maxpool_q7_HWC( img_buffer2,28,8,2,0,2,14,NULL,img_buffer1 );

  arm_convolve_HWC_q7_basic( img_buffer1,14,8,conv2_wt,16,5,0,1,conv2_bias,0,RIGHT_SHIFT2,img_buffer2,14,(q15_t *)col_buffer,NULL );

  Add(img_buffer2,add2_wt);

  arm_relu_q7( img_buffer1,16 * 14 * 14 );

  arm_maxpool_q7_HWC( img_buffer1,14,16,3,0,3,4,NULL,img_buffer2 );

  matmul(img_buffer2,shape2,matmul1_wt,shape1,img_buffer1,RIGHT_SHIFT3);

  Add(img_buffer1,add3_wt);*/

  for(int i = 0; i < 10 ; i++){
    printf( "%d: %d\n", i , output_data[i] );
  }
  return img_buffer1;
}
