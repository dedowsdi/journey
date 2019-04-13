#!/usr/bin/awk -f

FNR == 3 && ! rewound {
    rewound = 1
    rewind()
}

{ print FILENAME, FNR, $0 }
