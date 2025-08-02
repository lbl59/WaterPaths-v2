#!/bin/bash

#for JOB in 1 2 3 4 5; do
for JOB in 5; do
  cat <<EOF | sbatch
#!/bin/bash
#SBATCH -A m2702 -C cpu
#SBATCH --nodes=16
#SBATCH --ntasks-per-node=128
#SBATCH --qos=regular
#SBATCH --job-name=opt_mm4_n2048_v2020_noIns_seed${JOB}
#SBATCH --output=logs/opt_mm4_n2048_v2020_noIns_seed${JOB}.out
#SBATCH --error=logs/opt_mm4_n2048_v2020_noIns_seed${JOB}.err
#SBATCH --time=14:00:00
#SBATCH --mail-user=lbl59@cornell.edu
#SBATCH --mail-type=ALL

export OMP_NUM_THREADS=4
srun -n 2048 ./SedentoValleySimulation \
    -T \$OMP_NUM_THREADS \
    -t 2344 -r 200 \
    -d /pscratch/sd/l/lbl59/WaterPaths-IU/ \
    -C -1 -O rof_tables/ -I _bs200 \
    -U TestFiles/rdm_utilities_test_problem_opt_bs200.csv \
    -P TestFiles/rdm_dmp_test_problem_opt_bs200.csv \
    -W TestFiles/rdm_water_sources_test_problem_opt_bs200.csv \
    -A TestFiles/rdm_actions_test_problem_opt_bs200.csv \
    -b true -n 150000 -o 5000 -e ${JOB} -i 4
EOF
  sleep 0.5
done
