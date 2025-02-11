#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  double start;
  double end;
} Timer;

double get_time(Timer timer) {
  return timer.end - timer.start;
}

/// Check if the transpose of the matrix is correct.
bool check_correctness(int N, float **mat, float **mat_t) {
  bool correct = true;
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      if (mat[i][j] != mat_t[j][i]) {
        printf("Error: mat[%d][%d] = %f, mat_t[%d][%d] = %f\n", i, j, mat[i][j], j, i, mat_t[j][i]);
        correct = false;
      }
    }
  }
  if (correct) {
    printf("Matrix transpose is correct\n");
  }
  return correct;
}

/// Print the matrix.
void print_matrix(int N, float **mat) {
  for (int i = 0; i < N*N; i++) {
    printf("%f ", *((*mat) + i));
    if ((i+1) % N == 0) printf("\n");
  }
  printf("\n");
}

/// Initialize a matrix of size n x m. The matrix is stored in a contiguous block of memory.
void init_matrix(int N, int M, float*** mat) {
  float* mem = (float*) malloc(N*M*sizeof(float));
  *mat = (float**) malloc(N*sizeof(float*));
  for (int i = 0; i < N; i++) {
    (*mat)[i] = &(mem[i*M]);
  }
}

/// Initialize a random matrix of size n x n. The matrix is stored in a contiguous block of memory.
void fill_rand_matrix(int N, float*** mat) {
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      (*mat)[i][j] = rand() / (float) RAND_MAX;
    }
  }
}

/// Initialize a random matrix of size n x n. The matrix is stored in a contiguous block of memory.
void fill_sym_matrix(int N, float*** mat) {
  for (int i = 0; i < N; i++) {
    for (int j = 0; j <= i; j++) {
      (*mat)[i][j] = rand() / (float) RAND_MAX;
      (*mat)[j][i] = (*mat)[i][j];
    }
  }
}

void parse_args(int argc, char **argv, int *N, bool *check, bool *verbose) {
  if (argc < 2 || argc >= 5) {
    printf("Usage: %s <matrix_dim> [<check_correctness>] [<verbose>]\n", argv[0]);
    exit(1);
  } else {
    *check = false;
    *verbose = false;
    *N = atoi(argv[1]);
    if (argc >= 3 && strcmp(argv[2], "check") == 0) {
      *check = true;
    }
    if (argc >= 4 && strcmp(argv[3], "verbose") == 0) {
      *verbose = true;
    }
  }
}