#!/bin/bash
#SBATCH -A m2702 -C cpu
#SBATCH --nodes=1
#SBATCH --ntasks=100
#SBATCH --qos=debug
#SBATCH --job-name=profiling_all_pq_simulation_small_output
#SBATCH --output=logs/profiling_all_pq_simulation_small_output.out
#SBATCH --error=logs/profiling_all_pq_simulation_small_output.err
#SBATCH --time=00:30:00
#SBATCH --mail-user=lbl59@cornell.edu
#SBATCH --mail-type=ALL

export OMP_NUM_THREADS=100
srun ./SedentoValleySimulation -T ${OMP_NUM_THREADS} -t 2344 -r 100 \
    -d /pscratch/sd/l/lbl59/WaterPaths-IU/ \
    -s dvs_reference_set_base.csv \
    -f 0 -l 1 -C -1 -O rof_tables_all_pq/ -p true \
    -U TestFiles/rdm_utilities_test_problem_opt_bs200.csv \
    -P TestFiles/rdm_dmp_test_problem_opt_bs200.csv \
    -W TestFiles/rdm_water_sources_test_problem_opt_bs200.csv \
    -A TestFiles/rdm_actions_test_problem_opt_bs200.csv \
