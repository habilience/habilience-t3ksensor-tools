@echo off
rem 환경에 맞추어 설정
set PATH=%PATH%;C:\Qt\Qt5.2.0\Tools\mingw48_32\bin
set QMAKE=C:\Qt\qt-everywhere-opensource-src-5.2.0\qtbase\bin
set FWPATH=H:\TouchSensor

set chk_copy = n

set /p chk_copy=Copy Firmware? (Y/N):

if %chk_copy%==y goto copy_firmware
if %chk_copy%==Y goto copy_firmware
goto skip_copy

:copy_firmware

copy %FWPATH%\T3kApp_Release\Binary\x.x.fwb T3kCfgFE\resources\firmupBinary.fwb /Y

:skip_copy

mkdir release

mingw32-make.exe clean -w
%QMAKE%\qmake.exe T3k.pro -r -spec win32-g++ CONFIG+=release
mingw32-make.exe -j8
pause 
