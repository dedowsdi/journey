let s:data = []

function! StartCopy(reset)
  if a:reset | let s:data = [] | endif
  echohl Title | echo 'start copy...' | echohl None
  augroup BASKET_COPY_PASTE
    au!
    autocmd TextYankPost * call Copy()
  augroup END
endfunction

function! StartPaste()
  augroup BASKET_COPY_PASTE
    au!
  augroup END
  call Paste()
endfunction

function! Copy()
  let entry = {'text':@", 'mode':getregtype('"')}
  if s:data == [] || s:data[-1] != entry
    call add(s:data, entry)
  endif
endfunction

function! Paste()
  if s:data == [] | return | endif
  let prompt = "range n action . range can be *|a-b|-b|a-|a|a,b  everything can bo omited, action is normal command \n\n"
  let i = 0
  for entry in s:data
    let i += 1
    let idx = stridx(entry.text, "\n")
    let prompt .= printf("%d : %s : %s\n", i,
          \ substitute(entry.mode, '', '^V', ''),
          \ idx != -1 ? entry.text[0:stridx(entry.text, "\n")-1] . '...' : entry.text)
  endfor
  let inputStr = input(prompt)
  if inputStr ==# '' | return | endif

  let matches =  matchlist(inputStr, '\v(\A*)(n)?(.*)')
  let [selStr, action, cmds] = [matches[1], matches[2], matches[3]]
  let indices = GetSelection(selStr)
  call Print(indices, cmds, 1)
endfunction

" * or blank : all
" a-b        : range(a, b)
" a,b        : a and b
" a-         : a until last
" -b         : 1 until b
function! GetSelection(str)
  if a:str ==# '' || a:str ==# '*'
    return range(len(s:data))
  else
    let indices = []
    for item in split(a:str, ',')
      if item =~# '-'
        let [i0, i1] = split(item, '-', 1)
        if i0 ==# '' | let i0 = 1 | endif
        if i1 ==# '' | let i1 = len(s:data) | endif
        let indices += range(i0-1, i1-1)
      else
        let indices += [item - 1]
      endif
    endfor
    call filter(indices, 'v:val < len(s:data)') | return indices
  endif
endfunction

function! Print(indices, cmds, noTailCmd)
  let i = 0
  let [regText, regType] = [@a, getregtype('a')]
  for index in a:indices
    let i += 1
    let [entry, cpos] = [s:data[index], getcurpos()]
    call setreg('a', entry.text, entry.mode) | normal! "agp
    if entry.mode =~# '' | call cursor(cpos[1], col('.')) | endif
    if empty(a:cmds) || (a:noTailCmd && i == len(a:indices)) | continue | endif
    exec 'norm! ' a:cmds
  endfor
  call setreg('a', regText, regType)
endfunction

"finish
" ------------------------------------------------------------------------------
" test
" ------------------------------------------------------------------------------

nnoremap <leader>sc :call StartCopy(1)<cr>
nnoremap <leader>sC :call StartCopy(0)<cr>
nnoremap <leader>sp :call StartPaste()<cr>

let s:data = [{'mode':'v', 'text':'a'}, {'mode':'v', 'text':'b'}, {'mode':'v', 'text':'c'}]
let v:errors = []
call assert_equal([0,  1,  2], GetSelection('*'))
call assert_equal([0,  1,  2], GetSelection(''))
call assert_equal([0,  1,  2], GetSelection('1-'))
call assert_equal([0,  1,  2], GetSelection('1-3'))
call assert_equal([0,  1,  2], GetSelection('1,2,3'))
call assert_equal([0,  1,  2], GetSelection('1-1,2-3'))
call assert_equal([0,  1,  2], GetSelection('-3'))
call assert_equal([0],         GetSelection('1'))
call assert_equal([2],         GetSelection('3'))
call assert_equal([1],         GetSelection('2-2'))
call assert_equal([0,  1],     GetSelection('1-2'))
call assert_equal([0,  1,  2], GetSelection('1-9'))
for err in v:errors | echom err | endfor
call Paste()
