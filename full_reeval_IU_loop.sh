#!/bin/bash
#SBATCH -A m2702 -C cpu
#SBATCH --nodes=1000
#SBATCH --ntasks-per-node=100
#SBATCH --qos=regular
#SBATCH --job-name=reeval_sim_IU
#SBATCH --output=logs/reeval_sim_IU.out
#SBATCH --error=logs/reeval_sim_IU.err
#SBATCH --time=02:00:00
#SBATCH --mail-user=lbl59@cornell.edu
#SBATCH --mail-type=ALL

module load openmpi
export OMP_NUM_THREADS=4

START="$(date +%s)"

srun -n 200 python ./full_reeval_IU_loop.py

DURATION=$[ $(date +%s) - ${START} ]

echo ${DURATION}