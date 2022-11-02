@cd ..
@rmdir /Q /S nmake
@rmdir /Q /S lib
@rmdir /Q /S bin
@mkdir nmake && cd nmake
@cmake -G "NMake Makefiles" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug ../

@if %ERRORLEVEL% == 0 (
@rem     @devenv Tiny3D.sln
@rem     @cd ../Projects
) else (
    @cd ../Projects
    @pause
)