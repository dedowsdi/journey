#!/bin/bash

buildType=Debug
if [[ $# -gt 1 ]]; then
  buildType=$1
fi

buildDir=build/clang/$buildType
currentDir=$(pwd)

sh -c "cd $buildDir && \
       cmake \
         -DCMAKE_BUILD_TYPE:STRING=$buildType \
         -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOLEAN=ON \
         $currentDir"

ln -fs "$buildDir"/compile_commands.json compile_commands.json
