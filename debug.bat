@echo off
cls

Rem Build
if not exist Build\Debug mkdir Build\Debug\Run
cd Build\Debug
cmake.exe -S ..\..\ -B . -G"MinGW Makefiles" -D CMAKE_BUILD_TYPE=Debug
C:\Users\gerku\Documents\mingw64\bin\mingw32-make.exe
cd Run
echo n | copy /-y "C:\Users\gerku\Documents\json-c\install\bin\libjson-c.dll" .

Rem Launch
MyProject.exe
cd ..\..\..\

