cd ../
rm -rf xcode-ios
rm -rf bin
rm -rf lib
mkdir -p xcode-ios && cd xcode-ios
cmake -GXcode -DCMAKE_TOOLCHAIN_FILE=../CMake/Toolchains/iOS.cmake -DIOS_PLATFORM=OS -DCMAKE_OSX_ARCHITECTURES=arm64 ../
open Tiny3D.xcodeproj
cd ..
# cmake --build build_ios --config Release