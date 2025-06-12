@echo off
REM Windows Build Script for Peg Solitaire
REM Usage: build-windows.bat [version] [create-installer]

REM Get version from parameter or default
set VERSION=%1
if "%VERSION%"=="" set VERSION=1.0.0

REM Get installer flag from parameter or default
set CREATE_INSTALLER=%2
if "%CREATE_INSTALLER%"=="" set CREATE_INSTALLER=yes

echo Building Peg Solitaire v%VERSION% for Windows...
echo Create installer: %CREATE_INSTALLER%

REM Get script directory
set SCRIPT_DIR=%~dp0
set PROJECT_DIR=%SCRIPT_DIR%..
set BUILD_DIR=%PROJECT_DIR%\build-release-windows
set DEPLOY_DIR=%BUILD_DIR%\PegSolitaire

REM Try to find Qt installation
set QT_DIR=
if exist "C:\Qt\6.9.1\mingw_64" set QT_DIR=C:\Qt\6.9.1\mingw_64
if exist "C:\Qt\6.8.0\mingw_64" set QT_DIR=C:\Qt\6.8.0\mingw_64
if exist "C:\Qt\6.7.0\mingw_64" set QT_DIR=C:\Qt\6.7.0\mingw_64
if exist "C:\Qt\6.6.0\mingw_64" set QT_DIR=C:\Qt\6.6.0\mingw_64

REM Check if Qt is found or in PATH
if "%QT_DIR%"=="" (
    echo Qt installation not found in standard locations.
    echo Checking if windeployqt is in PATH...
    where windeployqt >nul 2>&1
    if %ERRORLEVEL% equ 0 (
        echo windeployqt found in PATH, will use that
    ) else (
        echo Please ensure Qt is installed and windeployqt is in your PATH
        echo Or set QT_DIR environment variable
    )
    if not "%QTDIR%"=="" (
        set QT_DIR=%QTDIR%
        echo Using QTDIR: %QT_DIR%
    )
) else (
    echo Using Qt from: %QT_DIR%
)

REM Clean previous build
if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"

REM Create build directory and navigate to project directory
mkdir "%BUILD_DIR%"
cd /d "%PROJECT_DIR%"

REM Configure with CMake
echo Configuring CMake for Release build...
cmake -B "%BUILD_DIR%" -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="%QT_DIR%"

if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed!
    echo Please check that CMake and Qt are properly installed
    pause
    exit /b 1
)

REM Build the project
echo Building project...
cmake --build "%BUILD_DIR%" --config Release

if %ERRORLEVEL% neq 0 (
    echo Build failed!
    pause
    exit /b 1
)

REM Check if executable exists (could be solitaire.exe or pegsolitaire.exe)
set EXECUTABLE=
if exist "%BUILD_DIR%\solitaire.exe" set EXECUTABLE=solitaire.exe
if exist "%BUILD_DIR%\pegsolitaire.exe" set EXECUTABLE=pegsolitaire.exe
if exist "%BUILD_DIR%\Release\solitaire.exe" set EXECUTABLE=Release\solitaire.exe
if exist "%BUILD_DIR%\Release\pegsolitaire.exe" set EXECUTABLE=Release\pegsolitaire.exe

if "%EXECUTABLE%"=="" (
    echo Executable not found! Checked for solitaire.exe and pegsolitaire.exe
    echo in %BUILD_DIR% and %BUILD_DIR%\Release\
    pause
    exit /b 1
)

echo Found executable: %EXECUTABLE%

REM Create deployment directory
mkdir "%DEPLOY_DIR%"
copy "%BUILD_DIR%\%EXECUTABLE%" "%DEPLOY_DIR%\" >nul

REM Deploy Qt dependencies
echo Deploying Qt dependencies...
if "%QT_DIR%"=="" (
    REM Try to use windeployqt from PATH
    windeployqt.exe "%DEPLOY_DIR%\%EXECUTABLE%" --qmldir "%PROJECT_DIR%" --verbose 2 --compiler-runtime
) else (
    REM Use windeployqt from Qt installation
    "%QT_DIR%\bin\windeployqt.exe" "%DEPLOY_DIR%\%EXECUTABLE%" --qmldir "%PROJECT_DIR%" --verbose 2 --compiler-runtime
)

if %ERRORLEVEL% neq 0 (
    echo Warning: windeployqt failed or not found
    echo You may need to manually copy Qt libraries
)

REM Copy additional files
if exist "%PROJECT_DIR%\README.md" copy "%PROJECT_DIR%\README.md" "%DEPLOY_DIR%\" >nul 2>&1
if exist "%PROJECT_DIR%\LICENSE" copy "%PROJECT_DIR%\LICENSE" "%DEPLOY_DIR%\" >nul 2>&1
if exist "%PROJECT_DIR%\CHANGELOG.md" copy "%PROJECT_DIR%\CHANGELOG.md" "%DEPLOY_DIR%\" >nul 2>&1

REM Rename executable to standardized name if needed
if not "%EXECUTABLE%"=="PegSolitaire.exe" (
    if exist "%DEPLOY_DIR%\%EXECUTABLE%" (
        ren "%DEPLOY_DIR%\%EXECUTABLE%" "PegSolitaire.exe"
        echo Renamed %EXECUTABLE% to PegSolitaire.exe
    )
)

REM Create installer if requested
if /i "%CREATE_INSTALLER%"=="yes" (
    echo.
    echo Creating Windows installer...
    
    REM Check for NSIS
    set NSIS_PATH=
    if exist "C:\Program Files (x86)\NSIS\makensis.exe" set NSIS_PATH=C:\Program Files (x86)\NSIS\makensis.exe
    if exist "C:\Program Files\NSIS\makensis.exe" set NSIS_PATH=C:\Program Files\NSIS\makensis.exe
    
    if "%NSIS_PATH%"=="" (
        echo NSIS not found in standard locations
        echo Please install NSIS from https://nsis.sourceforge.io/
        echo Or run makensis manually on packaging\windows-installer.nsi
    ) else (
        echo Using NSIS: %NSIS_PATH%
        
        REM Create installer with version info
        "%NSIS_PATH%" /DVERSION=%VERSION% /DBUILD_DIR="%BUILD_DIR%" "%SCRIPT_DIR%windows-installer.nsi"
        
        if %ERRORLEVEL% equ 0 (
            echo Installer created successfully: PegSolitaire-%VERSION%-Setup.exe
        ) else (
            echo Installer creation failed
        )
    )
)

echo.
echo ===============================================
echo Windows build completed successfully!
echo ===============================================
echo Version: %VERSION%
echo Build directory: %BUILD_DIR%
echo Deployment directory: %DEPLOY_DIR%
echo.
echo The portable version is ready in the deployment directory.
echo.
if /i "%CREATE_INSTALLER%"=="yes" (
    if exist "PegSolitaire-%VERSION%-Setup.exe" (
        echo Installer: PegSolitaire-%VERSION%-Setup.exe
        echo.
        echo To test the installer:
        echo   PegSolitaire-%VERSION%-Setup.exe
    )
) else (
    echo To run the portable version:
    echo   cd "%DEPLOY_DIR%"
    echo   PegSolitaire.exe
)
echo.
echo To create an installer manually:
echo   makensis packaging\windows-installer.nsi
echo.
pause
