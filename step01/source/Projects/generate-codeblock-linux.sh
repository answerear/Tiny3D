cd ../
rm -rf codeblocks-linux
mkdir -p codeblocks-linux && cd codeblocks-linux
cmake -G "CodeBlocks - Unix Makefiles" ../
codeblocks Tiny3D.cbp

