#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain.
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------

set_project_name(T3DReflect)

# Use CMAKE_CURRENT_SOURCE_DIR to calculate paths relative to the source tree.
# This works for both native (Windows) and cross-compiled (Android) builds.
set(NMAKE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../nmake")
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")

set(WHITE_LIST "Tiny3D::TAabb;Tiny3D::TCapsule;Tiny3D::TDegree;Tiny3D::TFrustum;Tiny3D::TMatrix2;Tiny3D::TMatrix3;Tiny3D::TMatrix4;Tiny3D::TObb;Tiny3D::TPlane;Tiny3D::TQuaternion;Tiny3D::TRadian;Tiny3D::TRay;Tiny3D::TSize;Tiny3D::TPoint;Tiny3D::TRect;Tiny3D::TSphere;Tiny3D::TTriangle;Tiny3D::TVector2;Tiny3D::TVector3;Tiny3D::TVector4;")

# RPP executable path: native build uses target file, otherwise a host-built rpp.
if (CMAKE_CROSSCOMPILING)
    set(TINY3D_HOST_RPP "" CACHE FILEPATH
        "Path to host-built ReflectionPreprocessor (rpp) executable for cross-compilation")
    if (NOT TINY3D_HOST_RPP)
        message(WARNING "CMAKE_CROSSCOMPILING is ON but TINY3D_HOST_RPP is not set. "
            "Set -DTINY3D_HOST_RPP=/path/to/rpp to enable reflection generation.")
    endif()
    set(RPP_EXE "${TINY3D_HOST_RPP}")
elseif (TARGET ReflectionPreprocessor)
    set(RPP_EXE "$<TARGET_FILE:ReflectionPreprocessor>")
else ()
    set(RPP_EXE "")
    foreach (_tiny3d_rpp_candidate
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/rpp.app/Contents/MacOS/rpp"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/rpp.app/Contents/MacOS/rpp"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/rpp"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/rpp")
        if (_tiny3d_rpp_candidate AND EXISTS "${_tiny3d_rpp_candidate}")
            set(RPP_EXE "${_tiny3d_rpp_candidate}")
            break()
        endif ()
    endforeach ()
    unset(_tiny3d_rpp_candidate)
endif()

# Helper macro to add reflection targets with existence check
macro(add_reflect_target TARGET_NAME SETTINGS_SUBDIR SRC_SUBDIR ADDITIONAL_ARGS)
    if ((CMAKE_CROSSCOMPILING AND NOT TINY3D_HOST_RPP) OR (NOT RPP_EXE))
        # No host RPP available: create a no-op target
        add_custom_target(${TARGET_NAME}
            COMMAND ${CMAKE_COMMAND} -E echo "WARNING: Skipping reflection generation: ${SETTINGS_SUBDIR} (no rpp)"
            COMMENT "Reflection generation skipped: ${SETTINGS_SUBDIR}"
        )
    else()
        set(SETTINGS_FILE "${NMAKE_DIR}/${SETTINGS_SUBDIR}/ReflectionSettings.json")
        if (EXISTS "${SETTINGS_FILE}")
            add_custom_target(${TARGET_NAME}
                COMMAND ${RPP_EXE} "${NMAKE_DIR}/${SETTINGS_SUBDIR}" "${SRC_DIR}/${SRC_SUBDIR}" ${ADDITIONAL_ARGS}
                COMMENT "Running reflection generation: ${SETTINGS_SUBDIR}"
            )
        else()
            # Settings file not found (likely not pre-generated via batch files)
            add_custom_target(${TARGET_NAME}
                COMMAND ${CMAKE_COMMAND} -E echo "Skipping ${TARGET_NAME}: ${SETTINGS_FILE} not found."
                COMMENT "Reflection generation skipped for ${SETTINGS_SUBDIR} (missing settings)"
            )
        endif()
    endif()

    # Native build: ensure ReflectionPreprocessor is built before any reflect sub-target runs
    if (NOT CMAKE_CROSSCOMPILING AND TARGET ReflectionPreprocessor)
        add_dependencies(${TARGET_NAME} ReflectionPreprocessor)
    endif()

    set_property(TARGET ${TARGET_NAME} PROPERTY FOLDER "Reflect")
endmacro()

# Define reflection targets
set(REFLECT_TARGETS "")

# Base targets (always needed for the engine)
list(APPEND REFLECT_TARGETS reflect_system reflect_math reflect_core_runtime)
add_reflect_target(reflect_system "System" "System" "-b -j 8")
add_reflect_target(reflect_math "Math" "Math" "-b -j 8")
add_reflect_target(reflect_core_runtime "Core/Runtime" "Core" "-b -j 8 -W \"${WHITE_LIST}\"")

# Optional targets (only if tools/editor are enabled)
if (TINY3D_BUILD_TOOLS OR TINY3D_BUILD_EDITOR)
    list(APPEND REFLECT_TARGETS reflect_core_editor reflect_editor_launcher reflect_editor_editor)
    add_reflect_target(reflect_core_editor "Core/Editor" "Core" "-b -j 8 -W \"${WHITE_LIST}\"")
    add_reflect_target(reflect_editor_launcher "Editor/TinyLauncher" "Editor/TinyLauncher" "-b -j 8")
    add_reflect_target(reflect_editor_editor "Editor/TinyEditor" "Editor/TinyEditor" "-b -j 8")
else()
    # Create empty targets if disabled so T3DReflect dependency remains valid
    add_custom_target(reflect_core_editor COMMAND ${CMAKE_COMMAND} -E echo "Reflection skipped (Editor disabled)")
    add_custom_target(reflect_editor_launcher COMMAND ${CMAKE_COMMAND} -E echo "Reflection skipped (Editor disabled)")
    add_custom_target(reflect_editor_editor COMMAND ${CMAKE_COMMAND} -E echo "Reflection skipped (Editor disabled)")
    set_property(TARGET reflect_core_editor PROPERTY FOLDER "Reflect")
    set_property(TARGET reflect_editor_launcher PROPERTY FOLDER "Reflect")
    set_property(TARGET reflect_editor_editor PROPERTY FOLDER "Reflect")
endif()

# Umbrella target: depends on active sub-targets
add_custom_target(${BIN_NAME}
    DEPENDS ${REFLECT_TARGETS}
    COMMENT "Running incremental reflection generation..."
)

# Setup project folder
set_property(TARGET ${BIN_NAME} PROPERTY FOLDER "Reflect")
