/* C source code is found in dgemm_example.c */

#define min(x,y) (((x) < (y)) ? (x) : (y))

#include <stdio.h>
#include <stdlib.h>
#include <mkl.h>
#include <mkl_service.h>
#include <math.h>
#include <sys/time.h>


int main(int argc, char ** argv)
{
    double *A, *B, *C;
    int m, n, k, i, j;
    double alpha, beta;

    if (argc < 3) {
	    perror("Usage: ./exec time_to_run num_threads\n");
    	    return -1;
    }
    int time_sec = atoi(argv[1]);		//Time in secs to run the benchmark
    int num_threads = atoi(argv[2]);		//Num of threads to use
						//Benchmark autotunes to L1 cache size

    double time_elapsed = 0.;
    struct timeval ts, tf;   

    const char* cache_size_path = "/sys/devices/system/cpu/cpu0/cache/index1/size";

    FILE *cache_size_fd;
	if (!(cache_size_fd = fopen(cache_size_path, "r"))) {
		perror("could not open cache size file");
		return -1;
	}

	char line[512];
	if(!fgets(line, 512, cache_size_fd)) {
		fclose(cache_size_fd);
		perror("could not read from cache size file");
		return -1;
	}

	// Strip newline
	const int newline_pos = strlen(line) - 1;
	if (line[newline_pos] == '\n') {
		line[newline_pos] = '\0';
	}

	// Get multiplier
	int multiplier = 1;
	const int multiplier_pos = newline_pos - 1;
	switch (line[multiplier_pos]) {
		case 'K':
			multiplier = 1024;
		break;
		case 'M':
			multiplier = 1024 * 1024;
		break;
		case 'G':
			multiplier = 1024 * 1024 * 1024;
		break;
	}
	// Remove multiplier
	if (multiplier != 1) {
		line[multiplier_pos] = '\0';
	}

	// Line should now be clear of non-numeric characters
	int value = atoi(line);
	
	int cache_size = (num_threads * value * multiplier)/sizeof(double);

	fclose(cache_size_fd);

	m = sqrt(cache_size/3.);
	n = m;
	k = m;

    alpha = 1.0; beta = 0.0;

    A = (double *)mkl_malloc( m*k*sizeof( double ), 64 );
    B = (double *)mkl_malloc( k*n*sizeof( double ), 64 );
    C = (double *)mkl_malloc( m*n*sizeof( double ), 64 );
    if (A == NULL || B == NULL || C == NULL) {
      printf( "\n ERROR: Can't allocate memory for matrices. Aborting... \n\n");
      mkl_free(A);
      mkl_free(B);
      mkl_free(C);
      return 1;
    }

    mkl_set_num_threads(num_threads);

    for (i = 0; i < (m*k); i++) {
        A[i] = (double)(i+1);
    }

    for (i = 0; i < (k*n); i++) {
        B[i] = (double)(-i-1);
    }

    for (i = 0; i < (m*n); i++) {
        C[i] = 0.0;
    }

    printf (" Computing matrix product using Intel(R) MKL dgemm function via CBLAS interface \n\n");
    gettimeofday(&ts, NULL);
    while (time_elapsed < time_sec) {
	    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, 
                m, n, k, alpha, A, k, B, n, beta, C, n);
	    gettimeofday(&tf,NULL);
	    time_elapsed = (tf.tv_sec - ts.tv_sec) + (tf.tv_usec - ts.tv_usec)*1e-6;
    }
    mkl_free(A);
    mkl_free(B);
    mkl_free(C);

    printf (" Example completed. \n\n");
    return 0;
}
