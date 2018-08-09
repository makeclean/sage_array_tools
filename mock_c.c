#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "mock_c.h"

char* local_array;

void mero_start() {
  return;
}

void mero_finish() {
  return;
}

int send_array(void *array, int bsz, int *cnt, int64_t *idhi, int64_t *idlo, int size) {
  *idhi = 1234;
  *idlo = 5678;
  *cnt = 500;  
   local_array = (char*)array;
   memcpy(local_array,array,size);
   printf("array sent\n");  
   printf("size %i\n",size);
   return 0;
}

void mero_send_array_int(int *array, int elements, int bsz, int *cnt, int64_t *idhi, int64_t *idlo){
  
  int size = elements*sizeof(int);

  send_array((void*)array,bsz,cnt,idhi,idlo,size);

  return;
}

int recieve_array(int **arrout, int elements, int bsz, int cnt, int64_t idhi, int64_t idlo) {
  //void *array_copy
  int size = elements * sizeof(int);
  char* arr = (char*)malloc(size);
  memcpy(arr,local_array,size);
  printf("%i %i\n",(int)arr[0],(int)arr[1]);
  *arrout = (void*) arr;
  printf("%d %d %d %d\n",arrout[0],arrout[1],arrout[2],arrout[3]); 
  printf("%d %d %d %d\n",arrout[396],arrout[397],arrout[398],arrout[399]);
  printf("copying data\n");
  printf("size %i\n", size);
  return;
}

// additional layer in the case of fortran 
void mero_recieve_array_int_f(farrays *arrout, int elements, int bsz, int cnt, int64_t idhi, int64_t idlo ) {
  mero_recieve_array_int((*arrout).c_array,elements,bsz,cnt,idhi,idlo);
}

void mero_recieve_array_int(int **arrout, int elements, int bsz, int cnt, int64_t idhi, int64_t idlo) {
  recieve_array(arrout,elements,bsz,cnt,idhi,idlo);  
  return;
}
/*
int main(int argc, char* argv[]) {
  int* array =  (int*) malloc ( 400 * sizeof(int) );
  for (int i = 0 ; i < 400 ; i++ ) {
    array[i] = i;
  }

  printf("%d %d\n", array[0], array[1]);

  int count = 0;
  int64_t idhi = 0;
  int64_t idlo = 0;

  int* array_out;
  printf("sending array\n");
  mero_send_array_int(array,400,4096,&count,&idhi,&idlo);
  mero_recieve_array_int(&array_out,400,4096,count,idhi,idlo);
  printf("array succefully received\n"); 
  for ( int i = 0 ; i < 400 ; i++ ) {
    printf("%i %i\n",array[i],array_out[i]);
  }
  free(array);
  free(array_out);
  //free(local_array);
  return 0;
}
*/
