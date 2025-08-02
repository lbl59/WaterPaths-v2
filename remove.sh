#!/bin/bash
#SBATCH -A m2702 -C cpu
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=100
#SBATCH --qos=debug
#SBATCH --job-name=remove_reeval_output_old
#SBATCH --output=logs/remove_reeval_output_old.out
#SBATCH --error=logs/remove_reeval_output_old.err
#SBATCH --time=00:30:00
#SBATCH --mail-user=lbl59@cornell.edu
#SBATCH --mail-type=ALL

rm -rf /pscratch/sd/l/lbl59/WaterPaths-IU/output_sim_base_refset_base_reeval_old/
