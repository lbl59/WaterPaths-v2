from mpi4py import MPI
import os
import numpy as np

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
n_ranks = comm.Get_size()

N_RDMs = 200 

OMP_NUM_THREADS = 4
N_REALIZATIONS = 200
DATA_DIR = "/pscratch/sd/l/lbl59/WaterPaths-IU/"
NUM_SOLS = 173
MODE='base'
REF='IU'
SOLS_FILE_NAME = f'objectives_dvs/dvs_{REF}_shaved_{NUM_SOLS}.csv'  # change this value
OUTPUT_DIR = f'output_sim_{MODE}_refset_{REF}_reeval/'

N_NODES = 4
N_PROCS_PER_NODE = 50
N_PROCS = int(N_PROCS_PER_NODE * N_NODES) # should be 200
N_RDMS_PER_PROC = int(N_RDMs/N_PROCS)  # should be 1

rdm_ids = list(range(N_RDMs))
rdm_chunks = np.array_split(rdm_ids, n_ranks)
my_rdms = rdm_chunks[rank]

for current_RDM in my_rdms:
    print(f"[Rank {rank}] Processing RDM {current_RDM}", flush=True)

    command_run_rdm = "./SedentoValleySimulation -T {} -t 2344 -r {} \
        -d {} -D TestFiles/DU_SOWs -R {} \
        -C -1 -O rof_tables_reeval/rdm_{}/ \
        -s {} -f 0 -l {} -v {} -p 0 \
        -U TestFiles/rdm_utilities_test_problem_reeval_200.csv \
        -P TestFiles/rdm_dmp_test_problem_reeval_200.csv \
        -W TestFiles/rdm_water_sources_test_problem_reeval_200.csv".format(
                                OMP_NUM_THREADS, N_REALIZATIONS, DATA_DIR, 
                                current_RDM, current_RDM, 
                                SOLS_FILE_NAME, NUM_SOLS, OUTPUT_DIR)

    os.system(command_run_rdm)

comm.Barrier()