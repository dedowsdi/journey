if 'linux' ==# $TERM
  let s:term = 'linux'
else
  if has('unix')
    call system('grep -q "Microsoft" /proc/version')
    let s:term = v:shell_error == 0 ?'wsl_xterm' :'unix_xterm'
    let desktopFile = system('printenv GIO_LAUNCHED_DESKTOP_FILE')
    if desktopFile =~# 'hyper'
      let s:emulator = 'hyper'
    endif
  else
    let s:term = 'unknown'
  endif
endif
