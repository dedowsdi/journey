" direct complete approach
inoremap <F5> <C-R>=CompleteMonths()<CR>

function! GetTypingWordBase()
  let curCol = col('.')
  if curCol == 1
    return ''
  endif

  try
    " move back cursor to last byte of typing word
    call cursor(line('.'), curCol - 1)
    return expand('<cword>')
  finally 
    " restore cursor
    call cursor(line('.'), curCol)
  endtry
endfunction

func! CompleteMonths()
  let base = GetTypingWordBase()
  let monthes = ['January', 'February', 'March',
  \ 'April', 'May', 'June', 'July', 'August', 'September',
  \ 'October', 'November', 'December']
  " -len(base) to delete base
  call complete(col('.') - len(base), filter(monthes, 'stridx(v:val, base) == 0'))
  return ''
endfunc

" completefunc approach
func! CompleteMonthsFunc(findstart, base)
  let monthes = ['January', 'February', 'March',
  \ 'April', 'May', 'June', 'July', 'August', 'September',
  \ 'October', 'November', 'December']

  if a:findstart == 1 
    " first invocation, return completion starts
    let wb = GetTypingWordBase()
    let l = filter(monthes, 'stridx(v:val, wb) == 0')
    if len(l) == 0
      " no candidates, return -1 error , or -2 staty in completion mode, -3
      " cancel and leave completion mode
      return -3
    endif
    
    " becareful here, it's byte index, not column?
    return col('.') - len(wb) - 1
  endif

  " second invocation, return completion
  if a:findstart == 0
    return { 'words' : filter(monthes, 'stridx(v:val, a:base) == 0'), 'refresh' : 'always' }
  endif

endfunc

let &completefunc='CompleteMonthsFunc'
