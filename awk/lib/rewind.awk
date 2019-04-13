# rewind.awk --- rewind the current file and start over
function rewind(    i)
{
    # shift remaining arguments up. becareful ARGIND starts from 0, not the
    # usual 1, be very careful.
    for (i = ARGC; i > ARGIND; i--)
        ARGV[i] = ARGV[i-1]
    # make sure gawk knows to keep going
    # shouldn't this be called before shifting?
    ARGC++
    # make current file next to get done
    ARGV[ARGIND+1] = FILENAME
    for(i in ARGV)
        print i, ARGV[i]
    # do it
    nextfile
}

