#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "utils.h"

/// Check if the matrix is symmetric. Each process checks a subset of the rows.
void check_sym(int N, float** mat, int rank, int size, int* ret) {
  int remainder = N % size;
  int start = rank * N / size + (rank < remainder ? rank : remainder);
  int end = start + N / size + (rank < remainder ? 1 : 0);
  int is_sym = 1;
  for (int i = start; i < end; ++i) {
    for (int j = 0; j < N; ++j) {
      if (mat[i][j] != mat[j][i]) {
        is_sym = 0;
        break;
      }
    }
  }
  MPI_Reduce(&is_sym, ret, 1, MPI_INT, MPI_LAND, 0, MPI_COMM_WORLD);
}

void transpose(int N, float** mat, float** mat_t, int rank, int size) {
  int remainder = N % size;
  int start = rank * N / size + (rank < remainder ? rank : remainder);
  int end = start + N / size + (rank < remainder ? 1 : 0);
  
  for (int i = start; i < end; i++) {
    for (int j = 0; j < N; ++j) {
      mat_t[j][i] = mat[i][j];
    }
  }
  
  MPI_Datatype block_type, new_block_type;
  MPI_Type_vector(N, 1, N, MPI_FLOAT, &block_type);
  MPI_Type_commit(&block_type);

  MPI_Type_create_resized(block_type, 0, 1*sizeof(float), &new_block_type);
  MPI_Type_commit(&new_block_type);

  if (rank == 0) {
    int *disp = calloc(size,sizeof(int));
    int *count = calloc(size,sizeof(int));
    for (int i = 0; i < size; ++i) {
      disp[i] = i * N / size + (i < remainder ? i : remainder);
      count[i] = N / size + (i < remainder ? 1 : 0);
    }

    MPI_Gatherv(MPI_IN_PLACE, end-start, new_block_type, mat_t[0], count, disp, new_block_type, 0, MPI_COMM_WORLD);
  } else {
    MPI_Gatherv(&(mat_t[0][start]), end-start, new_block_type, NULL, 0, NULL, MPI_FLOAT, 0, MPI_COMM_WORLD);
  }
  MPI_Type_free(&block_type);
  MPI_Type_free(&new_block_type);
}

int main(int argc, char *argv[]) {
  
  MPI_Init(&argc, &argv);
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  
  float **mat;
  bool verbose = false;
  bool symmetric = false;
  int is_sym = 0;
  Timer sym_timer;

  if (argc < 2 || argc >= 5) {
    printf("Usage: %s <matrix_dim> [<verbose>] [<symmetric>]\n", argv[0]);
    return 1;
  } else {
    if (argc >= 3 && strcmp(argv[3], "verbose") == 0) {
      verbose = true;
    }
    if (argc >= 4 && strcmp(argv[4], "symmetric") == 0) {
      symmetric = true;
    }
  }
  int N = atoi(argv[1]);
  srand(time(NULL));
  
  init_matrix(N, N, &mat);
  
  if (rank == 0) {
    if (symmetric) {
      fill_sym_matrix(N, &mat);
    } else {
      fill_rand_matrix(N, &mat);
    }
    if (verbose) {
      print_matrix(N, mat);
    }
  }

  sym_timer.start = MPI_Wtime();
  MPI_Bcast(mat[0], N*N, MPI_FLOAT, 0, MPI_COMM_WORLD);
  check_sym(N, mat, rank, size, &is_sym);
  sym_timer.end = MPI_Wtime();
  
  if (rank == 0) {
    printf("threads: %d, sym_time: %f, is_sym: %d\n", size, get_time(sym_timer), is_sym);
  }

  MPI_Finalize();
  return 0;
}