from mpi4py import MPI
import os
import numpy as np

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
num_ranks = comm.Get_size()

DATA_DIR = "/pscratch/sd/l/lbl59/WaterPaths-IU/"
NUM_SOLS = 173
MODE='base'
REF='IU'

N_PERTURBATIONS = 500 
N_NODES = 2
N_PROCS_PER_NODE = 50
N_PROCS = int(N_PERTURBATIONS * N_NODES) # should be 100
N_PERTURBATIONS_PER_PROC = int(N_PERTURBATIONS/N_PROCS)  # should be 5

OMP_NUM_THREADS = 4

perturbation_ids = list(range(N_PERTURBATIONS))
perturbation_chunks = np.array_split(perturbation_ids, num_ranks)
my_perturbations = perturbation_chunks[rank]

for current_perturbation in my_perturbations:
    SOLS_FILE_NAME = f'sol_perturbations/dvs_{REF}_p{current_perturbation}.csv'  # change this value
    OUTPUT_DIR = f'output_perturbations/output_refset_{MODE}_p{current_perturbation}/'

    command_run_rdm = "./SedentoValleySimulation -T {} -t 2344 -r 200 \
        -d {} -C -1 -O rof_tables_opt/ -I _bs200 \
        -s {} -f 0 -l {} -v {} -p 0 \
        -U TestFiles/rdm_utilities_test_problem_opt_bs200.csv \
        -P TestFiles/rdm_dmp_test_problem_opt_bs200.csv \
        -W TestFiles/rdm_water_sources_test_problem_opt_bs200.csv".format(
                                OMP_NUM_THREADS, DATA_DIR, 
                                SOLS_FILE_NAME, NUM_SOLS, OUTPUT_DIR)

    #print(command_run_rdm)
    os.system(command_run_rdm)

comm.Barrier()