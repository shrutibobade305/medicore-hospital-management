@echo off
echo ===================================================
echo   Starting MediCore C++ Backend (Port 8080)
echo ===================================================
cd /d "%~dp0\.."
if not exist "data" mkdir data
backend\build\medicore_server.exe
pause
