#!/bin/bash
#SBATCH --output=mpi.out
#SBATCH --ntasks=40
#SBATCH --partition=cpar
time mpirun -np 40 bin/k_means 10000000 32