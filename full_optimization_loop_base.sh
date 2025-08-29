#!/bin/bash

# CPUs per task x ntasks-per-node = 256
# omp_num_threads = ntasks-per-node / 2

export OMP_NUM_THREADS=4
for JOB in 1 2 3 4 5; do
  cat <<EOF | sbatch
#!/bin/bash
#SBATCH -A m2702 -C cpu
#SBATCH --nodes=32
#SBATCH --ntasks-per-node=64 
#SBATCH --cpus-per-task=4
#SBATCH --qos=regular
#SBATCH --job-name=opt_mm4_n2048_base_seed${JOB}
#SBATCH --output=logs/opt_mm4_n2048_base_seed${JOB}.out
#SBATCH --error=logs/opt_mm4_n2048_base_seed${JOB}.err
#SBATCH --time=02:00:00
#SBATCH --mail-user=lbl59@cornell.edu
#SBATCH --mail-type=ALL 

srun -n 2048 ./SedentoValleySimulation \
  -T ${OMP_NUM_THREADS} -t 2344 -r 200 \
  -d /pscratch/sd/l/lbl59/WaterPaths-IU/ \
  -v output_raw_base_fullExp/ -I _bs200 -M 1 \
  -C -1 -O rof_tables_opt_base/ \
  -U TestFiles/rdm_utilities_test_problem_opt_bs200.csv \
  -P TestFiles/rdm_dmp_test_problem_opt_bs200.csv \
  -W TestFiles/rdm_water_sources_test_problem_opt_bs200.csv \
  -b true -n 200000 -o 2000 -e ${JOB} -i 4
EOF
  sleep 0.5
done
