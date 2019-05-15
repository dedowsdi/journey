
" vertical E,W,B.
function! s:vertical_motion(motion)
  " changed from https://vi.stackexchange.com/questions/15151/move-to-the-first-last-non-whitespace-character-of-the-column-vertical-w-b-e
  let curcol = virtcol('.')
  let nextcol = curcol + 1
  "let pattern = printf('\v%%%dv\S.*(.{%%d}\s|.{,%%d}\n|%$)', curcol, prevcol, prevcol)

  if a:motion ==# 'E'
    let pattern = printf('\v%%%dv\S.*(\n.*%%<%dv$|\n.*%%%dv\s|%%$)', curcol, nextcol, curcol)
    let flag = 'W'
  elseif a:motion == 'B'
    let pattern = printf('\v^.*(%%<%dv$|%%%dv\s.*)\n.*\zs%%%dv\S', nextcol, curcol, curcol)
    let flag = 'bW'
  elseif a:motion == 'W'
    let pattern = printf('\v^.*(%%<%dv$|%%%dv\s.*)\n.*\zs%%%dv\S', nextcol, curcol, curcol)
    let flag = 'W'
  else
    echohl ErrorMsg | echo 'Not a valid motion: ' . a:motion | echohl None
  endif
  call search(pattern, flag)
endfunction

vnoremap ce :<c-u>exec 'norm! gv' <bar> call <sid>vertical_motion('E')<cr>
vnoremap cw :<c-u>exec 'norm! gv' <bar> call <sid>vertical_motion('W')<cr>
vnoremap cb :<c-u>exec 'norm! gv' <bar> call <sid>vertical_motion('B')<cr>

finish
