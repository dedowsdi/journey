#/bin/bash
ctags -R --sort=foldcase --links=yes --c++-kinds=+p --c-kinds=+p --exclude=build --exclude=cmake --exclude=CMake --exclude=CMakeLists --language-force=c++ demo common dependencies/osg/include dependencies/osg/src
