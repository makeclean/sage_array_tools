// struct so we can return a fortran array from c
typedef struct {
  int lenc; // length of the c array
  int lenf; // length of the fortran array
  int* f_array; // fortran array pointer
  int* c_array; // c array pointer
} farrays;

int send_arrary(void *array, int bsz, int *cnt, int64_t *idhi, int64_t *idlo, int size);
void mero_send_array_int(int *array, int elements, int bsz, int *cnt, int64_t *idhi, int64_t *idlo);

int recieve_array(int **arrout, int elements, int bsz, int cnt, int64_t idhi, int64_t idlo);
void mero_recieve_array_int(int **arrout, int elements, int bsz, int cnt, int64_t idhi, int64_t idlo);
void mero_recieve_array_int_f(farrays *arrout, int elements, int bsz, int cnt, int64_t idhi, int64_t idlo);

void mero_start();
void mero_finish();
