@echo off
REM =====================================================
REM  setup-ai-links.cmd
REM  Creates .cursor and .codebuddy junction links
REM  pointing to the shared .ai directory.
REM  Run once after cloning the repository.
REM =====================================================

cd /d "%~dp0"

if not exist ".ai\" (
    echo [ERROR] .ai directory not found. Make sure you are in the project root.
    exit /b 1
)

REM --- .cursor ---
if exist ".cursor\" (
    echo [SKIP] .cursor already exists.
) else (
    mklink /J .cursor .ai
    if errorlevel 1 (
        echo [ERROR] Failed to create .cursor junction.
    ) else (
        echo [OK]   .cursor -> .ai
    )
)

REM --- .codebuddy ---
if exist ".codebuddy\" (
    echo [SKIP] .codebuddy already exists.
) else (
    mklink /J .codebuddy .ai
    if errorlevel 1 (
        echo [ERROR] Failed to create .codebuddy junction.
    ) else (
        echo [OK]   .codebuddy -> .ai
    )
)

echo.
echo Done. Both Cursor and CodeBuddy should now read rules from .ai/
pause
