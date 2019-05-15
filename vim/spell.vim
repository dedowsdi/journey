"inoremap <C-l> <Esc>[s1z=`]a

" hella world !
" hella world !
" hella world !

inoremap <C-l> <c-g>u<Esc>:call <sid>fixLineSpellError()<cr>`]a<c-g>u

function! s:fixLineSpellError()
  " get current line number
  let lnum = line('.')
  " find last misspelled word before cursor
  normal! [s
  " do nothing if line changed
  if lnum != line('.') | return | endif
  " fix spell error if line doesn't change
  normal! 1z=
endfunction
