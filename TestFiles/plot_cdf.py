import numpy as np 
import matplotlib.pyplot as plt
import pandas as pd
import os 

def plot_flow_duration_curve(data, color, alpha, label, ax):
    """
    Plot the flow duration curve for the given data.
    
    Parameters:
    data (array-like): The data to plot.
    color (str): The color of the plot.
    
    Returns:
    None
    """
    num_realizations = data.shape[0]
    num_weeks = data.shape[1]
    num_hist_years = 50
    num_weeks_init = num_hist_years * 52
    num_weeks_actual = num_weeks - num_weeks_init
    #n = 52
    M = np.array(range(1, num_weeks_actual+1))
    P = (M-0.5)/num_weeks_actual

    data_subset = data[:, num_weeks_init:]

    #cdf_matrix = np.zeros((num_realizations, num_weeks), dtype=float)
    #sorted_data = np.zeros((num_realizations, num_weeks), dtype=float)
    for i in range(num_realizations):
        data_i = data_subset[i, :]
        sorted_data_i = np.sort(data_i, 0)[::-1]
        #sorted_data_semilog = np.log10(sorted_data_i)
        #sorted_data[i, :] = sorted_data_i
        #cdf_matrix_i = np.arange(1, len(sorted_data_i) + 1) / len(sorted_data_i)
        if i == 0:
            ax.semilogy(P, sorted_data_i, color=color, linewidth=1.5, alpha=alpha, label=label)
        else:
            ax.semilogy(P, sorted_data_i, color=color, linewidth=1.5, alpha=alpha)

fig, axs = plt.subplots(3, 4, figsize=(20, 15))

# for each file in the given directory, read the data and plot the CDF
inflows_full_dir = 'inflows/'
inflows_bs200_dir = 'inflows_bs200/'

# iterate over the files in the inflows_full_dir
for i, filename in enumerate(os.listdir(inflows_full_dir)):
    if filename.endswith('.csv'):
        # read the data
        inflows_full = pd.read_csv(os.path.join(inflows_full_dir, filename), header=None).values
        inflows_bs200 = pd.read_csv(os.path.join(inflows_bs200_dir, filename), header=None).values

        # plot the CDF
        ax = axs[i // 4, i % 4]
        plot_flow_duration_curve(inflows_full, 'lightgrey', 1.0, '1,000 realizations', ax)
        plot_flow_duration_curve(inflows_bs200, 'indianred', 1.0, '200 realizations', ax)
        ax.set_title(filename)
        ax.set_xlabel('Probability')
        ax.set_ylabel('Inflows (MGD')
        ax.legend(loc='lower right')

# save the figure
plt.tight_layout()
plt.savefig('cdf_inflows.png', dpi=300)


