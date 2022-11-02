
message(STATUS "Looking for clang ...")
message(STATUS "Looking for clang home : ${CLANG_HOME}")
message(STATUS "Looking for clang suffixes : ${CLANG_LIB_SUFFIXES}")

FIND_PATH(CLANG_INCLUDE_DIR 
  NAMES CMakeLists.txt
  HINTS ${CLANG_HOME}
  PATH_SUFFIXES include
#  PATHS include
)

FIND_LIBRARY(CLANG_LIBRARY_TEMP
  NAMES libclang libclang.a
  HINTS ${CLANG_HOME}
  PATH_SUFFIXES ${CLANG_LIB_SUFFIXES}
#  PATHS .
)

FIND_FILE(CLANG_BINARY
  NAMES "libclang.dll" "libclang.so" "libclang.dylib"
  HINTS ${CLANG_HOME}
  PATH_SUFFIXES ${CLANG_LIB_SUFFIXES}
)

message(STATUS "${CLANG_INCLUDE_DIR}")
message(STATUS "${CLANG_LIBRARY_TEMP}")
message(STATUS "${CLANG_BINARY}")

SET(CLANG_FOUND "NO")
IF(CLANG_LIBRARY_TEMP)
  # Set the final string here so the GUI reflects the final state.
  SET(CLANG_LIBRARY ${CLANG_LIBRARY_TEMP} CACHE STRING "Where the clang Libraries can be found")
  # Set the temp variable to INTERNAL so it is not seen in the CMake GUI
  SET(CLANG_LIBRARY_TEMP "${CLANG_LIBRARY_TEMP}" CACHE INTERNAL "")

  SET(CLANG_FOUND "YES")
ENDIF(CLANG_LIBRARY_TEMP)

