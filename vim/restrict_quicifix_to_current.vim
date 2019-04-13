function! FilterQflistByPath(path, exclude)
  let expr = a:exclude ? '!~#' : '=~#'
  let apath = fnamemodify(a:path, ":p")
  call setqflist(filter(getqflist(),
        \ printf('fnamemodify(bufname(v:val.bufnr),":p") %s "\\V\\^%s"', expr, apath)))
endfunction

command! -nargs=1 -bang -complete=file Fqf :call FilterQflistByPath(<q-args>, <bang>0)
