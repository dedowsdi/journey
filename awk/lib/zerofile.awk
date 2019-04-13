# zerofile.awk --- library file to process empty input files
BEGIN { Argind = 0 }

# blank file will trigger only BEGIN and END, it has no chance to update Argind
ARGIND > Argind + 1 {
    for (Argind++; Argind < ARGIND; Argind++)
        # call user providered zerofile function
        zerofile(ARGV[Argind], Argind)
}

# update Argind to ARGIND
ARGIND != Argind { Argind = ARGIND }

# if trailing files are blank, you need to handle it in END
END {
    if (ARGIND > Argind)
        for (Argind++; Argind <= ARGIND; Argind++)
        zerofile(ARGV[Argind], Argind)
}
