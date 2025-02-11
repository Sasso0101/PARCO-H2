# PARCO-H2 Project
Student: Salvatore Andaloro (email: [salvatore.andaloro@studenti.unitn.it](mailto:salvatore.andaloro@studenti.unitn.it))

The repository contains the following files:
```
|- README.md             : this file
|- main.pbs              : PBS script that runs all the implementations
|- plots.ipynb           : Jupyter notebook that generates the plots
|- src/                  : source code folder
|   |- Sequential.c      : sequential implementation
|   |- OpenMP.c          : OpenMP implementation (blocked)
|   |- MPI_Symm.c        : MPI implementation (symmetry checking)
|   |- MPI_Broadcast.c   : MPI implementation (broadcast)
|   |- MPI_Scatter.c     : MPI implementation (scatter)
|   |- MPI_Blocked.c     : MPI implementation (blocked)
|   |- MPI_Blocked_32.c  : MPI implementation (blocked with 32x32 blocks)
|   |- MPI_Blocked_64.c  : MPI implementation (blocked with 64x64 blocks)
|   |- MPI_Blocked_128.c : MPI implementation (blocked with 128x128 blocks)
|   |- utils.h           : utility functions
```
### Reproducibility instructions
Clone this repository to a local folder:
```bash 
git clone https://github.com/Sasso0101/PARCO-H2.git
```
Then, submit the `main.pbs` file to the cluster using the following command:
```bash
qsub -q short_cpuQ -v "PATH_TO_DIRECTORY=/home/<username>/<path_to_project_directory>" /home/<username>/<path_to_project_directory>/main.pbs
```
Where `<username>` has to be replaced with the actual username and `<path_to_project_directory>` has to be replaced with the complete path from the home directory to the project directory. If the repo has been downloaded directly from Github it should just be `PARCO-H2`.

To allow a quick completion of the script, each implementation will be run only once by default. The number of runs can be changed by setting the RUNS environment variable as follows:
```bash
qsub -q short_cpuQ -v PATH_TO_DIRECTORY=/home/<username>/<path_to_project_directory>,RUNS=<number_of_runs> /home/<username>/<path_to_project_directory>/main.pbs
```

Alternatively, the PBS file can be run as a regular bash script on any Linux machine with installed `gcc-9.1.0`, the openmp library and `mpich-3.2.1`. The `PATH_TO_DIRECTORY` environment variable can be specified using
```bash
export PATH_TO_DIRECTORY=/home/<username>/<path_to_project_directory>
```

### Expected output
The script should take approximately two to three minutes to complete. The standard output is written to the `stdout.o` file in the project directory. The script will print the runtimes of each version inside files in the `results/` directory, where the filename is in the following format:
```
|- <implementation_name>_<num_threads>_<matrix_size>.txt
```
The Jupyter notebook `plots.ipynb` can be used to generate the plots from the results. The Jupiter notebook requires `matplotlib`, `numpy`, `pprint` and `scienceplots` libraries to be installed.

For easy reference the uploaded notebook already contains the generated plots.