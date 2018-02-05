cd ../
rm -rf xcode-osx
mkdir -p xcode-osx && cd xcode-osx
cmake -G "Xcode" ../
open Tiny3D.xcodeproj
cd ..
# cmake --build build_ios --config Release
