import numpy as np
import pandas as pd 
import os
import sys 

def collect_perturbations_sim_dv(mode, dv_set, num_sols):
    num_perturbations = 500
    folder = f"output_perturbations/output_refset_{mode}_p"

    all_sols_perturbed = np.zeros((num_sols, num_perturbations, 15))

    # make a folder called dv_set if it does not exist
    folder_perturbed_allsols = f"output_perturbations/sim_{mode}_dv_{dv_set}"

    if not os.path.exists(folder_perturbed_allsols):
        os.makedirs(folder_perturbed_allsols)

    # for each perturbation, read the corresponding CSV file
    for perturbation_id in range(num_perturbations):
        file_path = f"{folder}{perturbation_id}/Objectives_sols0_to_{num_sols}.csv"
        if not os.path.exists(file_path):
            print(f"File {file_path} does not exist.")
            continue
        
        all_sols_p = np.loadtxt(file_path, delimiter=',')
        if all_sols_p.shape[0] != num_sols:
            print(f"Warning: Expected {num_sols} solutions, but found {all_sols_p.shape[0]} in perturbation {perturbation_id}.")
            continue
        all_sols_perturbed[:, perturbation_id, :] = all_sols_p

    # for each solution, save the perturbed values
    for sol_id in range(num_sols):
        output_filename = f"sol_{sol_id}_all_perturbations.csv"
        output_filepath = os.path.join(folder_perturbed_allsols, output_filename)

        np.savetxt(output_filepath, all_sols_perturbed[sol_id, :, :], delimiter=',')
        print(f"Saved perturbed values for solution {sol_id} \n\
              ({mode} simulation using {dv_set} refset) to {output_filepath}")

mode = 'base'
dv_set = 'IU'
num_sols = 173

collect_perturbations_sim_dv(mode, dv_set, num_sols)
print(f"Completed collecting perturbations for {mode} with dv_set {dv_set}.")