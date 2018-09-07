#!/bin/bash

buildType=Debug
compiler=gcc
if [[ $# -gt 1 ]]; then
  buildType=$1
fi
if [[ $# -gt 2 ]]; then
  compiler=$2
fi

buildDir=build/$compiler/$buildType
currentDir=$(pwd)

sh -c "cd $buildDir && \
       cmake \
         -DCMAKE_BUILD_TYPE:STRING=$buildType \
         -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOLEAN=ON \
         $currentDir"

ln -fs "$buildDir"/compile_commands.json compile_commands.json
