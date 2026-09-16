
@rem ==============================================================
@rem 走 CMake 原生反射链路的 VS2019 工程生成脚本。
@rem
@rem 与 generate-vs2019-x64-debug.bat 的区别：没有 cct、没有 nmake、
@rem 没有六次导 compile_commands.json 的往返。反射配置由 CMake 从各
@rem target 的属性直接写出，rpp 在构建期跑，产物清单在 configure 期
@rem 按源文件预声明，所以一次 configure + 一次 build 就够了。
@rem
@rem 详见 doc/todo/Reflection-Settings-CMake-Native-Design-todo.md §8 阶段 2
@rem ==============================================================

@rem ============ Setup MSVC environment (VS2019 / v142) ==========
@rem libclang 要靠 %INCLUDE% 找到 MSVC / Windows SDK 头文件。这一步
@rem 拿到的路径会被 CMake 写进 ReflectionSettings.json 的
@rem SystemIncludePath，缺了它每个翻译单元都会 file not found。
@call "%~dp0setup-msvc-env.bat" "[16.0,17.0)" 14.29
@if not %ERRORLEVEL% == 0 goto end

@cd /d "%~dp0.."

@rem ==================== 清掉上一次的产物 =========================
@rem 每次都从零开始：产物清单是 configure 期按源文件推导的，留着上一轮的
@rem 目录只会让「这个产物是这次生成的还是上次剩下的」变得说不清。
@rem 目录不存在时 rmdir 会往 stderr 刷一行，2>nul 吞掉。
@rmdir /Q /S vs2019-x64 2>nul
@rmdir /Q /S lib 2>nul
@rmdir /Q /S bin 2>nul

@rem VS 开着会占住 vs2019-x64 里的文件，rmdir 删不干净。此时若继续往下走，
@rem mkdir 失败会让 && 短路，cd 不执行，cmake 就把 source/ 当成构建目录了 ——
@rem 那会在源码树里撒一地 CMakeCache.txt，比直接报错难收拾得多。
@if exist vs2019-x64 (
	@echo.
	@echo [ERROR] vs2019-x64 delete failed. Close Visual Studio and retry.
	@cd Projects
	@goto end
)

@mkdir vs2019-x64
@cd vs2019-x64

@rem ==================== Configure ===============================
@rem TINY3D_BUILD_RTTR_TOOL=OFF：rpp 与引擎模块在同一个 sln 里，各模块
@rem 的反射 target 依赖 ReflectionPreprocessor，构建顺序由 CMake 保证，
@rem 不必再单独先构建一轮工具。
@cmake -G "Visual Studio 16 2019" -DTINY3D_BUILD_RTTR_TOOL=OFF -DTINY3D_COPY_RESOURCE=ON -DCMAKE_BUILD_TYPE=Debug ../
@if not %ERRORLEVEL% == 0 (
	@cd ../Projects
	@goto end
)

@rem ==================== Build ===================================
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
