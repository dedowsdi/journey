" vim:set foldmethod=expr noexpandtab:

function! FoldingFunction(lnum)
  return getline(v:lnum)[0]==#"\t"
endfunction
let Func = function('FoldingFunction') "FoldingFunction is name of function
setlocal foldexpr=Func(v:lnum)

finish

	fold
	fold
	fold
	fold
	fold
