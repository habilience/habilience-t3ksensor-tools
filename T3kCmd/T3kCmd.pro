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
DEFINES += _QT_COMPILER_ QUAZIP_STATIC USE_T3K_STATIC_LIBS _T3KHANDLE_REMOVE_PRV

linux-g++|linux-g++-32|linux-g++-64:DEFINES += OS_LINUX

macx:DEFINES += OS_MAC

CONFIG(debug, debug|release):DEFINES += _DEBUG
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

#Library
INCLUDEPATH += ../common/ $$PWD/../external/T3kHIDLibrary/include \

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

linux-g++:QMAKE_TARGET.arch = $$QMAKE_HOST.arch
linux-g++-32:QMAKE_TARGET.arch = x86
linux-g++-64:QMAKE_TARGET.arch = x86_64
linux-g++ {
    contains(QMAKE_TARGET.arch, x86_64):{
        message( "g++ building for 64bit" );
        CONFIG(debug, debug|release) {
            OBJECTS_DIR = $$PWD/.objs_x64/debug/
            MOC_DIR = $$PWD/.objs_x64/debug/
            DESTDIR = $$PWD/debug_x64
        }

        CONFIG(release, debug|release) {
            OBJECTS_DIR = $$PWD/.objs_x64/release/
            MOC_DIR = $$PWD/.objs_x64/release/
            DESTDIR = $$PWD/release_x64
        }

        LIBS += $$PWD/../external/T3kHIDLibrary/linux/64bit/T3kHIDLib-1.0.so.0.0.0
        QMAKE_RPATHDIR += $$PWD/../external/T3kHIDLibrary/linux/64bit
        PRE_TARGETDEPS += $$PWD/../external/T3kHIDLibrary/linux/64bit/T3kHIDLib-1.0.so.0.0.0
    }
    !contains(QMAKE_TARGET.arch, x86_64):{
        message( "g++ building for 32bit" );
        CONFIG(debug, debug|release) {
            OBJECTS_DIR = $$PWD/.objs/debug/
            MOC_DIR = $$PWD/.objs/debug/
            DESTDIR = $$PWD/debug
        }

        CONFIG(release, debug|release) {
            OBJECTS_DIR = $$PWD/.objs/release/
            MOC_DIR = $$PWD/.objs/release/
            DESTDIR = $$PWD/release
        }

        LIBS += $$PWD/../external/T3kHIDLibrary/linux/32bit/T3kHIDLib-1.0.so.0.0.0
        QMAKE_RPATHDIR += $$PWD/../external/T3kHIDLibrary/linux/32bit
        PRE_TARGETDEPS += $$PWD/../external/T3kHIDLibrary/linux/32bit/T3kHIDLib-1.0.so.0.0.0
    }

    LIBS +=
}

linux-g++-32 {
    message( "building for 32bit" );
    CONFIG(debug, debug|release) {
        OBJECTS_DIR = $$PWD/.objs/debug/
        MOC_DIR = $$PWD/.objs/debug/
        DESTDIR = $$PWD/debug
    }

    CONFIG(release, debug|release) {
        OBJECTS_DIR = $$PWD/.objs/release/
        MOC_DIR = $$PWD/.objs/release/
        DESTDIR = $$PWD/release
    }

    LIBS += $$PWD/../external/T3kHIDLibrary/linux/32bit/T3kHIDLib-1.0.so.0.0.0
    QMAKE_RPATHDIR += $$PWD/../external/T3kHIDLibrary/linux/32bit
    PRE_TARGETDEPS += $$PWD/../external/T3kHIDLibrary/linux/32bit/T3kHIDLib-1.0.so.0.0.0

    LIBS +=
}

linux-g++-64 {
    message( "building for 64bit" );
    CONFIG(debug, debug|release) {
        OBJECTS_DIR = $$PWD/.objs_x64/debug/
        MOC_DIR = $$PWD/.objs_x64/debug/
        DESTDIR = $$PWD/debug_x64
    }

    CONFIG(release, debug|release) {
        OBJECTS_DIR = $$PWD/.objs_x64/release/
        MOC_DIR = $$PWD/.objs_x64/release/
        DESTDIR = $$PWD/release_x64
    }

    LIBS += $$PWD/../external/T3kHIDLibrary/linux/64bit/T3kHIDLib-1.0.so.0.0.0
    QMAKE_RPATHDIR += $$PWD/../external/T3kHIDLibrary/linux/64bit
    PRE_TARGETDEPS += $$PWD/../external/T3kHIDLibrary/linux/64bit/T3kHIDLib-1.0.so.0.0.0

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
    ../common/TPDPEventMultiCaster.cpp \
    HIDCmdThread.cpp \
    QExFuncThread.cpp

HEADERS += \
    ../common/T3kHandle.h \
    ../common/TPDPEventMultiCaster.h \
    HIDCmdThread.h \
    DefineString.h \
    QExFuncThread.h


win32:RC_FILE = T3kCmd.rc

RESOURCES += \
    T3kCmd.qrc

OTHER_FILES += \
    T3kCmd.rc
