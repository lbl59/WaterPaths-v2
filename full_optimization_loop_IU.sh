#!/bin/bash

# CPUs per task x ntasks-per-node = 256
# omp_num_threads = ntasks-per-node / 2

export OMP_NUM_THREADS=2 
#for JOB in 2 3 4 5; do
for JOB in 3; do
  cat <<EOF | sbatch
#!/bin/bash
#SBATCH -A m2702 -C cpu
#SBATCH --nodes=32
#SBATCH --ntasks-per-node=64 
#SBATCH --cpus-per-task=4
#SBATCH --qos=regular 
#SBATCH --job-name=opt_mm4_n2048_IU_newrun_seed${JOB}
#SBATCH --output=logs/opt_mm4_n2048_IU_newrun_seed${JOB}.out
#SBATCH --error=logs/opt_mm4_n2048_IU_newrun_seed${JOB}.err
#SBATCH --time=00:15:00
#SBATCH --mail-user=lbl59@cornell.edu
#SBATCH --mail-type=ALL 

srun -n 2048 ./SedentoValleySimulation \
    -T ${OMP_NUM_THREADS} -t 2344 -r 200 \
    -d /pscratch/sd/l/lbl59/WaterPaths-IU/ \
    -v output_raw_IU_new/ -I _bs200 \
    -C -1 -O rof_tables_opt_csv/ \
    -U TestFiles/rdm_utilities_test_problem_opt_bs200.csv \
    -P TestFiles/rdm_dmp_test_problem_opt_bs200.csv \
    -W TestFiles/rdm_water_sources_test_problem_opt_bs200.csv \
    -A TestFiles/rdm_actions_test_problem_opt_bs200.csv \
    -b true -n 30000 -o 5000 -e ${JOB} -i 4
EOF
  sleep 0.5
done
