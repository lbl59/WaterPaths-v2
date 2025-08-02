import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import colormaps, cm
from matplotlib.collections import PatchCollection
from matplotlib.patches import Rectangle
from matplotlib.lines import Line2D
from pandas.plotting import parallel_coordinates
from parallel_plot_functions import *

figsize = (12,6)
fontsize = 14
fig_dir = 'figures/'
data_dir = 'objectives_dvs/'

objs_names = ['REL', 'RF', 'INPC', 'PFC', 'WCC']
num_masters = 4
num_nfe = 150000
num_seeds = 1
modes = ['IU', 'base']
objs = pd.DataFrame()
#objs_noIns = pd.DataFrame()

# import csv files
for m in modes:
    #curr_mode = 'wIns'
    #curr_df = objs_wIns
    # for each seed, import the csv file and append to the dataframe
    df = pd.read_csv(f'{data_dir}objs_reference_set_{m}.csv', index_col=None, names=objs_names).astype(float)
    df['Category'] = [m]*df.shape[0]
    objs = pd.concat([objs, df], ignore_index=True)

    print(f'Num sols in mode {m}: {df.shape[0]}')
'''
df1 = pd.read_csv(f'{data_dir}objs_MM4_N150000_bs200_wIns_seed1.csv', index_col=None, names=objs_names).astype(float)
df2 = pd.read_csv(f'{data_dir}objs_MM4_N150000_bs200_wIns_seed2.csv', index_col=None, names=objs_names).astype(float)
#df3 = pd.read_csv(f'{data_dir}objs_MM{num_masters}_p{num_procs}_full_NFE15K.csv', index_col=None, names=objs_names).astype(float)

print('Data upload complete')

df1['Category'] = ['wIns']*df1.shape[0]
df2['Category'] = ['wIns']*df2.shape[0]
#df3['Category'] = ['full']*df3.shape[0]

# combine dataframes
objs = pd.concat([df1,df2], ignore_index=True)
'''

objs['REL'] = 1.0 - objs['REL']

#print(objs.head())

print('Plotting parallel plot...')
### Now color by a categorical variable that isnt one of the axes
color_dict_categorical = {'base': '#F39A9D', 'IU': '#3F6C51'}
#color_dict_categorical = {'wIns': '#F39A9D', 'noIns': '#6DB1BF', 'full': '#3F6C51'}

custom_parallel_coordinates(objs, columns_axes=objs_names, axis_labels = objs_names, 
                            zorder_by=4, ideal_direction='top',
                            alpha_base=0.7, lw_base=1.5, fontsize=fontsize, figsize=figsize,
                            minmaxs=['max','min','min','min','min'], 
                            colorbar_ticks_continuous=range(0,1,5),
                            color_by_categorical = 'Category', color_dict_categorical=color_dict_categorical,
                            save_fig_filename = f'{fig_dir}parallelplot_MM{num_masters}_N{num_nfe}_compare_IU_base.png') 

print('Parallel plot complete')