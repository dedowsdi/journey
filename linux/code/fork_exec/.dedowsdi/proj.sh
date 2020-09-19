#!/usr/bin/env bash

export CPP_BUILD_TYPE=${CPP_BUILD_TYPE:-Debug}
export CPP_BUILD_DIR=build/$(mycpp -b)/$CPP_BUILD_TYPE
export GL_FILE_PATH="$GL_FILE_PATH;$(pwd)/data"

vim +/main +'norm! zz' a.cpp
