#include "vanilla_out.h"
#include <iostream>
#include <string>
#include <cstring>
#include <fstream>

using namespace std; 
#include <stdio.h>
#include <stdlib.h>

using std::string;
//Serial pc(USBTX, USBRX, 9600); 

int main()
{
  //mean_subtract();
    //run_nn();
  //char temp[2]={'A','B'};
  //char temp2[1]={','};
  //char *change;

  //change = strtok(temp, temp2);
  //pc.puts(change);
  //printf("%s\n",pc.puts(change));

	    int index = 0;
        int i;
        //int n ;
        //char *a = (char *)malloc(100*sizeof(string));
        char **b = (char **)malloc(1600*sizeof(string));;
     	ifstream fi("vidu.txt", ifstream::in);

     	char temp[1600];
    	fi.getline(temp, 1600);
	    string line = temp;
	    cout << line << std::endl;
        char *p;
        p = strtok(temp, ",. "); //cat chuoi bang cac ky tu ,. va space
        while(p != NULL)
       {
                b[index] = p;
                index++;
                p = strtok(NULL, ",. "); //cat chuoi tu vi tri dung lai truoc do
        }
        for (i = 0; i<=index-1; i++)  //in ra cac tu theo thu tu dao nguoc
                printf ("\n%s ", b[i]);
                 printf ("\n");
        
        //for (i=0;i<=temp;i++) {
          //for(int j=0;j<=temp;j++){
          	  //cout<<c[i][j];
          	  //cout<< " ";
			  //}
			//}
     //pc.puts(temp);
	 fi.close();
  //vanilla_main(false);
  /* code */
  return 0;
}