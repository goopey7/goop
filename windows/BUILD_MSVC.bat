@echo off
setlocal enabledelayedexpansion
powershell -Command "Invoke-WebRequest https://github.com/microsoft/vswhere/releases/download/3.1.7/vswhere.exe -OutFile vswhere.exe"

:: Use vswhere.exe to find the Visual Studio installation directory
for /f "tokens=*" %%i in ('vswhere.exe -latest -products * -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe') do (
    set "msbuild_path=%%i"
    goto :found
)

:: If msbuild.exe is not found, display an error message
echo "msbuild.exe not found!"
exit /b 1

:found
:: Output the path to msbuild.exe
echo Found msbuild.exe at: %msbuild_path%

mkdir ..\build
cd ..\build
cmake ..
"%msbuild_path%" goop.sln
pause


