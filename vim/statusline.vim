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

set statusline=
set laststatus=2
set statusline+=%1*\ %f\ %*
set statusline+=%2*\ %<%F%*
set statusline+=%2*%m%*
set statusline+=%=
set statusline+=%1*%=%5l%*
set statusline+=%2*/%L%*
hi User1 guifg=#eea040 guibg=#222222
hi User2 guifg=#dd3333 guibg=#222222
hi StatusLine guifg=#222222
"hi User1 ctermfg=4 ctermbg=0
"hi User2 ctermfg=3 ctermbg=0
