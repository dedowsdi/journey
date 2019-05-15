read file line by line, include last line even if it the file doesn't end with new line.
```
while IFS='' read -r line || [[ -n "$line" ]]; do
    echo "Text read from file: $line"
done < file
```
