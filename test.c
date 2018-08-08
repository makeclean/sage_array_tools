
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

/* main */
int main(int argc, char **argv) {

   int bsz = 4096;

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
