#!/bin/bash
#SBATCH -A m2702 -C cpu
#SBATCH -N 1 -n 1
#SBATCH --qos=debug
#SBATCH --job-name=reeval_with_opt_csv_files_rdm1
#SBATCH --output=logs/reeval_with_opt_csv_files_rdm1.out
#SBATCH --error=logs/reeval_with_opt_csv_files_rdm1.err
#SBATCH --time=00:30:00
#SBATCH --mail-user=lbl59@cornell.edu
#SBATCH --mail-type=ALL

export OMP_NUM_THREADS=4

DATA_DIR="/pscratch/sd/l/lbl59/WaterPaths-IU/"

srun ./SedentoValleySimulation -T ${OMP_NUM_THREADS} -t 2344 -r 200 \
    -d ${DATA_DIR} -f 0 -l 173 -R 30 -D TestFiles/DU_SOWs/ \
    -C -1 -O rof_tables_reeval/rdm_30/ -p 0 -v output_og_method/ \
    -s objectives_dvs/dvs_IU_shaved_173.csv \
    -U TestFiles/rdm_utilities_test_problem_reeval_200.csv \
    -P TestFiles/rdm_dmp_test_problem_reeval_200.csv \
    -W TestFiles/rdm_water_sources_test_problem_reeval_200.csv \
