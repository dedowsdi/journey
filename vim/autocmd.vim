augroup au_test
  au!
  " this one is which you're most likely to use?
  autocmd BufReadPre * echom 'BufReadPre ' . expand('<afile>')
  autocmd BufRead * echom 'BufRead ' . expand('<afile>')
  autocmd BufReadPost * echom 'BufReadPost ' . expand('<afile>')
  autocmd BufEnter * echom 'BufEnter ' . expand('<afile>')
  autocmd BufWinEnter * echom 'BufWinEnter ' . expand('<afile>')
  autocmd FileType * echom 'FileType ' . expand('<afile>') . ' : ' . expand('<amatch>')
augroup end
finish

"0
"1
"2
"3
"4
"5
