@echo off
rem CMD ,run "SIM800C32.bat all" or "SIM800C32.bat clean"
set path=%CD%\build\winmake;%path%
rem make -f build\Makefile --debug=b PROJ=%~n0 CFG=user %1
make -f build\Makefile --silent PROJ=%~n0 CFG=user %1
rem pause
