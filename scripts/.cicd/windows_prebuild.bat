

@echo off
setlocal EnableDelayedExpansion

set scriptDirectory=%~dp0
cd /D %scriptDirectory%..\..
set "repositoryRoot=%cd%\"

git submodule sync --recursive
git submodule update --init --recursive


"%repositoryRoot%contrib/cpputils/scripts/.cicd/windows_prebuild.bat"
if not "!ERRORLEVEL!"=="0" (exit /b !ERRORLEVEL!)

exit /b 0

endlocal
