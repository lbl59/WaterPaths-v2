#!/bin/bash
#SBATCH -A m2702 -C cpu
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=50
#SBATCH --cpus-per-task=4
#SBATCH --qos=preempt
#SBATCH --job-name=sim_base_dv_IU_perturbed
#SBATCH --output=logs/sim_base_dv_IU_perturbed.out
#SBATCH --error=logs/sim_base_dv_IU_perturbed.err
#SBATCH --time=03:00:00
#SBATCH --mail-user=lbl59@cornell.edu
#SBATCH --mail-type=ALL

START="$(date +%s)"

srun -n 100 -c 4 python ./simulate_perturbations_sim_base_dv_IU.py

DURATION=$[ $(date +%s) - ${START} ]

echo ${DURATION}