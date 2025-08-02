#!/bin/bash
# Usage: ./extract_dvs.sh input_file output_file

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <input_file> <output_file>"
    exit 1
fi

INPUT_FILE="$1"
OUTPUT_FILE="$2"

# Define the new header row with the specified labels
HEADER="RT_W,RT_D,RT_F,TT_D,TT_F,LMA_W,LMA_D,LMA_F,AP_W,AP_D,AP_F,INSU_W,INSU_D,INSU_F,INSP_W,INSP_D,INSP_F,INF_W,INF_D,INF_F,NRR_W,CRL,CRH,WR1,SCR,GQ,NRR_F,WR2,DR,FR"

# Write the header to the output file
echo "$HEADER" > "$OUTPUT_FILE"

# Process each line:
# - Print up to the first 30 fields (or all if fewer), comma-separated into the output file.
awk '{
    max = (NF < 30) ? NF : 30;
    for(i = 1; i < max; i++){
        printf "%s,", $i;
    }
    print $max;
}' "$INPUT_FILE" >> "$OUTPUT_FILE"

echo "Header and first 30 columns have been saved as a CSV file in $OUTPUT_FILE."