
message(STATUS "Looking for protobuf ...")

FIND_PATH(PROTOBUF_INCLUDE_DIR 
  NAMES google
  HINTS ${PROTOBUF_HOME}
  PATH_SUFFIXES include/
#  PATHS include
)

FIND_LIBRARY(PROTOBUF_LIBRARY_TEMP
  NAMES "libprotobuf" "libprotobuf.a"
  HINTS ${PROTOBUF_HOME}
  PATH_SUFFIXES ${PROTOBUF_SUFFIXES}
#  PATHS .
)

FIND_FILE(PROTOBUF_BINARY
  NAMES "libprotobuf.dll" "libprotobuf.so" "libprotobuf.dylib"
  HINTS ${PROTOBUF_HOME}
  PATH_SUFFIXES ${PROTOBUF_SUFFIXES}
)

FIND_LIBRARY(PROTOBUF_LITE_LIBRARY_TEMP
  NAMES "libprotobuf-lite" "libprotobuf-lite.a"
  HINTS ${PROTOBUF_HOME}
  PATH_SUFFIXES ${PROTOBUF_SUFFIXES}
#  PATHS .
)

FIND_FILE(PROTOBUF_LITE_BINARY
  NAMES "libprotobuf-lite.dll" "libprotobuf-lite.so" "libprotobuf-lite.dylib"
  HINTS ${PROTOBUF_HOME}
  PATH_SUFFIXES ${PROTOBUF_SUFFIXES}
)


message(STATUS "${PROTOBUF_INCLUDE_DIR}")
message(STATUS "${PROTOBUF_LIBRARY_TEMP}")
message(STATUS "${PROTOBUF_BINARY}")
message(STATUS "${PROTOBUF_LITE_LIBRARY_TEMP}")
message(STATUS "${PROTOBUF_LITE_BINARY}")

SET(PROTOBUF_FOUND "NO")
IF(PROTOBUF_LIBRARY_TEMP)
  # Set the final string here so the GUI reflects the final state.
  SET(PROTOBUF_LIBRARY ${PROTOBUF_LIBRARY_TEMP} CACHE STRING "Where the protobuf Library can be found")
  # Set the temp variable to INTERNAL so it is not seen in the CMake GUI
  SET(PROTOBUF_LIBRARY_TEMP "${PROTOBUF_LIBRARY_TEMP}" CACHE INTERNAL "")

  SET(PROTOBUF_FOUND "YES")
ENDIF(PROTOBUF_LIBRARY_TEMP)

SET(PROTOBUF_LITE_FOUND "NO")
IF(PROTOBUF_LITE_LIBRARY_TEMP)
  # Set the final string here so the GUI reflects the final state.
  SET(PROTOBUF_LITE_LIBRARY ${PROTOBUF_LITE_LIBRARY_TEMP} CACHE STRING "Where the protobuf-lite Library can be found")
  # Set the temp variable to INTERNAL so it is not seen in the CMake GUI
  SET(PROTOBUF_LITE_LIBRARY_TEMP "${PROTOBUF_LITE_LIBRARY_TEMP}" CACHE INTERNAL "")

  SET(PROTOBUF_LITE_FOUND "YES")
ENDIF(PROTOBUF_LITE_LIBRARY_TEMP)

