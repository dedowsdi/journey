" auto include 1st osg::balabala, except ost::ref_ptr of current line
function! osg#include()
  let str = getline('.')
  let class = matchstr(str, '\vosg\w*::(ref_ptr)@!\w*')
  if class != ""
    let head = substitute(class, '::', '/', '')
    exec 'normal \cI<'.head.'>'
    normal! ``
  endif
endfunction
