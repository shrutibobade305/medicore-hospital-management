@echo off
echo ===================================================
echo   Starting MediCore React Frontend (Port 5173)
echo ===================================================
cd /d "%~dp0\..\frontend"
npm run dev
pause
