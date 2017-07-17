#/bin/bash
ctags -R --sort=foldcase --links=yes  --fields=KsSi --excmd=number --c++-kinds=+p --c-kinds=+p --exclude=build --exclude=cmake --exclude=CMake --exclude=CMakeLists --language-force=c++ practice common dependencies/osg/include dependencies/osg/src
