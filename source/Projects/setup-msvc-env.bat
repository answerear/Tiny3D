@echo off
@rem ==============================================================================
@rem Prepare the MSVC build environment for the generate-*.bat scripts.
@rem
@rem   call setup-msvc-env.bat <vswhere version range> [vcvars_ver] [-refresh]
@rem   e.g. call setup-msvc-env.bat "[16.0,17.0)" 14.29
@rem
@rem Why this is needed: the libclang that rpp uses only looks at %INCLUDE% to find
@rem the MSVC and Windows SDK headers; its registry based auto detection does not
@rem work. A build started by double clicking a .bat has no such environment, so
@rem reflection parsing dies with "'WinSock.h' file not found" on every file.
@rem This script sets the environment up explicitly and pins the toolset to the one
@rem matching the CMake generator, so we never parse with one version of the headers
@rem and compile with another. cct then bakes %INCLUDE% into ReflectionSettings.json
@rem so that builds without an MSVC environment (Gradle, plain shells) can run rpp.
@rem
@rem vcvarsall.bat costs a few seconds every time, which is pure overhead for the
@rem generate scripts that call this on every run. So the resulting environment is
@rem recorded once into .msvc-env-<key>.cmd next to this script and replayed on the
@rem following runs. Pass -refresh (or set T3D_MSVC_ENV_REFRESH=1) to rebuild it;
@rem deleting the file works too.
@rem
@rem Keep this file ASCII only: cmd.exe decodes .bat files with the OEM code page.
@rem Note the version range contains a ')', so it must stay quoted everywhere.
@rem ==============================================================================

@rem Already inside a developer prompt: keep what is there. Running vcvarsall on
@rem top of an initialized environment is what breaks things, because VsDevCmd.bat
@rem reuses the inherited VSINSTALLDIR and then looks for its own helpers under
@rem someone else's Visual Studio install.
if defined INCLUDE (
    echo Reusing the MSVC environment of the current shell.
    exit /b 0
)

call :clear_vs_env

set "T3D_ENV_REFRESH="
if defined T3D_MSVC_ENV_REFRESH set "T3D_ENV_REFRESH=1"
if /i "%~2" == "-refresh" set "T3D_ENV_REFRESH=1"
if /i "%~3" == "-refresh" set "T3D_ENV_REFRESH=1"

set "VC_TOOLSET=%~2"
if /i "%VC_TOOLSET%" == "-refresh" set "VC_TOOLSET="

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
    echo ERROR: vswhere.exe not found at "%VSWHERE%".
    exit /b 1
)

set "VSWHERE_OUT=%TEMP%\tiny3d-vswhere.txt"
"%VSWHERE%" -version %1 -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath > "%VSWHERE_OUT%"

set "VS_INSTALL_PATH="
for /f "usebackq tokens=*" %%i in ("%VSWHERE_OUT%") do (
    if not defined VS_INSTALL_PATH set "VS_INSTALL_PATH=%%i"
)
del "%VSWHERE_OUT%" 2>nul

if not defined VS_INSTALL_PATH (
    echo ERROR: no Visual Studio %1 with the C++ x64 toolset was found.
    exit /b 1
)

@rem ------------------------------------------------------------------------------
@rem One cache per (version range, toolset) pair: generate-vs2019 and generate-vs2022
@rem ask for different toolchains and must not share a recording. The range is turned
@rem into a file name friendly key, e.g. "[16.0,17.0)" + 14.29 -> 160-170_1429.
@rem ------------------------------------------------------------------------------
set "ENV_CACHE_KEY=%~1_%VC_TOOLSET%"
set "ENV_CACHE_KEY=%ENV_CACHE_KEY:[=%"
set "ENV_CACHE_KEY=%ENV_CACHE_KEY:]=%"
set "ENV_CACHE_KEY=%ENV_CACHE_KEY:(=%"
set "ENV_CACHE_KEY=%ENV_CACHE_KEY:)=%"
set "ENV_CACHE_KEY=%ENV_CACHE_KEY:.=%"
set "ENV_CACHE_KEY=%ENV_CACHE_KEY:,=-%"
set "ENV_CACHE=%~dp0.msvc-env-%ENV_CACHE_KEY%.cmd"

if defined T3D_ENV_REFRESH goto run_vcvars
if not exist "%ENV_CACHE%" goto run_vcvars

@rem The cache prepends to PATH, so keep a copy to undo a rejected replay.
set "ENV_PATH_BACKUP=%PATH%"
call "%ENV_CACHE%"

@rem A stale cache is worse than no cache: it would silently parse with headers
@rem from a Visual Studio that is no longer installed. Check that the recording
@rem still matches this machine before trusting it.
if not defined INCLUDE goto cache_rejected
if /i not "%T3D_MSVC_ENV_CACHE_STAMP%" == "%VS_INSTALL_PATH%" goto cache_rejected
if not defined VCToolsInstallDir goto cache_rejected
if not exist "%VCToolsInstallDir%include\vcruntime.h" goto cache_rejected

echo Reusing the cached MSVC environment from "%ENV_CACHE%".
goto success

:cache_rejected
echo The cached MSVC environment is stale, rebuilding it from vcvarsall.bat.
set "PATH=%ENV_PATH_BACKUP%"
call :clear_vs_env
del "%ENV_CACHE%" 2>nul

:run_vcvars
echo Using Visual Studio at "%VS_INSTALL_PATH%".

@rem Ask for the pinned toolset only if it is really installed, otherwise
@rem vcvarsall fails and leaves a half initialized environment behind. Note
@rem 'if exist' cannot glob an intermediate path component, hence the for /d.
set "VC_TOOLSET_DIR="
if defined VC_TOOLSET for /d %%d in ("%VS_INSTALL_PATH%\VC\Tools\MSVC\%VC_TOOLSET%.*") do (
    if exist "%%~fd\include\vcruntime.h" set "VC_TOOLSET_DIR=%%~fd"
)

if defined VC_TOOLSET if not defined VC_TOOLSET_DIR (
    echo WARNING: toolset %VC_TOOLSET% is not installed here, using the default one.
    set "VC_TOOLSET="
)

set "VCVARSALL=%VS_INSTALL_PATH%\VC\Auxiliary\Build\vcvarsall.bat"
if not exist "%VCVARSALL%" (
    echo ERROR: vcvarsall.bat not found at "%VCVARSALL%".
    exit /b 1
)

@rem Both dump names have to exist before the 'before' snapshot is taken, or they
@rem would show up as additions of vcvarsall and end up recorded in the cache.
set "ENV_DUMP_BEFORE=%TEMP%\tiny3d-env-before-%RANDOM%.txt"
set "ENV_DUMP_AFTER=%TEMP%\tiny3d-env-after-%RANDOM%.txt"
set > "%ENV_DUMP_BEFORE%"

if defined VC_TOOLSET (
    call "%VCVARSALL%" x64 -vcvars_ver=%VC_TOOLSET%
) else (
    call "%VCVARSALL%" x64
)

if not defined INCLUDE (
    echo ERROR: vcvarsall.bat did not set the INCLUDE environment variable.
    echo        The Visual Studio installation at "%VS_INSTALL_PATH%" may be broken.
    del "%ENV_DUMP_BEFORE%" 2>nul
    exit /b 1
)

set > "%ENV_DUMP_AFTER%"
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0write-msvc-env-cache.ps1" -Before "%ENV_DUMP_BEFORE%" -After "%ENV_DUMP_AFTER%" -Output "%ENV_CACHE%" -Stamp "%VS_INSTALL_PATH%"
if not %ERRORLEVEL% == 0 (
    echo WARNING: could not record the MSVC environment cache, the next run will
    echo          call vcvarsall.bat again.
    del "%ENV_CACHE%" 2>nul
)
del "%ENV_DUMP_BEFORE%" "%ENV_DUMP_AFTER%" 2>nul

:success
call :clear_script_vars
exit /b 0

@rem ------------------------------------------------------------------------------
@rem A half initialized environment is worse than none: an inherited VSINSTALLDIR
@rem sends VsDevCmd.bat looking for devinit.exe under the wrong install, which is
@rem exactly what is left behind by a VsDevCmd run that failed.
@rem ------------------------------------------------------------------------------
:clear_vs_env
set "INCLUDE="
set "EXTERNAL_INCLUDE="
set "LIB="
set "LIBPATH="
set "VSINSTALLDIR="
set "VCINSTALLDIR="
set "VCToolsInstallDir="
set "VCToolsRedistDir="
set "VCToolsVersion="
set "DevEnvDir="
set "VSCMD_VER="
set "VSCMD_ARG_app_plat="
set "VSCMD_ARG_HOST_ARCH="
set "VSCMD_ARG_TGT_ARCH="
set "VisualStudioVersion="
set "WindowsSdkDir="
set "WindowsSDKVersion="
exit /b 0

@rem ------------------------------------------------------------------------------
@rem This script runs via 'call', so its locals would otherwise leak into the
@rem caller and, worse, into the next recorded cache.
@rem ------------------------------------------------------------------------------
:clear_script_vars
set "T3D_ENV_REFRESH="
set "T3D_MSVC_ENV_CACHE_STAMP="
set "ENV_CACHE="
set "ENV_CACHE_KEY="
set "ENV_PATH_BACKUP="
set "ENV_DUMP_BEFORE="
set "ENV_DUMP_AFTER="
set "VSWHERE="
set "VSWHERE_OUT="
set "VS_INSTALL_PATH="
set "VCVARSALL="
set "VC_TOOLSET="
set "VC_TOOLSET_DIR="
exit /b 0
