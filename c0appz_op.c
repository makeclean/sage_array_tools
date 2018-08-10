/* -*- C -*- */
/*
 * COPYRIGHT 2014 SEAGATE LLC
 *
 * THIS DRAWING/DOCUMENT, ITS SPECIFICATIONS, AND THE DATA CONTAINED
 * HEREIN, ARE THE EXCLUSIVE PROPERTY OF SEAGATE LLC,
 * ISSUED IN STRICT CONFIDENCE AND SHALL NOT, WITHOUT
 * THE PRIOR WRITTEN PERMISSION OF SEAGATE TECHNOLOGY LIMITED,
 * BE REPRODUCED, COPIED, OR DISCLOSED TO A THIRD PARTY, OR
 * USED FOR ANY PURPOSE WHATSOEVER, OR STORED IN A RETRIEVAL SYSTEM
 * EXCEPT AS ALLOWED BY THE TERMS OF SEAGATE LICENSES AND AGREEMENTS.
 *
 * YOU SHOULD HAVE RECEIVED A COPY OF SEAGATE'S LICENSE ALONG WITH
 * THIS RELEASE. IF NOT PLEASE CONTACT A SEAGATE REPRESENTATIVE
 * http://www.xyratex.com/contact
 *
 * Original author:  Ganesan Umanesan <ganesan.umanesan@seagate.com>
 * Original creation date: 10-Jan-2017
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/time.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include "c0appz.h"

#include "clovis/clovis.h"
#include "clovis/clovis_idx.h"

#ifndef DEBUG
#define DEBUG 0
#endif

/* from http://stackoverflow.com/questions/3437404/min-and-max-in-c */

#define MAX(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define MIN(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

#define MAXC0RC   	4
#define SZC0RCSTR  	256
#define SZC0RCFILE 	256
#define C0RCFLE 	"./.cappzrc"
#define CLOVIS_MAX_BLOCK_COUNT (200)

/* static variables */
static struct m0_clovis          *clovis_instance = NULL;
static struct m0_clovis_container clovis_container;
static struct m0_clovis_realm     clovis_uber_realm;
static struct m0_clovis_config    clovis_conf;
static struct m0_idx_dix_config   dix_conf;

static char c0rc[8][SZC0RCSTR];
static char c0rcfile[SZC0RCFILE] = C0RCFLE;
static struct timeval wclk_t = {0, 0};
static clock_t cput_t = 0;

//srand(time(NULL));

static char program_name[5] = ".p121";

/*
 *******************************************************************************
 * STATIC FUNCTION PROTOTYPES
 *******************************************************************************
 */
static char *trim(char *str);
static int open_entity(struct m0_clovis_entity *entity);
static int create_object(struct m0_uint128 id);
static int write_data_to_object(struct m0_uint128 id, struct m0_indexvec *ext,
				struct m0_bufvec *data, struct m0_bufvec *attr);

int c0appz_recieve_array(int64_t idhi, int64_t idlo, int bsz, int cnt, void **arrin, int size);
int c0appz_send_array(void *array, int bsz, int cnt, int64_t *idhi, int64_t *idlo, int size);

int c0appz_send_array_int(int *array, int elements, int bsz, int *cnt, int64_t *idhi, int64_t *idlo);

/*
 *******************************************************************************
 * EXTERN FUNCTIONS
 *******************************************************************************
 */

void mero_start();
void mero_finish();
void mero_recieve_array_int(int **arrin, int elements, int bsz, int cnt, int64_t idhi, int64_t idlo);
void mero_send_array_int(int *array, int elements, int bsz, int *cnt, int64_t *idhi, int64_t *idlo);

void mero_start() {

    srand(time(NULL)); // seed the rn
		
    c0appz_timein();
    c0appz_setrc(program_name);
    c0appz_putrc();

   /* initialize resources */
   if (c0appz_init(0) != 0) {
     fprintf(stderr,"error! clovis initialization failed.\n");
   }

  return;
}

void mero_finish() {
   /* free resources*/
   c0appz_free();
   
   /* time out */
   c0appz_timeout(0);
   return;
}

/* send the int array to mero */
void mero_send_array_int(int *array, int elements, int bsz, int *cnt, int64_t *idhi, int64_t *idlo){
  
   int ec = c0appz_send_array_int(array,elements,bsz,cnt,idhi,idlo);
   #ifdef DEBUG
     printf("mero_send test %i %i %i %i %i\n",elements,bsz,*cnt, elements*sizeof(int), bsz*(*cnt));
   #endif

   if( ec != 0 ) {
     fprintf(stderr,"error! failed to send array to mero\n");	
   }
}

/* send the int array to mero */
int c0appz_send_array_int(int *array, int elements, int bsz, int *cnt, int64_t *idhi, int64_t *idlo){
    
    int size = elements * sizeof(int); // byte size
	
   *cnt = ceil(size/(bsz*1.0)); // block count
	
    return c0appz_send_array((void *)array, bsz, *cnt, idhi, idlo, size);    
}

/* send the long array to mero */
int c0appz_send_array_long(long *array, int elements, int bsz, int *cnt, int64_t *idhi, int64_t *idlo){
    
    int size = elements * sizeof(long); // byte size
	
   *cnt = ceil(size/(bsz*1.0)); // block count
	
    return c0appz_send_array((void *)array, bsz, *cnt, idhi, idlo, size);    
}

/* send the float array to mero */
int c0appz_send_array_float(float *array, int elements, int bsz, int *cnt, int64_t *idhi, int64_t *idlo){
    	
    int size = elements * sizeof(float); // byte size

   *cnt = ceil(size/(bsz*1.0)); // block count

    return c0appz_send_array((void *)array, bsz, *cnt, idhi, idlo, size);    

}

/* send the double array to mero */
int c0appz_send_array_double(double *array, int elements, int bsz, int *cnt, int64_t *idhi, int64_t *idlo){
    
    int size = elements * sizeof(double); // byte size of array

   *cnt = ceil(size/(bsz*1.0)); // block count

    return c0appz_send_array((void *)array, bsz, *cnt, idhi, idlo, size);    
}

/* recieved the int array from mero */
void mero_recieve_array_int(int **arrin, int elements, int bsz, int cnt, int64_t idhi, int64_t idlo){
  		
   #ifdef DEBUG
     printf("mero_recieve test %i %i %i %i %i\n",elements,bsz,cnt, elements*sizeof(int), bsz*cnt);
   #endif

   int ec = 0;
   ec = c0appz_recieve_array_int(idhi,idlo,elements,bsz,cnt,arrin); 

   if( ec != 0 ) {
     fprintf(stderr,"error! failed to receive array from mero\n");	
   } else {
     c0appz_rm(idhi, idlo);
   }

   return;
}

/* fucntion to get the int array */
int c0appz_recieve_array_int(int64_t idhi, int64_t idlo, int elements, int bsz, int cnt, int **arrin){
    
    int size = elements * sizeof(int); // calculate size of array

    return c0appz_recieve_array(idhi, idlo, bsz, cnt, (void **) arrin, size);
}

/* fucntion to get the long array */
int c0appz_recieve_array_long(int64_t idhi, int64_t idlo, int elements, int bsz, int cnt, long **arrin){
    
    int size = elements*sizeof(long); // calculate size of array

    return c0appz_recieve_array(idhi, idlo, bsz, cnt, (void **) arrin, size);
}

/* fucntion to get the float array */
int c0appz_recieve_array_float(int64_t idhi, int64_t idlo, int elements, int bsz, int cnt, float **arrin){
    
    int size = elements*sizeof(float); // calculate the size of the array

    return c0appz_recieve_array(idhi, idlo, bsz, cnt, (void **) arrin, size);
}

/* fucntion to get the double array */
int c0appz_recieve_array_double(int64_t idhi, int64_t idlo, int elements, int bsz, int cnt, double **arrin){
      
    int size = elements*sizeof(double); // calculate the size of the array

    return c0appz_recieve_array(idhi, idlo, bsz, cnt, (void **) arrin, size);
}

/*
 * c0appz_timeout()
 * time out execution.
 */
int c0appz_timeout(int sz)
{
	double ct;	/* cpu time in seconds 	*/
	double wt;	/* wall time in seconds	*/
	double bw;	/* bandwidth in MBs		*/
	struct timeval tv;

	/* cpu time */
	ct = (double)(clock() - cput_t) / CLOCKS_PER_SEC;
	bw = (double)(sz) / (ct * 1000000);
	fprintf(stderr,"[ cput: %0.2lf s %0.2lf Mbs ]", ct, bw);

	/* wall time */
	gettimeofday(&tv, 0);
	wt  = (double)(tv.tv_sec - wclk_t.tv_sec);
	wt += (double)(tv.tv_usec - wclk_t.tv_usec)/1000000;
	bw  = (double)(sz) / (wt * 1000000);
	fprintf(stderr,"[ wclk: %0.2lf s %0.2lf Mbs ]", wt, bw);
	fprintf(stderr,"\n");
	return 0;
}



/*
 * c0appz_timein()
 * time in execution.
 */
int c0appz_timein()
{
	cput_t = clock();
	gettimeofday(&wclk_t, 0);
	return 0;
}

int c0appz_recieve_array(int64_t idhi, int64_t idlo, int bsz, int cnt, void **arrin, int size)
{
	int                  i;
	int                  rc;
	int                block_count;
	struct m0_uint128    id;
	struct m0_clovis_op *ops[1] = {NULL};
	struct m0_clovis_obj obj;
	int64_t             last_index;
	struct m0_indexvec   ext;
	struct m0_bufvec     data;
	struct m0_bufvec     attr;

	/* ids */
	id.u_hi = idhi;
	id.u_lo = idlo;
        char * arr = (char *)malloc(size);
        *arrin = (void *) arr;

        int offset = 0;
	last_index = 0;
	while (cnt > 0) {
		block_count = (cnt > CLOVIS_MAX_BLOCK_COUNT)?
			      CLOVIS_MAX_BLOCK_COUNT:cnt;

		/* we want to read <clovis_block_count> from the beginning of the object */
		rc = m0_indexvec_alloc(&ext, block_count);
		if (rc != 0) return rc;

		/*
		 * this allocates <clovis_block_count> * 4K buffers for data, and initialises
		 * the bufvec for us.
		 */
		rc = m0_bufvec_alloc(&data, block_count, bsz);
		if (rc != 0) return rc;
		rc = m0_bufvec_alloc(&attr, block_count, 1);
		if(rc != 0) return rc;

		//last_index = 0;
		for (i = 0; i < block_count; i++) {
			ext.iv_index[i] = last_index ;
			ext.iv_vec.v_count[i] = bsz;
			last_index += ext.iv_vec.v_count[i];

			/* we don't want any attributes */
			attr.ov_vec.v_count[i] = 0;

		}

		M0_SET0(&obj);
		/* Read the requisite number of blocks from the entity */
		m0_clovis_obj_init(&obj, &clovis_uber_realm, &id,
				   m0_clovis_layout_id(clovis_instance));

		/* open entity */
		rc = open_entity(&obj.ob_entity);
		if (rc < 0) {
			fprintf(stderr,"error! [%d]\n", rc);
			fprintf(stderr,"object not found\n");
			return rc;
		}


		/* Create the read request */
		m0_clovis_obj_op(&obj, M0_CLOVIS_OC_READ, &ext, &data, &attr, 0, &ops[0]);
		assert(rc == 0);
		assert(ops[0] != NULL);
		assert(ops[0]->op_sm.sm_rc == 0);

		m0_clovis_op_launch(ops, 1);

		/* wait */
		rc = m0_clovis_op_wait(
					ops[0],
				    M0_BITS(M0_CLOVIS_OS_FAILED,
					M0_CLOVIS_OS_STABLE),
					M0_TIME_NEVER
					);
		assert(rc == 0);
		assert(ops[0]->op_sm.sm_state == M0_CLOVIS_OS_STABLE);
		assert(ops[0]->op_sm.sm_rc == 0);

		/* putchar the output */
		for (i = 0; i < block_count; i++) {                   
                        memcpy(&(arr[offset]), data.ov_buf[i], MIN(size,data.ov_vec.v_count[i]));         
                        offset+=data.ov_vec.v_count[i];
                        size -= data.ov_vec.v_count[i];
		}

		/* fini and release */
		m0_clovis_op_fini(ops[0]);
		m0_clovis_entity_fini(&obj.ob_entity);

		m0_indexvec_free(&ext);
		m0_bufvec_free(&data);
		m0_bufvec_free(&attr);

		cnt -= block_count;
        }
	m0_clovis_op_free(ops[0]);

	return 0;
}

int c0appz_generate_id(int64_t *idhi, int64_t *idlo) {

  int64_t hi = rand();
  int64_t lo = rand();

  *idhi = hi;
  *idlo = lo;

  return 0;
}

/* 
 * c0appz_cp()
 * copy to an object.
 */
int c0appz_send_array(void *array, int bsz, int cnt, int64_t *idhi, int64_t *idlo, int size)
{
	int                i;
	int                rc;
	int                block_count;
	int64_t           last_index;
       // int              offset;
	struct m0_uint128  id;
	struct m0_indexvec ext;
	struct m0_bufvec   data;
	struct m0_bufvec   attr;

 
        // Need to call fgen here
        

        c0appz_generate_id(idhi, idlo);
        #ifdef DEBUG
          printf("== Fgen called %ld %ld\n", (long)(*idhi), (long)(*idlo));
        #endif

        //*idhi = 0;
        //*idlo = 1048577;
	/* ids */
	id.u_hi = *idhi;
	id.u_lo = *idlo;

        /* create object */
	rc = create_object(id);

	if (rc != 0) {
		fprintf(stderr, "can't create object!\n");
		return rc;
	}


	last_index = 0;
       // offset=0;
        
        char *arr = (char *)array;
	while (cnt > 0) {
		block_count = (cnt > CLOVIS_MAX_BLOCK_COUNT)?
			      CLOVIS_MAX_BLOCK_COUNT:cnt;

		/* Allocate block_count * 4K data buffer. */
		rc = m0_bufvec_alloc(&data, block_count, bsz);
		if (rc != 0)
			return rc;

		/* Allocate bufvec and indexvec for write. */
		rc = m0_bufvec_alloc(&attr, block_count, 1);
		if(rc != 0)
			return rc;

		rc = m0_indexvec_alloc(&ext, block_count);
		if (rc != 0)
			return rc;

		/*
		 * Prepare indexvec for write: <clovis_block_count> from the
		 * beginning of the object.
		 */
                
		for (i = 0; i < block_count; i++) {
			ext.iv_index[i] = last_index ;


			ext.iv_vec.v_count[i] = bsz;
                        
                        
                        memcpy(data.ov_buf[i], &(arr[last_index]), MIN(size,bsz));
                        size -= ext.iv_vec.v_count[i];
			last_index += ext.iv_vec.v_count[i];

			/* we don't want any attributes */
			attr.ov_vec.v_count[i] = 0;
		}

		/* Read data from source file. */
                //array_to_blocks(array, &offset, bsz, &data);

		/* Copy data to the object*/
		rc = write_data_to_object(id, &ext, &data, &attr);
		if (rc != 0) {
			fprintf(stderr, "writing to object failed!\n");
			return rc;
		}

		/* Free bufvec's and indexvec's */
		m0_indexvec_free(&ext);
		m0_bufvec_free(&data);
		m0_bufvec_free(&attr);

		cnt -= block_count;
	}

    return 0;
}






int c0cp_file(char *filename, int bsz, int cnt){

    FILE              *fp;
    int64_t *idhi = NULL;
    int64_t *idlo = NULL;



    /* open src file */
    fp = fopen(filename, "rb");
    if (fp == NULL) {
        fprintf(stderr,"error! could not open input file %s\n",filename);
        return 1;
    }

    void *data = malloc(cnt*bsz);


    int rc;
    rc = fread(data, bsz, cnt, fp);
    if (rc < 1)
	return -1;


    int ret = c0appz_send_array(data, bsz, cnt, idhi, idlo, cnt*bsz);
    
    if(ret==0)
       printf("Written %d bytes to mero object %ld %ld\n", bsz*cnt, (long) *idhi, (long) *idlo);
    
    return 0;

}


/*
 * c0appz_rm()
 * delete object.
 */
int c0appz_rm(int64_t idhi, int64_t idlo)
{
	int rc;
	struct m0_clovis_obj obj;
	struct m0_clovis_op *ops[1] = {NULL};
	struct m0_uint128 id;

	id.u_hi = idhi;
	id.u_lo = idlo;

	memset(&obj, 0, sizeof(struct m0_clovis_obj));
	m0_clovis_obj_init(&obj, &clovis_uber_realm, &id,
			   m0_clovis_layout_id(clovis_instance));
	rc = open_entity(&obj.ob_entity);
	if (rc < 0) {
		fprintf(stderr,"error! [%d]\n", rc);
		fprintf(stderr,"object not found\n");
		return rc;
	}

	m0_clovis_entity_delete(&obj.ob_entity, &ops[0]);
	m0_clovis_op_launch(ops, ARRAY_SIZE(ops));
	rc = m0_clovis_op_wait(
		ops[0], M0_BITS(M0_CLOVIS_OS_FAILED, M0_CLOVIS_OS_STABLE),
		m0_time_from_now(3,0));

	m0_clovis_op_fini(ops[0]);
	m0_clovis_op_free(ops[0]);
	m0_clovis_entity_fini(&obj.ob_entity);

	return rc;
}

/*
 * open_entity()
 * open clovis entity.
 */
static int open_entity(struct m0_clovis_entity *entity)
{
	int                                 rc;
	struct m0_clovis_op *ops[1] = {NULL};

	m0_clovis_entity_open(entity, &ops[0]);
	m0_clovis_op_launch(ops, 1);
	m0_clovis_op_wait(
			ops[0],
			M0_BITS(M0_CLOVIS_OS_FAILED,M0_CLOVIS_OS_STABLE),
			M0_TIME_NEVER
			);
	rc = m0_clovis_rc(ops[0]);
	m0_clovis_op_fini(ops[0]);
	m0_clovis_op_free(ops[0]);
	ops[0] = NULL;

	return rc;
}

/*
 * c0appz_init()
 * init clovis resources.
 */
int c0appz_init(int idx)
{
    int   i;
	int   rc;
    FILE *fp;
    char *str=NULL;
    char  buf[SZC0RCSTR];
    char* filename = C0RCFLE;

	/* read c0rc file */
    filename = c0rcfile;
    fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr,"error! could not open resource file %s\n",filename);
        return 1;
    }

    /* move fp */
    i = 0;
    while ((i != idx * MAXC0RC) && (fgets(buf, SZC0RCSTR, fp) != NULL)) {
        str = trim(buf);
        if(str[0] == '#') continue;     /* ignore comments     	*/
        if(strlen(str) == 0) continue;  /* ignore empty space   */
        i++;
    }

    /* read c0rc */
    i = 0;
    while (fgets(buf, SZC0RCSTR, fp) != NULL) {

    	#if DEBUG
    	fprintf(stderr,"rd:->%s<-", buf);
    	fprintf(stderr,"\n");
		#endif

    	str = trim(buf);
    	if(str[0] == '#') continue;		/* ignore comments 		*/
    	if(strlen(str) == 0) continue;	/* ignore empty space 	*/

    	memset(c0rc[i], 0x00, SZC0RCSTR);
    	strncpy(c0rc[i], str, SZC0RCSTR);

		#if DEBUG
    	fprintf(stderr,"wr:->%s<-", c0rc[i]);
    	fprintf(stderr,"\n");
		#endif

    	i++;
    	if(i == MAXC0RC) break;
    }
    fclose(fp);

    /* idx check */
    if (i != MAXC0RC) {
    	fprintf(stderr,"error! [[ %d ]] wrong resource index.\n",idx);
    	return 2;
    }

    clovis_conf.cc_is_oostore            = true;
    clovis_conf.cc_is_read_verify        = false;
    clovis_conf.cc_local_addr            = c0rc[0];
    clovis_conf.cc_ha_addr               = c0rc[1];
    clovis_conf.cc_profile               = c0rc[2];
    clovis_conf.cc_process_fid           = c0rc[3];
    clovis_conf.cc_tm_recv_queue_min_len = M0_NET_TM_RECV_QUEUE_DEF_LEN;
    clovis_conf.cc_max_rpc_msg_size      = M0_RPC_DEF_MAX_RPC_MSG_SIZE;

    /* IDX_MERO */
    clovis_conf.cc_idx_service_id   = M0_CLOVIS_IDX_DIX;
    dix_conf.kc_create_meta = false;
    clovis_conf.cc_idx_service_conf = &dix_conf;

	#if DEBUG
	fprintf(stderr,"\n---\n");
    fprintf(stderr,"%s,", (char *)clovis_conf.cc_local_addr);
    fprintf(stderr,"%s,", (char *)clovis_conf.cc_ha_addr);
    fprintf(stderr,"%s,", (char *)clovis_conf.cc_profile);
    fprintf(stderr,"%s,", (char *)clovis_conf.cc_process_fid);
    fprintf(stderr,"%s", (char *)clovis_conf.cc_idx_service_conf);
	fprintf(stderr,"\n---\n");
	#endif

	/* clovis instance */
	rc = m0_clovis_init(&clovis_instance, &clovis_conf, true);
	if (rc != 0) {
		fprintf(stderr,"failed to initilise Clovis\n");
		return rc;
	}

	/* And finally, clovis root realm */
	m0_clovis_container_init(&clovis_container, 
				 NULL, &M0_CLOVIS_UBER_REALM,
				 clovis_instance);
	rc = clovis_container.co_realm.re_entity.en_sm.sm_rc;
	if (rc != 0) {
		fprintf(stderr,"failed to open uber realm\n");
		return rc;
	}

	/* success */
	clovis_uber_realm = clovis_container.co_realm;
	return 0;
}

static int create_object(struct m0_uint128 id)
{
	int                  rc;
	struct m0_clovis_obj obj;
	struct m0_clovis_op *ops[1] = {NULL};

	memset(&obj, 0, sizeof(struct m0_clovis_obj));

	m0_clovis_obj_init(&obj, &clovis_uber_realm, &id,
			   m0_clovis_layout_id(clovis_instance));

	rc = open_entity(&obj.ob_entity);
	if (!(rc < 0)) {
		fprintf(stderr,"error! [%d]\n", rc);
		fprintf(stderr,"object already exists\n");
		return 1;
	}

	m0_clovis_entity_create(NULL,&obj.ob_entity, &ops[0]);

	m0_clovis_op_launch(ops, ARRAY_SIZE(ops));

	rc = m0_clovis_op_wait(
		ops[0], M0_BITS(M0_CLOVIS_OS_FAILED, M0_CLOVIS_OS_STABLE),
		m0_time_from_now(3,0));

	m0_clovis_op_fini(ops[0]);
	m0_clovis_op_free(ops[0]);
	m0_clovis_entity_fini(&obj.ob_entity);

	return 0;
}



static int write_data_to_object(struct m0_uint128 id,
				struct m0_indexvec *ext,
				struct m0_bufvec *data,
				struct m0_bufvec *attr)
{
	int                  rc;
	struct m0_clovis_obj obj;
	struct m0_clovis_op *ops[1] = {NULL};

	memset(&obj, 0, sizeof(struct m0_clovis_obj));

	/* Set the object entity we want to write */
	m0_clovis_obj_init(&obj, &clovis_uber_realm, &id,
			   m0_clovis_layout_id(clovis_instance));

	open_entity(&obj.ob_entity);

	/* Create the write request */
	m0_clovis_obj_op(&obj, M0_CLOVIS_OC_WRITE,
			 ext, data, attr, 0, &ops[0]);

	/* Launch the write request*/
	m0_clovis_op_launch(ops, 1);

	/* wait */
	rc = m0_clovis_op_wait(ops[0],
				M0_BITS(M0_CLOVIS_OS_FAILED,
				M0_CLOVIS_OS_STABLE),
				M0_TIME_NEVER);

	/* fini and release */
	m0_clovis_op_fini(ops[0]);
	m0_clovis_op_free(ops[0]);
	m0_clovis_entity_fini(&obj.ob_entity);

	return rc;
}


/*
 *******************************************************************************
 * UTILITY FUNCTIONS
 *******************************************************************************
 */

/*
 * trim()
 * trim leading/trailing white spaces.
 */
static char *trim(char *str)
{
	char *end;

	/* null */
	if(!str) return str;

	/* trim leading spaces */
	while(isspace((unsigned char)*str)) str++;

	/* all spaces */
	if(*str == 0) return str;

	/* trim trailing spaces, and */
	/* write new null terminator */
	end = str + strlen(str) - 1;
	while(end > str && isspace((unsigned char)*end)) end--;
	*(end+1) = 0;

	/* success */
	return str;
}

/*
 * c0appz_free()
 * free clovis resources.
 */
int c0appz_free(void)
{
	m0_clovis_fini(clovis_instance, true);
	return 0;
}

/*
 * c0appz_setrc()
 * set c0apps resource filename
 */
int c0appz_setrc(char *rcfile)
{
	char buf1[256];
	char buf2[256];

	/* null */
	if(!rcfile) {
		fprintf(stderr, "error! null rc filename.\n");
		return 1;
	}

	/* add hostname */
	memset(buf1, 0x00, 256);
	memset(buf2, 0x00, 256);
	gethostname(buf1, 256);
	sprintf(buf2,"%s/%s",rcfile,buf1);

	/* update rc filename */
	memset(c0rcfile, 0x00, SZC0RCFILE);
	strncpy(c0rcfile, buf2, strlen(buf2));

	/* success */
	return 0;
}

/*
 * c0appz_putrc()
 * print c0apps resource filename
 */
int c0appz_putrc(void)
{
	/* print rc filename */
	fprintf(stderr, "%s", c0rcfile);
	fprintf(stderr, "\n");
	return 0;
}




/*
 *******************************************************************************
 * END
 *******************************************************************************
 */


/*
 *  Local variables:
 *  c-indentation-style: "K&R"
 *  c-basic-offset: 8
 *  tab-width: 8
 *  fill-column: 80
 *  scroll-step: 1
 *  End:
 */
