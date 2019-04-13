#!/bin/bash

n=1
[[ $# -gt 0 ]] && n=$1
# echo iteration "$n"

# setup background
data=()

for i in $(seq 1 63) ; do
  #statements
  data+=($(printf '_%.0s' {1..100}))
done

function buildTree()
{
  row=$1
  col=$2
  seg=$3
  # up
  for (( k = 0; k < seg; k++ )); do
    index=$((row+k))
    data[$index]=${data[index]:0:col}1${data[index]:col+1}
  done
  # left and right
  for (( k = 0; k < seg; k++ )); do
    index=$((row+seg+k))
    data[$index]=${data[index]:0:col-1-k}1${data[index]:col-k}
    data[$index]=${data[index]:0:col+1+k}1${data[index]:col+2+k}
  done
}

segLen=16
startRow=0 # start row of next level
startCol=49 # start col of next level
numBranches=1
for (( i = 0; i < n; i++ )); do
  for (( j = 0; j < numBranches; j++ )); do
    buildTree $startRow $(( startCol + segLen * 4 * j )) $segLen
  done

  if [[ $segLen -eq 1 ]]; then
    break
  fi
  startRow=$(( startRow + segLen * 2 ))
  startCol=$(( startCol - segLen ))
  segLen=$(( segLen/2 ))
  numBranches=$(( numBranches * 2 ))
done


for line in "${data[@]}" ; do
  echo "$line"
done | tac
