
@rem ==============================================================
@rem VS2022 工程生成。反射配置由 CMake 从 target 属性写出，rpp 在构建期
@rem 跑，产物清单在 configure 期按源文件预声明。一次 configure 即可。
@rem ==============================================================

@call "%~dp0setup-msvc-env.bat" "[17.0,18.0)"
@if not %ERRORLEVEL% == 0 goto end

@cd /d "%~dp0.."

@rmdir /Q /S vs2022-x64 2>nul
@rmdir /Q /S lib 2>nul
@rmdir /Q /S bin 2>nul

@if exist vs2022-x64 (
	@echo.
	@echo [ERROR] vs2022-x64 delete failed. Close Visual Studio and retry.
	@cd Projects
	@goto end
)

@mkdir vs2022-x64
@cd vs2022-x64

@cmake -G "Visual Studio 17 2022" -A x64 -DTINY3D_BUILD_RTTR_TOOL=OFF -DTINY3D_COPY_RESOURCE=ON -DCMAKE_BUILD_TYPE=Debug ../
@if not %ERRORLEVEL% == 0 (
	@cd ../Projects
	@goto end
)

@if %ERRORLEVEL% == 0 (
	@cmake --open ../vs2022-x64
	@cd ../Projects
) else (
	@cd ../Projects
	@pause
)

:end
