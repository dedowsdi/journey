# ftrans.awk --- handle datafile transitions
#
# user supplies beginfile() and endfile() functions

# if you supply the same file twice to transfile.awk, it won't work, this rule
# fix that problem.

FNR == 1 {
    if (_filename_ != "")
        endfile(_filename_)
    _filename_ = FILENAME
    beginfile(FILENAME)
}
END { endfile(_filename_) }
