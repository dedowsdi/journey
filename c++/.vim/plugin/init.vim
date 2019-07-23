call misc#proj#loadMap('c')
let g:mycppBuildDir = "./build/gcc/Debug"
let g:mycppDebugGui = 0
let &makeprg = 'cd ' . g:mycppBuildDir . ' && make'
"executable make target and path
"let g:mycppMakes = {}
"class name prefix
"let g:mycppClassPrefix='_PacExport'
"files included into newly created head file
"let g:mycppPreRequisite=['pacConsolePreRequisite.h']
"files included into newly created src file
"let g:mycppStable=['pacStable.h']

"call mycpp#addDependencies('dependencies')
