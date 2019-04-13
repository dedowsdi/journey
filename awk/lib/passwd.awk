
BEGIN {
    # tailor this to suit your system
    _pw_awklib = "/home/pntandcnt/journey/awk/lib/"
}

function _pw_init(      oldfs, oldrs, olddol0, pwcat, using_fw, using_fpat)
{
    if (_pw_inited)
        return

    # save old setting
    oldfs = FS
    oldrs = RS
    olddol0 = $0
    using_fw = (PROCINFO["FS"] == "FIELDWIDTHS")
    using_fpat = (PROCINFO["FS"] == "FPAT")

    FS = ":"
    RS = "\n"
    pwcat = _pw_awklib "pwcat"
    while ((pwcat | getline) > 0) {
        _pw_byname[$1] = $0
        _pw_byuid[$3] = $0
        _pw_bycount[++_pw_total] = $0
    }
    close(pwcat)
    _pw_count = 0
    _pw_inited = 1

    # restore oldsetting
    FS = oldfs
    if (using_fw)
        FIELDWIDTHS = FIELDWIDTHS # is it necessary?
    else if (using_fpat)
        FPAT = FPAT # is it necessary?
    RS = oldrs
    $0 = olddol0
}

function getpwnam(name)
{
    _pw_init()
    return _pw_byname[name]
}
function getpwuid(uid)
{
    _pw_init()
    return _pw_byuid[uid]
}
function getpwent()
{
    _pw_init()
    if (_pw_count < _pw_total)
        return _pw_bycount[++_pw_count]
    return ""
}
function endpwent()
{
    _pw_count = 0
}
class name
{
    }
