#!/bin/bash
#SBATCH -A m2702 -C cpu
#SBATCH --nodes=4
#SBATCH --ntasks-per-node=50
#SBATCH --cpus-per-task=4
#SBATCH --qos=regular
#SBATCH --job-name=gen_rof_tables_reeval_200rdm
#SBATCH --output=logs/gen_rof_tables_reeval_200rdm.out
#SBATCH --error=logs/gen_rof_tables_reeval_200rdm.err
#SBATCH --time=02:00:00
#SBATCH --mail-user=lbl59@cornell.edu
#SBATCH --mail-type=ALL

srun --cpu-bind=cores -n 200 -c 4 python genROFTables_reeval_script.py