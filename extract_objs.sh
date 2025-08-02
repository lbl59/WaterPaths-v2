#!/bin/bash
# Usage: ./process_file.sh input_file output_file

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <input_file> <output_file>"
    exit 1
fi

INPUT_FILE="$1"
OUTPUT_FILE="$2"

# Clear (or create) the output CSV file.
> "$OUTPUT_FILE"

# Process each line:
# - Determine the starting field (if the line has fewer than five fields, start at field 1)
# - Print the fields as comma-separated values into the user-defined output file.
awk '{
    start = (NF - 4 > 0) ? NF - 4 : 1;
    for(i = start; i < NF; i++){
        printf "%s,", $i;
    }
    print $NF;
}' "$INPUT_FILE" > "$OUTPUT_FILE"

echo "Extracted last five columns have been saved as a CSV file in $OUTPUT_FILE."

# Check if the last line of the input file contains a '#' character.
last_line=$(tail -n 1 "$INPUT_FILE")
if [[ "$last_line" != *"#"* ]]; then
    # Ensure the input file ends with a newline and append a line with '#'
    #echo "" >> "$INPUT_FILE"
    echo "#" >> "$INPUT_FILE"
    echo "A '#' has been appended to the input file."
else
    echo "The last line of the input file already contains a '#'."
fi