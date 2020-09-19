" turn new ..(..) to make_oref<..>(..), only works if new is at or before cursor
" in current line.
function ufo#new_to_make() abort

  if search('\v<new>', 'bc', line('.'))
    exe "norm! dwimake_oref<\<esc>f(i>"
  endif
  
endfunction
