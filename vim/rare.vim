function! G_google(...)
  if len(a:000) == 0|return|endif
  let searchItems = shellescape(join(a:000, '+'))
  let cmd = 'google-chrome https://www.google.com/\#q=' . searchItems . '>/dev/null'
  if has('nvim')
    call jobstart(cmd)
  else
    silent! execute '!' . cmd
  endif
endfunction

function! G_smartSplit()
  let direction = str2float(winwidth(0))/winheight(0) >= 204.0/59 ? 'vsplit':'split'
  exec 'rightbelow ' . direction
endfunction

function! s:set_clipboard(str)
  let s = myvim#literalize(a:str, 2)
  call system(printf("echo '%s' | DISPLAY=:0 xsel -i", s))
endfunction
if $TERM ==# 'linux'
  vnoremap \y y:call <SID>set_clipboard(@")<cr>
endif

" ------------------------------------------------------------------------------
" chrono 
" ------------------------------------------------------------------------------
let myvim#chrono = { 'time':reltime() }

function! myvim#chrono.reset() abort dict 
  let self.time = reltime()
endfunction

function! myvim#chrono.get() abort dict
  return reltimestr(reltime(self.time))
endfunction

function! myvim#newChrono() abort
  return deepcopy(g:myvim#chrono)
endfunction
