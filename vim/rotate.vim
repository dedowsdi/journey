


" A line with more than k characters.
" A longer line with more than k character.
" A short line.
" Rotate here: ------v--
" This is long enough.
"
" This is not enough.
" Wrapping around to the first line.

function! Rotate0(start_line, end_line, col)
  call cursor(a:start_line, a:col)
  let @"=''
  while 1
    if(col('.') == a:col)
      if @" == ''
        norm! mmx
      else
        norm! vp
      endif
    endif
    if (line('.') >= a:end_line)
      break
    endif
    norm! j
  endwhile
  norm! `mP
endfunction

function! RotateColumn(s, e, c)
  call cursor(a:s, 1)
  if search(printf('\v%%>%dl%%<%dl%%%dc', a:s-1, a:e+1, a:c), 'b')
    norm! yl
    exec printf('%d,%dg/\v%%%dc/ norm! %d|vp', a:s, a:e, a:c, a:c)
  endif
endfunction
