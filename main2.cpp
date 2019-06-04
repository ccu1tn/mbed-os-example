#include "mbed.h"
#include <iostream>
#include <string>
#include <stdlib.h>
# include <stdio.h>
#include <ctype.h>
#include <string>
#include <cstring>
#include "arm_nn_tables.h"
#include "arm_math.h"
#include "arm_nnfunctions.h"
#include "arm_nnsupportfunctions.h"
#include "vanilla_out.h"

typedef int8_t q7_t;
using std::string;
Serial pc(USBTX, USBRX, 9600); // tx, rx 發射, 接收
char* itoa(int n,char s[]);
 
int main() {
    char buffer[1600];
    char str[784];
    char *change;
    
    char second[2] = ",";
    int index = 0, index_2 = 0;
    //string ret_str_result;
    const char *ret_char_result = (char *)malloc(sizeof(char) * 28 * 28);
    
    string result_string = "";
    q7_t* change_type = (q7_t*)malloc(sizeof(q7_t) * 28 * 28);
    q7_t* ret_q7_result = (q7_t*)malloc(sizeof(q7_t) * 28 * 28);
    
    while(1) {
        
        if(index == 0){
            pc.gets(buffer, 1600);
            change = strtok(buffer, second);
        }else{
            change = strtok(NULL, second);
            if(change == NULL){
                index = 0;
            }
        }
        change_type = (q7_t*)atoi(change);
        
        ret_q7_result = vanilla_main(false, change_type);
        
        for(index_2 = 0; index_2 < 784; index_2++){
            //ret_int_result = (int)ret_result[index_2];
            //result_string = result_string + ret_int_result;
            ret_char_result = itoa((int)ret_q7_result[index_2], str);
        }
        
        pc.puts(ret_char_result);
        index++;
        
    }
}


char* itoa(int n,char s[]){
    int i,sign;
    if((sign=n)<0){       
        n=-n;
    }
    i=0;
    
    do{
      s[i++]=n%10+'0';
    }

    while ((n/=10)>0);

    if(sign<0){
        s[i++]='-';
    }
    s[i]='\0';
    return s;
}