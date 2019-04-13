#/usr/bin/awk -f
{a=$0 ;
    if(getline)
        printf "%s;%s\n", a, $0
    else
        print a
}
