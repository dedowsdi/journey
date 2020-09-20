mkdir build
cd build
cmake -G "Ninja" ^
      -DCMAKE_CXX_FLAGS=-DWIN32 ^
      -DCMAKE_EXPORT_COMPILE_COMMANDS=on ^
      -DCMAKE_BUILD_TYPE=Debug ^
      -DCMAKE_C_COMPILER="C:/Program Files/LLVM/bin/clang.exe" ^
      -DCMAKE_CXX_COMPILER="C:/Program Files/LLVM/bin/clang++.exe" ^
      -DCMAKE_TOOLCHAIN_FILE="E:/src/vcpkg/scripts/buildsystems/vcpkg.cmake" ^
      ..\

      :: -DCMAKE_PREFIX_PATH="E:/src/vcpkg/installed/x64-windows" ^
