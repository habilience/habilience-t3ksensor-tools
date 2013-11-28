rem use mingw32-make of applicable version
set PATH=%PATH%;C:\Qt\Qt5.1.1\Tools\mingw48_32\bin
call configure -prefix %CD%\qtbase -static -opensource -confirm-license -debug-and-release -platform win32-g++ -nomake tests -nomake examples -opengl desktop -no-openssl -nomake tools
.\qtbase\bin\qmake.exe qtsdk.pro
mingw32-make -j8