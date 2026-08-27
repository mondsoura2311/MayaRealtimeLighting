@echo off
REM Maya Real-time Lighting Plugin - Drag & Drop Installer (Windows)
REM This script can be dragged and dropped onto Maya to install the plugin

setlocal enabledelayedexpansion

echo ========================================
echo MayaRealtimeLighting Plugin Installer
echo ========================================
echo.

REM Get Maya version (default to 2025)
set MAYA_VERSION=%1
if "%MAYA_VERSION%"==" " set MAYA_VERSION=2025

REM Set plugin directory
set PLUGIN_DIR=%USERPROFILE%\Documents\maya\%MAYA_VERSION%\plug-ins
set PLUGIN_EXT=.mll

echo OS: Windows
echo Maya Version: %MAYA_VERSION%
echo Plugin Dir: %PLUGIN_DIR%
echo.

REM Create plugin directory
echo Creating plugin directory...
if not exist "%PLUGIN_DIR%" mkdir "%PLUGIN_DIR%"
echo OK - Done
echo.

REM Find and copy plugin
echo Looking for plugin file...
for /r . %%F in (*MayaRealtimeLighting%PLUGIN_EXT%) do (
    set PLUGIN_FILE=%%F
    goto :found
)

echo Error - Plugin file not found!
echo Expected: MayaRealtimeLighting%PLUGIN_EXT%
pause
exit /b 1

:found
echo OK - Found: %PLUGIN_FILE%
echo Copying to plugin directory...
copy "%PLUGIN_FILE%" "%PLUGIN_DIR%\" >nul
echo OK - Installed
echo.

echo ========================================
echo Installation Complete!
echo ========================================
echo.
echo Next steps:
echo   1. Restart Maya
echo   2. Go to Windows ^> Settings/Preferences ^> Plug-in Manager
echo   3. Search for 'MayaRealtimeLighting'
echo   4. Check 'Loaded' and 'Auto load'
echo   5. Click 'Apply'
echo.
pause
