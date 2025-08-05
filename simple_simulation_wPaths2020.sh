#!/bin/bash
#SBATCH -A m2702 -C cpu
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=100
#SBATCH --qos=debug
#SBATCH --job-name=simulate_test_pfc_wcc
#SBATCH --output=logs/simulate_test_pfc_wcc.out
#SBATCH --error=logs/simulate_test_pfc_wcc.err
#SBATCH --time=00:30:00
#SBATCH --mail-user=lbl59@cornell.edu
#SBATCH --mail-type=ALL

export OMP_NUM_THREADS=50

srun ./SedentoValleySimulation -T ${OMP_NUM_THREADS} -t 2344 -r 200 \
    -d /pscratch/sd/l/lbl59/WaterPaths-IU/ \
    -s LScompSol98_DVs.csv -m 0 -p 0 \
    -C -1 -O rof_tables_opt_parquet/ -I _bs200\
    -U TestFiles/rdm_utilities_test_problem_opt_bs200.csv \
    -P TestFiles/rdm_dmp_test_problem_opt_bs200.csv \
    -W TestFiles/rdm_water_sources_test_problem_opt_bs200.csv

