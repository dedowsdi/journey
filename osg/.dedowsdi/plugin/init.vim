set tags+=/usr/local/source/osg3.6_gl2/tags
set tags+=/usr/local/source/bullet3/tags
set path^=/usr/local/source/osg3.6_gl2/**
set path^=/usr/local/source/bullet3/**

call misc#proj#load_map('c')

let g:external_files = [
      \'/usr/local/source/osg',
      \'/usr/local/source/OpenSceneGraph-Data',
      \'/usr/local/source/bullet3',
      \'/usr/local/source/osgbullet-master',
      \]
