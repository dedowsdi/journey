" map 1-9 to nth item
" map shift+1-9 to last nth item
let s:shiftKeys = '!@#$%^&*('
for s:i in range(1, 9)
  exe printf('inoremap <expr> %d pumvisible() ? <sid>select_pum(%d - 1) : %d ', s:i, s:i, s:i)
  let skey = s:shiftKeys[s:i-1]
  exe printf('inoremap <expr> %s pumvisible() ? <sid>select_pum(-%d) : "%s" ', skey, s:i, skey)
endfor

" map 0 to original item
inoremap <expr> 0 pumvisible() ? '<c-e>' : 0

function! s:select_pum(index)
  let compInfo = complete_info()
  let idx = a:index >= 0 ? a:index : a:index + len(compInfo.items)
  let d = idx - compInfo.selected
  return repeat( d > 0 ? "\<c-n>" : "\<c-p>", abs(d))
endfunction

finish

[:h complete_info()][1]

[1]:https://vimhelp.org/eval.txt.html#complete_info%28%29
/boot
/tmp123!@#$%%!@#$()*)(*&^^%%&*
