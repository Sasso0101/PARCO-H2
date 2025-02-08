# PARCO-H2 Project
Student: Salvatore Andaloro (email: [salvatore.andaloro@studenti.unitn.it](mailto:salvatore.andaloro@studenti.unitn.it))

The repository contains the following files:
```
|- main.pbs           : PBS script that runs all the implementations
```
### Reproducibility instructions
Clone this repository to a local folder:
```bash 
git clone https://github.com/Sasso0101/PARCO-H2.git
```
Then, submit the `main.pbs` file to the cluster using the following command:
```bash
qsub -q short_cpuQ -v PATH_TO_DIRECTORY=/home/<username>/<path_to_project_directory> /home/<username>/<path_to_project_directory>/main.pbs
```
Where `<username>` has to be replaced with the actual username and `<path_to_project_directory>` has to be replaced with the complete path from the home directory to the project directory. The path to the project should not contain spaces or commas, otherwise the qsub may interpret it incorrectly.

To allow a quick completion of the script, each implementation will be run only once by default. The number of runs can be changed by setting the RUNS environment variable as follows:
```bash
qsub -q short_cpuQ -v PATH_TO_DIRECTORY=/home/<username>/<path_to_project_directory>,RUNS=<number_of_runs> /home/<username>/<path_to_project_directory>/main.pbs
```

Alternatively, the PBS file can be run as a regular bash script on any Linux machine with `gcc-9.1.0` and the openmp library installed and by specifying the `PATH_TO_DIRECTORY` environment variable using
```bash
export PATH_TO_DIRECTORY=/home/<username>/<path_to_project_directory>
```

### Expected output