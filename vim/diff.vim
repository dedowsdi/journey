function! s:diffSelf() abort
  " create tmp file in vertice split
  exec 'vsplit ' . tempname().'_'.expand('%:t')
  " read original file into tmp file, remove blank 1st line, save it.
  read # | 1d |  w
  " make split window part of diff windows
  diffthis
  " go back to original window
  wincmd p
  " make current window part of diff windows
  diffthis
endfunction

command DiffSelf :call <sid>diffSelf()
