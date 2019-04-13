#!/bin/bash
read n
s=0
for item in $(cat) ; do
    s=$((s+item))
done
printf '%.3f' "$(echo "scale=4; $s/$n" | bc)"
