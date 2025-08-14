#!/bin/bash
#SBATCH -A m2702 -C cpu
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --qos=debug
#SBATCH --job-name=simulate_LS98_IU
#SBATCH --output=logs/simulate_LS98_IU.out
#SBATCH --error=logs/simulate_LS98_IU.err
#SBATCH --time=00:30:00
#SBATCH --mail-user=lbl59@cornell.edu
#SBATCH --mail-type=ALL

export OMP_NUM_THREADS=100

srun ./SedentoValleySimulation -T ${OMP_NUM_THREADS} -t 2344 -r 200 \
    -d /pscratch/sd/l/lbl59/WaterPaths-IU/ \
    -s LScompSol98_DVs.csv -v output_LS98/ -I _bs200\
    -m 0 -C -1 -O rof_tables_opt_base_pq/ -p 1 \
    -U TestFiles/rdm_utilities_test_problem_opt_bs200.csv \
    -P TestFiles/rdm_dmp_test_problem_opt_bs200.csv \
    -W TestFiles/rdm_water_sources_test_problem_opt_bs200.csv
