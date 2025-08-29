#!/bin/bash
#SBATCH -A m2702 -C cpu
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --qos=debug
#SBATCH --job-name=simulate_IU_dv_base
#SBATCH --output=logs/simulate_IU_dv_base.out
#SBATCH --error=logs/simulate_IU_dv_base.err
#SBATCH --time=00:30:00
#SBATCH --mail-user=lbl59@cornell.edu
#SBATCH --mail-type=ALL

export OMP_NUM_THREADS=256

srun ./SedentoValleySimulation -T ${OMP_NUM_THREADS} -t 2344 -r 200 \
    -d /pscratch/sd/l/lbl59/WaterPaths-IU/ \
    -s objectives_dvs/dvs_base_truncated.csv -v output_sim_IU_dv_base/ -I _bs200 -M 1 \
    -f 0 -l 77 -C -1 -O rof_tables_opt_IU/ -p 0 \
    -U TestFiles/rdm_utilities_test_problem_opt_bs200.csv \
    -P TestFiles/rdm_dmp_test_problem_opt_bs200.csv \
    -W TestFiles/rdm_water_sources_test_problem_opt_bs200.csv \
    -A TestFiles/rdm_actions_test_problem_opt_bs200.csv
