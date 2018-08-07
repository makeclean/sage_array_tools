
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

/* main */
int main(int argc, char **argv) {

   int bsz = 4096;
   int cnt = 1600;
   int *arr;
   int *arr2;

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


   int loop;
   int i;
   for( loop = 0 ; loop < 10 ; loop++){
     int size = (20 * ( loop + 1 ) * ( bsz * 4 ));
     arr = (int *) malloc (size*sizeof(int));

     for( i = 0 ;  i < size; i++){
       arr[i] = i + loop;
     }
     int64_t idhi, idlo; // high low unique 64bit ints

     printf("Size of %d - %d (blk %d cnt %d)\n", size, (int)(size*sizeof(int)), bsz, cnt);

     c0appz_cparray_int(arr, size, bsz, &cnt, &idhi, &idlo);
     //c0appz_cparray(arr, bsz, cnt, &idhi, &idlo, size*sizeof(int));
     printf("==Cat==\n");
     c0appz_catarray_int(idhi, idlo, size, bsz, cnt, &arr2);
     //c0appz_catarray(idhi, idlo, bsz, cnt, (void **)&arr2, size*sizeof(int));
     printf("==Test==\n");
     //cntr=0;

     for(i=0; i<size; i++){
       if(arr[i] != arr2[i])
	 printf("%d -- %d\t%d\n", i, arr[i], arr2[i]);
     }
      
     c0appz_rm(idhi, idlo);
     printf("== Solution validated == \n");
    
     free(arr);
     free(arr2);
   }

   /* free resources*/
   c0appz_free();
   
   /* time out */
   c0appz_timeout(0);

   return 0;
}
