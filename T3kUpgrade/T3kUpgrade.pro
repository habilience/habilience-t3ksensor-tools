#-------------------------------------------------
#
# Project created by QtCreator 2013-06-20T16:49:11
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = T3kUpgrade
CONFIG(debug, debug|release): TARGET = $$join(TARGET,,,d)

CONFIG += static staticlib

TEMPLATE = app

DEFINES += USE_T3K_STATIC_LIBS QUAZIP_STATIC NO_LANGUAGE

linux-g++|linux-g++-32|linux-g++-64:DEFINES += OS_LINUX

macx:DEFINES += OS_MAC

CONFIG(debug, debug|release):DEFINES += _DEBUG
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

#Library
INCLUDEPATH +=  ../common/ \
                $$PWD/../external/quazip \
                $$PWD/../external/T3kHIDLibrary/include \

DEPENDPATH +=   $$PWD/../external/quazip \
                $$PWD/../external/T3kHIDLibrary/include \

#################################################################################################
## win32
win32 {
    CONFIG += static staticlib
    QMAKE_CFLAGS_RELEASE = -Os
    QMAKE_LFLAGS += -static -flto

    CONFIG(release, debug|release) {
        LIBS += -L$$OUT_PWD/../external/quazip/release/ -lquazip \
                -L$$PWD/../external/T3kHIDLibrary/win32/StaticLib/Lib/ -lT3kHIDLib
        PRE_TARGETDEPS += $$OUT_PWD/../external/quazip/release/libquazip.a \
                            $$PWD/../external/T3kHIDLibrary/win32/StaticLib/Lib/libT3kHIDLib.a
    }

    CONFIG(debug, debug|release) {
        LIBS += -L$$OUT_PWD/../external/quazip/debug/ -lquazipd \
                -L$$PWD/../external/T3kHIDLibrary/win32/StaticLib/Lib/ -lT3kHIDLibd
        PRE_TARGETDEPS += $$OUT_PWD/../external/quazip/debug/libquazipd.a \
                            $$PWD/../external/T3kHIDLibrary/win32/StaticLib/Lib/libT3kHIDLibd.a
    }

    LIBS += -lsetupapi -lole32 -luuid -lws2_32 \
}
##
#################################################################################################



#################################################################################################
## linux-g++
linux-g++:QMAKE_TARGET.arch = $$QMAKE_HOST.arch
linux-g++-32:QMAKE_TARGET.arch = x86
linux-g++-64:QMAKE_TARGET.arch = x86_64

linux-g++ { # depend on Qt Creator's setting
    contains(QMAKE_TARGET.arch, x86_64):{
        message( "building for 64bit" );
        CONFIG(debug, debug|release): OBJECTS_DIR = $$PWD/.objs_x64/debug/
        CONFIG(debug, debug|release): MOC_DIR = $$PWD/.objs_x64/debug/
        CONFIG(release, debug|release): OBJECTS_DIR = $$PWD/.objs_x64/release/
        CONFIG(release, debug|release): MOC_DIR = $$PWD/.objs_x64/release/
        CONFIG(debug, debug|release): DESTDIR = $$PWD/debug_x64
        CONFIG(release, debug|release): DESTDIR = $$PWD/release_x64

        LIBS += $$PWD/../external/T3kHIDLibrary/linux/64bit/T3kHIDLib-1.0.so.0.0.0

        CONFIG(release, debug|release) {
            LIBS += -L$$PWD/../external/quazip/ -lquazip_x64
            QMAKE_RPATHDIR += '-Wl,-rpath,\'\$$ORIGIN/\''
        }
        CONFIG(debug, debug|release) {
            LIBS += -L$$PWD/../external/quazip/ -lquazipd_x64
        }
    }
    !contains(QMAKE_TARGET.arch, x86_64):{
        message( "building for 32bit" );
        CONFIG(debug, debug|release): OBJECTS_DIR = $$PWD/.objs/debug/
        CONFIG(debug, debug|release): MOC_DIR = $$PWD/.objs/debug/
        CONFIG(release, debug|release): OBJECTS_DIR = $$PWD/.objs/release/
        CONFIG(release, debug|release): MOC_DIR = $$PWD/.objs/release/
        CONFIG(debug, debug|release): DESTDIR = $$PWD/debug
        CONFIG(release, debug|release): DESTDIR = $$PWD/release

        LIBS += $$PWD/../external/T3kHIDLibrary/linux/32bit/T3kHIDLib-1.0.so.0.0.0

        CONFIG(release, debug|release) {
            LIBS += -L$$PWD/../external/quazip/ -lquazip
            QMAKE_RPATHDIR += '-Wl,-rpath,\'\$$ORIGIN/\''
        }
        CONFIG(debug, debug|release) {
            LIBS += -L$$PWD/../external/quazip/ -lquazipd
        }
    }
}

linux-g++-32 { # generic g++ 32bit compiler
    message( "building for 32bit" );
    CONFIG(debug, debug|release): OBJECTS_DIR = $$PWD/.objs/debug/
    CONFIG(debug, debug|release): MOC_DIR = $$PWD/.objs/debug/
    CONFIG(release, debug|release): OBJECTS_DIR = $$PWD/.objs/release/
    CONFIG(release, debug|release): MOC_DIR = $$PWD/.objs/release/
    CONFIG(debug, debug|release): DESTDIR = $$PWD/debug
    CONFIG(release, debug|release): DESTDIR = $$PWD/release

    LIBS += $$PWD/../external/T3kHIDLibrary/linux/32bit/T3kHIDLib-1.0.so.0.0.0

    CONFIG(release, debug|release) {
        LIBS += -L$$PWD/../external/quazip/ -lquazip
        QMAKE_RPATHDIR += '-Wl,-rpath,\'\$$ORIGIN/\''
    }
    CONFIG(debug, debug|release) {
        LIBS += -L$$PWD/../external/quazip/ -lquazipd
    }
}
linux-g++-64 { # generic g++ 64bit compiler
    message( "building for 64bit" );
    CONFIG(debug, debug|release): OBJECTS_DIR = $$PWD/.objs_x64/debug/
    CONFIG(debug, debug|release): MOC_DIR = $$PWD/.objs_x64/debug/
    CONFIG(release, debug|release): OBJECTS_DIR = $$PWD/.objs_x64/release/
    CONFIG(release, debug|release): MOC_DIR = $$PWD/.objs_x64/release/
    CONFIG(debug, debug|release): DESTDIR = $$PWD/debug_x64
    CONFIG(release, debug|release): DESTDIR = $$PWD/release_x64

    LIBS += $$PWD/../external/T3kHIDLibrary/linux/64bit/T3kHIDLib-1.0.so.0.0.0

    CONFIG(release, debug|release) {
        LIBS += -L$$PWD/../external/quazip/ -lquazip_x64
        QMAKE_RPATHDIR += '-Wl,-rpath,\'\$$ORIGIN/\''
    }
    CONFIG(debug, debug|release) {
        LIBS += -L$$PWD/../external/quazip/ -lquazipd_x64
    }
}
##
#################################################################################################



#################################################################################################
## macx
macx {
    DEFINES += OS_MAC
    CONFIG += static staticlib
    LIBS += -framework CoreFoundation \
            -framework IOKit \
            -framework CoreServices \

    CONFIG(release, debug|release): {
        LIBS += -L$$OUT_PWD/../external/quazip/ -lquazip \
                -L$$PWD/../external/T3kHIDLibrary/mac/ -lT3kHIDLibStatic

        PRE_TARGETDEPS += $$OUT_PWD/../external/quazip/libquazip.a \
                            $$PWD/../external/T3kHIDLibrary/mac/libT3kHIDLibStatic.a
    }

    CONFIG(debug, debug|release): {
        LIBS += -L$$OUT_PWD/../external/quazip/ -lquazipd \
                -L$$PWD/../external/T3kHIDLibrary/mac/ -lT3kHIDLibStaticd

        PRE_TARGETDEPS += $$OUT_PWD/../external/quazip/libquazipd.a \
                            $$PWD/../external/T3kHIDLibrary/mac/libT3kHIDLibStaticd.a
    }
}
##
#################################################################################################


SOURCES += main.cpp \
    dialog.cpp \
    ../common/qsingleapplication/qtsinglecoreapplication.cpp \
    ../common/qsingleapplication/qtsingleapplication.cpp \
    ../common/qsingleapplication/qtlockedfile_win.cpp \
    ../common/qsingleapplication/qtlockedfile_unix.cpp \
    ../common/qsingleapplication/qtlockedfile.cpp \
    ../common/qsingleapplication/qtlocalpeer.cpp \
    ../common/QUtils.cpp \
    ../common/ui/QLicenseWidget.cpp \
    QSlidingStackedWidget.cpp \
    QFWDPacket.cpp \
    QBriefingDialog.cpp \
    QPartCheckBox.cpp

HEADERS  += dialog.h \
    ../common/qsingleapplication/qtsinglecoreapplication.h \
    ../common/qsingleapplication/qtsingleapplication.h \
    ../common/qsingleapplication/qtlockedfile.h \
    ../common/qsingleapplication/qtlocalpeer.h \
    ../common/QUtils.h \
    ../common/ui/QLicenseWidget.h \
    QSlidingStackedWidget.h \
    QFWDPacket.h \
    QBriefingDialog.h \
    QPartCheckBox.h

FORMS    += dialog.ui \
    briefing_dialog.ui \
    ../common/ui/QLicenseWidget.ui \

RESOURCES += \
    T3kUpgrade.qrc

win32:RC_FILE = T3kUpgrade.rc

macx:ICON =

OTHER_FILES += \
    T3kUpgrade.rc
