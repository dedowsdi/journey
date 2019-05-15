function! ToggleShell()
  let gs = get(g:, 'global_shell', -1)
  if gs == -1 || !bufexists(gs)
    " split and create terminal if it doesn't exist
    botright split | terminal
    let g:global_shell = bufnr('')
  else
    if bufwinid(gs) != -1
      " close terminal if it's already open
      call win_gotoid(bufwinid(gs)) | q
    else
      " split, change to terminal buffer
      botright split | exec gs . 'buffer'
    endif
  endif
endfunction

nnoremap <a-t> :call ToggleShell()<cr>
tnoremap <a-t> <c-\><c-n>:call ToggleShell()<cr>
command! Shell call ToggleShell()
