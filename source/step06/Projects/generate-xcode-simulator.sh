cd ../
rm -rf xcode-simulator
mkdir -p xcode-simulator && cd xcode-simulator
cmake -GXcode -DCMAKE_TOOLCHAIN_FILE=../CMake/Toolchains/iOS.cmake -DIOS_PLATFORM=SIMULATOR -DCMAKE_OSX_ARCHITECTURES=x86_64 ../
open Tiny3D.xcodeproj
cd ..
# cmake --build build_ios --config Release