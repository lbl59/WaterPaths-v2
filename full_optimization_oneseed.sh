#!/bin/bash
#SBATCH -A m2702 -C cpu
#SBATCH --nodes=4
#SBATCH --ntasks-per-node=128
#SBATCH --qos=debug
#SBATCH --job-name=opt_mm2_n512_noIns_base_seed1
#SBATCH --output=logs/opt_mm2_n512_noIns_base_seed1.out
#SBATCH --error=logs/opt_mm2_n512_noIns_base_seed1.err
#SBATCH --time=00:30:00
#SBATCH --mail-user=lbl59@cornell.edu
#SBATCH --mail-type=ALL

OMP_NUM_THREADS=2

srun -n 512 ./SedentoValleySimulation \
    -T $OMP_NUM_THREADS \
    -t 2344 -r 200 \
    -d /pscratch/sd/l/lbl59/WaterPaths-IU/ \
    -v output_raw_base_noIns/ \
    -C -1 -O rof_tables_opt_base_pq/ -I _bs200 \
    -U TestFiles/rdm_utilities_test_problem_opt_bs200.csv \
    -P TestFiles/rdm_dmp_test_problem_opt_bs200.csv \
    -W TestFiles/rdm_water_sources_test_problem_opt_bs200.csv \
    -b true -n 15000 -o 1500 -e 1 -i 2