#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include "utils.h"

#define BLOCK_SIZE 64

void init_rand(float **m, int size) {
    #pragma omp parallel for simd
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            m[i][j] = (float)rand();
        }
    }
}

void print_mat(float **m, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf("%12.0f ", m[i][j]);
        }
        printf("\n");
    }
}

int check_sym(float **m, int size) {
    int is_sym = 1;
    #pragma omp parallel for simd reduction(&:is_sym)
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < i; j++) {
            if (m[i][j] != m[j][i]) {
                is_sym &= 0;
            }
        }
    }
    return is_sym;
}

void blocked_transpose(float **m, float **t, int i1, int i2, int j1, int j2, int size) {
    // Test for overflows
    i2 = (i2 < size) ? i2 : size;
    j2 = (j2 < size) ? j2 : size;
    // Perform matrix transposition on the block
    for (int i = i1; i < i2; i++) {
        #pragma omp simd
        for (int j = j1; j < j2; j++) {
            t[i][j] = m[j][i];
        }
    }
}

// Divide the matrix into blocks and transpose each block
void divide_transpose(float **m, float **t, int size) {
    #pragma omp parallel for
    for (int i = 0; i < size; i += BLOCK_SIZE) {
        for (int j = 0; j < size; j += BLOCK_SIZE) {
            blocked_transpose(m, t, i, i + BLOCK_SIZE, j, j + BLOCK_SIZE, size);
        }
    }
}

int main(int argc, char **argv) {
    bool check, verbose;
    int N;

    parse_args(argc, argv, &N, &check, &verbose);
    srand(time(NULL));
    
    // Allocate memory for the matrices
    float **m = (float **)malloc(N * sizeof(float *));
    float **t = (float **)malloc(N * sizeof(float *));
    for (int i = 0; i < N; i++) {
        m[i] = (float *)malloc(N * sizeof(float));
        t[i] = (float *)malloc(N * sizeof(float));
    }
    
    init_rand(m, N);
    
    double start, end;

    // Compute blocked transpose
    start = omp_get_wtime();
    divide_transpose(m, t, N);
    end = omp_get_wtime();

    // Print wall time
    if (verbose) {
        printf("Time taken for matrix transposition: %.9fs\n", end-start);
        printf("- Input matrix -\n");
        print_mat(m, N);
        printf("- Transposed matrix -\n");
        print_mat(t, N);
    } else {
        printf("threads: %d, transpose_time: %f\n", omp_get_max_threads(), end-start);
    }
    return 0;
}
