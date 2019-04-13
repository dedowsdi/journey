" objs  : any valid text object without i or a, eg    '{[(<'
" ai    : 'a' or 'i'
" force : start visual mode even if no valid text object found
"
" visually select minimal textobject.
function! SelectMinTextObject(objs, ai, force)
  " init data
  let [minObj, minObjStart, minObjEnd] = ['', [], []]

  " split objs into list, loop it
  for obj in split(a:objs, '\zs')

    " get textobject range, continue if it's blank
    let [startPos, endPos] = GetTextObjectPos(a:ai . obj)
    if startPos == [] | continue | endif

    " updat data if it's empty or current text object is smaller
    if minObj ==# '' || minObjStart[1] < startPos[1] ||
          \ minObjStart[1] == startPos[1] && minObjStart[2] < startPos[2]
      let [minObj, minObjStart, minObjEnd] = [obj, startPos, endPos]
    endif

  endfor

  " return if no valid obj found
  if minObj ==# ''
    if a:force
      normal! v
    endif
    return 0
  endif

  " visually select textobject.
  execute 'normal! v'. a:ai . minObj

  return 1
endfunction

" get text object range as [startPos, endPos], pos format is the same as
" getpos(), except it returns [[],[]] if text object is blank
function! GetTextObjectPos(obj)
  try
    " record cursor, @a, '[, ']
    let [ cursorPos, regText, regType, yankPos0, yankPos1 ] =
          \ [ getcurpos(), getreg('"'), getregtype('"'),
          \ getpos("'["), getpos("']") ]
    " clear register. it's necessary because "ay.. won't fill a if .. is blank
    let @a = ''
    " copy text object into @a if it's not blank
    exec 'normal! "ay' . a:obj
    " return blank if nothing copied, otherwise return start and end of copied
    " area
    return @a ==# '' ? [[],[]] : [getpos("'["), getpos("']")]
  finally
    " restore cursor, @a, '[, ']
    call setpos('.', cursorPos) | call setreg('a', regText, regType)
    call setpos("'[", yankPos0) | call setpos("']", yankPos1)
  endtry
endfunction

" you can't implement omap with vmap, because miminal v area is 1 character,
" miminal o area is 0 character
vnoremap ip <esc>:call SelectMinTextObject('({[<', 'i', 1)<cr>
vnoremap ap <esc>:call SelectMinTextObject('({[<', 'a', 1)<cr>
onoremap ip :call SelectMinTextObject('({[<', 'i', 0)<cr>
onoremap ap :call SelectMinTextObject('({[<', 'a', 0)<cr>

" dd xsd  sdl(sddi{dss[ asd]aa }e9<888>n 0:)"wdwww
