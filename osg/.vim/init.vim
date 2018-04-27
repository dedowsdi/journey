call myvim#switchRtp('./.vim')
let g:mycppBuildDir = "./build/Debug/"

nnoremap <leader>oI :call osg#include()<CR>

au BufNewFile,BufRead * if &ft == '' && match(expand('%:p'), 'OpenSceneGraph/include\|include/osg') != -1   | set ft=cpp | endif
