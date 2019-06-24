command -nargs=? List call s:list(expand('<sfile>'), expand('<slnum>'), <f-args>)

function! s:list(sfile, slnum, ...)
  let hsize = a:0 > 0 ? (a:1 - 1) / 2.0 : 5
  if hsize < 0 | let hsize = 0 | endif

  if a:sfile =~# '\v^function.*'

    " note that function head and end is also returned from :function. slnum
    " starts from 1 after function head
    let lidx = a:slnum
    let funcName = split(a:sfile[9:], '\v\.\.')[-1]

    " take care of numbered function
    if funcName =~# '\v^\d+$'
      let funcName = printf('{%d}', funcName)
    endif

    let lines = split(execute('function ' . funcName), "\n")
    call map(lines, {idx, val -> printf('%s%',
          \ (idx == lidx ? '* ' : '  ')) . val})
  else
    let lidx = a:slnum - 1
    let lines = readfile(a:sfile)
    let digits = len(len(lines) + '')
    call map(lines, {idx, val -> printf('%s%*d : ',
          \ (idx == lidx ? '* ' : '  '), digits, idx+1) . val})
  endif

  " display one more line after current line if slnum is even
  let start_line = max([lidx - float2nr(floor(hsize)), 0])
  let end_line = min([lidx + float2nr(round(hsize)), len(lines) - 1])

  while start_line <= end_line
    echo lines[start_line]
    let start_line += 1
  endwhile
endfunction

function! Test(i)
  echo 1
  echo 2
  echo 3
  echo 4
  echo 5
  call Test1(a:i)
  echo 6
  echo 7
  echo 8
endfunction

let s:dic = {}
function s:dic.test()
  echo 1
  echo 2
  echo 3
  echo 4
endfunction

function! Test1(j)
  echo 1
  echo 2
  call s:test()
  echo 3
endfunction

function! s:test()
  echo 1
  echo 2
  call s:dic.test()
  echo 3
endfunction

echo 2
echo 3
echo 4
echo 5
echo 6
echo 7
echo 8
echo 9
echo 10
echo 11
echo 12
