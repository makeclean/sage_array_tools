
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/time.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include "c0appz.h"

void test_integer(int bsz); // test_integer
void test_float(int bsz);

void test_high_level_int(int bsz);
void test_high_level_long(int bsz);
void test_high_level_float(int bsz);
void test_high_level_double(int bsz);

/* main */
int main(int argc, char **argv) {

   int bsz = 4096;
   mero_start();
   test_high_level_int(bsz);
   test_high_level_long(bsz);
   test_high_level_float(bsz);
   test_high_level_double(bsz);
   mero_finish();
   return 0;

   srand(time(NULL));

   /* time in */
   c0appz_timein();

   char str[256];
   sprintf(str,".%src",basename(argv[0]));
   c0appz_setrc(str);
   c0appz_putrc();

   /* initialize resources */
   if (c0appz_init(0) != 0) {
     fprintf(stderr,"error! clovis initialization failed.\n");
     return -2;
   }

   /* test ints */
   test_integer(bsz);
   /* test floats */
   test_float(bsz);

   /* free resources*/
   c0appz_free();
   
   /* time out */
   c0appz_timeout(0);

   return 0;
}

/* test the integer send and recieve functions */
void test_integer(int bsz) {

   int *arr;
   int *arr2;

   printf("=== integer array tests ===\n");

   for ( int loop = 0 ; loop < 10 ; loop++ ) {
     int size = (20 * ( loop + 1 ) * ( bsz * 4 ));
     arr = (int *) malloc (size*sizeof(int));

     for( int i = 0 ;  i < size; i++){
       arr[i] = i + loop;
     }

     int64_t idhi, idlo; // high low unique 64bit ints

     int cnt; // number of blocks to send
     c0appz_send_array_int(arr, size, bsz, &cnt, &idhi, &idlo);
     printf("== Cat ==\n");
     printf("Loop %d of % d Size of %d - %d (blk %d cnt %d)\n", loop, 10, size, (int)(size*sizeof(int)), bsz, cnt);
     c0appz_recieve_array_int(idhi, idlo, size, bsz, cnt, &arr2);
     printf("== Test ==\n");

     for(int i = 0; i < size; i++){
       if(arr[i] != arr2[i])
	 printf("%d -- %d\t%d\n", i, arr[i], arr2[i]);
     }
     printf("== Removing array == \n"); 
     c0appz_rm(idhi, idlo);
     printf("== Solution validated == \n");
    
     free(arr);
     free(arr2);
   }
   printf("=== end of integer array tests ===\n");
   return;
}

/* test the float send and recieve functions */
void test_float(int bsz) {

   float *arr;
   float *arr2;

   printf("=== float array tests ===\n");

   for ( int loop = 0 ; loop < 10 ; loop++ ) {
     int size = (20 * ( loop + 1 ) * ( bsz * 4 ));
     arr = (float *) malloc (size*sizeof(float));

     for( int i = 0 ;  i < size; i++){
       arr[i] = (float) i + (float) loop;
     }

     int64_t idhi, idlo; // high low unique 64bit ints
     
     int cnt; // number of blocks to send
     c0appz_send_array_float(arr, size, bsz, &cnt, &idhi, &idlo);
     printf("== Cat ==\n");
     printf("Loop %d of % d Size of %d - %d (blk %d cnt %d)\n", loop, 10, size, (int)(size*sizeof(float)), bsz, cnt);
     c0appz_recieve_array_float(idhi, idlo, size, bsz, cnt, &arr2);
     printf("== Test ==\n");

     for(int i = 0 ; i < size ; i++){
       if(arr[i] != arr2[i])
	 printf("%i -- %f\t%f\n", i, arr[i], arr2[i]);
     }
     printf("== Removing array == \n"); 
     c0appz_rm(idhi, idlo);
     printf("== Solution validated == \n");
    
     free(arr);
     free(arr2);
   }
   printf("=== end of float array tests ===\n");
   return;
}

/* test of integer array send */
void test_high_level_int(int bsz) {

  //  mero_start();
   int cnt;
   int64_t idhi,idlo;
   int elements = 10000;
   int *array = (int*) malloc (elements*sizeof(elements));
   for ( int i = 0 ; i < elements ; i++ ) {
     array[i] = i;
   }
   mero_send_array_int(array,elements,bsz,&cnt,&idhi,&idlo);
   printf("send succesfully\n");
   
   int *array_recieved; // array to recieve
   printf("high level test %i %i %i %i %i\n",elements,bsz,cnt, elements*sizeof(int), bsz*cnt);
   mero_recieve_array_int(&array_recieved,elements,bsz,cnt,idhi,idlo);

   for ( int i = 0 ; i < elements ; i++ ) {
     if(array[i] != array_recieved[i]) {
       printf("no match\n");
     }
   }
   printf("test successful\n");
   //mero_finish();
   return;
}

/* test of long array send */
void test_high_level_long(int bsz) {

  //mero_start();
   int cnt;
   int64_t idhi,idlo;
   int elements = 10000;
   long *array = (long*) malloc (elements*sizeof(long));
   for ( int i = 0 ; i < elements ; i++ ) {
     array[i] = (long)i;
   }
   mero_send_array_long(array,elements,bsz,&cnt,&idhi,&idlo);
   printf("send succesfully\n");
   
   long *array_recieved; // array to recieve
   printf("high level test %i %i %i %i %i\n",elements,bsz,cnt, elements*sizeof(long), bsz*cnt);
   mero_recieve_array_long(&array_recieved,elements,bsz,cnt,idhi,idlo);

   for ( int i = 0 ; i < elements ; i++ ) {
     if(array[i] != array_recieved[i]) {
       printf("no match\n");
     }
   }
   printf("test successful\n");
   //mero_finish();
   return;
}

/* test of float array send */
void test_high_level_float(int bsz) {

  //mero_start();
   int cnt;
   int64_t idhi,idlo;
   int elements = 10000;
   float *array = (float*) malloc (elements*sizeof(float));
   for ( int i = 0 ; i < elements ; i++ ) {
     array[i] = (float)i;
   }
   mero_send_array_float(array,elements,bsz,&cnt,&idhi,&idlo);
   printf("send succesfully\n");
   
   float *array_recieved; // array to recieve
   printf("high level test %i %i %i %i %i\n",elements,bsz,cnt, elements*sizeof(float), bsz*cnt);
   mero_recieve_array_float(&array_recieved,elements,bsz,cnt,idhi,idlo);

   for ( int i = 0 ; i < elements ; i++ ) {
     if(array[i] != array_recieved[i]) {
       printf("no match\n");
     }
   }
   printf("test succesful\n");
   //mero_finish();
   return;
}

/* test of double array send */
void test_high_level_double(int bsz) {

  //mero_start();
   int cnt;
   int64_t idhi,idlo;
   int elements = 10000;
   double *array = (double*) malloc (elements*sizeof(double));
   for ( int i = 0 ; i < elements ; i++ ) {
     array[i] = (double)i;
   }
   mero_send_array_double(array,elements,bsz,&cnt,&idhi,&idlo);
   printf("send succesfully\n");
   
   double *array_recieved; // array to recieve
   printf("high level test %i %i %i %i %i",elements,bsz,cnt, elements*sizeof(double), bsz*cnt);
   mero_recieve_array_double(&array_recieved,elements,bsz,cnt,idhi,idlo);

   for ( int i = 0 ; i < elements ; i++ ) {
     if(array[i] != array_recieved[i]) {
       printf("no match\n");
     }
   }
   //   mero_finish();
   return;
}
