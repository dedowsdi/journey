" map command recognize <keycode> to as special keys, no matter it's quoted or \
" escaped, note that this happens only on the :map command it self,  which means
" you can't return '<Esc>' from a function and hope vim to recognize it as
" sepcial keys.
"
" without <expr>, {rhs} is a sequence of keystrokes, with <expr>, rhs is an
" expression, it's evaluated to get a sequence of keystrokes.
"
" vim can map only keycode or terminal sequence starts with <Esc>. Both of
" them must be a full match, eg:
"     <f3>k
 map ORk :echo "<f3>k"<cr>
" if you press f3, terminal send key OR to vim, it doesn't match ORk, it
" will be break down to ^[, O and R (a new line starts with R will be created) .
" if map O exists, OR will be break down to O and R
"
" one weird thing about function key is set <f3> is totally different from
" <ctrl-v><f3>. Don't know how vim know <f3> is pressed.
"
" all the same
inoremap <down> 
inoremap <down> <Esc>
" note that "\<lt>Esc>" will be changed to "\<Esc>", which is 
inoremap <up> <c-r>="\<lt>Esc>"<cr>
inoremap <expr> <left> '<Esc>'
" noth that you don't need to add :call before, and<cr> after RetrunEsc()
inoremap <expr> <right> ReturnEsc()

function! ReturnEsc()
  return "\<Esc>"
endfunction

nnoremap <expr> A 1 ? '' : 'I'

finish
