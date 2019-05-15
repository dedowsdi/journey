for c in split('kbdfule', '\zs')
  exec printf('set <a-%s>=%s', c, c)
endfor

function! s:setPos0()
  let s:pos0 = getcmdpos() | return ''
endfunction
function! s:checkPos0()
  if !exists('s:pos0') | throw 'missing s:pos0, forget to call s:getPos0()?' | endif
endfunction
function! s:clearPos0()
  unlet s:pos0
endfunction

function! s:cmdlineAltUL(ul)
  call s:checkPos0()
  let [pos1, cmdline ] = [getcmdpos(), getcmdline()]
  let ToCase = a:ul == 'u' ? function('toupper') : function('tolower')
  let cmdline = cmdline[0 : s:pos0-2] . ToCase(cmdline[s:pos0 - 1 : pos1-1]) . cmdline[pos1:]
  call s:clearPos0() | return cmdline
endfunction

function! s:cmdlineAltD()
  call s:checkPos0()
  let [pos1, cmdline ] = [getcmdpos(), getcmdline()]
  let cmdline = cmdline[0 : s:pos0-2] . cmdline[pos1-1:]
  call setcmdpos(s:pos0)
  call s:clearPos0() | return cmdline
endfunction

cnoremap <a-k> <c-\>e(getcmdpos() == 1 ? '' : getcmdline()[0:getcmdpos()-2])<cr>
cnoremap <a-u> <c-r>=<sid>setPos0()<cr><c-right><c-\>e<sid>cmdlineAltUL('u')<cr>
cnoremap <a-l> <c-r>=<sid>setPos0()<cr><c-right><c-\>e<sid>cmdlineAltUL('l')<cr>
cnoremap <a-d> <c-r>=<sid>setPos0()<cr><c-right><c-\>e<sid>cmdlineAltD()<cr>
cmap <a-b> <c-left>
cmap <a-f> <c-right>
