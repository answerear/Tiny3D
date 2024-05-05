#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain.
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------

set_project_name(imgui_dx11)

message(STATUS "Generating project : ${BIN_NAME}")


if (TINY3D_CORE_RTTR)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif (TINY3D_CORE_RTTR)


if (MSVC)
    # Add definition only for Windows.
    if (TINY3D_BUILD_SHARED_LIBS)
        add_definitions(-DIMGUI_EXPORT -D_USRDLL)
    endif (TINY3D_BUILD_SHARED_LIBS)

    # Disable some warning for protobuf
    add_definitions(
        /wd4244 # 'conversion' conversion from 'type1' to 'type2', possible loss of data
        /wd4267 # 'var' : conversion from 'size_t' to 'type', possible loss of data
        /wd4309 # 'conversion' : truncation of constant value
        /wd4800 # 'type' : forcing value to bool 'true' or 'false' (performance warning)
    )
endif (MSVC)

set (IMGUI_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..")
set (IMGUI_BACKENDS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../backends")


message(STATUS "Imgui source :${IMGUI_SOURCE_DIR}")
message(STATUS "Imgui backends : ${IMGUI_BACKENDS_DIR}")

# Setup search header path for this project.
include_directories(
    "${IMGUI_SOURCE_DIR}"
	"${IMGUI_BACKENDS_DIR}"
	"${SDL2_INCLUDE_DIR}"
    )
	
# Setup header files for this project.
set_project_files(Include ${IMGUI_SOURCE_DIR}/ .h)

# Setup source files for this project.
set_project_files(Source ${IMGUI_SOURCE_DIR}/ .cpp)

list(APPEND SOURCE_FILES ${IMGUI_BACKENDS_DIR}/imgui_impl_sdl2.h ${IMGUI_BACKENDS_DIR}/imgui_impl_dx11.h ${IMGUI_BACKENDS_DIR}/imgui_impl_sdl2.cpp ${IMGUI_BACKENDS_DIR}/imgui_impl_dx11.cpp)

message(STATUS "${SOURCE_FILES}")

# Setup all files for building this project.
if (TINY3D_BUILD_SHARED_LIBS)
    # Make dynamic library for this project.
    add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
else (TINY3D_BUILD_SHARED_LIBS)
    # Make static library for this project.
    add_library(${LIB_NAME} STATIC ${SOURCE_FILES})
endif (TINY3D_BUILD_SHARED_LIBS)


if (TINY3D_OS_WINDOWS)
    # Windows

    # Setup link libraries for this project.
    if (MSVC_VERSION GREATER_EQUAL 1900)
        # Visual Studio version is 2015 or later.
        target_link_libraries(
            ${LIB_NAME}
            LINK_PRIVATE ${SDL2_LIBRARY}
            )
    else ()
        # Visual Studio version is older than 2015
        target_link_libraries(
            ${LIB_NAME}
            LINK_PRIVATE ${SDL2_LIBRARY}
            )
    endif ()


    install(TARGETS ${LIB_NAME}
        RUNTIME DESTINATION bin/Debug CONFIGURATIONS Debug
        LIBRARY DESTINATION bin/Debug CONFIGURATIONS Debug
        ARCHIVE DESTINATION lib/Debug CONFIGURATIONS Debug
        )

    install(DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}/Include/ DESTINATION include/Core
        FILE_PERMISSIONS GROUP_READ OWNER_READ
        PATTERN "Android" EXCLUDE
        PATTERN "iOS" EXCLUDE
        PATTERN "Linux" EXCLUDE
        PATTERN "MacOSX" EXCLUDE
        PATTERN "Unix" EXCLUDE
        PATTERN "Windows" EXCLUDE
        PATTERN "Direct3D9" EXCLUDE
        PATTERN "Direct3D11" EXCLUDE
        PATTERN "OpenGL3Plus" EXCLUDE
        PATTERN "OpenGLES2" EXCLUDE
        PATTERN "OpenGLES3" EXCLUDE
        )

    add_custom_command(TARGET ${BIN_NAME}
        PRE_LINK
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}
        COMMAND ${CMAKE_COMMAND} -E copy ${SDL2_BINARY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
        COMMAND ${CMAKE_COMMAND} -E copy ${SDL2_BINARY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}
        )
    
    add_custom_command(TARGET ${BIN_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/../assets/config/Windows/Tiny3D.cfg ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/Tiny3D.cfg
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/../assets/config/Windows/Tiny3D.cfg ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/Tiny3D.cfg
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/Assets/icon"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/Assets/icon"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/Assets/models"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/Assets/models"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/Assets/textures"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/Assets/textures"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/Assets/builtin/icon"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/Assets/builtin/icon"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/Assets/builtin/program"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/Assets/builtin/program"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/Assets/builtin/materials"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/Assets/builtin/materials"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/../assets/icon" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/Assets/icon"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/../assets/icon" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/Assets/icon"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/../assets/models" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/Assets/models"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/../assets/models" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/Assets/models"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/../assets/textures" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/Assets/textures"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/../assets/textures" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/Assets/textures"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/../assets/builtin/icon" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/Assets/builtin/icon"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/../assets/builtin/icon" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/Assets/builtin/icon"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/../assets/builtin/program" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/Assets/builtin/program"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/../assets/builtin/program" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/Assets/builtin/program"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/../assets/builtin/materials" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/Assets/builtin/materials"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/../assets/builtin/materials" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/Assets/builtin/materials"
        )
elseif (TINY3D_OS_MACOSX)
    # Mac OS X

    target_link_libraries(
        ${LIB_NAME}
        LINK_PRIVATE ${SDL2_LIBRARY}
        )
elseif (TINY3D_OS_LINUX)
    # Linux
    target_link_libraries(
        ${LIB_NAME}
        LINK_PRIVATE ${SDL2_LIBRARY}
        )

    add_custom_command(TARGET ${BIN_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/../assets/config/Linux/Tiny3D.cfg ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Tiny3D.cfg
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/lib${BIN_NAME}.so" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lib${BIN_NAME}.so"
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/Assets/icon
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/Assets/icon
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/Assets/scripts
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/Assets/scripts
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/Assets/textures
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/Assets/textures
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/../assets/icon ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Assets/icon
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/../assets/scripts ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Assets/scripts
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/../assets/textures ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Assets/textures
        )
endif (TINY3D_OS_WINDOWS)


# Setup project folder
set_property(TARGET ${BIN_NAME} PROPERTY FOLDER "External/imgui")


message(STATUS "Generating project : ${BIN_NAME} - done")
