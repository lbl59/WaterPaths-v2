#!/bin/bash
#SBATCH -A m2702 -C cpu
#SBATCH --nodes=8
#SBATCH --ntasks-per-node=128
#SBATCH --qos=debug
#SBATCH --job-name=opt_mm4_n1024_v2020_base
#SBATCH --output=logs/opt_mm4_n1024_v2020_base.out
#SBATCH --error=logs/opt_mm4_n1024_v2020_base.err
#SBATCH --time=00:30:00
#SBATCH --mail-user=lbl59@cornell.edu
#SBATCH --mail-type=ALL

export OMP_NUM_THREADS=4
srun -n 1024 ./SedentoValleySimulation -T ${OMP_NUM_THREADS} -t 2344 -r 200 \
    -d /pscratch/sd/l/lbl59/WaterPaths-IU/ -I _bs200 \
    -C -1 -O rof_tables/ \
    -U TestFiles/rdm_utilities_test_problem_opt_bs200.csv \
    -P TestFiles/rdm_dmp_test_problem_opt_bs200.csv \
    -W TestFiles/rdm_water_sources_test_problem_opt_bs200.csv \
    -A TestFiles/rdm_actions_test_problem_opt_bs200_base.csv \
    -b true -n 5000 -o 500 -e 1 -i 4
