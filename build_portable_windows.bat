@echo off
setlocal

:: ---------------------------------------------------------------
:: Portable Windows build for idler
:: Requirements: Qt6 in PATH (or set QT_DIR below), CMake, MSVC/MinGW
:: Output: build\portable\  — copy this folder anywhere and run idler.exe
:: ---------------------------------------------------------------

:: Optional: set your Qt install path if qt tools are not in PATH
:: set QT_DIR=C:\Qt\6.x.x\msvc2019_64

if defined QT_DIR (
    set PATH=%QT_DIR%\bin;%PATH%
)

set BUILD_DIR=build\release
set PORTABLE_DIR=build\portable

echo [1/3] Configuring...
cmake -B %BUILD_DIR% -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 ( echo CMake configure failed & exit /b 1 )

echo [2/3] Building...
cmake --build %BUILD_DIR% --config Release
if errorlevel 1 ( echo Build failed & exit /b 1 )

echo [3/3] Deploying Qt DLLs (portable)...
if not exist %PORTABLE_DIR% mkdir %PORTABLE_DIR%
copy /Y %BUILD_DIR%\Release\idler.exe %PORTABLE_DIR%\idler.exe
windeployqt --release --no-translations --no-system-d3d-compiler --no-opengl-sw %PORTABLE_DIR%\idler.exe

echo.
echo Done! Portable build is in: %PORTABLE_DIR%
echo Copy that folder to any Windows 11 machine and run idler.exe
