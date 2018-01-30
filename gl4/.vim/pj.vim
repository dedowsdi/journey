"debugger, lldb or gdb
"build dir, such as ./build/Debug, ./build/RelWithDebInfo
let g:mycppBuildDir = "./build/Debug"
set rtp +=./.vim
"executable make target and path
"let g:mycppMakes = {}
"class name prefix
"let g:mycppClassPrefix='_PacExport'
"files included into newly created head file
"let g:mycppPreRequisite=['pacConsolePreRequisite.h']
"files included into newly created src file
"let g:mycppStable=['pacStable.h']

"call mycpp#addDependencies('dependencies')

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
  let s = substitute(s, 'matrix', 'mx', '')
  return s
endfunction

function! ShrinkShaderMatrixName(name)
  let s = CamelToUnderScore(a:name)
  let s = ShrinkMatrixName(s)
  return s
endfunction
