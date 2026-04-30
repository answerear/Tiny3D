#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain.
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------

set_project_name(T3DReflect)

set(RPP_EXE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/rpp")
set(NMAKE_DIR "${CMAKE_CURRENT_BINARY_DIR}/../../nmake")
set(SRC_DIR "${CMAKE_CURRENT_BINARY_DIR}/../..")
set(WHITE_LIST "Tiny3D::TAabb;Tiny3D::TDegree;Tiny3D::TFrustum;Tiny3D::TMatrix2;Tiny3D::TMatrix3;Tiny3D::TMatrix4;Tiny3D::TObb;Tiny3D::TPlane;Tiny3D::TQuaternion;Tiny3D::TRadian;Tiny3D::TRay;Tiny3D::TSize;Tiny3D::TPoint;Tiny3D::TRect;Tiny3D::TSphere;Tiny3D::TTriangle;Tiny3D::TVector2;Tiny3D::TVector3;Tiny3D::TVector4;")

# Generate reflect.bat at configure time with paths resolved
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/reflect.bat"
"@echo off\n\
powershell -NoProfile -Command \"& { $p = @(); $p += Start-Process '${RPP_EXE}' -ArgumentList '${NMAKE_DIR}/System','${SRC_DIR}/System','-b','-j','8' -NoNewWindow -PassThru; $p += Start-Process '${RPP_EXE}' -ArgumentList '${NMAKE_DIR}/Math','${SRC_DIR}/Math','-b','-j','8' -NoNewWindow -PassThru; $p += Start-Process '${RPP_EXE}' -ArgumentList '${NMAKE_DIR}/Core/Runtime','${SRC_DIR}/Core','-b','-j','8','-W','${WHITE_LIST}' -NoNewWindow -PassThru; $p += Start-Process '${RPP_EXE}' -ArgumentList '${NMAKE_DIR}/Core/Editor','${SRC_DIR}/Core','-b','-j','8','-W','${WHITE_LIST}' -NoNewWindow -PassThru; $p += Start-Process '${RPP_EXE}' -ArgumentList '${NMAKE_DIR}/Editor/TinyLauncher','${SRC_DIR}/Editor/TinyLauncher','-b','-j','8' -NoNewWindow -PassThru; $p += Start-Process '${RPP_EXE}' -ArgumentList '${NMAKE_DIR}/Editor/TinyEditor','${SRC_DIR}/Editor/TinyEditor','-b','-j','8' -NoNewWindow -PassThru; $p | Wait-Process }\"\n"
)

# Parallel incremental reflection generation
# All 6 sub-projects run in parallel with -b (incremental) and -j 8 (8 threads each)
add_custom_target(${BIN_NAME}
    COMMAND cmd /c "${CMAKE_CURRENT_BINARY_DIR}/reflect.bat"
    COMMENT "Running incremental reflection generation (parallel, 6 sub-projects)..."
)

# Setup project folder
set_property(TARGET ${BIN_NAME} PROPERTY FOLDER "Kernel")
