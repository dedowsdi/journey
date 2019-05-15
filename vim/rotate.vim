


" A line with more than k characters.
" A longer line with more than k character.
" A short line.
" Rotate here: ------v--
" This is long enough.
"
" This is not enough.
" Wrapping around to the first line.

function! Rotate0(start_line, end_line, col)
  callncursor(a:start_line, a:col)
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

" s : start line, e : end line, c : col
function! RotateColumn(s, e, c)
  " place cursor at line a:s
  call cursor(a:s, 1)
  " search backward to find last line that's longer enough
  if search(printf('\v%%>%dl%%<%dl%%%dc', a:s-1, a:e+1, a:c), 'b')
    " copy current character into @"
    norm! yl
    " on each line, if character exists in screen column a:c, swap it with @"
    exec printf('%d,%dg/\v%%%dc/ norm! %d|vp', a:s, a:e, a:c, a:c)
  endif
endfunction

" s : start line, e : end line, c : col
function! RotateColumn2(s, e, c)
  " set @" to any letter
  let @"='x'
  " copy all lines to below a:e
  exec a:s.','.a:e.'t'.a:e
  " for each line, if character exists in screen column a:c, swap it with @"
  exec printf('%d,%dg/\v%%%dc/ norm! %d|vp', a:s, a:e*2-a:s+1, a:c, a:c)
  " remove original lines
  exec a:s.','.a:e.'d'
endfunction
