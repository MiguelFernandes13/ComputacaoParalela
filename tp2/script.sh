#!/bin/bash


#SBATCH --output=tp2Output.txt


echo "Loading gcc 7.2.0..."
module load gcc/7.2.0

echo "Compiling..."
make
echo "Makefile OK!"

echo "running with CP_CLUSTERS=10"
make runseq CP_CLUSTERS=10

echo "Script ran flawlessly!"