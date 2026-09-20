
@rem ==============================================================
@rem VS2019 工程生成。反射配置由 CMake 从 target 属性写出，rpp 在构建期
@rem 跑，产物清单在 configure 期按源文件预声明。一次 configure 即可，
@rem 不再经过 cct / nmake / compile_commands.json。
@rem
@rem 详见 doc/todo/Reflection-Settings-CMake-Native-Design-todo.md
@rem ==============================================================

@rem ============ Setup MSVC environment (VS2019 / v142) ==========
@rem libclang 要靠 %INCLUDE% 找到 MSVC / Windows SDK 头文件。这一步
@rem 拿到的路径会被 CMake 写进 ReflectionSettings.json 的
@rem SystemIncludePath，缺了它每个翻译单元都会 file not found。
@call "%~dp0setup-msvc-env.bat" "[16.0,17.0)" 14.29
@if not %ERRORLEVEL% == 0 goto end

@cd /d "%~dp0.."

@rem ==================== 清掉上一次的产物 =========================
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

@rem TINY3D_BUILD_RTTR_TOOL=OFF：rpp 与引擎模块在同一个 sln 里，各模块
@rem 的反射 target 依赖 ReflectionPreprocessor，构建顺序由 CMake 保证。
@cmake -G "Visual Studio 16 2019" -DTINY3D_BUILD_RTTR_TOOL=OFF -DTINY3D_COPY_RESOURCE=ON -DCMAKE_BUILD_TYPE=Debug ../
@if not %ERRORLEVEL% == 0 (
	@cd ../Projects
	@goto end
)

@if %ERRORLEVEL% == 0 (
	@cmake --open ../vs2019-x64
	@cd ../Projects
) else (
	@cd ../Projects
	@pause
)

:end
