
@rem ==============================================================
@rem VS2019 Debug 全量构建：configure + cmake --build，产出引擎、工具和 Samples。
@rem
@rem 日常开工程请用 generate-vs2019-x64-debug.bat（只生成 sln，不编产物）。
@rem ==============================================================

@call "%~dp0setup-msvc-env.bat" "[16.0,17.0)" 14.29
@if not %ERRORLEVEL% == 0 goto end

@cd /d "%~dp0.."

@rmdir /Q /S vs2019-x64 2>nul
@rmdir /Q /S lib 2>nul
@rmdir /Q /S bin 2>nul

@if exist vs2019-x64 (
	@echo.
	@echo [ERROR] vs2019-x64 delete failed. Close Visual Studio and retry.
	@cd Projects
	@goto end
)

@mkdir vs2019-x64
@cd vs2019-x64

@cmake -G "Visual Studio 16 2019" -DTINY3D_BUILD_RTTR_TOOL=OFF -DTINY3D_COPY_RESOURCE=ON -DCMAKE_BUILD_TYPE=Debug ../
@if not %ERRORLEVEL% == 0 (
	@cd ../Projects
	@goto end
)

@rem 不加 -m：ResourceApp 与 SkyboxApp 的 POST_BUILD 都会用 bundlebuilder
@rem 重打同一份 assets/samples/bundle 再拷到同一个运行目录，并行构建下
@rem 两者互相踩会报 MSB3073。与反射链路无关，但会让构建莫名失败。
@cmake --build . --config Debug

@if %ERRORLEVEL% == 0 (
	@cmake --open ../vs2019-x64
	@cd ../Projects
) else (
	@cd ../Projects
	@pause
)

:end
