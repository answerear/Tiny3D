cd ../
rm -rf xcode-osx
rm -rf bin
rm -rf lib
mkdir -p xcode-osx && cd xcode-osx
CMAKE_C_COMPILER=$(xcrun -find cc)
CMAKE_CXX_COMPILER=$(xcrun -find c++)
cmake -DCMAKE_CXX_COMPILER=$CMAKE_CXX_COMPILER -DCMAKE_C_COMPILER=$CMAKE_C_COMPILER -DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM=8GEE4MA6EY -G "Xcode" ../
open Tiny3D.xcodeproj
cd ..
# cmake --build build_ios --config Release
