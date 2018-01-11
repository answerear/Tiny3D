cd ..
rmdir /Q /S vs2015
mkdir vs2015 && cd vs2015
cmake -G "Visual Studio 14 2015" ../
cd ../Projects
