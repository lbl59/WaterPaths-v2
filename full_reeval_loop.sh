#!/bin/bash
#SBATCH -A m2702 -C cpu
#SBATCH --nodes=4
#SBATCH --ntasks-per-node=50
#SBATCH --cpus-per-task=4
#SBATCH --qos=debug
#SBATCH --job-name=reeval_sim_base_dv_base_opt_rdm
#SBATCH --output=logs/reeval_sim_base_dv_base_opt_rdm.out
#SBATCH --error=logs/reeval_sim_base_dv_base_opt_rdm.err
#SBATCH --time=00:30:00
#SBATCH --mail-user=lbl59@cornell.edu
#SBATCH --mail-type=ALL

START="$(date +%s)"

srun -n 200 -c 4 python ./full_reeval_loop_sim_base_dv_base.py

DURATION=$[ $(date +%s) - ${START} ]

echo ${DURATION}