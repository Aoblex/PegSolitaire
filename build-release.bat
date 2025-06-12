@echo off
REM Local Windows Release Build Script
REM This script builds the project locally similar to the GitHub Actions workflow

echo ==============================================
echo    Peg Solitaire - Local Release Build
echo ==============================================
echo.

REM Check if Qt6 is available
where qmake >nul 2>nul
if %errorlevel% neq 0 (
    echo Error: Qt6 not found in PATH
    echo Please install Qt6 and add it to your system PATH
    pause
    exit /b 1
)

REM Check if CMake is available
where cmake >nul 2>nul
if %errorlevel% neq 0 (
    echo Error: CMake not found in PATH
    echo Please install CMake and add it to your system PATH
    pause
    exit /b 1
)

echo [1/5] Cleaning previous build...
if exist build rmdir /s /q build
if exist deploy rmdir /s /q deploy
if exist *.zip del *.zip

echo [2/5] Configuring CMake...
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 17 2022" -A x64
if %errorlevel% neq 0 (
    echo Error: CMake configuration failed
    pause
    exit /b 1
)

echo [3/5] Building project...
cmake --build build --config Release --parallel
if %errorlevel% neq 0 (
    echo Error: Build failed
    pause
    exit /b 1
)

echo [4/5] Deploying Qt dependencies...
mkdir deploy
copy build\Release\PegSolitaire.exe deploy\

REM Find Qt6 installation path
for %%i in (qmake.exe) do set "QT_BIN_PATH=%%~dpi"
set "WINDEPLOYQT=%QT_BIN_PATH%windeployqt.exe"

if not exist "%WINDEPLOYQT%" (
    echo Warning: windeployqt.exe not found, manual Qt DLL copying required
) else (
    "%WINDEPLOYQT%" --release --no-translations --no-system-d3d-compiler --no-opengl-sw deploy\PegSolitaire.exe
)

echo [5/5] Creating portable package...
for /f "tokens=2 delims==" %%a in ('wmic OS Get localdatetime /value') do set "dt=%%a"
set "timestamp=%dt:~0,8%-%dt:~8,6%"
powershell -command "Compress-Archive -Path 'deploy\*' -DestinationPath 'PegSolitaire-Windows-x64-local-%timestamp%.zip'"

echo.
echo ==============================================
echo         Build Complete!
echo ==============================================
echo.
echo Executable: deploy\PegSolitaire.exe
echo Package: PegSolitaire-Windows-x64-local-%timestamp%.zip
echo.
echo You can now run the game or distribute the ZIP file.
echo.
pause
