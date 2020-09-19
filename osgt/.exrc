set rtp+=.dedowsdi,.dedowsdi/after

set path^=/usr/local/source/osg/include/**,/usr/local/source/osg/src/**
set path^=/usr/local/source/OpenSceneGraph-Data/shaders/**
set path^=/usr/local/source/bullet3/src/**
set path^=/usr/include/AL/**
set path^=util/**,test/**

set tags+=/usr/local/source/osg/tags
set tags+=/usr/include/GL/gltag

Proj cpp
let g:cdef_get_set_style = 'camel'
let &makeprg = 'make -j 3 -sC ' . $CPP_BUILD_DIR
