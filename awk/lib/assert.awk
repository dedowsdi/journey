#/usr/bin/awk -i

function assert(condition, string)
{
    if (! condition) {
        printf("%s:%d: assertion failed: %s\n",
               FILENAME, FNR, string) > "/dev/stderr"
        _assert_exit = 1
        exit 1
    }
}

# END implies read the whole file, unless you exit in BEGIN.
# this file must be included first, so END of this file  will be executed before
# other END.
END {
    if (_assert_exit)
        exit 1
}
