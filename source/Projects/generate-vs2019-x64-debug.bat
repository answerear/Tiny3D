
@rem ================= Generate base projects =====================
@cd ..
@rmdir /Q /S vs2019-x64
@rmdir /Q /S lib
@rmdir /Q /S bin
@mkdir vs2019-x64 && cd vs2019-x64
@cmake -G "Visual Studio 16 2019" -DTINY3D_BUILD_RTTR_TOOL=ON -DTINY3D_COPY_RESOURCE=OFF -DCMAKE_BUILD_TYPE=Debug ../
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
@cmake -G "NMake Makefiles" -DTINY3D_SYSTEM_RTTR=ON -UTINY3D_MATH_RTTR -UTINY3D_CORE_RTTR -UTINY3D_CORE_EDITOR_RTTR -UTINY3D_METAFSARCHIVE_RTTR -UTINY3D_LAUNCHER_RTTR -UTINY3D_EDITOR_RTTR  -DTINY3D_COPY_RESOURCE=OFF -DCMAKE_BUILD_TYPE=Debug ../
@if not %ERRORLEVEL% == 0 (
	@cd ../Projects
	@goto end
)
@rem Copy System compile commands to the project folder.
xcopy compile_commands.json .\System 

@rem ==================== Math project =======================
@cmake -G "NMake Makefiles" -DTINY3D_MATH_RTTR=ON -UTINY3D_SYSTEM_RTTR -UTINY3D_CORE_RTTR -UTINY3D_CORE_EDITOR_RTTR -UTINY3D_METAFSARCHIVE_RTTR -UTINY3D_LAUNCHER_RTTR -UTINY3D_EDITOR_RTTR  -DTINY3D_COPY_RESOURCE=OFF -DCMAKE_BUILD_TYPE=Debug ../
@if not %ERRORLEVEL% == 0 (
	@cd ../Projects
	@goto end
)
@rem Copy Math compile commands to the project folder.
xcopy compile_commands.json .\Math 

@rem ==================== Core runtime project =========================
@cmake -G "NMake Makefiles" -DTINY3D_CORE_RTTR=ON -UTINY3D_SYSTEM_RTTR -UTINY3D_MATH_RTTR -UTINY3D_CORE_EDITOR_RTTR -UTINY3D_METAFSARCHIVE_RTTR -UTINY3D_LAUNCHER_RTTR -UTINY3D_EDITOR_RTTR  -DTINY3D_COPY_RESOURCE=OFF -DCMAKE_BUILD_TYPE=Debug ../
@if not %ERRORLEVEL% == 0 (
	@cd ../Projects
	@goto end
)
@rem Copy Core compile commands to the project folder.
xcopy compile_commands.json .\Core\Runtime 

@rem ==================== Core editor project =========================
@cmake -G "NMake Makefiles" -DTINY3D_CORE_EDITOR_RTTR=ON -UTINY3D_SYSTEM_RTTR -UTINY3D_MATH_RTTR -UTINY3D_CORE_RTTR -UTINY3D_METAFSARCHIVE_RTTR -UTINY3D_LAUNCHER_RTTR -UTINY3D_EDITOR_RTTR  -DTINY3D_COPY_RESOURCE=OFF -DCMAKE_BUILD_TYPE=Debug ../
@if not %ERRORLEVEL% == 0 (
	@cd ../Projects
	@goto end
)
@rem Copy Core compile commands to the project folder.
xcopy compile_commands.json .\Core\Editor 

@rem ==================== MetaFSArchive project =========================
@rem @cmake -G "NMake Makefiles" -DTINY3D_METAFSARCHIVE_RTTR=ON -UTINY3D_SYSTEM_RTTR -UTINY3D_MATH_RTTR -UTINY3D_CORE_RTTR -UTINY3D_CORE_EDITOR_RTTR -UTINY3D_LAUNCHER_RTTR -DCMAKE_BUILD_TYPE=Debug ../
@rem @if not %ERRORLEVEL% == 0 (
@rem 	@cd ../Projects
@rem 	@goto end
@rem )
@rem Copy Core compile commands to the project folder.
@rem xcopy compile_commands.json .\Plugins\Archive\MetaFileSystem 

@rem ==================== Editor/TinyLauncher project =========================
@cmake -G "NMake Makefiles" -DTINY3D_LAUNCHER_RTTR=ON -UTINY3D_SYSTEM_RTTR -UTINY3D_MATH_RTTR -UTINY3D_CORE_RTTR -UTINY3D_CORE_EDITOR_RTTR -UTINY3D_METAFSARCHIVE_RTTR -UTINY3D_EDITOR_RTTR  -DTINY3D_COPY_RESOURCE=OFF -DCMAKE_BUILD_TYPE=Debug ../
@if not %ERRORLEVEL% == 0 (
	@cd ../Projects
	@goto end
)
@rem Copy Editor/TinyLauncher compile commands to the project folder.
xcopy compile_commands.json .\Editor\TinyLauncher 


@rem ==================== Editor/TinyEditor project =========================
@cmake -G "NMake Makefiles" -DTINY3D_EDITOR_RTTR=ON -UTINY3D_SYSTEM_RTTR -UTINY3D_MATH_RTTR -UTINY3D_CORE_RTTR -UTINY3D_CORE_EDITOR_RTTR -UTINY3D_METAFSARCHIVE_RTTR -UTINY3D_LAUNCHER_RTTR  -DTINY3D_COPY_RESOURCE=OFF -DCMAKE_BUILD_TYPE=Debug ../
@if not %ERRORLEVEL% == 0 (
	@cd ../Projects
	@goto end
)
@rem Copy Editor/TinyEditor compile commands to the project folder.
xcopy compile_commands.json .\Editor\TinyEditor 


@rem =============== Generate ReflectionSettings.json =============
@mkdir ..\vs2019-x64\System\Generated
@mkdir ..\vs2019-x64\Math\Generated
@mkdir ..\vs2019-x64\Core\Runtime\Generated
@mkdir ..\vs2019-x64\Core\Editor\Generated
@mkdir ..\vs2019-x64\Editor\TinyLauncher\Generated
@mkdir ..\vs2019-x64\Editor\TinyEditor\Generated

@rem Parallel cct.exe
powershell -NoProfile -Command "& { $p = @(); $p += Start-Process '..\bin\Windows\Debug\cct.exe' -ArgumentList '.\System','..\vs2019-x64\System\Generated' -NoNewWindow -PassThru; $p += Start-Process '..\bin\Windows\Debug\cct.exe' -ArgumentList '.\Math','..\vs2019-x64\Math\Generated' -NoNewWindow -PassThru; $p += Start-Process '..\bin\Windows\Debug\cct.exe' -ArgumentList '.\Core\Runtime','..\vs2019-x64\Core\Runtime\Generated' -NoNewWindow -PassThru; $p += Start-Process '..\bin\Windows\Debug\cct.exe' -ArgumentList '.\Core\Editor','..\vs2019-x64\Core\Editor\Generated' -NoNewWindow -PassThru; $p += Start-Process '..\bin\Windows\Debug\cct.exe' -ArgumentList '.\Editor\TinyLauncher','..\..\vs2019-x64\Editor\TinyLauncher\Generated' -NoNewWindow -PassThru; $p += Start-Process '..\bin\Windows\Debug\cct.exe' -ArgumentList '.\Editor\TinyEditor','..\..\vs2019-x64\Editor\TinyEditor\Generated' -NoNewWindow -PassThru; $p | Wait-Process }"


@rem Generate reflection source by ReflectionPreprocessor (parallel).
powershell -NoProfile -Command "& { $p = @(); $p += Start-Process '..\bin\Windows\Debug\rpp.exe' -ArgumentList '.\System','..\System','-r','-j','8' -NoNewWindow -PassThru; $p += Start-Process '..\bin\Windows\Debug\rpp.exe' -ArgumentList '.\Math','..\Math','-r','-j','8' -NoNewWindow -PassThru; $p += Start-Process '..\bin\Windows\Debug\rpp.exe' -ArgumentList '.\Core\Runtime','..\Core','-r','-j','8','-W','Tiny3D::TAabb;Tiny3D::TDegree;Tiny3D::TFrustum;Tiny3D::TMatrix2;Tiny3D::TMatrix3;Tiny3D::TMatrix4;Tiny3D::TObb;Tiny3D::TPlane;Tiny3D::TQuaternion;Tiny3D::TRadian;Tiny3D::TRay;Tiny3D::TSize;Tiny3D::TPoint;Tiny3D::TRect;Tiny3D::TSphere;Tiny3D::TTriangle;Tiny3D::TVector2;Tiny3D::TVector3;Tiny3D::TVector4;' -NoNewWindow -PassThru; $p += Start-Process '..\bin\Windows\Debug\rpp.exe' -ArgumentList '.\Core\Editor','..\Core','-r','-j','8','-W','Tiny3D::TAabb;Tiny3D::TDegree;Tiny3D::TFrustum;Tiny3D::TMatrix2;Tiny3D::TMatrix3;Tiny3D::TMatrix4;Tiny3D::TObb;Tiny3D::TPlane;Tiny3D::TQuaternion;Tiny3D::TRadian;Tiny3D::TRay;Tiny3D::TSize;Tiny3D::TPoint;Tiny3D::TRect;Tiny3D::TSphere;Tiny3D::TTriangle;Tiny3D::TVector2;Tiny3D::TVector3;Tiny3D::TVector4;' -NoNewWindow -PassThru; $p += Start-Process '..\bin\Windows\Debug\rpp.exe' -ArgumentList '.\Editor\TinyLauncher','..\Editor\TinyLauncher','-r','-j','8' -NoNewWindow -PassThru; $p += Start-Process '..\bin\Windows\Debug\rpp.exe' -ArgumentList '.\Editor\TinyEditor','..\Editor\TinyEditor','-r','-j','8' -NoNewWindow -PassThru; $p | Wait-Process }"


@rem ==================== Generate all projects ===================
@cd ../vs2019-x64
@cmake -G "Visual Studio 16 2019" -DTINY3D_BUILD_RTTR_TOOL=OFF -DTINY3D_COPY_RESOURCE=ON -DCMAKE_BUILD_TYPE=Debug ../


@if %ERRORLEVEL% == 0 (
	@cmake --open ../vs2019-x64
    @cd ../Projects
) else (
    @cd ../Projects
    @pause
)

:end





