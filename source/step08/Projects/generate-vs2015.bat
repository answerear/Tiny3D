@cd ..
@rmdir /Q /S vs2015
@rmdir /Q /S lib
@rmdir /Q /S bin
@mkdir vs2015 && cd vs2015
@cmake -G "Visual Studio 14 2015" ../

@if %ERRORLEVEL% == 0 (
    @devenv Tiny3D.sln
    @cd ../Projects
) else (
    @cd ../Projects
    @pause
)
