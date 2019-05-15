function! A()
  echom expand('<sfile>')
  call B()
endfunction

function! B()
  echom expand('<sfile>')
  call C()
endfunction

function! C()
  echom expand('<sfile>')
endfunction

call A()
