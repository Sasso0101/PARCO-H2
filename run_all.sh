module load gcc91 mpich-3.2.1--gcc-9.1.0

mpicc MPI_Broadcast.c check_correctness.h -o MPI_Broadcast.out

mpirun -np 24 ./MPI_Broadcast.out 4 nocheck verbose