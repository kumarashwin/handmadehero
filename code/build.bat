@echo off
pushd ..\build
g++ ..\code\win32_handmade.cpp -o win32_handmade -lgdi32
popd