
FIND_PATH(FREEIMAGE_INCLUDE_DIR FreeImage.h
  HINTS
  ${FREEIMAGE_HOME}
  PATH_SUFFIXES include
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local/include/FreeImage
  /usr/include/FreeImage
  /sw # Fink
  /opt/local # DarwinPorts
  /opt/csw # Blastwave
  /opt
)

FIND_LIBRARY(FREEIMAGE_LIBRARY_TEMP
  NAMES FreeImage
  HINTS
  ${FREEIMAGE_HOME}
  PATH_SUFFIXES lib64 lib prebuilt/win32/${MSVC_CXX_ARCHITECTURE_ID}
  PATHS
  /sw
  /opt/local
  /opt/csw
  /opt
)

FIND_FILE(FREEIMAGE_BINARY
  NAMES "FreeImage.dll"
  HINTS
  ${FREEIMAGE_HOME}
  PATH_SUFFIXES lib64 lib prebuilt/win32/${MSVC_CXX_ARCHITECTURE_ID}
)



SET(FREEIMAGE_FOUND "NO")
IF(FREEIMAGE_LIBRARY_TEMP)
  # Set the final string here so the GUI reflects the final state.
  SET(FREEIMAGE_LIBRARY ${FREEIMAGE_LIBRARY_TEMP} CACHE STRING "Where the FreeImage Library can be found")
  # Set the temp variable to INTERNAL so it is not seen in the CMake GUI
  SET(FREEIMAGE_LIBRARY_TEMP "${FREEIMAGE_LIBRARY_TEMP}" CACHE INTERNAL "")

  SET(FREEIMAGE_FOUND "YES")
ENDIF(FREEIMAGE_LIBRARY_TEMP)

