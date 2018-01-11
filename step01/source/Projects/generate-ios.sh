cd ../
rm -rf build-simulator
mkdir -p build-simulator && cd build-simulator
cmake -GXcode -DCMAKE_TOOLCHAIN_FILE=../CMake/Toolchains/iOS.cmake -DIOS_PLATFORM=OS -DCMAKE_OSX_ARCHITECTURES=arm64 ../
open Tiny3D.xcodeproj
cd ..
# cmake --build build_ios --config Release