#!/bin/bash

duration=${1:-0.5s}
bin_dir=${2:-build/gcc/Debug/bin}

cd ${bin_dir} || {  echo "failed to cd to $bin_dir" && exit 1 ; }

echo > test_exe.log

exec &>> test_exe.log

find . -mindepth 1 -maxdepth 1 -executable -type f  ! -name '.*' -print0 |
while IFS='' read -r -d $'\0' item ; do
    printf -- '-%.0s' {1..80} && printf '\n'
    printf 'testing %s\n' $item
    $item&
    pid=$!
    sleep "$duration"
    ps --pid "$pid" &> /dev/null && kill -9 "$pid"
done
