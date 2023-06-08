
@rem ================= Generate base projects =====================
@cd ..
@rmdir /Q /S vs2019-x64
@rmdir /Q /S lib
@rmdir /Q /S bin
@mkdir vs2019-x64 && cd vs2019-x64
@cmake -G "Visual Studio 16 2019" -DTINY3D_BUILD_RTTR_TOOL=ON -DCMAKE_BUILD_TYPE=Debug ../
@if not %ERRORLEVEL% == 0 (
	@cd ../Projects
	@goto end
)

@rem ============== Build ReflectionPreprocessor tool =============
@cmake --build ./
@if not %ERRORLEVEL% == 0 (
	@cd ../Projects
	@goto end
)

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
@mkdir ..\vs2019-x64\System\Generated
..\bin\Windows\Debug\cct.exe .\System ..\vs2019-x64\System\Generated
@mkdir ..\vs2019-x64\Math\Generated
..\bin\Windows\Debug\cct.exe .\Math ..\vs2019-x64\Math\Generated
@mkdir ..\vs2019-x64\Core\Generated
..\bin\Windows\Debug\cct.exe .\Core ..\vs2019-x64\Core\Generated


@rem Generate reflection source by ReflectionPreprocessor.
..\bin\Windows\Debug\ReflectionPreprocessor.exe .\System ..\System
..\bin\Windows\Debug\ReflectionPreprocessor.exe .\Math ..\Math
..\bin\Windows\Debug\ReflectionPreprocessor.exe .\Core ..\Core


@rem ==================== Generate all projects ===================
@cd ../vs2019-x64
@cmake -G "Visual Studio 16 2019" -DTINY3D_BUILD_RTTR_TOOL=OFF -DCMAKE_BUILD_TYPE=Debug ../


@if %ERRORLEVEL% == 0 (
	@cmake --open ../vs2019-x64
    @cd ../Projects
) else (
    @cd ../Projects
    @pause
)

:end





