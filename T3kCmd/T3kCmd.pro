#-------------------------------------------------
#
# Project created by QtCreator 2013-06-20T16:48:35
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET = T3kCmd
CONFIG(debug, debug|release): TARGET = $$join(TARGET,,,d)

CONFIG   += console static staticlib
CONFIG   -= app_bundle

TEMPLATE = app

#Define
DEFINES += _QT_COMPILER_ QUAZIP_STATIC _T3KHANDLE_REMOVE_PRV USE_T3K_STATIC_LIBS

linux-g++|linux-g++-64:DEFINES += OS_LINUX

macx:DEFINES += OS_MAC

CONFIG(debug, debug|release):DEFINES += _DEBUG
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

#Library
INCLUDEPATH += $$PWD/../external/T3kHIDLibrary/include \

DEPENDPATH += $$PWD/../external/T3kHIDLibrary/include \

win32 {
    QMAKE_LFLAGS += -static

    CONFIG(release, debug|release) {
        LIBS += -L$$PWD/../external/T3kHIDLibrary/win32/StaticLib/Lib/ -lT3kHIDLib
        PRE_TARGETDEPS += $$PWD/../external/T3kHIDLibrary/win32/StaticLib/Lib/libT3kHIDLib.a
    }

    CONFIG(debug, debug|release) {
        LIBS += -L$$PWD/../external/T3kHIDLibrary/win32/StaticLib/Lib/ -lT3kHIDLibd
        PRE_TARGETDEPS += $$PWD/../external/T3kHIDLibrary/win32/StaticLib/Lib/libT3kHIDLib.a
    }

    LIBS += -lsetupapi -lole32 -luuid -lws2_32 \
}

linux-g++ {
    CONFIG(release, debug|release) {
        LIBS += $$PWD/../external/T3kHIDLibrary/linux/32bit/T3kHIDLib-1.0.so.0.0.0

        PRE_TARGETDEPS += $$PWD/../external/T3kHIDLibrary/linux/32bit/T3kHIDLib-1.0.so.0.0.0
    }

    CONFIG(debug, debug|release) {
        LIBS += $$PWD/../external/T3kHIDLibrary/linux/32bit/T3kHIDLib-1.0.so.0.0.0

        PRE_TARGETDEPS += $$PWD/../external/T3kHIDLibrary/linux/32bit/T3kHIDLib-1.0.so.0.0.0
    }

    LIBS +=
}

linux-g++-64 {
    CONFIG(release, debug|release) {
        LIBS += $$PWD/../external/T3kHIDLibrary/linux/64bit/T3kHIDLib-1.0.so.0.0.0

        PRE_TARGETDEPS += $$PWD/../external/T3kHIDLibrary/linux/64bit/T3kHIDLib-1.0.so.0.0.0
    }

    CONFIG(debug, debug|release) {
        LIBS += $$PWD/../external/T3kHIDLibrary/linux/64bit/T3kHIDLib-1.0.so.0.0.0

        PRE_TARGETDEPS += $$PWD/../external/T3kHIDLibrary/linux/64bit/T3kHIDLib-1.0.so.0.0.0
    }

    LIBS +=
}

macx: {
    CONFIG(release, debug|release): {
        LIBS += -L$$PWD/../external/T3kHIDLibrary/mac/ -lT3kHIDLibStatic

        PRE_TARGETDEPS += $$PWD/../external/T3kHIDLibrary/mac/libT3kHIDLibStatic.a
    }

    CONFIG(debug, debug|release): {
        LIBS += -L$$PWD/../external/T3kHIDLibrary/mac/ -lT3kHIDLibStaticd

        PRE_TARGETDEPS += $$PWD/../external/T3kHIDLibrary/mac/libT3kHIDLibStaticd.a
    }

    LIBS += -framework CoreFoundation \
            -framework IOKit \
            -framework CoreServices \
}

SOURCES += main.cpp \
    ../common/T3kHandle.cpp \
    HIDCmdThread.cpp \
    T3kHIDNotify.cpp \
    QExFuncThread.cpp

HEADERS += \
    ../common/T3kHandle.h \
    HIDCmdThread.h \
    DefineString.h \
    T3kHIDNotify.h \
    QExFuncThread.h


win32:RC_FILE = ./resources/T3kCmd.rc
