#!/bin/bash

#for JOB in 1 2 3 4 5; do
for JOB in 1; do
  cat <<EOF | sbatch
#!/bin/bash
#SBATCH -A m2702 -C cpu
#SBATCH --nodes=32
#SBATCH --ntasks-per-node=64 
#SBATCH --cpus-per-task=4
#SBATCH --qos=regular
#SBATCH --job-name=opt_mm4_n2048_c4_updatedObjs_base_seed${JOB}
#SBATCH --output=logs/opt_mm4_n2048_c4_updatedObjs_base_seed${JOB}.out
#SBATCH --error=logs/opt_mm4_n2048_c4_updatedObjs_base_seed${JOB}.err
#SBATCH --time=24:00:00
#SBATCH --mail-user=lbl59@cornell.edu
#SBATCH --mail-type=ALL

OMP_NUM_THREADS=20

srun -n 2048 -c 4 ./SedentoValleySimulation \
    -T ${OMP_NUM_THREADS} \
    -t 2344 -r 200 \
    -d /pscratch/sd/l/lbl59/WaterPaths-IU/ \
    -v output_raw_base/ \
    -C -1 -O rof_tables_opt_parquet/ -I _bs200 \
    -U TestFiles/rdm_utilities_test_problem_opt_bs200.csv \
    -P TestFiles/rdm_dmp_test_problem_opt_bs200.csv \
    -W TestFiles/rdm_water_sources_test_problem_opt_bs200.csv \
    -b true -n 75000 -o 5000 -e ${JOB} -i 4
EOF
  sleep 0.5
done
