#!/bin/bash
buildType=${1:-Debug}
compiler=${2:-gcc}

cd "$(realpath "${BASH_SOURCE[0]%/*}")/.."

sourceDir="$(pwd)"
buildDir="build/$compiler/$buildType"

(
cd "$buildDir" && \
       cmake \
         -DCMAKE_CXX_FLAGS:STRING= \
         -DCMAKE_BUILD_TYPE:STRING="$buildType" \
         -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOLEAN=ON \
         "$sourceDir"
)

ln -fs "$buildDir"/compile_commands.json compile_commands.json
