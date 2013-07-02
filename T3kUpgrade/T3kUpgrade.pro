#-------------------------------------------------
#
# Project created by QtCreator 2013-06-20T16:49:11
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = T3kUpgrade
TEMPLATE = app

CONFIG += static staticlib
QMAKE_CFLAGS_RELEASE = -Os
QMAKE_LFLAGS += -static -flto

Debug:DEFINES += _DEBUG
Release:DEFINES += QT_NO_DEBUG_OUTPUT

linux-g++:DEFINES += OS_LINUX
linux-g++-64:DEFINES += OS_LINUX

macx:DEFINES += OS_MAC

DEFINES += USE_T3K_STATIC_LIBS

#win32:LIBS += ../external/T3kHIDLibrary/win32/Dll/Lib/T3kHIDLib.lib
win32:LIBS += ../external/T3kHIDLibrary/win32/StaticLib/Lib/libT3kHIDLib.a
win32:LIBS += ../external/quazip/release/libquazip.a
win32:LIBS += -lsetupapi

linux-g++:LIBS += ../external/T3kHIDLibrary/linux/lib/x32/T3kHIDLib-1.0.so.0.0.0 \
                  ../quazip-build-desktop/libquazip.so.1

linux-g++-64:LIBS += ../external/T3kHIDLibrary/linux/lib/x64/T3kHIDLib-1.0.so.0.0.0 \
                     ../quazip-build-desktop/libquazip.so.1

macx:LIBS += -framework CoreFoundation \
            -framework IOKit \
            -framework CoreServices \
            ../quazip-build-desktop/libquazip.1.0.0.dylib

Release:macx:LIBS += ../external/T3kHIDLibrary/mac/lib/Release/libT3kHIDLib.dylib
Debug:macx:LIBS += ../external/T3kHIDLibrary/mac/lib/Debug/libT3kHIDLib.dylib

INCLUDEPATH += ../external/quazip/zlib/
INCLUDEPATH += ../external/quazip/

win32:INCLUDEPATH += ../external/T3kHIDLibrary/win32/Dll/Include/
macx:INCLUDEPATH += ../external/T3kHIDLibrary/mac/include/
linux-g++-64:INCLUDEPATH += ../external/T3kHIDLibrary/linux/include/
linux-g++:INCLUDEPATH += ../external/T3kHIDLibrary/linux/include/


SOURCES += main.cpp\
    #    dialog.cpp \
    #QSlidingStackedWidget.cpp \
    #QFWDPacket.cpp
    QSlidingStackedWidget.cpp \
    QFWDPacket.cpp \
    dialog.cpp

HEADERS  += \ #dialog.h \
    QSlidingStackedWidget.h \
    QFWDPacket.h \
    dialog.h
    #QSlidingStackedWidget.h \
    #QFWDPacket.h

FORMS    += dialog.ui

RESOURCES += \
    T3kUpgrade.qrc

win32:RC_FILE = T3kUpgrade.rc

macx:ICON = resources/T3kUpgrade.icns

OTHER_FILES += \
    T3kUpgrade.rc
