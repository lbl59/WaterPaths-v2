#!/bin/bash
#SBATCH -A m2702 -C cpu
#SBATCH --nodes=32
#SBATCH --ntasks-per-node=64
#SBATCH --qos=regular
#SBATCH --cpus-per-task=4
#SBATCH --job-name=opt_mm4_n2048_noIns_IU_seed1
#SBATCH --output=logs/opt_mm4_n2048_noIns_IU_seed1.out
#SBATCH --error=logs/opt_mm4_n2048_noIns_IU_seed1.err
#SBATCH --time=00:30:00
#SBATCH --mail-user=lbl59@cornell.edu
#SBATCH --mail-type=ALL

OMP_NUM_THREADS=4

srun -n 2048 ./SedentoValleySimulation \
    -T $OMP_NUM_THREADS \
    -t 2344 -r 200 -I _bs200 \
    -d /pscratch/sd/l/lbl59/WaterPaths-IU/ \
    -v output_raw_IU_noIns/ \
    -C -1 -O rof_tables_opt_IU/ \
    -U TestFiles/rdm_utilities_test_problem_opt_bs200.csv \
    -P TestFiles/rdm_dmp_test_problem_opt_bs200.csv \
    -W TestFiles/rdm_water_sources_test_problem_opt_bs200.csv \
    -A TestFiles/rdm_actions_test_problem_opt_bs200.csv \
    -b true -n 50000 -o 5000 -e 1 -i 4