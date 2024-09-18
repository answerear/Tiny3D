
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
@cmake -G "NMake Makefiles" -DTINY3D_SYSTEM_RTTR=ON -UTINY3D_MATH_RTTR -UTINY3D_CORE_RTTR -UTINY3D_CORE_EDITOR_RTTR -UTINY3D_METAFSARCHIVE_RTTR -UTINY3D_LAUNCHER_RTTR -DCMAKE_BUILD_TYPE=Debug ../
@if not %ERRORLEVEL% == 0 (
	@cd ../Projects
	@goto end
)
@rem Copy System compile commands to the project folder.
xcopy compile_commands.json .\System 

@rem ==================== Math project =======================
@cmake -G "NMake Makefiles" -DTINY3D_MATH_RTTR=ON -UTINY3D_SYSTEM_RTTR -UTINY3D_CORE_RTTR -UTINY3D_CORE_EDITOR_RTTR -UTINY3D_METAFSARCHIVE_RTTR -UTINY3D_LAUNCHER_RTTR -DCMAKE_BUILD_TYPE=Debug ../
@if not %ERRORLEVEL% == 0 (
	@cd ../Projects
	@goto end
)
@rem Copy Math compile commands to the project folder.
xcopy compile_commands.json .\Math 

@rem ==================== Core runtime project =========================
@cmake -G "NMake Makefiles" -DTINY3D_CORE_RTTR=ON -UTINY3D_SYSTEM_RTTR -UTINY3D_MATH_RTTR -UTINY3D_CORE_EDITOR_RTTR -UTINY3D_METAFSARCHIVE_RTTR -UTINY3D_LAUNCHER_RTTR -DCMAKE_BUILD_TYPE=Debug ../
@if not %ERRORLEVEL% == 0 (
	@cd ../Projects
	@goto end
)
@rem Copy Core compile commands to the project folder.
xcopy compile_commands.json .\Core\Runtime 

@rem ==================== Core editor project =========================
@cmake -G "NMake Makefiles" -DTINY3D_CORE_EDITOR_RTTR=ON -UTINY3D_SYSTEM_RTTR -UTINY3D_MATH_RTTR -UTINY3D_CORE_RTTR -UTINY3D_METAFSARCHIVE_RTTR -UTINY3D_LAUNCHER_RTTR -DCMAKE_BUILD_TYPE=Debug ../
@if not %ERRORLEVEL% == 0 (
	@cd ../Projects
	@goto end
)
@rem Copy Core compile commands to the project folder.
xcopy compile_commands.json .\Core\Editor 

@rem ==================== MetaFSArchive project =========================
rem @cmake -G "NMake Makefiles" -DTINY3D_METAFSARCHIVE_RTTR=ON -UTINY3D_SYSTEM_RTTR -UTINY3D_MATH_RTTR -UTINY3D_CORE_RTTR -UTINY3D_CORE_EDITOR_RTTR -UTINY3D_LAUNCHER_RTTR -DCMAKE_BUILD_TYPE=Debug ../
rem @if not %ERRORLEVEL% == 0 (
rem 	@cd ../Projects
rem 	@goto end
rem )
@rem Copy Core compile commands to the project folder.
rem xcopy compile_commands.json .\Plugins\Archive\MetaFileSystem 

@rem ==================== Editor/TinyLauncher project =========================
@cmake -G "NMake Makefiles" -DTINY3D_LAUNCHER_RTTR=ON -UTINY3D_SYSTEM_RTTR -UTINY3D_MATH_RTTR -UTINY3D_CORE_RTTR -UTINY3D_CORE_EDITOR_RTTR -UTINY3D_METAFSARCHIVE_RTTR -DCMAKE_BUILD_TYPE=Debug ../
@if not %ERRORLEVEL% == 0 (
	@cd ../Projects
	@goto end
)
@rem Copy Editor/TinyLauncher compile commands to the project folder.
xcopy compile_commands.json .\Editor\TinyLauncher 


@rem =============== Generate ReflectionSettings.json =============
@mkdir ..\vs2019-x64\System\Generated
..\bin\Windows\Debug\cct.exe .\System ..\vs2019-x64\System\Generated
@mkdir ..\vs2019-x64\Math\Generated
..\bin\Windows\Debug\cct.exe .\Math ..\vs2019-x64\Math\Generated
@mkdir ..\vs2019-x64\Core\Runtime\Generated
..\bin\Windows\Debug\cct.exe .\Core\Runtime ..\vs2019-x64\Core\Runtime\Generated
@mkdir ..\vs2019-x64\Core\Editor\Generated
..\bin\Windows\Debug\cct.exe .\Core\Editor ..\vs2019-x64\Core\Editor\Generated
rem @mkdir ..\vs2019-x64\Plugins\Archive\MetaFileSystem\Generated
rem ..\bin\Windows\Debug\cct.exe .\Plugins\Archive\MetaFileSystem ..\..\..\vs2019-x64\Plugins\Archive\MetaFileSystem\Generated
@mkdir ..\vs2019-x64\Editor\TinyLauncher\Generated
..\bin\Windows\Debug\cct.exe .\Editor\TinyLauncher ..\..\vs2019-x64\Editor\TinyLauncher\Generated


@rem Generate reflection source by ReflectionPreprocessor.
..\bin\Windows\Debug\rpp.exe .\System ..\System -r
..\bin\Windows\Debug\rpp.exe .\Math ..\Math -r
..\bin\Windows\Debug\rpp.exe .\Core\Runtime ..\Core -r
..\bin\Windows\Debug\rpp.exe .\Core\Editor ..\Core -r
rem ..\bin\Windows\Debug\rpp.exe .\Plugins\Archive\MetaFileSystem ..\Plugins\Archive\MetaFileSystem -r
..\bin\Windows\Debug\rpp.exe .\Editor\TinyLauncher ..\Editor\TinyLauncher -r


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





