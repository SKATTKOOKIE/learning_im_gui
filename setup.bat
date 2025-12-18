@echo off
REM setup.bat - Initialize and configure git submodules for the project

echo ================================================
echo Setting up project dependencies...
echo ================================================

REM Check if git is available
where git >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo Error: git is not installed or not in PATH
    exit /b 1
)

REM Initialize and update all submodules
echo.
echo Initializing git submodules...
git submodule update --init --recursive
if %ERRORLEVEL% NEQ 0 (
    echo Error: Failed to initialize submodules
    exit /b 1
)

REM Configure Boost
echo.
echo Configuring Boost...
cd external\boost
git checkout boost-1.83.0
if %ERRORLEVEL% NEQ 0 (
    echo Error: Failed to checkout Boost 1.83.0
    cd ..\..
    exit /b 1
)
echo Checked out Boost 1.83.0

echo Initializing Boost submodules (this may take a few minutes)...
git submodule update --init --recursive
if %ERRORLEVEL% NEQ 0 (
    echo Warning: Some Boost submodules may not have initialized
)

echo.
echo Generating Boost headers...
call bootstrap.bat
if %ERRORLEVEL% NEQ 0 (
    echo Warning: Bootstrap failed, but headers may still work
)
.\b2 headers
if %ERRORLEVEL% NEQ 0 (
    echo Warning: Header generation had issues, but may still work
)
echo Boost headers generated

cd ..\..

REM Configure GLFW
echo.
echo Configuring GLFW...
cd external\glfw
git checkout 3.4
if %ERRORLEVEL% NEQ 0 (
    echo Error: Failed to checkout GLFW 3.4
    cd ..\..
    exit /b 1
)
echo Checked out GLFW 3.4
cd ..\..

echo.
echo ================================================
echo Setup complete!
echo ================================================
echo.
echo You can now build the project:
echo   mkdir build
echo   cd build
echo   cmake -G "Visual Studio 17 2022" ..
echo   cmake --build . --config Release
echo.
pause@echo off
REM setup.bat - Initialize and configure git submodules for the project

echo ================================================
echo Setting up project dependencies...
echo ================================================

REM Check if git is available
where git >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo Error: git is not installed or not in PATH
    exit /b 1
)

REM Initialize and update all submodules
echo.
echo Initializing git submodules...
git submodule update --init --recursive
if %ERRORLEVEL% NEQ 0 (
    echo Error: Failed to initialize submodules
    exit /b 1
)

REM Configure Boost
echo.
echo Configuring Boost...
cd external\boost
git checkout boost-1.83.0
if %ERRORLEVEL% NEQ 0 (
    echo Error: Failed to checkout Boost 1.83.0
    cd ..\..
    exit /b 1
)
echo Checked out Boost 1.83.0
git submodule update --init --recursive
if %ERRORLEVEL% NEQ 0 (
    echo Warning: Some Boost submodules may not have initialized
)
echo Initialized Boost submodules
cd ..\..

REM Configure GLFW
echo.
echo Configuring GLFW...
cd external\glfw
git checkout 3.4
if %ERRORLEVEL% NEQ 0 (
    echo Error: Failed to checkout GLFW 3.4
    cd ..\..
    exit /b 1
)
echo Checked out GLFW 3.4
cd ..\..

echo.
echo ================================================
echo Setup complete!
echo ================================================
echo.
echo You can now build the project:
echo   mkdir build
echo   cd build
echo   cmake -G "Visual Studio 17 2022" ..
echo   cmake --build . --config Release
echo.
pause