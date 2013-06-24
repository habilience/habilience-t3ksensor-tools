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

linux-g++:DEFINES += OS_LINUX
linux-g++-64:DEFINES += OS_LINUX

macx:DEFINES += OS_MAC

Debug:DEFINES += _DEBUG
Release:DEFINES += QT_NO_DEBUG_OUTPUT

win32:LIBS += ../external/T3kHIDLibrary/win32/Dll/Lib/T3kHIDLib.lib

linux-g++:LIBS += ../external/T3kHIDLibrary/linux/lib/x32/T3kHIDLib-1.0.so.0.0.0

linux-g++-64:LIBS += ../external/T3kHIDLibrary/linux/lib/x64/T3kHIDLib-1.0.so.0.0.0

macx:LIBS += -framework CoreFoundation \
            -framework IOKit \
            -framework CoreServices

Release:macx:LIBS += ../external/T3kHIDLibrary/mac/lib/Release/libT3kHIDLib.dylib
Debug:macx:LIBS += ../external/T3kHIDLibrary/mac/lib/Debug/libT3kHIDLib.dylib

SOURCES += main.cpp\
        dialog.cpp \
    QSlidingStackedWidget.cpp

HEADERS  += dialog.h \
    QSlidingStackedWidget.h

FORMS    += dialog.ui

RESOURCES += \
    T3kUpgrade.qrc

win32:RC_FILE = T3kUpgrade.rc

macx:ICON = resources/T3kUpgrade.icns

OTHER_FILES += \
    T3kUpgrade.rc
