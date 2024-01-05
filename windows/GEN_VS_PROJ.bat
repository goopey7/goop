@echo off
mkdir ..\build
cd ..\build
cmake --build .. --target clean
cmake -G "Visual Studio 17" ..
start goop.sln
