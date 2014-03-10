rem 환경에 맞추어 설정
set PATH=%PATH%;C:\Qt\Qt5.2.1\Tools\mingw48_32\bin
set QMAKE=C:\Qt\qt-everywhere-opensource-src-5.2.0\qtbase\bin

mkdir release

mingw32-make.exe clean -w
%QMAKE%\qmake.exe T3k.pro -r -spec win32-g++ CONFIG+=release
mingw32-make.exe -j8
pause
