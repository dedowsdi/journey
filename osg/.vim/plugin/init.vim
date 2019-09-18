call misc#proj#load_map('c')
let g:mycpp_build_dir = "./build/gcc/Debug/"
set tags+=/usr/local/source/osg3.6/tags

nnoremap <leader>oI :call osg#include()<CR>
