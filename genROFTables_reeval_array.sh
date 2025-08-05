#!/bin/bash
#SBATCH -A m2702 -C cpu
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --qos=regular
#SBATCH --job-name=genROFs_reeval_rdm0_to_rdm199
#SBATCH --output=logs/genROFs_reeval_rdm%a.out
#SBATCH --error=logs/genROFs_reeval_rdm%a.err
#SBATCH --time=00:05:00
#SBATCH --mail-user=lbl59@cornell.edu
#SBATCH --mail-type=ALL
#SBATCH --array=0-199

export OMP_NUM_THREADS=256

rdm=${SLURM_ARRAY_TASK_ID}

echo "Generating ROF tables for RDM ${rdm}..."

srun ./SedentoValleySimulation -T ${OMP_NUM_THREADS} -t 2344 -r 200 \
    -d /pscratch/sd/l/lbl59/WaterPaths-IU/ \
    -R ${rdm} -D TestFiles/DU_SOWs \
    -s objectives_dvs/dvs_base_shaved_56.csv -m 0 -C 1 \
    -O rof_tables_reeval/rdm_${rdm}/ \
    -U TestFiles/rdm_utilities_test_problem_reeval_200.csv \
    -P TestFiles/rdm_dmp_test_problem_reeval_200.csv \
    -W TestFiles/rdm_water_sources_test_problem_reeval_200.csv \
