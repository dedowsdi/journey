#!/bin/bash
build_type=${1:-Debug}
compiler=${2:-gcc}

cd "$(realpath "${BASH_SOURCE[0]%/*}")/.."

source_dir="$(pwd)"
build_dir="build/$compiler/$build_type"

(
cd "$build_dir" && \
       cmake \
         -DCMAKE_PREFIX_PATH:STRING='/usr/local/osg/gl2;/opt/Qt5.7.0/5.7/gcc_64' \
         -DBUILD_SHARED_LIBS:BOOLEAN=ON \
         -DCMAKE_CXX_FLAGS:STRING= \
         -DCMAKE_BUILD_TYPE:STRING="$build_type" \
         -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOLEAN=ON \
         "$source_dir"
)

ln -fs "$build_dir"/compile_commands.json compile_commands.json
