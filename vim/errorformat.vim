set errorformat&
let l = split(&errorformat, '\v(\\)@<!,')

let lines=[
      \ "CMakeFiles/Makefile2|292| recipe for target 'demo/CMakeFiles/teapot.dir/all' failed",
      \ "|| make[2]: *** [demo/CMakeFiles/teapot.dir/all] Error 2",
      \ ]

let idx = 0
for ef in l
  let &errorformat = ef
  " weird, "efm" has no effect ?
  let res = getqflist({"lines":lines, "efm":"xxxxxxxxxx"})
  if res != {}
    echo idx ":" ef
  endif
  let idx += 1
endfor

finish

%*[^"]"%f"%*\D%l: %m
"%f"%*\D%l: %m
%-G%f:%l: (Each undeclared identifier is reported only once
%-G%f:%l: for each function it appears in.)
%-GIn file included from %f:%l:%c:
%-GIn file included from %f:%l:%c\,
%-GIn file included from %f:%l:%c
%-GIn file included from %f:%l
%-G%*[ ]from %f:%l:%c
%-G%*[ ]from %f:%l:
%-G%*[ ]from %f:%l\,
%-G%*[ ]from %f:%l
%f:%l:%c:%m
%f(%l):%m
%f:%l:%m
"%f"\, line %l%*\D%c%*[^ ] %m
%D%*\a[%*\d]: Entering directory %*[`']%f'
%X%*\a[%*\d]: Leaving directory %*[`']%f'
%D%*\a: Entering directory %*[`']%f'
%X%*\a: Leaving directory %*[`']%f'
%DMaking %*\a in %f
%f|%l| %m
