@cd ..
@rmdir /Q /S ninja
@rmdir /Q /S lib
@rmdir /Q /S bin
@mkdir ninja && cd ninja
@cmake -G "Ninja" -DCMAKE_MAKE_PROGRAM="../../tools/ninja/ninja.exe" -DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=cl -DCMAKE_BUILD_TYPE=Debug ../

@if %ERRORLEVEL% == 0 (
@rem     @devenv Tiny3D.sln
@rem     @cd ../Projects
) else (
    @cd ../Projects
    @pause
)


