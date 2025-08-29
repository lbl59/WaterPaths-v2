#!/bin/bash
#SBATCH -A m2702 -C cpu
#SBATCH --nodes=4
#SBATCH --ntasks-per-node=64
#SBATCH --qos=debug
#SBATCH --cpus-per-task=4
#SBATCH --job-name=opt_mm4_n256_base_revert_smallExp
#SBATCH --output=logs/opt_mm4_n256_base_revert_smallExp.out
#SBATCH --error=logs/opt_mm4_n256_base_revert_smallExp.err
#SBATCH --time=00:30:00
#SBATCH --mail-user=lbl59@cornell.edu
#SBATCH --mail-type=ALL

OMP_NUM_THREADS=4

srun -n 256 ./SedentoValleySimulation \
    -T $OMP_NUM_THREADS \
    -t 2344 -r 200 -I _bs200 \
    -d /pscratch/sd/l/lbl59/WaterPaths-IU/ \
    -v output_raw_IU_smallExp/ \
    -C -1 -O rof_tables_opt_IU_smallExp/ \
    -U TestFiles/rdm_utilities_test_problem_opt_bs200.csv \
    -P TestFiles/rdm_dmp_test_problem_opt_bs200.csv \
    -W TestFiles/rdm_water_sources_test_problem_opt_bs200.csv \
    -A TestFiles/rdm_actions_test_problem_opt_bs200.csv \
    -b true -n 10000 -o 1000 -e 1 -i 4