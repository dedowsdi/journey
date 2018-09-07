"debugger, lldb or gdb
"build dir, such as ./build/Debug, ./build/RelWithDebInfo
let g:mycppBuildDir = './build/gcc/Debug'
set rtp +=./.vim,./.vim/after
"executable make target and path
"let g:mycppMakes = {}
"class name prefix
"let g:mycppClassPrefix='_PacExport'
"files included into newly created head file
"let g:mycppPreRequisite=['pacConsolePreRequisite.h']
"files included into newly created src file
"let g:mycppStable=['pacStable.h']

"call mycpp#addDependencies('dependencies')
"
function! CamelToUnderScore(name)
  let s = substitute(a:name, '\v\C^[A-Z]', '\l\0', '')
  return substitute(s, '\v\C[A-Z]', '_\l\0', 'g')
endfunction

function! ShrinkMatrixName(name)
  let s = substitute(a:name, 'model_view_proj', 'mvp', '')
  let s = substitute(s, 'model_view', 'mv', '')
  let s = substitute(s, 'view_proj', 'vp', '')
  let s = substitute(s, 'inverse_transpose', 'it', '')
  let s = substitute(s, 'inverse', 'i', '')
  let s = substitute(s, 'transpose', 't', '')
  let s = substitute(s, 'view', 'v', '')
  let s = substitute(s, 'model', 'm', '')
  let s = substitute(s, 'proj', 'p', '')
  let s = substitute(s, 'matrix', 'mat', '')
  return s
endfunction

function! ShrinkShaderMatrixName(name)
  let s = CamelToUnderScore(a:name)
  let s = ShrinkMatrixName(s)
  return s
endfunction

" convert old style demo to new one
function! OpToOo()
  " get display mode and window size
  normal! gg
  if !search('glutInitDisplayMode')
    echoe 'failed to get display mode'
    return
  endif
  normal! f(ldi)
  let display_mode = @"

  if !search('glutInitWindowSize')
    echoe 'failed to get window mode'
    return
  endif
  normal! f(ldi)
  let comma_index = stridx(@", ',')
  let wnd_width = @"[0 : comma_index - 1]
  let wnd_height = @"[comma_index + 1 : ]

  " change include, reserve comment block
  normal! gg
  if !search('\v#include\s+')
    echoe 'failed to search first include'
    return
  endif
  let include_start = line('.')

  normal! G
  if !search('\v^#include\s*', 'b')
    echoe 'failed to find last include'
    return
  endif
  exec printf('%d,.d', include_start)
  normal! O#include "app.h"

  let title = expand('%:t')
  let title = title[0 : stridx(title, '.') - 1]

  " add name space class
  let s = [ 'namespace zxd{'
        \ , ''
        \ , 'class app0 : public app{'
        \ , 'void init_info(){'
        \ , '  app::init_info();'
        \ , '  m_info.title = "' . title . '";'
        \ , '  m_info.display_mode = ' . display_mode . ';'
        \ , '  m_info.wnd_width = ' . wnd_width . ';'
        \ , '  m_info.wnd_height = ' . wnd_height . ';'
        \ , '}' 
        \ ]
  call append(line('.'), s)

  " add name space end
  if !search('\vint\s+main')
    echoe 'failed to search main'
    return
  endif
  let s = [ '};', '}']
  call append(line('.') - 1, s)

  " change main
  normal! jdi}

  let s = [ 'zxd::app0 _app0;'
        \ , '_app0.run(argc, argv);'
        \ , 'return 0;'
        \  ]
  call append(line('.') - 1, s)

  " rename init to create_scene
  exec ':%s/\vvoid\s+\zsinit\s*\ze\(/create_scene'

  " remove glutPostRedisplay
  global /glutPostRedisplay/ :d

  " remove case 27
  if search('\v case\s+27')
    .,.+2d
  endif

  " add super call to call back
  if search('\vvoid\s+reshape')
    normal! oapp::reshape(w, h);
  endif

  if search('\vvoid\s+keyboard')
    normal! oapp::keyboard(key, x, y);
  endif

endfunction
