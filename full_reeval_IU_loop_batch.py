from mpi4py import MPI
import os
import sys 

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
size = comm.Get_size()

# get SLURM array task ID from command line
batch_num = int(sys.argv[1]) if len(sys.argv) > 1 else 0

N_RDMs = 200 
N_REALIZATIONS = 200
N_PERTURBATIONS_PER_BATCH = 50
OMP_NUM_THREADS = 100

DATA_DIR = "/pscratch/sd/l/lbl59/WaterPaths-IU/"
NUM_SOLS = 56
MODE='base'

all_tasks = []
for rdm in range(N_RDMs):
    for p in range(N_PERTURBATIONS_PER_BATCH):
        all_tasks.append((rdm, p))   # (200,100) matrix of tasks for a total of 100,000 tasks
    
N_NODES = 1000
N_PROCS_PER_NODE = 100
N_PROCS = int(N_PROCS_PER_NODE * N_NODES)  # should be 10,000
N_TASKS_PER_PROC = int(len(all_tasks) / N_PROCS)  # should be 1

# Ensure each process gets a unique set of tasks
if len(all_tasks) % N_PROCS != 0:
    raise ValueError("Number of tasks is not evenly divisible by number of processes.")

# split tasks among processes
tasks_per_rank = (len(all_tasks) + size - 1) // size  # ceiling division
start_idx = rank * tasks_per_rank
end_idx = min(start_idx + tasks_per_rank, len(all_tasks))
my_tasks = all_tasks[start_idx:end_idx]

# Each rank processes its tasks
for rdm, p in my_tasks:
    p_batch = (batch_num * N_PERTURBATIONS_PER_BATCH) + p
    sols_file_name = f'objectives_dvs/dvs_{MODE}_shaved_{NUM_SOLS}_p{p_batch}.csv'

    command_run_rdm = "./SedentoValleySimulation -T {} -t 2344 -r {} -d {} -D TestFiles/DU_SOWs \
        -C -1 -O rof_tables_reeval/rdm_{}/ -R {} \
        -s {} -f 0 -l {} -p 0 \
        -U TestFiles/rdm_utilities_test_problem_reeval_200.csv \
        -P TestFiles/rdm_dmp_test_problem_reeval_200.csv \
        -W TestFiles/rdm_water_sources_test_problem_reeval_200.csv \
        -A TestFiles/rdm_actions_test_problem_reeval_200.csv".format(
                                OMP_NUM_THREADS, N_REALIZATIONS, DATA_DIR, 
                                rdm, rdm, 
                                sols_file_name, NUM_SOLS)

    print(f"[Rank {rank}] RDM {rdm}, Perturbation {p_batch}")
    os.system(command_run_rdm)

comm.Barrier()