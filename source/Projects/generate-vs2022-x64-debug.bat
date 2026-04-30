
@rem ================= Generate base projects =====================
@cd ..
@rmdir /Q /S vs2022-x64
@rmdir /Q /S lib
@rmdir /Q /S bin
@mkdir vs2022-x64 && cd vs2022-x64
@cmake -G "Visual Studio 17 2022" -A x64 -DTINY3D_BUILD_RTTR_TOOL=ON -DCMAKE_BUILD_TYPE=Debug ../

@rem ============== Build ReflectionPreprocessor tool =============
@cmake --build ./

@cd ..
@rmdir /Q /S nmake
@mkdir nmake && cd nmake

@rem ==================== System project =======================
@cmake -G "NMake Makefiles" -DTINY3D_SYSTEM_RTTR=ON -UTINY3D_MATH_RTTR -UTINY3D_CORE_RTTR -DCMAKE_BUILD_TYPE=Debug ../
@if not %ERRORLEVEL% == 0 (
	@cd ../Projects
	@goto end
)
@rem Copy System compile commands to the project folder.
xcopy compile_commands.json .\System 

@rem ==================== Math project =======================
@cmake -G "NMake Makefiles" -DTINY3D_MATH_RTTR=ON -UTINY3D_SYSTEM_RTTR -UTINY3D_CORE_RTTR -DCMAKE_BUILD_TYPE=Debug ../
@if not %ERRORLEVEL% == 0 (
	@cd ../Projects
	@goto end
)
@rem Copy Math compile commands to the project folder.
xcopy compile_commands.json .\Math 

@rem ==================== Core project =========================
@cmake -G "NMake Makefiles" -DTINY3D_CORE_RTTR=ON -UTINY3D_SYSTEM_RTTR -UTINY3D_MATH_RTTR -DCMAKE_BUILD_TYPE=Debug ../
@if not %ERRORLEVEL% == 0 (
	@cd ../Projects
	@goto end
)
@rem Copy Core compile commands to the project folder.
xcopy compile_commands.json .\Core 


@rem =============== Generate ReflectionSettings.json =============
@mkdir ..\vs2022-x64\System\Generated
@mkdir ..\vs2022-x64\Math\Generated
@mkdir ..\vs2022-x64\Core\Generated

@rem Parallel cct.exe
powershell -NoProfile -Command "& { $p = @(); $p += Start-Process '..\bin\Windows\Debug\cct.exe' -ArgumentList '.\System','..\vs2022-x64\System\Generated' -NoNewWindow -PassThru; $p += Start-Process '..\bin\Windows\Debug\cct.exe' -ArgumentList '.\Math','..\vs2022-x64\Math\Generated' -NoNewWindow -PassThru; $p += Start-Process '..\bin\Windows\Debug\cct.exe' -ArgumentList '.\Core','..\vs2022-x64\Core\Generated' -NoNewWindow -PassThru; $p | Wait-Process }"


@rem Generate reflection source by ReflectionPreprocessor (parallel).
powershell -NoProfile -Command "& { $p = @(); $p += Start-Process '..\bin\Windows\Debug\ReflectionPreprocessor.exe' -ArgumentList '.\System','..\System','-j','8' -NoNewWindow -PassThru; $p += Start-Process '..\bin\Windows\Debug\ReflectionPreprocessor.exe' -ArgumentList '.\Math','..\Math','-j','8' -NoNewWindow -PassThru; $p += Start-Process '..\bin\Windows\Debug\ReflectionPreprocessor.exe' -ArgumentList '.\Core','..\Core','-j','8' -NoNewWindow -PassThru; $p | Wait-Process }"


@rem ==================== Generate all projects ===================
@cd ../vs2022-x64
@cmake -G "Visual Studio 17 2022" -A x64 -DTINY3D_BUILD_RTTR_TOOL=OFF -DCMAKE_BUILD_TYPE=Debug ../


@if %ERRORLEVEL% == 0 (
	@cmake --open ../vs2022-x64
    @cd ../Projects
) else (
    @cd ../Projects
    @pause
)

:end





