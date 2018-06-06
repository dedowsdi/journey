" auto include 1st osg::balabala, except ost::ref_ptr of current line
function! osg#include()
  let str = getline('.')
  let class = matchstr(str, '\vosg\w*::(ref_ptr)@!\w*')
  if class != ""
    let head = substitute(class, '::', '/', '')
    let pattern = '\V' . substitute(head, '/', '\/', 'g')
    if !search(pattern, 'n')
      exec 'normal \cI<'.head.'>'
      normal! ``
    endif
  endif
endfunction
