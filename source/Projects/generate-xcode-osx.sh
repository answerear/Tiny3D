cd ../
rm -rf xcode-osx
mkdir -p xcode-osx && cd xcode-osx
CMAKE_C_COMPILER=$(xcrun -find cc)
CMAKE_CXX_COMPILER=$(xcrun -find c++)
cmake -D CMAKE_CXX_COMPILER=$CMAKE_CXX_COMPILER -D CMAKE_C_COMPILER=$CMAKE_C_COMPILER -G "Xcode" ../
open Tiny3D.xcodeproj
cd ..
# cmake --build build_ios --config Release
