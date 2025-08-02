#!/bin/bash
BS_SAMPLE='bs200 bs500 full'
PROCS='2048 4072'
MASTERS='1 2 3 4'
INPUT_FOLDER="output"
OUTPUT_FOLDER="output_processed"

for bs in $BS_SAMPLE; do
  for proc in $PROCS; do
        for m in $MASTERS; do
            INPUT_FILE="${INPUT_FOLDER}/runtime_MM4_S1_M${m}_${proc}_${bs}_NFE15K.runtime"
            OUTPUT_FILE="${OUTPUT_FOLDER}/runtime_MM4_S1_M${m}_${proc}_${bs}_NFE15K.csv"

            # Start fresh
            echo "REL,RF,INPC,PFC,WCC,NFE_Value" > "$OUTPUT_FILE"

            # Use awk to process the file
            awk '
            /\/\/NFE=/ {
                in_section = 1
                split($0, arr, "=")
                nfe_value = arr[2]
                next
            }
            /^#/ {
                in_section = 0
                next
            }
            in_section {
                n = split($0, vals, " ")
                if (n >= 5) {
                    printf "%s,%s,%s,%s,%s,%s\n", vals[n-4], vals[n-3], vals[n-2], vals[n-1], vals[n], nfe_value >> "'$OUTPUT_FILE'"
                }
            }
            ' "$INPUT_FILE"
        done
    done
done