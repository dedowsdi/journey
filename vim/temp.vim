" vim:set et ts=4 sw=4 ts=4 tw=80:

function! GetV(lnum, vnum) abort
  return matchstr(getline(a:lnum), '\%' . a:vnum . 'v.')
endfunction

" ov : o for omap, v for v map
" jk : j or k or jk. j for down, k for up.
" visuall block wisely select current column until blank line.
function! SelectScreenColumn(ov, jk) abort
  let [curVnum, curLnum, colLnum0, colLnum1, lnum] =
              \ [virtcol('.')] + repeat([line('.')], 4)
  " do nothing if cursor in blank
  if GetV(curLnum, curVnum) =~# '\v\s'
    if a:ov ==# 'v' | exec 'normal! ' | endif | return
  endif

  " get column end
  if stridx(a:jk, 'j') != -1
    while 1
      let lnum = lnum + 1
      if lnum > line('$') || GetV(lnum, curVnum) =~# '\v^$|\s'
        let colLnum0 = lnum - 1 | break
      endif
    endwhile
  endif

  " get column start
  if stridx(a:jk, 'k') != -1
    let lnum = curLnum
    while 1
      let lnum = lnum - 1
      if lnum <= 0 || GetV(lnum, curVnum) =~# '\v^$|\s'
        let colLnum1 = lnum + 1 | break
      endif
    endwhile
  endif

  " visual select
  call cursor(colLnum0, col('.'))
  exec 'normal! '
  call cursor(colLnum1, col('.'))
endfunction

vnoremap ic :<c-u>call SelectScreenColumn('v', 'j')<cr>
onoremap ic :call SelectScreenColumn('o', 'j')<cr>

"a
"a
"a
"a
"	aa
"	bb
"	cc
"	dd
"a
"a
"a
"a
