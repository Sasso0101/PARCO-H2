#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
    int size = 4096;
    int verbose = 0;
    if (argc > 1) {
        size = strtol(argv[1], NULL, 10);
    }
    if (argc > 2) {
        verbose = 1;
    }

    // Allocate memory for the matrices
    float **m = (float **)malloc(size * sizeof(float *));
    float **t = (float **)malloc(size * sizeof(float *));
    for (int i = 0; i < size; i++) {
        m[i] = (float *)malloc(size * sizeof(float));
        t[i] = (float *)malloc(size * sizeof(float));
    }

    init_rand(m, size);
    struct timespec start, end;
    
    int is_sym = 0;
    // Check if the matrix is symmetric
    clock_gettime(CLOCK_MONOTONIC, &start);
    is_sym = check_sym(m, size);
    clock_gettime(CLOCK_MONOTONIC, &end);
    // Print some output to stderr to avoid the compiler optimizing the check away
    fprintf(stderr, "%d", is_sym);

    long seconds = end.tv_sec - start.tv_sec;
    long nanoseconds = end.tv_nsec - start.tv_nsec;
    double elapsed = seconds + nanoseconds*1e-9;
    // Print wall time
    if (verbose) {
        printf("Time taken for the symmetry check: %.9fs\n", elapsed);
    } else {
        printf("%.9f,", elapsed);
    }

    // Compute transpose
    clock_gettime(CLOCK_MONOTONIC, &start);
    transpose(m, t, size);
    clock_gettime(CLOCK_MONOTONIC, &end);

    seconds = end.tv_sec - start.tv_sec;
    nanoseconds = end.tv_nsec - start.tv_nsec;
    elapsed = seconds + nanoseconds*1e-9;

    // Print some output to stderr to avoid the compiler optimizing the transpose away
    fprintf(stderr, "%f", t[size-1][size-1]);

    // Print wall time
    if (verbose) {
        printf("Time taken for matrix transposition: %.9fs\n", elapsed);
        printf("- Input matrix -\n");
        print_mat(m, size);
        printf("- Transposed matrix -\n");
        print_mat(t, size);
    } else {
        printf("%.9f\n", elapsed);
    }

    return 0;
}
