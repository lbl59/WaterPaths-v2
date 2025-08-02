#!/bin/bash
#SBATCH -A m2702 -C cpu
#SBATCH -N 1 -n 1
#SBATCH --qos=preempt
#SBATCH --job-name=simulate_one_perturbation_base
#SBATCH --output=logs/simulate_one_perturbation_base.out
#SBATCH --error=logs/simulate_one_perturbation_base.err
#SBATCH --time=12:00:00
#SBATCH --mail-user=lbl59@cornell.edu
#SBATCH --mail-type=ALL

export OMP_NUM_THREADS=2

DATA_DIR="/pscratch/sd/l/lbl59/WaterPaths-IU/"
SOLS_FILE_NAME = "sol_perturbations/dvs_base_p0.csv"

srun ./SedentoValleySimulation -T ${OMP_NUM_THREADS} -t 2344 \
    -r 200 -d ${DATA_DIR} -C 1 -O rof_tables_opt/ \
    -I _bs200 -s {} -f 0 -l 56 -v output_test_perturbation -p 0 \
    -U TestFiles/rdm_utilities_test_problem_opt_bs200.csv \
    -P TestFiles/rdm_dmp_test_problem_opt_bs200.csv \
    -W TestFiles/rdm_water_sources_test_problem_opt_bs200.csv