#!/usr/bin/env sh

mycmake "$@" \
    -DCMAKE_MODULE_PATH:STRING=/usr/local/source/osg/CMakeModules \
    -DCMAKE_PREFIX_PATH:STRING=/opt/Qt5/5.7/gcc_64
