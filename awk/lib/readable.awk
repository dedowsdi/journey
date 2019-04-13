
# readable.awk --- library file to skip over unreadable files

BEGIN {
    for (i = 1; i < ARGC; i++) {
        # skip assignment or standard input
        if (ARGV[i] ~ /^[a-zA-Z_][a-zA-Z0-9_]*=.*/ \
            || ARGV[i] == "-" || ARGV[i] == "/dev/stdin")
            continue
        else if ((getline junk < ARGV[i]) < 0) # unreadable, getline doesn't cause fatal error
            delete ARGV[i]
        else
            close(ARGV[i])
    }
}
