#!/bin/bash
#SBATCH -A m2702 -C cpu
#SBATCH --nodes=4
#SBATCH --ntasks-per-node=50
#SBATCH --qos=regular
#SBATCH --job-name=reeval_sim_IU_dv_IU_batch
#SBATCH --output=logs/reeval_sim_IU_dv_IU_batch%a.out
#SBATCH --error=logs/reeval_sim_IU_dv_IU_batch%a.err
#SBATCH --time=02:00:00
#SBATCH --mail-user=lbl59@cornell.edu
#SBATCH --mail-type=ALL
#SBATCH --array=0-199

module load openmpi
export OMP_NUM_THREADS=8

START="$(date +%s)"

srun -n 200 python ./full_reeval_IU_loop_batch.py ${SLURM_ARRAY_TASK_ID}

DURATION=$[ $(date +%s) - ${START} ]

echo ${DURATION}