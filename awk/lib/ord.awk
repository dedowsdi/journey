#!/usr/bin/awk -i

# ord.awk --- do ord and chr
# Global identifiers:

# _ord_: numerical values indexed by characters
# _ord_init: function to initialize _ord_
BEGIN { _ord_init() }

function _ord_init(   low, high, i, t)
{
    low = sprintf("%c", 7) # BEL is ascii 7
    if (low == "\a") {
        # regular ascii
        low = 0
        high = 127
    } else if (sprintf("%c", 128 + 7) == "\a") {
        # ascii, mark parity
        low = 128
        high = 255
    } else {
        # ebcdic(!)
        low = 0
        high = 255
    }

    for (i = low; i <= high; i++) {
        t = sprintf("%c", i)
        _ord_[t] = i
    }
}
