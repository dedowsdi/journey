call misc#proj#loadMap('c')
let g:mycppBuildDir = "./build/gcc/Debug/"
set tags+=dependences/osg/tags

nnoremap <leader>oI :call osg#include()<CR>
