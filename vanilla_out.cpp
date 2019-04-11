#include "mbed.h"
#include <stdint.h>
#include <stdio.h>
#include "arm_math.h"
#include "parameter.h"
#include "weights.h"
#include "arm_nnfunctions.h"
//#include "inputs.h"
#ifdef _RTE_
#include "RTE_Components.h"
#ifdef RTE_Compiler_EventRecorder
#include "EventRecorder.h"
#endif
#endif

static q7_t conv1_wt[3*11*11*96] = CONV1_WT;
static q7_t conv1_bias[96] = CONV1_BIAS;

static q7_t conv2_wt[96*5*5*256] = CONV2_WT;
static q7_t conv2_bias[256] = CONV2_BIAS;

static q7_t conv3_wt[256*3*3*384] = CONV3_WT;
static q7_t conv3_bias[384] = CONV3_BIAS;

/*static q7_t conv4_wt[384*3*3*384] = CONV4_WT;
static q7_t conv4_bias[384] = CONV4_BIAS;

static q7_t conv5_wt[384*3*3*256] = CONV5_WT;
*/
static q7_t ip1_wt[IP1_DIM * IP1_OUT] = IP1_WT;
static q7_t ip1_bias[IP1_OUT] = IP1_BIAS;
/*
static q7_t ip2_wt[IP2_DIM * IP2_OUT] = IP2_WT;
static q7_t ip2_bias[IP2_OUT] = IP2_BIAS;

static q7_t ip3_wt[IP3_DIM * IP3_OUT] = IP3_WT;
static q7_t ip3_bias[IP3_OUT] = IP3_BIAS;
static q7_t conv5_bias[256] = CONV5_BIAS;
*/
uint8_t image_data[11 * 11 * 96] = IMG_DATA ;//IMG_DATA;
q7_t out_data[10];

q7_t col_buffer[2*5*5*32*2];
q7_t scratch_buffer[40960];//96*54*54


int main(){
  #ifdef RTE_Compiler_EventRecorder
    EventRecorderInitialize (EventRecordAll, 1);
  #endif

  printf("start execution\n");
  q7_t *img_buffer1 = scratch_buffer;
  q7_t *img_buffer2 = img_buffer1+ 32*32*32;

  int mean_data[3] = INPUT_MEAN_SHIFT;
  unsigned int scale_data[3] = INPUT_RIGHT_SHIFT;
  /*for(int i = 0 ; i < 32*32*3 ; i += 3){
    img_buffer[i] = (q7_t)__SSAT( ((((int)image_data[i] - mean_data[0]<<7) + (0x1<<(scale_data[0]-1))) >> scale_data[0], 8);
    img_buffer[i+1] = (q7_t)__SSAT( ((((int)image_data[i+1] - mean_data[1]<<7) + (0x1<<(scale_data[1]-1))) >> scale_data[1], 8);
    img_buffer[i+2] = (q7_t)__SSAT( ((((int)image_data[i+2] - mean_data[2]<<7) + (0x1<<(scale_data[2]-1))) >> scale_data[2], 8);
  }*/
// phan sua
  for(int i=0; i<32*32*3; i+=3) {
    //image_data[i] = (q7_t)__SSAT( ((int)(image_data[i] - mean[i]) >> DATA_RSHIFT), 8);
    image_data[i] = (q7_t)__SSAT( ((((int)image_data[i]  - mean_data[0])<<7) + (0x1<<(scale_data[0]-1))) 
    >> scale_data[0], 8);
    image_data[i+1] = (q7_t)__SSAT(((((int)image_data[i+1]- mean_data[1])<<7) + (0x1<<(scale_data[1]-1))) 
    >> scale_data[1], 8);
    image_data[i+2] = (q7_t)__SSAT(((((int)image_data[i+2]- mean_data[2])<<7) + (0x1<<(scale_data[2]-1))) 
    >> scale_data[2], 8);
  }

  arm_convolve_HWC_q7_fast(img_buffer2,224,3,conv1_wt,96,11,0,4,conv1_bias,0,11,img_buffer1,54, (q15_t*)col_buffer, NULL);

  arm_relu_q7( img_buffer1,96 * 54 * 54 ); //CONV1_OUT_DIM * CONV1_OUT_DIM * CONV1_OUT_CH

  arm_maxpool_q7_HWC( img_buffer1,54,96,3,0,2,26,NULL,img_buffer2 );

  arm_convolve_HWC_q7_fast( img_buffer2,26,96,conv2_wt,256,5,2,1,conv2_bias,CONV1_BIAS_LSHIFT,CONV2_OUT_RSHIFT,img_buffer1,26,(q15_t *)col_buffer,NULL );

  arm_relu_q7( img_buffer1, 256 * 26 * 26 );

  arm_maxpool_q7_HWC( img_buffer1,26,256,3,0,2,12,NULL,img_buffer2 );

  arm_convolve_HWC_q7_fast( img_buffer2,12,256,conv3_wt,384,3,1,1,conv3_bias,CONV3_BIAS_LSHIFT,CONV3_OUT_RSHIFT,img_buffer1,12,(q15_t *)col_buffer,NULL );

  arm_relu_q7( img_buffer1,384 * 12 * 12 );

  //arm_convolve_HWC_q7_fast( img_buffer1,12,384,conv4_wt,384,3,1,1,conv4_bias,left-shift of bias,right-shift of output,img_buffer2,12,(q15_t *)col_buffer,NULL );

  arm_relu_q7( img_buffer2,384 * 12 * 12 );

  //arm_convolve_HWC_q7_fast( img_buffer2,12,384,conv5_wt,256,3,1,1,conv5_bias,left-shift of bias,right-shift of output,img_buffer1,12,(q15_t *)col_buffer,NULL );

  arm_relu_q7( img_buffer1,256 * 12 * 12 );

  arm_maxpool_q7_HWC( img_buffer1,12,256,3,0,2,6,NULL,img_buffer2 );

  arm_fully_connected_q7_opt(img_buffer2,ip1_wt,IP1_DIM,10,IP1_BIAS_LSHIFT,IP1_OUT_RSHIFT,ip1_bias,out_data,(q15_t *)img_buffer1);

  arm_relu_q7( img_buffer1,4096 * 1 * 1 );

  //arm_fully_connected_q7_opt(img_buffer1,ip1_wt,IP_DIM,10,IP2_BIAS_LSHIFT,IP2_OUT_RSHIFT,ip2_bias,out_data,(q15_t *)img_buffer2);

  arm_relu_q7( img_buffer2,4096 * 1 * 1 );

  //arm_fully_connected_q7_opt(img_buffer2,ip1_wt,IP3_DIM,10,IP3_BIAS_LSHIFT,IP3_OUT_RSHIFT,ip3_bias,out_data,(q15_t *)img_buffer1);

  arm_softmax_q7(out_data,10,out_data);

  for(int i = 0; i < 10 ; i++){
    printf( "%d: %d\n", i , out_data[i] );
  }
  return 0;
}
