@echo off
@rem ==============================================================================
@rem Prepare the MSVC build environment for the generate-*.bat scripts.
@rem
@rem   call setup-msvc-env.bat <vswhere version range> [vcvars_ver]
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

set "VCVARSALL=%VS_INSTALL_PATH%\VC\Auxiliary\Build\vcvarsall.bat"
if not exist "%VCVARSALL%" (
    echo ERROR: vcvarsall.bat not found at "%VCVARSALL%".
    exit /b 1
)

echo Using Visual Studio at "%VS_INSTALL_PATH%".

@rem Ask for the pinned toolset only if it is really installed, otherwise
@rem vcvarsall fails and leaves a half initialized environment behind. Note
@rem 'if exist' cannot glob an intermediate path component, hence the for /d.
set "VC_TOOLSET=%~2"
set "VC_TOOLSET_DIR="
if defined VC_TOOLSET for /d %%d in ("%VS_INSTALL_PATH%\VC\Tools\MSVC\%VC_TOOLSET%.*") do (
    if exist "%%~fd\include\vcruntime.h" set "VC_TOOLSET_DIR=%%~fd"
)

if defined VC_TOOLSET if not defined VC_TOOLSET_DIR (
    echo WARNING: toolset %VC_TOOLSET% is not installed here, using the default one.
    set "VC_TOOLSET="
)

if defined VC_TOOLSET (
    call "%VCVARSALL%" x64 -vcvars_ver=%VC_TOOLSET%
) else (
    call "%VCVARSALL%" x64
)

if not defined INCLUDE (
    echo ERROR: vcvarsall.bat did not set the INCLUDE environment variable.
    echo        The Visual Studio installation at "%VS_INSTALL_PATH%" may be broken.
    exit /b 1
)

exit /b 0

@rem ------------------------------------------------------------------------------
@rem A half initialized environment is worse than none: an inherited VSINSTALLDIR
@rem sends VsDevCmd.bat looking for devinit.exe under the wrong install, which is
@rem exactly what is left behind by a VsDevCmd run that failed.
@rem ------------------------------------------------------------------------------
:clear_vs_env
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
