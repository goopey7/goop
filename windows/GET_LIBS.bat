@echo off
cd /d %~dp0

:menu
cls
echo Select your Visual Studio version:
echo 1. Visual Studio 2022
echo 2. Visual Studio 2019
echo 3. Visual Studio 2017
echo 4. Visual Studio 2015
echo 5. Visual Studio 2013
echo 6. Visual Studio 2012

choice /c 123456 /n /m "Enter the number corresponding to your Visual Studio version: "

set "vs_version="
if errorlevel 6 set "vs_version=2012"
if errorlevel 5 set "vs_version=2013"
if errorlevel 4 set "vs_version=2015"
if errorlevel 3 set "vs_version=2017"
if errorlevel 2 set "vs_version=2019"
if errorlevel 1 set "vs_version=2022"

if "%vs_version%"=="" (
    echo Invalid choice. Please select a valid option.
    pause
    goto menu
)

echo You selected Visual Studio %vs_version%
powershell -Command "Invoke-WebRequest https://github.com/glfw/glfw/releases/download/3.3.8/glfw-3.3.8.bin.WIN64.zip -OutFile glfw.zip"
powershell Expand-Archive glfw.zip
powershell mv glfw/glfw-3.3.8.bin.WIN64 .
powershell rmdir glfw
powershell mv glfw-3.3.8.bin.WIN64 glfw
powershell rm glfw.zip

rem Remove directories matching the pattern glfw/lib-* except for glfw/lib-vc%vs_version%
for /d %%d in (glfw\lib-*) do (
    if /i "%%~nxd" neq "lib-vc%vs_version%" (
        rd /s /q "%%d"
    )
)

powershell mv glfw/lib-vc%vs_version% glfw/lib

powershell -Command "Invoke-WebRequest https://github.com/libsdl-org/SDL/releases/download/release-2.28.4/SDL2-devel-2.28.4-VC.zip -OutFile sdl2.zip"
powershell Expand-Archive sdl2.zip
powershell mv sdl2/SDL2-2.28.4 .
powershell rmdir sdl2
powershell mv SDL2-2.28.4 sdl2
powershell rm sdl2.zip

powershell Remove-Item -Path "sdl2/cmake" -Recurse -Force
powershell Remove-Item -Path "sdl2/docs" -Recurse -Force
powershell Remove-Item -Path "sdl2/lib/x86" -Recurse -Force
powershell rm sdl2/BUGS.txt
powershell rm sdl2/WhatsNew.txt
powershell mv sdl2/lib/x64/* sdl2/lib/
powershell Remove-Item -Path "sdl2/lib/x64" -Recurse -Force

powershell mkdir sdl2/include/SDL2
powershell mv sdl2/include/*.h sdl2/include/SDL2/

powershell -Command "Invoke-WebRequest https://github.com/g-truc/glm/releases/download/0.9.9.8/glm-0.9.9.8.zip -OutFile glm.zip"
powershell Expand-Archive glm.zip
powershell mv glm/glm/glm glm-include
powershell Remove-Item -Path "glm" -Recurse -Force
powershell mkdir glm
powershell mkdir glm/include
powershell mv glm-include glm/include/glm
powershell rm glm.zip

powershell -Command "Invoke-WebRequest 'https://drive.google.com/uc?export=download&id=1V6SwbnuC8cfvvl3lpNlFpv_rDQuuBXYI' -OutFile 'assimp.zip'"
powershell Expand-Archive assimp.zip
powershell rm assimp.zip

pause
