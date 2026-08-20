#!/bin/bash
# Generate Xcode project for macOS.
# Mirrors generate-vs2019-x64-debug.bat:
#   wipe → generate tool project → build cct/rpp → export compile_commands
#   → cct + rpp → regenerate full Xcode project → open IDE.

set -euo pipefail

cd "$(dirname "$0")/.."
SOURCE_DIR="$(pwd)"
XCODE_DIR="${SOURCE_DIR}/xcode-osx"
NMAKE_DIR="${SOURCE_DIR}/nmake"

CMAKE_C_COMPILER="$(xcrun -find cc)"
CMAKE_CXX_COMPILER="$(xcrun -find c++)"

# 引擎 / Sample / 插件跟仓库里的 OSX 预编译库走 Intel（FreeImage、ShaderConductor、FBX）。
# rpp 在 macOS 上链接当前 Xcode 的 libclang（LLVM 15 解析不了新 SDK 的 libc++）。
ENGINE_ARCH="x86_64"
TOOL_ARCH="${ENGINE_ARCH}"
echo "    engine/tool arch: ${ENGINE_ARCH}"

XCODE_CMAKE_COMMON=(
    -G Xcode
    -DCMAKE_C_COMPILER="${CMAKE_C_COMPILER}"
    -DCMAKE_CXX_COMPILER="${CMAKE_CXX_COMPILER}"
    -DCMAKE_OSX_DEPLOYMENT_TARGET=13.0
    -DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM=8GEE4MA6EY
    -DCMAKE_BUILD_TYPE=Debug
)
XCODE_CMAKE_ARGS=(
    "${XCODE_CMAKE_COMMON[@]}"
    -DCMAKE_OSX_ARCHITECTURES="${ENGINE_ARCH}"
)
XCODE_CMAKE_TOOL_ARGS=(
    "${XCODE_CMAKE_COMMON[@]}"
    -DCMAKE_OSX_ARCHITECTURES="${TOOL_ARCH}"
)

UNIX_CMAKE_ARGS=(
    -G "Unix Makefiles"
    -DCMAKE_C_COMPILER="${CMAKE_C_COMPILER}"
    -DCMAKE_CXX_COMPILER="${CMAKE_CXX_COMPILER}"
    -DCMAKE_OSX_ARCHITECTURES="${ENGINE_ARCH}"
    -DCMAKE_OSX_DEPLOYMENT_TARGET=13.0
    -DCMAKE_BUILD_TYPE=Debug
    -DTINY3D_COPY_RESOURCE=OFF
)

CORE_WHITELIST="Tiny3D::TAabb;Tiny3D::TDegree;Tiny3D::TFrustum;Tiny3D::TMatrix2;Tiny3D::TMatrix3;Tiny3D::TMatrix4;Tiny3D::TObb;Tiny3D::TPlane;Tiny3D::TQuaternion;Tiny3D::TRadian;Tiny3D::TRay;Tiny3D::TSize;Tiny3D::TPoint;Tiny3D::TRect;Tiny3D::TSphere;Tiny3D::TTriangle;Tiny3D::TVector2;Tiny3D::TVector3;Tiny3D::TVector4;"

trap 'cd "${SOURCE_DIR}/Projects"' EXIT

wait_jobs() {
    local status=0
    local pid
    for pid in "$@"; do
        wait "${pid}" || status=1
    done
    return "${status}"
}

find_tool() {
    local name="$1"
    local candidates=(
        "${SOURCE_DIR}/bin/OSX/Debug/${name}.app/Contents/MacOS/${name}"
        "${SOURCE_DIR}/bin/OSX/Debug/${name}"
        "${SOURCE_DIR}/bin/OSX/${name}.app/Contents/MacOS/${name}"
        "${SOURCE_DIR}/bin/OSX/${name}"
    )
    local c
    for c in "${candidates[@]}"; do
        if [[ -x "${c}" ]]; then
            printf '%s\n' "${c}"
            return 0
        fi
    done
    echo "ERROR: ${name} was not built (looked under bin/OSX)." >&2
    return 1
}

export_compile_commands() {
    local enable_flag="$1"
    local dest="$2"
    shift 2

    cmake "${UNIX_CMAKE_ARGS[@]}" \
        -D"${enable_flag}=ON" \
        "$@" \
        ../

    mkdir -p "${dest}"
    cp -f compile_commands.json "${dest}/"
}

# ================= Generate base projects =====================
echo "==> Cleaning CMake output directories"
rm -rf "${XCODE_DIR}"
rm -rf "${SOURCE_DIR}/build-osx"
rm -rf "${NMAKE_DIR}"
rm -rf "${SOURCE_DIR}/rttr-osx"
rm -rf "${SOURCE_DIR}/bin"
rm -rf "${SOURCE_DIR}/lib"

echo "==> Generating Xcode project (RTTR tools only, ${TOOL_ARCH})"
mkdir -p "${XCODE_DIR}"
cd "${XCODE_DIR}"
cmake "${XCODE_CMAKE_TOOL_ARGS[@]}" \
    -DTINY3D_BUILD_RTTR_TOOL=ON \
    -DTINY3D_COPY_RESOURCE=OFF \
    ../

# ============== Build ReflectionPreprocessor tool =============
echo "==> Building prerequisite tools (cct, rpp)"
cmake --build . --config Debug --parallel

CCT="$(find_tool cct)"
RPP="$(find_tool rpp)"
echo "    cct: ${CCT}"
echo "    rpp: ${RPP}"

cd "${SOURCE_DIR}"
rm -rf "${NMAKE_DIR}"
mkdir -p "${NMAKE_DIR}"
cd "${NMAKE_DIR}"

# ==================== System project =======================
echo "==> Exporting compile_commands.json (System)"
export_compile_commands TINY3D_SYSTEM_RTTR ./System \
    -UTINY3D_MATH_RTTR -UTINY3D_CORE_RTTR -UTINY3D_CORE_EDITOR_RTTR \
    -UTINY3D_METAFSARCHIVE_RTTR -UTINY3D_LAUNCHER_RTTR -UTINY3D_EDITOR_RTTR

# ==================== Math project =======================
echo "==> Exporting compile_commands.json (Math)"
export_compile_commands TINY3D_MATH_RTTR ./Math \
    -UTINY3D_SYSTEM_RTTR -UTINY3D_CORE_RTTR -UTINY3D_CORE_EDITOR_RTTR \
    -UTINY3D_METAFSARCHIVE_RTTR -UTINY3D_LAUNCHER_RTTR -UTINY3D_EDITOR_RTTR

# ==================== Core runtime project =========================
echo "==> Exporting compile_commands.json (Core/Runtime)"
export_compile_commands TINY3D_CORE_RTTR ./Core/Runtime \
    -UTINY3D_SYSTEM_RTTR -UTINY3D_MATH_RTTR -UTINY3D_CORE_EDITOR_RTTR \
    -UTINY3D_METAFSARCHIVE_RTTR -UTINY3D_LAUNCHER_RTTR -UTINY3D_EDITOR_RTTR

# ==================== Core editor project =========================
echo "==> Exporting compile_commands.json (Core/Editor)"
export_compile_commands TINY3D_CORE_EDITOR_RTTR ./Core/Editor \
    -UTINY3D_SYSTEM_RTTR -UTINY3D_MATH_RTTR -UTINY3D_CORE_RTTR \
    -UTINY3D_METAFSARCHIVE_RTTR -UTINY3D_LAUNCHER_RTTR -UTINY3D_EDITOR_RTTR

# ==================== Editor/TinyLauncher project =========================
echo "==> Exporting compile_commands.json (Editor/TinyLauncher)"
export_compile_commands TINY3D_LAUNCHER_RTTR ./Editor/TinyLauncher \
    -UTINY3D_SYSTEM_RTTR -UTINY3D_MATH_RTTR -UTINY3D_CORE_RTTR \
    -UTINY3D_CORE_EDITOR_RTTR -UTINY3D_METAFSARCHIVE_RTTR -UTINY3D_EDITOR_RTTR

# ==================== Editor/TinyEditor project =========================
echo "==> Exporting compile_commands.json (Editor/TinyEditor)"
export_compile_commands TINY3D_EDITOR_RTTR ./Editor/TinyEditor \
    -UTINY3D_SYSTEM_RTTR -UTINY3D_MATH_RTTR -UTINY3D_CORE_RTTR \
    -UTINY3D_CORE_EDITOR_RTTR -UTINY3D_METAFSARCHIVE_RTTR -UTINY3D_LAUNCHER_RTTR

# =============== Generate ReflectionSettings.json =============
mkdir -p \
    "${XCODE_DIR}/System/Generated" \
    "${XCODE_DIR}/Math/Generated" \
    "${XCODE_DIR}/Core/Runtime/Generated" \
    "${XCODE_DIR}/Core/Editor/Generated" \
    "${XCODE_DIR}/Editor/TinyLauncher/Generated" \
    "${XCODE_DIR}/Editor/TinyEditor/Generated"

echo "==> Running cct"
pids=()
"${CCT}" ./System ../xcode-osx/System/Generated & pids+=($!)
"${CCT}" ./Math ../xcode-osx/Math/Generated & pids+=($!)
"${CCT}" ./Core/Runtime ../xcode-osx/Core/Runtime/Generated & pids+=($!)
"${CCT}" ./Core/Editor ../xcode-osx/Core/Editor/Generated & pids+=($!)
"${CCT}" ./Editor/TinyLauncher ../../xcode-osx/Editor/TinyLauncher/Generated & pids+=($!)
"${CCT}" ./Editor/TinyEditor ../../xcode-osx/Editor/TinyEditor/Generated & pids+=($!)
wait_jobs "${pids[@]}"

echo "==> Running rpp"
pids=()
"${RPP}" ./System ../System -r -j 8 & pids+=($!)
"${RPP}" ./Math ../Math -r -j 8 & pids+=($!)
"${RPP}" ./Core/Runtime ../Core -r -j 8 -W "${CORE_WHITELIST}" & pids+=($!)
"${RPP}" ./Core/Editor ../Core -r -j 8 -W "${CORE_WHITELIST}" & pids+=($!)
"${RPP}" ./Editor/TinyLauncher ../Editor/TinyLauncher -r -j 8 & pids+=($!)
"${RPP}" ./Editor/TinyEditor ../Editor/TinyEditor -r -j 8 & pids+=($!)
wait_jobs "${pids[@]}"

# ==================== Generate all projects ===================
echo "==> Generating full Xcode project (${ENGINE_ARCH})"
cd "${XCODE_DIR}"
cmake "${XCODE_CMAKE_ARGS[@]}" \
    -DTINY3D_BUILD_RTTR_TOOL=OFF \
    -DTINY3D_COPY_RESOURCE=ON \
    ../

echo "==> Opening Xcode"
open Tiny3D.xcodeproj
