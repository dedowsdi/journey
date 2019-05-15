nnoremap cn :set opfunc=SearchAndChange<CR>g@
vnoremap cn :<c-u>call SearchAndChange(visualmode(), 1)<CR>
nnoremap dn :set opfunc=SearchAndDelete<CR>g@
vnoremap dn :<c-u>call SearchAndDelete(visualmode(), 1)<CR>
nnoremap <leader><leader> :set opfunc=SetSearchPattern<CR>g@
vnoremap <leader><leader> :<c-u>call SetSearchPattern(visualmode(), 1)<cr>

function! GetVisualString() abort
  return GetMarkString("'<", "'>", visualmode())
endfunction

function! GetMarkString(m0, m1, vmode)
  return GetPosString(getpos(a:m0), getpos(a:m1), a:vmode)
endfunction

function! GetPosString(p0, p1, vmode)
  let [lnum1, col1] = a:p0[1:2]
  let [lnum2, col2] = a:p1[1:2]
  let lines = getline(lnum1, lnum2)
  if a:vmode ==# 'v'
    let lines = map(lines, 'v:val[col1-1 : col2-1]')
  elseif a:vmode ==# 'V'
    let lines[-1] .= "\n"
  else
    let lines[-1] = lines[-1][:col2 - 1]
    let lines[0] = lines[0][col1 - 1:]
  endif
  return join(lines, "\n")
endfunction

function! Operand(type, visual)
  if a:visual
    return [GetVisualString(), visualmode()]
  else
    let vmode = a:type ==# 'line' ? 'V' : a:type ==# 'char' ? 'v' : "\<ctrl-v>"
    return [GetMarkedString("'[", "']", vmode), vmode]
  endif
endfunction

function! SearchLiteral(type, ...)
  let @/ = Operand(a:type, a:0 > 0)[0]
  let @/ = LiteralizeVim(@/)
endfunction

function! SearchAndChange(type, ...)
  call ExecuteOperator(a:type, a:0 > 0, '\\')
  call feedkeys('cgn')
endfunction

function! SearchAndDelete(type, ...)
  call ExecuteOperator(a:type, a:0 > 0, '\\')
  call feedkeys('dgn')
endfunction

function! ExecuteOperator(type, visual, operator)
  if a:visual | exec 'norm gv'.a:operator | return | endif
  " backup last visual
  let [m0, m1, lvm] = [getpos("'<"), getpos("'>"), visualmode()]
  let vcmd = a:type ==# 'line' ? "'[V']" :  "`[v`]"
  exec 'norm '.vcmd.a:operator
  " restore last visual and cursor after that
  let pos = getcurpos()
  call setpos("'<", m0) | call setpos("'>", m1) | exec "norm! `<".lvm."`>\<esc>"
  call setpos('.', pos)
endfunction

finish
" 12345 aaa aaa
" 12345 aaa aaa
" 12345 aaa aaa
" 12345 aaa aaa
" 12345 aaa aaa
" 12345 aaa aaa

function! OperatorFunc(type,...)
  " how to call another operator with current motion?
endfunction
