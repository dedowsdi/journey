#!/usr/bin/env bash

# . ~/.vim/script/vim_proj.sh

export CPP_SOURCE='**/*.h **/*.cpp'
export CPP_BUILD_TYPE=${CPP_BUILD_TYPE:-Debug}
export CPP_BUILD_DIR=build/$(mycpp -b)/$CPP_BUILD_TYPE

export OSG_FILE_PATH=".:$(pwd)/data:$OSG_FILE_PATH"
export OSG_WINDOW="100 100 1280 720"
# unset OSG_WINDOW
export OSG_NOTIFY_LEVEL=NOTICE
# export OSG_THREADING=SingleThreaded

vim --cmd 'set exrc'
