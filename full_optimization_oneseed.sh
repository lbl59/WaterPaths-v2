#!/bin/bash
#SBATCH -A m2702 -C cpu
#SBATCH --nodes=32
#SBATCH --ntasks-per-node=8
#SBATCH --qos=regular
#SBATCH --cpus-per-task=32
#SBATCH --job-name=opt_mm4_n256_base_updated_src_seed1
#SBATCH --output=logs/opt_mm4_n256_base_updated_src_seed1.out
#SBATCH --error=logs/opt_mm4_n256_base_updated_src_seed1.err
#SBATCH --time=00:10:00
#SBATCH --mail-user=lbl59@cornell.edu
#SBATCH --mail-type=ALL

OMP_NUM_THREADS=32

srun -n 256 ./SedentoValleySimulation \
    -T $OMP_NUM_THREADS \
    -t 2344 -r 200 -I _bs200 -M 1 \
    -d /pscratch/sd/l/lbl59/WaterPaths-IU/ \
    -v output_raw_base_updated_src/ \
    -C -1 -O rof_tables_opt_base/ \
    -U TestFiles/rdm_utilities_test_problem_opt_bs200.csv \
    -P TestFiles/rdm_dmp_test_problem_opt_bs200.csv \
    -W TestFiles/rdm_water_sources_test_problem_opt_bs200.csv \
    -b true -n 10000 -o 1000 -e 1 -i 4