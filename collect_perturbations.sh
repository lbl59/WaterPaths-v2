#!/bin/bash
#SBATCH -A m2702 -C cpu
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=100
#SBATCH --qos=debug
#SBATCH --job-name=collect_perturbations
#SBATCH --output=logs/collect_perturbations.out
#SBATCH --error=logs/collect_perturbations.err
#SBATCH --time=00:30:00
#SBATCH --mail-user=lbl59@cornell.edu
#SBATCH --mail-type=ALL


python ./collect_perturbations.py