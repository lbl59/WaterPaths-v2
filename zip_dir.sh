#!/bin/bash
#SBATCH -A m2702 -C cpu
#SBATCH --nodes=4
#SBATCH --ntasks-per-node=100
#SBATCH --qos=debug
#SBATCH --job-name=zip_objs_sim_base
#SBATCH --output=logs/zip_objs_sim_base.out
#SBATCH --error=logs/zip_objs_sim_base.err
#SBATCH --time=00:30:00
#SBATCH --mail-user=lbl59@cornell.edu
#SBATCH --mail-type=ALL

# Define the directory name
DIR_NAME="/pscratch/sd/l/lbl59/WaterPaths-IU/output_sim_base_refset_IU_reeval/objectives"
ZIP_FILE="/pscratch/sd/l/lbl59/WaterPaths-IU/output_sim_base_refset_IU_reeval/objectives.tar.bz2"
tar -cvjf "$ZIP_FILE" "$DIR_NAME"