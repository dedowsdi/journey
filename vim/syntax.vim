" you cann't use lookadd and lookbehind in syn-region pattern ?

" desired columns and minimal columns
let [b:logCols, b:minCols, b:match_id] = [80, 10, -1]

function! UpdateConceal()
  " remove old match
  if b:match_id != -1 | call matchdelete(b:match_id) | endif
  let b:logCols = max([b:logCols, b:minCols])
  echo 'log columns : ' . b:logCols
  let b:match_id =  matchadd('Conceal', printf('\v^.+\ze.{%d}$', b:logCols), 10, -1)
endfunction

call UpdateConceal()
nnoremap <silent> <buffer> <c-right> :let b:logCols += 1 \| call UpdateConceal()<cr>
nnoremap <silent> <buffer> <c-left> :let b:logCols -= 1 \| call UpdateConceal()<cr>
