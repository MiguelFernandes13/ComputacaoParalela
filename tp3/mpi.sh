#!/bin/bash
#SBATCH --ntasks=4
#SBATCH --partition=cpar
time mpirun -np 4 bin/k_means 10000000 4