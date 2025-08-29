import numpy as np 
import pandas as pd 
import os 
import sys 

print('Uploading DV file...')
dv_names = ['RT_W', 'RT_D', 'RT_F', 'TT_D', 'TT_F', 'LMA_W', 'LMA_D', 'LMA_F', \
    'AP_W', 'AP_D', 'AP_F', 'IT_W', 'IT_D', 'IT_F', 'IP_W', 'IP_D', 'IP_F', \
    'INF_W', 'INF_D', 'INF_F', 'NRR_W', 'CRL', 'CRH', 'WR1', 'SCR', 'GQR', \
    'NRR_F', 'WR2_W', 'WR2_D', 'WR2_F']

objs_dvs_folder = 'objectives_dvs/'
mode = 'IU'
num_sols = 550
dv_file = f'{objs_dvs_folder}dvs_{mode}_truncated.csv'
dv_file_data = np.loadtxt(dv_file, delimiter=',', dtype=float)
dv_file_df = pd.DataFrame(dv_file_data, columns=dv_names)

print('Uploading DV perturbations...')
dv_perturbations = pd.read_csv('rdm_actions_test_problem_reeval_500_with_header.csv',
                               index_col=None, header=0)
output_folder = f'{objs_dvs_folder}/dvs_{mode}_perturbed/'
# turn all header names into upper case 
dv_perturbations.columns = dv_perturbations.columns.str.upper()
# remove any leading or trailing whitespace from column names
dv_perturbations.columns = dv_perturbations.columns.str.strip()
colnames = dv_perturbations.columns.tolist()

#print('Colnames in perturbations:', colnames)
num_perturbed_dvs = len(colnames)

# for each perturbation, create a new file with the perturbation applied
for p in range(dv_perturbations.shape[0]):
    perturbed_dv_file = dv_file_data.copy()  # make a copy of the original DV file data
    perturbation = dv_perturbations.iloc[p, :].values
    
    for c, col in enumerate(colnames):
        original_dv_val = dv_file_df[col].values
        # find the index of the current colname in the dv_names list
        col_idx_in_dv_names = dv_names.index(col)

        # add perturbation from (p,c) to the original dv_file_data
        perturbed_dv_file[:, col_idx_in_dv_names] = original_dv_val + perturbation[c]
    
    perturbed_dv_file[:, num_perturbed_dvs:] = dv_file_data[:, num_perturbed_dvs:]  # keep the rest of the columns unchanged
    # clip all values to be within [0, 1] for better performance and readability
    perturbed_dv_file = np.clip(perturbed_dv_file, 0, 1)
    # write the perturbed data to a new file
    output_filename = f'{output_folder}dvs_{mode}_p{p}.csv'

    np.savetxt(output_filename, perturbed_dv_file, delimiter=',', fmt='%.2f')
