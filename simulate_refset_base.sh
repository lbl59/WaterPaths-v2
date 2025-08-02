#!/bin/bash
#SBATCH -A m2702 -C cpu
#SBATCH --nodes=1
#SBATCH --ntasks=100
#SBATCH --qos=debug
#SBATCH --job-name=simulate_test
#SBATCH --output=logs/simulate_test.out
#SBATCH --error=logs/simulate_test.err
#SBATCH --time=00:30:00
#SBATCH --mail-user=lbl59@cornell.edu
#SBATCH --mail-type=ALL

export OMP_NUM_THREADS=100

srun ./SedentoValleySimulation -T ${OMP_NUM_THREADS} -t 2344 -r 200 \
    -d /pscratch/sd/l/lbl59/WaterPaths-IU/ \
    -s sol_perturbations/dvs_base_p0.csv \
    -f 0 -l 56 -C -1 -O rof_tables_opt/ -I _bs200 -p 0 -v output_test/ \
    -U TestFiles/rdm_utilities_test_problem_opt_bs200.csv \
    -P TestFiles/rdm_dmp_test_problem_opt_bs200.csv \
    -W TestFiles/rdm_water_sources_test_problem_opt_bs200.csv