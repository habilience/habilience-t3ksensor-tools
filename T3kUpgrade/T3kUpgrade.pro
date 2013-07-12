#-------------------------------------------------
#
# Project created by QtCreator 2013-06-20T16:49:11
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = T3kUpgrade
CONFIG(debug, debug|release): TARGET = $$join(TARGET,,,d)

TEMPLATE = app

CONFIG += static staticlib

DEFINES += USE_T3K_STATIC_LIBS QUAZIP_STATIC

linux-g++|linux-g++-64:DEFINES += OS_LINUX
macx:DEFINES += OS_MAC

CONFIG(debug, debug|release):DEFINES += _DEBUG
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

#Library
INCLUDEPATH += $$PWD/../external/quazip \
                $$PWD/../external/T3kHIDLibrary/include \
                $$PWD../external/quazip/zlib/ \

DEPENDPATH += $$PWD/../external/quazip \
                $$PWD/../external/T3kHIDLibrary/include \
                $$PWD../external/quazip/zlib/ \


win32 {

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

    LIBS += -lsetupapi
}

linux-g++ {
    LIBS += #-L/usr/lib -lusb-1.0 \

    QMAKE_RPATH += ./

    CONFIG(release, debug|release) {
        LIBS += -L$$OUT_PWD/../external/quazip/ -lquazip \
                #-L$$PWD/../external/T3kHIDLibrary/linux/32bit/ -lT3kHIDLib-1.0.so.0.0.0
                $$PWD/../external/T3kHIDLibrary/linux/32bit/T3kHIDLib-1.0.so.0.0.0

        PRE_TARGETDEPS += $$OUT_PWD/../external/quazip/libquazip.a \
                           $$PWD/../external/T3kHIDLibrary/linux/32bit/T3kHIDLib-1.0.so.0.0.0
    }

    CONFIG(debug, debug|release) {
        LIBS += -L$$OUT_PWD/../external/quazip/ -lquazipd \
                #-L$$PWD/../external/T3kHIDLibrary/linux/32bit/ -lT3kHIDLibStatic
                $$PWD/../external/T3kHIDLibrary/linux/32bit/T3kHIDLib-1.0.so.0.0.0

        PRE_TARGETDEPS += $$OUT_PWD/../external/quazip/libquazipd.a \
                            $$PWD/../external/T3kHIDLibrary/linux/32bit/T3kHIDLib-1.0.so.0.0.0
    }
}

linux-g++-64 {
    LIBS += #-L/usr/lib -lusb-1.0 \

    QMAKE_RPATHDIR  += ./

    CONFIG(release, debug|release) {
        LIBS += -L$$OUT_PWD/../external/quazip/ -lquazip \
                #-L$$PWD/../external/T3kHIDLibrary/linux/64bit/ -lT3kHIDLib-1.0.so.0.0.0
                $$PWD/../external/T3kHIDLibrary/linux/64bit/T3kHIDLib-1.0.so.0.0.0

        PRE_TARGETDEPS += $$OUT_PWD/../external/quazip/libquazip.a \
                           $$PWD/../external/T3kHIDLibrary/linux/64bit/T3kHIDLib-1.0.so.0.0.0
    }

    CONFIG(debug, debug|release) {
        LIBS += -L$$OUT_PWD/../external/quazip/ -lquazipd \
                #-L$$PWD/../external/T3kHIDLibrary/linux/64bit/ -lT3kHIDLib-1.0.so.0.0.0
                $$PWD/../external/T3kHIDLibrary/linux/64bit/T3kHIDLib-1.0.so.0.0.0

        PRE_TARGETDEPS += $$OUT_PWD/../external/quazip/libquazipd.a \
                            $$PWD/../external/T3kHIDLibrary/linux/64bit/T3kHIDLib-1.0.so.0.0.0
    }
}

macx: {
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

SOURCES += main.cpp \
    QSlidingStackedWidget.cpp \
    QFWDPacket.cpp \
    dialog.cpp \
    QBriefingDialog.cpp

HEADERS  += dialog.h \
    QSlidingStackedWidget.h \
    QFWDPacket.h \
    QBriefingDialog.h

FORMS    += dialog.ui \
    briefing_dialog.ui

RESOURCES += \
    T3kUpgrade.qrc

win32:RC_FILE = T3kUpgrade.rc

macx:ICON =

OTHER_FILES += \
    T3kUpgrade.rc
