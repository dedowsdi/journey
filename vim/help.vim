function! s:getHelpFile(tag)
  exec ':h ' . a:tag
  let fname = expand('%:t') | q | return fname
endfunction

function! s:createVimhelpLink(tag)
  let anchor = substitute(a:tag, '[^a-zA-Z_\- \t]',
        \ '\=printf("%%%2X", char2nr(submatch(0)))', 'g')
  return 'https://vimhelp.org/' . s:getHelpFile(a:tag) . '.html#' . anchor
endfunction

function! s:createNvimhelpLink(tag)
  return 'https://neovim.io/doc/user/' .
        \ fnamemodify(s:getHelpFile(a:tag), ':r') . '.html#' . a:tag
endfunction

"let @+ =  s:createVimhelpLink(':let-@')
"echo @+
let @+ =  s:createNvimhelpLink(':let-@')
echo @+

command! -nargs=+ LinkVimHelp let @" = s:createVimhelpLink(<q-args>)
command! -nargs=+ LinkNvimHelp let @" = s:createNvimhelpLink(<q-args>)

finish
