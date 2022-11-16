#!/bin/bash


#SBATCH --output=tp2Output.txt

echo "Loading gcc 7.2.0..."
module load gcc/7.2.0

echo "Compiling..."
make
echo "Makefile OK!"

n=1

while [ $n -le 20 ]
do
	echo "running with $n cpus-per-task"
	srun --partition=cpar --cpus-per-task=$n perf stat -e instructions,cycles make runpar CP_CLUSTERS=4

	sleep 5
	n=$((n+1))
done

echo "Script ran flawlessly!"
