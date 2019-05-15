function s:exec_on_term(lnum1, lnum2)
  " get terminal buffer
  let g:terminal_buffer = get(g:, 'terminal_buffer', -1)
  " open new terminal if it doesn't exist
  if g:terminal_buffer == -1 || !bufexists(g:terminal_buffer)
    terminal
    let g:terminal_buffer = bufnr('')
    wincmd p
  " split a new window if terminal buffer hidden
  elseif bufwinnr(g:terminal_buffer) == -1
    exec 'sbuffer ' . g:terminal_buffer
    wincmd p
  endif
  " join lines with "\<cr>", note the extra "\<cr>" for last line
  " send joined lines to terminal.
  call term_sendkeys(g:terminal_buffer,
        \ join(getline(a:lnum1, a:lnum2), "\<cr>") . "\<cr>")
endfunction

command! -range ExecOnTerm call s:exec_on_term(<line1>, <line2>)
nnoremap <leader>ex :ExecOnTerm<cr>
vnoremap <leader>ex :ExecOnTerm<cr>

finish

echo abc
echo def
echo aaaa

