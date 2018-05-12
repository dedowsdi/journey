#!/bin/bash

# autocomplete for *.txt
# source this file before you call ./sample_command

touch file1.txt file2.txt file2.doc file30.txt file4.zzz
touch sample_command
chmod +x sample_command
# exclude everything else except *.txt
complete -f -X '!*.txt' sample_command
