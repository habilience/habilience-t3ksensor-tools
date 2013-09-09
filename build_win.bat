rem 환경에 맞추어 설정
set MINGW32=C:\Qt\Qt5.1.1\Tools\mingw48_32\bin
set QMAKE=C:\Qt\qt-everywhere-opensource-src-5.1.1\qtbase\bin

mkdir release

path=%MINGW32%\;%QMAKE%\;%path%

mingw32-make.exe clean -w

qmake.exe T3k.pro -r -spec win32-g++ CONFIG+=release
mingw32-make.exe -j8
