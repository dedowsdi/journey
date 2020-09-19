#!/usr/bin/env bash

mycmake -DCMAKE_MODULE_PATH=/usr/local/source/osg/CMakeModules \
        -DPRINT_STAGE=on \
        "$@" 
