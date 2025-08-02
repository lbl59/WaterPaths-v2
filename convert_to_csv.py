
import numpy as np
import pandas as pd
import os
import pyarrow.parquet as pq
import pyarrow as pa

folder = 'TestFiles/DU_SOWs/rdm_82/evaporation'

# for each file in the folder, convert the csv to parquet
for filename in os.listdir(folder):
    if filename.endswith('.parquet'):
        parquet_file = os.path.join(folder, filename)
        
        # Read the Parquet file
        df = pd.read_parquet(f'{parquet_file}', engine="pyarrow")
        df_values = df.values
        df_save = np.zeros(df_values.shape, dtype=float)
        df_save[:] = df_values[:]
        
        # Convert to CSV
        csv_file = os.path.join(folder, filename.replace('.parquet', '.csv'))

        # save as CSV file with float values
        np.savetxt(csv_file, df_values, delimiter=',')

        print(f"Converted {parquet_file} to {csv_file}")
