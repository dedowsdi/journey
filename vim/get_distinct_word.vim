" [
" [(isPhysicallySettledFxFwd, NO,"Y"),(isPhysicallySettledFxFwd,isPhysicallySettledFxSwap,"N")],
" [(isPhysicallySettledFxSwap,NO,"Y"),(isPhysicallySettledFxSwap, isPhysicallySettledCommodity,"Y")],
" [(isPhysicallySettledCommodity,NO,"Y"),(isPhysicallySettledCommodity,YES,"Y")]
" ]

function! GetWords(lnum0, lnum1)
  " there has no set in viml, use dict as set.
  let words = {}
  " loop every line
  for lnum in range(a:lnum0, a:lnum1)
    " replace non word character to space, split by space to get words
    let newWords = split(substitute(getline(lnum), '\v\W+', ' ', 'g'))
    for word in newWords
      " add word to set
      let words[word] = 1
    endfor
  endfor
  " return all keys as list
  return keys(words)
endfunction
echo GetWords(1, 5)
