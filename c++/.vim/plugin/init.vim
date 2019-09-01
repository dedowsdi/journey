call misc#proj#load_map('c')
let g:mycpp_build_dir = "./build/gcc/Debug"
let g:mycpp_debug_gui = 0
let &makeprg = 'cd ' . g:mycpp_build_dir . ' && make'
"executable make target and path
"let g:mycppMakes = {}
"class name prefix
"let g:mycppClassPrefix='_PacExport'
"files included into newly created head file
"let g:mycppPreRequisite=['pacConsolePreRequisite.h']
"files included into newly created src file
"let g:mycppStable=['pacStable.h']

"call mycpp#addDependencies('dependencies')
