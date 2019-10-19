call misc#proj#load_map('c')
let g:mycpp_build_dir = "./build/gcc/Debug/"
set tags+=/usr/local/source/osg3.6/tags
let g:external_files = [
      \'/usr/local/source/osg3.6_gl2',
      \'/usr/local/source/OpenSceneGraph-Data',
      \]

nnoremap <leader>oI :call osg#include()<CR>
