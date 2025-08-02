#!/bin/bash
#SBATCH -A m2702 -C cpu
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=100
#SBATCH --qos=debug
#SBATCH --job-name=convert_demands_to_parquet
#SBATCH --output=logs/convert_demands_to_parquet.out
#SBATCH --error=logs/convert_demands_to_parquet.err
#SBATCH --time=00:30:00
#SBATCH --mail-user=lbl59@cornell.edu
#SBATCH --mail-type=ALL


python ./convert_csv_to_parquet.py