#!/usr/bin/env bash

mycmake -DCMAKE_MODULE_PATH=/usr/local/source/osg/CMakeModules \
        -DQt5_DIR=/opt/Qt5/5.7/gcc_64/lib/cmake/Qt5 \
        -DPRINT_STAGE=on \
        "$@" 
