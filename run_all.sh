# Import modules
module load gcc91 mpich-3.2.1--gcc-9.1.0

# path=$PATH_TO_DIRECTORY

# # If path is empty, terminate script with error
# if [ -z "$path" ]; then
#   echo "Error: path to directory is missing"
#   exit 1
# fi

# Select the working directory
# cd $path

# Handle signals
trap "exit" INT

# Remove directories
rm -rf bin/
rm -rf results/

# Create directories
mkdir -p bin/
mkdir -p results/

# Compile codes
gcc-9.1.0 -o bin/sequential Sequential.c
gcc-9.1.0 -fopenmp -o bin/openmp OpenMP.c

mpicc src/MPI_Broadcast.c -o bin/MPI_Broadcast -lm
mpicc src/MPI_Scatter.c -o bin/MPI_Scatter -lm
mpicc src/MPI_Blocks.c -o bin/MPI_Blocks -lm

SIZES=(64 128 256 512 1024 2048 4096)
THREADS=(1 2 4 8 16 32 64)
declare -A STRONG_SCALING_SIZES=([1]=8 [2]=11 [4]=16 [8]=22 [16]=32 [32]=45 [64]=64)


for size in ${SIZES[@]}; do
  if [ $size -le 512 ]; then
    runs=50
  else
    runs=5
  fi
  printf "Running for size: $size with one thread\n"
  for ((i=1; i<=$runs; i++)); do
    ./bin/sequential $size nocheck silent >> results/Sequential_1_$size.txt
    mpirun -np 1 ./bin/MPI_Broadcast $size nocheck silent >> results/MPI-Broadcast_1_$size.txt
    mpirun -np 1 ./bin/MPI_Scatter $size nocheck silent >> results/MPI-Scatter_1_$size.txt
    mpirun -np 1 ./bin/MPI_Blocks $size nocheck silent >> results/MPI-Blocks_1_$size.txt
  done
done

size=4096
for thread in ${THREADS[@]}; do
  export OMP_NUM_THREADS=$thread
  printf "Running weak scaling size: $size, threads: $thread \n"
  for i in {1..10}; do
    ./bin/openmp $size nocheck silent >> results/OpenMP_$thread\_$size.txt
    timeout 10s mpirun -np $thread ./bin/MPI_Broadcast $size nocheck silent >> results/MPI-Broadcast_$thread\_$size.txt
    timeout 10s mpirun -np $thread ./bin/MPI_Scatter $size nocheck silent >> results/MPI-Scatter_$thread\_$size.txt
    timeout 10s mpirun -np $thread ./bin/MPI_Blocks $size nocheck silent >> results/MPI-Blocks_$thread\_$size.txt
  done
done

for thread in ${!STRONG_SCALING_SIZES[@]}; do
  size=${STRONG_SCALING_SIZES[$thread]}
  export OMP_NUM_THREADS=$thread
  if [ $size -le 512 ]; then
    runs=50
  else
    runs=5
  fi
  printf "Running strong scaling size: $size, threads: $thread \n"
  for ((i=1; i<=$runs; i++)); do
    ./bin/openmp $size nocheck silent >> results/OpenMP_$thread\_$size.txt
    timeout 10s mpirun -np $thread ./bin/MPI_Broadcast $size nocheck silent >> results/MPI-Broadcast_$thread\_$size.txt
    timeout 10s mpirun -np $thread ./bin/MPI_Scatter $size nocheck silent >> results/MPI-Scatter_$thread\_$size.txt
    timeout 10s mpirun -np $thread ./bin/MPI_Blocks $size nocheck silent >> results/MPI-Blocks_$thread\_$size.txt
  done
done