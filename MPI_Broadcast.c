#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "utils.h"

void transpose(int N, float** mat, float** mat_t, int rank, int size) {
  int remainder = N % size;
  int start = rank * N / size + (rank < remainder ? rank : remainder);
  int end = start + N / size + (rank < remainder ? 1 : 0);
  
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

    MPI_Gatherv(mat[0], (end-start)*N, MPI_FLOAT, mat_t[0], count, disp, new_block_type, 0, MPI_COMM_WORLD);
  } else {
    MPI_Gatherv(mat[start], (end-start)*N, MPI_FLOAT, NULL, 0, NULL, MPI_FLOAT, 0, MPI_COMM_WORLD);
  }
  MPI_Type_free(&block_type);
  MPI_Type_free(&new_block_type);
}

int main(int argc, char *argv[]) {
  
  MPI_Init(&argc, &argv);
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  
  float **mat, **mat_t;
  bool check = false;
  bool verbose = false;
  Timer transpose_timer;

  if (argc < 2 || argc >= 5) {
    printf("Usage: %s <matrix_dim> [<check_correctness>] [<verbose>]\n", argv[0]);
    return 1;
  } else {
    if (argc >= 3 && strcmp(argv[2], "check") == 0) {
      check = true;
    }
    if (argc >= 4 && strcmp(argv[3], "verbose") == 0) {
      verbose = true;
    }
  }
  int N = atoi(argv[1]);
  srand(time(NULL));
  
  init_matrix(N, N, &mat);
  init_matrix(N, N, &mat_t);
  
  if (rank == 0) {
    fill_rand_matrix(N, &mat);
    if (verbose) {
      print_matrix(N, mat);
    }
  }

  transpose_timer.start = MPI_Wtime();
  MPI_Bcast(mat[0], N*N, MPI_FLOAT, 0, MPI_COMM_WORLD);
  transpose(N, mat, mat_t, rank, size);
  transpose_timer.end = MPI_Wtime();
  
  if (rank == 0) {
    if (verbose) {
      print_matrix(N, mat_t);
    }
    if (check) {
      check_correctness(N, mat, mat_t);
    }
    printf("threads: %d, transpose_time: %f\n", size, get_time(transpose_timer));
  }

  MPI_Finalize();
  return 0;
}