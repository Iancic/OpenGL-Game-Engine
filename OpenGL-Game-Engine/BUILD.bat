@echo off
setlocal enabledelayedexpansion

REM --- Folder to nuke ---
set BUILD_DIR=build

REM --- Delete build folder if it exists ---
if exist "%BUILD_DIR%" (
    echo Deleting %BUILD_DIR%...
    rmdir /s /q "%BUILD_DIR%"
) else (
    echo %BUILD_DIR% does not exist, skipping delete.
)

REM --- Run premake (or your Lua script) ---
echo Running premake5...
premake5 vs2022

endlocal
pause