#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain.
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------

set_project_name(T3DReflect)


add_custom_target(${BIN_NAME}
		COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/ReflectionPreprocessor ${CMAKE_CURRENT_BINARY_DIR}/../../nmake/System ${CMAKE_CURRENT_BINARY_DIR}/../../System -b
		COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/ReflectionPreprocessor ${CMAKE_CURRENT_BINARY_DIR}/../../nmake/Math ${CMAKE_CURRENT_BINARY_DIR}/../../Math -b
		COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/ReflectionPreprocessor ${CMAKE_CURRENT_BINARY_DIR}/../../nmake/Core ${CMAKE_CURRENT_BINARY_DIR}/../../Core -b
		COMMAND cmake -G "Visual Studio 16 2019" -DTINY3D_BUILD_RTTR_TOOL=OFF -DCMAKE_BUILD_TYPE=Debug ../)


# Setup project folder
set_property(TARGET ${BIN_NAME} PROPERTY FOLDER "Kernel")
