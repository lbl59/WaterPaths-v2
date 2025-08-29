#!/bin/bash
#SBATCH -A m2702 -C cpu
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --qos=debug
#SBATCH --job-name=genROFs_new_opt_IU
#SBATCH --output=logs/genROFs_new_opt_IU.out
#SBATCH --error=logs/genROFs_new_opt_IU.err
#SBATCH --time=00:30:00
#SBATCH --mail-user=lbl59@cornell.edu
#SBATCH --mail-type=ALL

export OMP_NUM_THREADS=256

srun ./SedentoValleySimulation -T ${OMP_NUM_THREADS} -t 2344 -r 200 \
    -d /pscratch/sd/l/lbl59/WaterPaths-IU/ \
    -s sample_solutions.csv -m 0 -I _bs200 \
    -C 1 -O rof_tables_opt_IU_smallExp/ \
    -U TestFiles/rdm_utilities_test_problem_opt_bs200.csv \
    -P TestFiles/rdm_dmp_test_problem_opt_bs200.csv \
    -W TestFiles/rdm_water_sources_test_problem_opt_bs200.csv \
    -A TestFiles/rdm_actions_test_problem_opt_bs200.csv