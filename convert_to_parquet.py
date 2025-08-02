
import numpy as np
import pandas as pd
import os
import pyarrow.parquet as pq
import pyarrow as pa

file_type = 'evaporation'
folder = f'TestFiles/{file_type}_bs200/'
col_names = 'week_'

# for each file in the folder, convert the csv to parquet
for filename in os.listdir(folder):
    if filename.endswith('.csv'):
        csv_file = os.path.join(folder, filename)
        parquet_file = os.path.join(folder, filename.replace('.csv', '.parquet'))
        
        # Read the CSV file
        df = pd.read_csv(csv_file, header=None, index_col=None)
        num_cols = df.shape[1]
        # Rename columns to colname_0, colname_1, ..., colname_n
        df.columns = [f'{col_names}{i}' for i in range(num_cols)]

        # Convert to Parquet with zstd level 6 compression
        df.to_parquet(parquet_file, engine='pyarrow', index=False, 
                      compression='zstd', compression_level=6)
        print(f"Converted {csv_file} to {parquet_file}")
