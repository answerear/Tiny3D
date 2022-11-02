@if not defined _echo echo off
for /f "usebackq delims=" %%i in (`..\..\tools\vswhere\vswhere.exe -prerelease -latest -property installationPath`) do (
  if exist "%%i\Common7\Tools\vsdevcmd.bat" (
    %comspec% /k "%%i\Common7\Tools\vsdevcmd.bat" %*
    exit /b
  )
)

rem Instance or command prompt not found
exit /b 2