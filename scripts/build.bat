@echo off
echo ===================================================
echo   MediCore - Building Full-Stack Project
echo ===================================================

echo [1/2] Building C++ Backend...
cd /d "%~dp0\..\backend"
if not exist "build" mkdir build
cd build
cmake -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER="C:/MinGW/bin/g++.exe" -DCMAKE_C_COMPILER="C:/MinGW/bin/gcc.exe" ..
cmake --build . --config Release

echo [2/2] Building React Frontend...
cd /d "%~dp0\..\frontend"
call npm run build

echo ===================================================
echo   Build complete! Run scripts/run_all.bat to start
echo ===================================================
pause
