#!/bin/bash
#SBATCH -A m2702 -C cpu
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --qos=debug
#SBATCH --job-name=simulate_base_dv_IU
#SBATCH --output=logs/simulate_base_dv_IU.out
#SBATCH --error=logs/simulate_base_dv_IU.err
#SBATCH --time=00:30:00
#SBATCH --mail-user=lbl59@cornell.edu
#SBATCH --mail-type=ALL

export OMP_NUM_THREADS=256

srun ./SedentoValleySimulation -T ${OMP_NUM_THREADS} -t 2344 -r 200 \
    -d /pscratch/sd/l/lbl59/WaterPaths-IU/ \
    -s objectives_dvs/dvs_IU_truncated.csv -f 0 -l 105 -p 0 \
    -v output_sim_base_dv_IU/ -M 1 \
    -C -1 -O rof_tables_opt_base/ -I _bs200 \
    -U TestFiles/rdm_utilities_test_problem_opt_bs200.csv \
    -P TestFiles/rdm_dmp_test_problem_opt_bs200.csv \
    -W TestFiles/rdm_water_sources_test_problem_opt_bs200.csv
    

