import numpy as np 
import pandas as pd
import os

simulation_setup = "IU"
refset_setup = "base"
sol_num = 56
objs_dir = f"objs_reeval/sim_{simulation_setup}_refset_{refset_setup}/"

num_rdm = 200 
# Initialize objs_all_rdm outside the loop so it is always defined
objs_all_rdm = np.zeros((sol_num, num_rdm, 15), dtype=float)  # Assuming 15 objectives, adjust as necessary

# for each csv file in objs_dir, read the csv file and combine them into a single dataframe
for rdm in range(num_rdm):
    rdm_file = f"{objs_dir}Objectives_RDM{rdm}_sols0_to_{sol_num}.csv"

    # check if file exists
    if not os.path.exists(rdm_file):
        print(f"File {rdm_file} does not exist. Skipping...")
        continue

    objs_all_rdm_data = np.loadtxt(rdm_file, delimiter=',')
    
    for sol in range(sol_num):  # sol_num is inclusive, so we go from 0 to sol_num
        objs_all_rdm[sol, rdm, :] = objs_all_rdm_data[sol, :]

for sol in range(sol_num):
    # return the combined array as a CSV file 
    combined_objs_file_name = f"{objs_dir}combined_objs_sol{sol}.csv"
    objs_sol = objs_all_rdm[sol, :, :]  # Extract the objectives for the current solution

    # check that objs_sol has the correct shape (200, 15)
    if objs_sol.shape != (num_rdm, 15):
        print(f"Shape mismatch for solution {sol}: expected (200, 15), got {objs_sol.shape}")
        continue
    else:
        np.savetxt(combined_objs_file_name, objs_sol, delimiter=',', fmt='%.6f')

