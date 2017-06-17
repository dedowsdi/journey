"debugger, lldb or gdb
let g:mycppDebugger = "gdb"
"build dir, such as ./build/Debug, ./build/RelWithDebInfo
let g:mycppBuildDir = "./build/Debug/"
let g:mycppNamespaces = [
\ "osg",
\ "osgUtil",
\ "osgDB",
\ "osgViewer",
\ "osgGA",
\ "osgViewer",
\ "Nodekits",
\ "osgFX",
\ "osgParticle",
\ "osgSim",
\ "osgManipulator",
\ "osgShadow",
\ "osgTerrain",
\ "osgText",
\ "osgWidget",
\ "osgAnimation",
\ "osgVolume"
\ ]

"executable make target and path
"let g:mycppMakes = {}
"class name prefix
"let g:mycppClassPrefix='_PacExport'
"files included into newly created head file
"let g:mycppPreRequisite=['pacConsolePreRequisite.h']
"files included into newly created src file
"let g:mycppStable=['pacStable.h']

"call mycpp#addDependencies('dependencies')
let g:mycppExeArgs = { 
      \ "helloworld" : "--model cessna.osg" ,
      \ "log" : "cessna.osg" ,
      \ "infoVisitor" : "cessnafire.osg"
      \ }

" abbrivation
au BufNewFile,BufRead * 
      \ if &ft == '' && match(expand('%:p'), 'OpenSceneGraph/include\|include/osg') != -1   | set ft=cpp | endif
