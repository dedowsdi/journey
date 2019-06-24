#!/bin/bash
buildType=Debug
compiler=gcc
[[ $# -gt 1 ]] && buildType=$1
[[ $# -gt 2 ]] && compiler=$2

buildDir=build/$compiler/$buildType
currentDir=$(pwd)

sh -c "cd $buildDir && \
       cmake \
         -DCMAKE_CXX_FLAGS:STRING= \
         -DCMAKE_BUILD_TYPE:STRING=$buildType \
         -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOLEAN=ON \
         $currentDir"

ln -fs "$buildDir"/compile_commands.json compile_commands.json
