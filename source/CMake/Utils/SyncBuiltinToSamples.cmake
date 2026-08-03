#-------------------------------------------------------------------------------
# 把 BuiltinGenerator 产出的内置资源同步到 assets/samples/meshes
#
# 编辑器和 Sample 是两条独立的资源链路：编辑器从 assets/editor/builtin 复制到
# <Project>/Temp/builtin 挂 MetaFS，.tshader 是 openProject 时用 scc 现编译的；
# Sample 走 assets/samples/meshes 经 BundleBuilder 打成 bundle，.tshader 必须已
# 入库。既有先例是「双份文件 + 相同 UUID」，这个脚本负责保持两边不漂移。
#
# 用 cmake -P 在构建期执行（配置期文件可能还没生成），需要传入：
#   -DBUILTIN_DIR=<assets/editor/builtin>
#   -DSAMPLES_DIR=<assets/samples/meshes>
#   -DSCC_EXECUTABLE=<scc 可执行文件路径>
#-------------------------------------------------------------------------------

if (NOT BUILTIN_DIR OR NOT SAMPLES_DIR)
    message(FATAL_ERROR "BUILTIN_DIR and SAMPLES_DIR must be provided")
endif ()

set(SYNC_ASSETS
    "shaders/Skybox-Cubemap.shader"
    "shaders/Skybox-Cubemap.shader.meta"
    "materials/Skybox-Cubemap.tmat"
    "materials/Skybox-Cubemap.tmat.meta"
    "textures/skybox_default.ttex"
    "textures/skybox_default.ttex.meta"
    # 带 blocks 贴图的测试立方体（与 TextureApp 同源），Sample 用它验证天空盒遮挡。
    # test-cube → Test-Material.tmat → blocks.ttex；Tiny3DStandard.tmat 仍绑白图，一并保留
    "meshes/test-cube.tmesh"
    "meshes/test-cube.tmesh.meta"
    "materials/Test-Material.tmat"
    "materials/Test-Material.tmat.meta"
    "textures/blocks.ttex"
    "textures/blocks.ttex.meta"
    "textures/white.ttex"
    "textures/white.ttex.meta")

foreach (ASSET ${SYNC_ASSETS})
    set(SRC "${BUILTIN_DIR}/${ASSET}")
    get_filename_component(ASSET_NAME "${ASSET}" NAME)
    set(DST "${SAMPLES_DIR}/${ASSET_NAME}")

    if (NOT EXISTS "${SRC}")
        message(WARNING "Missing builtin asset: ${SRC}. Run BuiltinGenerator first.")
        continue()
    endif ()

    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SRC}" "${DST}")
    message(STATUS "Synced ${ASSET_NAME}")
endforeach ()

# 从 ShaderLab meta 里取出已编译 shader 的 UUID，传给 scc，保证 Sample 侧烘焙出
# 来的 .tshader 跟编辑器侧是同一个 guid，材质的引用才不会断
set(SHADER_META "${SAMPLES_DIR}/Skybox-Cubemap.shader.meta")
set(SHADER_UUID "")

if (EXISTS "${SHADER_META}")
    file(READ "${SHADER_META}" META_CONTENT)
    # ShaderUUID 之后第一个 32 位十六进制字符串就是编译产物的 guid
    string(REGEX MATCH "\"ShaderUUID\".*\"RTTI_Value\"[ \t\r\n]*:[ \t\r\n]*\"([0-9a-fA-F]+)\""
        _MATCHED "${META_CONTENT}")
    if (CMAKE_MATCH_1)
        set(SHADER_UUID "${CMAKE_MATCH_1}")
    endif ()
endif ()

if (NOT SCC_EXECUTABLE OR NOT EXISTS "${SCC_EXECUTABLE}")
    message(WARNING "scc not found; Skybox-Cubemap.tshader was NOT rebaked.")
    return()
endif ()

get_filename_component(SCC_DIR "${SCC_EXECUTABLE}" DIRECTORY)

set(SCC_ARGS "${SAMPLES_DIR}/Skybox-Cubemap.shader" -t hlsl,glsl,essl,spirv -o "${SAMPLES_DIR}")
if (SHADER_UUID)
    list(APPEND SCC_ARGS -u "${SHADER_UUID}")
endif ()

execute_process(
    COMMAND "${SCC_EXECUTABLE}" ${SCC_ARGS}
    WORKING_DIRECTORY "${SCC_DIR}"
    RESULT_VARIABLE SCC_RESULT)

if (NOT SCC_RESULT EQUAL 0)
    message(FATAL_ERROR "scc failed to bake Skybox-Cubemap.tshader (exit ${SCC_RESULT})")
endif ()

message(STATUS "Baked Skybox-Cubemap.tshader for hlsl/glsl/essl/spirv")

# scc 只吐 .tshader，BundleBuilder 还要靠 .tshader.meta 才能按 guid 建立引用，
# 这里照 Tiny3DStandard.tshader.meta 的格式补一份
if (SHADER_UUID)
    file(WRITE "${SAMPLES_DIR}/Skybox-Cubemap.tshader.meta"
"{
  \"Magic\": \"T3D\",
  \"Version\": \"0.0.1\",
  \"Object\": {
    \"RTTI_Type\": \"Tiny3D::MetaShader\",
    \"RTTI_Value\": {
      \"UUID\": {
        \"RTTI_Type\": \"Tiny3D::UUID\",
        \"RTTI_Value\": {
          \"RTTI_Type\": \"std::string\",
          \"RTTI_Value\": \"${SHADER_UUID}\"
        }
      }
    }
  }
}")
    message(STATUS "Wrote Skybox-Cubemap.tshader.meta (${SHADER_UUID})")
endif ()
