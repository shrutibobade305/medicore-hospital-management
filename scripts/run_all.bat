@echo off
echo ===================================================
echo   Launching MediCore Full-Stack Application
echo ===================================================

echo Starting Backend Server on http://localhost:8080...
start "MediCore Backend" cmd /k "call "%~dp0\run_backend.bat""

timeout /t 2 /nobreak >nul

echo Starting Frontend Server on http://localhost:5173...
start "MediCore Frontend" cmd /k "call "%~dp0\run_frontend.bat""

echo ===================================================
echo MediCore launched! Open http://localhost:5173 in browser.
echo ===================================================
