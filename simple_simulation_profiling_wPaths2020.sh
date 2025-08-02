#!/bin/bash
#SBATCH -A m2702 -C cpu
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --qos=debug
#SBATCH --job-name=simulate_refset_IU_1real
#SBATCH --output=logs/simulate_refset_IU_1real.out
#SBATCH --error=logs/simulate_refset_IU_1real.err
#SBATCH --time=00:30:00
#SBATCH --mail-user=lbl59@cornell.edu
#SBATCH --mail-type=ALL

export OMP_NUM_THREADS=1

srun -n 1 ./SedentoValleySimulation -T ${OMP_NUM_THREADS} -t 2344 -r 1 \
    -d /pscratch/sd/l/lbl59/WaterPaths-IU/ \
    -s sample_solutions.csv -I _bs200 \
    -m 0 -C -1 -O rof_tables/ -c true \
    -U TestFiles/rdm_utilities_test_problem_opt_bs200.csv \
    -P TestFiles/rdm_dmp_test_problem_opt_bs200.csv \
    -W TestFiles/rdm_water_sources_test_problem_opt_bs200.csv \
    -A TestFiles/rdm_actions_test_problem_opt_bs200.csv
