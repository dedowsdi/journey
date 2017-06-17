#heads and srcs
macro(addCppRecursive dir retVal  )
    file( GLOB_RECURSE ${
  retVal} ${
  dir}/*.h ${dir}/*.cpp ${dir}/*.c  ${dir}/*.hpp )
endmacro()

macro(addCppDirect dir retVal)
    file( GLOB ${retVal} ${dir}/*.h ${dir}/*.cpp ${dir}/*.c  ${dir}/*.hpp )
endmacro()

macro(findLibraryRequird var filename)
    FIND_LIBRARY(${var} ${filename})
    IF(${var})
        MESSAGE("found " ${var})
    ELSE()
        MESSAGE(FATAL_ERROR "${var} not found !!!!!")
    ENDIF()
endmacro()

macro(findPathRequird var filename)
	# Cannot use ARGN directly with list() command.
	# Copy to a variable first.
	set (extra_macro_args ${ARGN})
	# Did we get any optional args?
    list(LENGTH extra_macro_args num_extra_args)
    if (${num_extra_args} GREATER 0)
        list(GET extra_macro_args 0 suffix)
        FIND_PATH(${var} ${filename} PATH_SUFFIXES ${suffix})
    else ()
        FIND_PATH(${var} ${filename})
    endif ()

    IF(${var})
        MESSAGE("found " ${var})
    ELSE()
        MESSAGE(FATAL_ERROR "${var} not found !!!!!")
    ENDIF()
endmacro()
