#!/bin/bash
# Refresh existing Xcode project for macOS.
# Mirrors refresh-vs2019-x64.bat: re-run cmake in xcode-osx without regenerating RTTR.
#
# Usage: ./refresh-xcode-osx.sh

set -euo pipefail

cd "$(dirname "$0")/.."
SOURCE_DIR="$(pwd)"
XCODE_DIR="${SOURCE_DIR}/xcode-osx"

if [ ! -d "${XCODE_DIR}" ]; then
    echo "ERROR: ${XCODE_DIR} does not exist. Run generate-xcode-osx.sh first." >&2
    exit 1
fi

cd "${XCODE_DIR}"
cmake -G Xcode \
    -DCMAKE_OSX_ARCHITECTURES=x86_64 \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=13.0 \
    -DCMAKE_BUILD_TYPE=Debug \
    -DTINY3D_BUILD_RTTR_TOOL=OFF \
    -DTINY3D_INCREMENTAL_RTTR=ON \
    ./

cd "${SOURCE_DIR}/Projects"
