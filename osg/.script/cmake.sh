buildDir=build/Debug
cd $buildDir
cmake -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOLEAN=ON ../..
cd ../../
ln -s $buildDir/compile_commands.json compile_commands.json
