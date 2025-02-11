#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils.h"

void init_rand(float **m, int size) {
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
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < i; j++) {
            if (m[i][j] != m[j][i]) {
                is_sym = 0;
            }
        }
    }
    return is_sym;
}

void transpose(float **m, float **t, int size) {
    for (int i = 0; i < size; i++) {
        // gcc thinks that this loop can't be vectorized because of a data dependence
        // since this is not the case, force gcc to ignore the data dependency check
        // by adding the ignore vector dependency pragma (ivdep)
        #pragma GCC ivdep
        for (int j = 0; j < size; j++) {
            t[i][j] = m[j][i];
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
    struct timespec start, end;

    // Compute transpose
    clock_gettime(CLOCK_MONOTONIC, &start);
    transpose(m, t, N);
    clock_gettime(CLOCK_MONOTONIC, &end);

    long seconds = end.tv_sec - start.tv_sec;
    long nanoseconds = end.tv_nsec - start.tv_nsec;
    double elapsed = seconds + nanoseconds*1e-9;

    // Print wall time
    if (verbose) {
        printf("Time taken for matrix transposition: %.9fs\n", elapsed);
        printf("- Input matrix -\n");
        print_mat(m, N);
        printf("- Transposed matrix -\n");
        print_mat(t, N);
    } else {
        printf("transpose_time: %f\n", elapsed);
    }

    return 0;
}
