let &statusline='hello statusline'
" display current buffer only
let &statusline='%t'
" display all buffers
"
let &statusline='%!GetAllBufferTails()'

function! GetAllBufferTails()
  let buffer_numbers = map(filter(getbufinfo(), 'v:val.listed'), 'v:val.bufnr')
  let buffer_names = map(buffer_numbers, 'fnamemodify(bufname(v:val), ":t")')
  return join(buffer_names, ' | ')
endfunction
