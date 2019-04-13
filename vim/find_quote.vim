inoremap <TAB> <c-r>=GetSomethingBaseOnQuotePos()<cr>

function! GetSomethingBaseOnQuotePos()
  if col('.') - 1 > stridx(getline('.'), '"')
    return "after quote"
  else
    return "before quote"
  endif
endfunction

" test
