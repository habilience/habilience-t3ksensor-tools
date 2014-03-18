#-------------------------------------------------
#
# Project created by QtCreator 2013-06-20T16:45:32
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = T3kSoftlogic
CONFIG(debug, debug|release): TARGET = $$join(TARGET,,,d)

CONFIG += static staticlib

TEMPLATE = app

#Define
DEFINES += _QT_COMPILER_ USE_T3K_STATIC_LIBS \
            USE_SOFTLOGIC_OUTPUT_MOUSE USE_SOFTLOGIC_OUTPUT_AUDIO_CONTROL \
            NO_LANGUAGE

linux-g++|linux-g++-32|linux-g++-64:DEFINES += OS_LINUX

macx:DEFINES += OS_MAC

CONFIG(debug, debug|release):DEFINES += _DEBUG
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_DEBUG

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
            QMAKE_RPATHDIR += $$PWD/../external/T3kHIDLibrary/linux/64bit
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
        PRE_TARGETDEPS += $$PWD/../external/T3kHIDLibrary/linux/64bit/T3kHIDLib-1.0.so.0.0.0
    }
    !contains(QMAKE_TARGET.arch, x86_64):{
        message( "g++ building for 32bit" );
        CONFIG(debug, debug|release) {
            QMAKE_RPATHDIR += $$PWD/../external/T3kHIDLibrary/linux/64bit
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
        PRE_TARGETDEPS += $$PWD/../external/T3kHIDLibrary/linux/32bit/T3kHIDLib-1.0.so.0.0.0
    }

    LIBS +=
}

linux-g++-32 {
    message( "building for 32bit" );
    CONFIG(debug, debug|release) {
        QMAKE_RPATHDIR += $$PWD/../external/T3kHIDLibrary/linux/64bit
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
    PRE_TARGETDEPS += $$PWD/../external/T3kHIDLibrary/linux/32bit/T3kHIDLib-1.0.so.0.0.0

    LIBS +=
}

linux-g++-64 {
    message( "building for 64bit" );
    CONFIG(debug, debug|release) {
        QMAKE_RPATHDIR += $$PWD/../external/T3kHIDLibrary/linux/64bit
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

SOURCES += main.cpp\
    ../common/qsingleapplication/qtsinglecoreapplication.cpp \
    ../common/qsingleapplication/qtsingleapplication.cpp \
    ../common/qsingleapplication/qtlockedfile_win.cpp \
    ../common/qsingleapplication/qtlockedfile_unix.cpp \
    ../common/qsingleapplication/qtlockedfile.cpp \
    ../common/qsingleapplication/qtlocalpeer.cpp \
    ../common/QT3kDevice.cpp \
    ../common/QT3kDeviceEventHandler.cpp \
    ../common/QIniFormat.cpp \
    ../common/QKeyMapStr.cpp \
    ../common/QUtils.cpp \
    ../common/QGUIUtils.cpp \
    ../common/ui/UnderlineLabel.cpp \
    ../common/ui/QHoverComboBox.cpp \
    ../common/ui/ColorTabBar.cpp \
    ../common/ui/Q2ColorTabWidget.cpp \
    ../common/ui/QLicenseWidget.cpp \
    T3kSoftlogicDlg.cpp \
    TabPanelWidget.cpp \
    TabKeyDesignWidget.cpp \
    TabLogicDesignWidget.cpp \
    TabCalibrationWidget.cpp \
    Softkey.cpp \
    T3kCommonData.cpp \
    SoftKeyDesignToolWidget.cpp \
    KeyDesignWidget.cpp \
    SelectDeviceWidget.cpp \
    KeyTracker.cpp \
    LayoutToolWidget.cpp \
    ArrangeHelperWidget.cpp \
    LogicDesignWidget.cpp \
    EditGateWidget.cpp \
    UserDefinedKeyWidget.cpp \
    KeyLineEditWidget.cpp \
    LogicGalleryWidget.cpp \


HEADERS  += \
    ../common/qsingleapplication/qtsinglecoreapplication.h \
    ../common/qsingleapplication/qtsingleapplication.h \
    ../common/qsingleapplication/qtlockedfile.h \
    ../common/qsingleapplication/qtlocalpeer.h \
    ../common/QT3kDevice.h \
    ../common/QT3kDeviceEventHandler.h \
    ../common/QIniFormat.h \
    ../common/QKeyMapStr.h \
    ../common/QUtils.h \
    ../common/QGUIUtils.h \
    ../common/ui/UnderlineLabel.h \
    ../common/ui/QHoverComboBox.h \
    ../common/ui/ColorTabBar.h \
    ../common/ui/Q2ColorTabWidget.h \
    ../common/ui/QLicenseWidget.h \
    T3kSoftlogicDlg.h \
    TabPanelWidget.h \
    TabKeyDesignWidget.h \
    TabLogicDesignWidget.h \
    TabCalibrationWidget.h \
    SoftkeyDef.h \
    SoftKey.h \
    T3kCommonData.h \
    SoftKeyDesignToolWidget.h \
    KeyDesignWidget.h \
    SelectDeviceWidget.h \
    KeyTracker.h \
    LayoutToolWidget.h \
    ArrangeHelperWidget.h \
    LogicDesignWidget.h \
    EditGateWidget.h \
    UserDefinedKeyWidget.h \
    KeyLineEditWidget.h \
    LogicGalleryWidget.h \


FORMS    += \
    ../common/ui/QLicenseWidget.ui \
    T3kSoftlogicDlg.ui \
    TabPanelWidget.ui \
    TabCalibrationWidget.ui \
    TabKeyDesignWidget.ui \
    TabLogicDesignWidget.ui \
    SoftKeyDesignToolWidget.ui \
    SelectDeviceWidget.ui \
    LayoutToolWidget.ui \
    ArrangeHelperWidget.ui \
    EditGateWidget.ui \
    LogicGalleryWidget.ui

win32:RC_FILE = ./resources/T3kSoftlogic.rc

OTHER_FILES +=

RESOURCES += \
    T3kSoftlogic.qrc
