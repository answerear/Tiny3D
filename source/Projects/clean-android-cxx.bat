@echo off
@rem ==============================================================================
@rem Delete every Android <module>/.cxx build tree.
@rem
@rem   clean-android-cxx.bat [-y]
@rem     -y   delete without asking
@rem
@rem .cxx is where Android Studio keeps the per ABI CMake build tree, including
@rem CMakeCache.txt. Several reflection related variables are cached there
@rem (TINY3D_SYSTEM_INCLUDE_DIRS_CACHED guards the toolchain header detection), so
@rem a tree configured by an older engine revision keeps replaying stale values
@rem instead of re-detecting. Removing .cxx forces a clean configure - that is why
@rem this one kind has its own script, separate from clean-android.bat.
@rem
@rem Covered: source/Platform/Android (the engine module tree) plus every Sample.
@rem
@rem Unlike .gradle and libs, .cxx needs no git safety check: it is pure ninja /
@rem CMake output, it is listed in every module .gitignore, and no .cxx file is
@rem tracked anywhere in the repository. clean-android.bat guards the kinds where
@rem that is not true.
@rem
@rem Gradle recreates all of it on the next externalNativeBuild. Expect the trees
@rem to be large - a few GB each.
@rem
@rem Keep this file ASCII only: cmd.exe decodes .bat files with the OEM code page.
@rem ==============================================================================

setlocal EnableDelayedExpansion

for %%i in ("%~dp0..") do set "SOURCE_DIR=%%~fi"
if not exist "%SOURCE_DIR%\Samples" (
    echo ERROR: Samples directory not found at "%SOURCE_DIR%\Samples".
    exit /b 1
)

set "ASSUME_YES="
if /i "%~1" == "-y" set "ASSUME_YES=1"
if /i "%~1" == "/y" set "ASSUME_YES=1"

set /a FOUND=0
echo Scanning for Android module/.cxx under "%SOURCE_DIR%" ...
echo.
call :for_each_root :scan

echo.
if %FOUND% == 0 (
    echo Nothing to do: no .cxx directory found.
    exit /b 0
)
echo %FOUND% directory^(ies^) to delete.

if not defined ASSUME_YES (
    echo.
    set /p "ANSWER=Delete them? [y/N] "
    if /i not "!ANSWER!" == "y" (
        echo Aborted, nothing was deleted.
        exit /b 1
    )
)

echo.
set /a FAILED=0
call :for_each_root :clean

echo.
if %FAILED% == 0 (
    echo Done.
    exit /b 0
)
echo %FAILED% directory^(ies^) could not be removed.
exit /b 1

@rem ------------------------------------------------------------------------------
@rem Call %1 once per Android project root, as: <label> "<abs path>" "<label path>".
@rem A root is a directory holding the top level build.gradle; its modules are the
@rem subdirectories that hold one too.
@rem ------------------------------------------------------------------------------
:for_each_root
if exist "%SOURCE_DIR%\Platform\Android\build.gradle" (
    call %~1 "%SOURCE_DIR%\Platform\Android" "Platform\Android"
)
for /d %%d in ("%SOURCE_DIR%\Samples\*") do (
    if exist "%%~fd\Android\build.gradle" call %~1 "%%~fd\Android" "%%~nxd\Android"
)
exit /b 0

@rem ------------------------------------------------------------------------------
:scan
for /d %%m in ("%~1\*") do (
    if exist "%%~fm\build.gradle" if exist "%%~fm\.cxx\" (
        set /a FOUND+=1
        echo   %~2\%%~nxm\.cxx
    )
)
exit /b 0

@rem ------------------------------------------------------------------------------
:clean
for /d %%m in ("%~1\*") do (
    if exist "%%~fm\build.gradle" if exist "%%~fm\.cxx\" (
        echo Removing %~2\%%~nxm\.cxx
        rd /s /q "%%~fm\.cxx"
        @rem Android Studio and the gradle daemon keep handles on these trees, so
        @rem rd can delete part of one and still report success. Re-check instead
        @rem of claiming a clean tree - that state is far harder to diagnose later.
        if exist "%%~fm\.cxx\" (
            set /a FAILED+=1
            echo   [ERROR] delete failed. Close Android Studio, stop the gradle daemon, retry.
        )
    )
)
exit /b 0
