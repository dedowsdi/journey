#!/usr/bin/awk -i

# readfile.awk --- read an entire file at once
function readfile(file,     tmp, save_rs)
{
    save_rs = RS
    RS = "^$" # never match if file is not empty
    getline tmp < file
    close(file)
    RS = save_rs
    return tmp
}
