#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "utils.h"

#define INNER_BLOCK_SIZE 16

void transpose(int N, float** mat, float** mat_t, int rank, int size) {
  int outer_block_length = N / size;
  int outer_block_size = outer_block_length * outer_block_length;
  float **mat_local;
  init_matrix(outer_block_length, outer_block_length, &mat_local);
  int *disp, *count;
  MPI_Datatype send_type;

  if (rank == 0) {
    if (N % size != 0) {
      printf("Matrix size must be divisible by the number of processes\n");
      return;
    }
    int array_elements[] = {N, N};
    int array_of_subsizes[] = {outer_block_length, outer_block_length};
    int array_of_starts[] = {0, 0};
    MPI_Type_create_subarray(2, array_elements, array_of_subsizes, array_of_starts, MPI_ORDER_C, MPI_FLOAT, &send_type);
    MPI_Type_commit(&send_type);

    disp = calloc(size,sizeof(int));
    count = calloc(size,sizeof(int));
    for (int i = 0; i < size; ++i) {
      disp[i] = 
        ((i * outer_block_length) % N) + 
        ((i * outer_block_length) / N) * outer_block_length * N;
      count[i] = 1;
    }
    MPI_Scatterv(mat[0], count, disp, send_type, mat_local[0], outer_block_size, MPI_FLOAT, 0, MPI_COMM_WORLD);
  } else {
    MPI_Scatterv(NULL, NULL, NULL, MPI_FLOAT, mat_local[0], outer_block_size, MPI_FLOAT, 0, MPI_COMM_WORLD);
  }

  // Transpose the local block by smaller blocks
  for (int i = 0; i < outer_block_length; i += INNER_BLOCK_SIZE) {
    for (int j = 0; j < outer_block_length; j += INNER_BLOCK_SIZE) {
      int iend = (i + INNER_BLOCK_SIZE < outer_block_length) ? i + INNER_BLOCK_SIZE : outer_block_length;
      int jend = (j + INNER_BLOCK_SIZE < outer_block_length) ? j + INNER_BLOCK_SIZE : outer_block_length;
      for (int ii = i; ii < iend; ii++) {
        for (int jj = j; jj < jend; jj++) {
          mat_t[jj][ii] = mat_local[ii][jj];
        }
      }
    }
  }
  
  if (rank == 0) {
    for (int i = 0; i < size; ++i) {
      disp[i] = 
        ((i * outer_block_length) % N) * N + 
        ((i * outer_block_length) / N) * outer_block_length;
    }
    MPI_Gatherv(mat_local[0], outer_block_size, MPI_FLOAT, mat_t[0], count, disp, send_type, 0, MPI_COMM_WORLD);
  } else {
    MPI_Gatherv(mat_local[0], outer_block_size, MPI_FLOAT, NULL, 0, NULL, MPI_FLOAT, 0, MPI_COMM_WORLD);
  }
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