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
DEFINES += _QT_COMPILER_ QUAZIP_STATIC _T3KHANDLE_REMOVE_PRV USE_T3K_STATIC_LIBS

linux-g++|linux-g++-64:DEFINES += OS_LINUX

macx:DEFINES += OS_MAC

CONFIG(debug, debug|release):DEFINES += _DEBUG
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_DEBUG

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

SOURCES += main.cpp\
    ../common/qsingleapplication/qtsinglecoreapplication.cpp \
    ../common/qsingleapplication/qtsingleapplication.cpp \
    ../common/qsingleapplication/qtlockedfile_win.cpp \
    ../common/qsingleapplication/qtlockedfile_unix.cpp \
    ../common/qsingleapplication/qtlockedfile.cpp \
    ../common/qsingleapplication/qtlocalpeer.cpp \
    ../common/T3kHandle.cpp \
    ../common/TPDPEventMultiCaster.cpp \
    ../common/QIniFormat.cpp \
    ../common/ui/QUnderlineLabel.cpp \
    ../common/ui/QHoverComboBox.cpp \
    T3kSoftlogicDlg.cpp \
    TabPanelWidget.cpp \
    TabKeyDesignWidget.cpp \
    TabLogicDesignWidget.cpp \
    TabCalibrationWidget.cpp \
    Softkey.cpp \
    T3kCommonData.cpp \
#    KeyTracker.cpp \
#    UserDefinedKeyCtrl.cpp \
#    LogicDesignerWnd.cpp \
#    LayoutToolBar.cpp \
#    KeyMapStr.cpp \
#    KeyEdit.cpp \
#    GPIOListCtrl.cpp \
#    GdipWndExt.cpp \
#    GdipTabWnd.cpp \
#    GdiPlusInitializer.cpp \
#    GdiplusExt.cpp \
#    DSelectModel.cpp \
#    DLogicGallery.cpp \
#    DEditGate.cpp \
#    DArrangeHelper.cpp \
#    BorderStyleEdit.cpp
    SoftKeyDesignToolWidget.cpp \
    KeyDesignWidget.cpp \
    SelectDeviceWidget.cpp \
    KeyTracker.cpp \
    ResizingGraphicsItem.cpp \
    GraphicsKeyItem.cpp \
    GraphicsButtonItem.cpp \
    EdgeResizeGrabber.cpp


HEADERS  += \
    ../common/qsingleapplication/qtsinglecoreapplication.h \
    ../common/qsingleapplication/qtsingleapplication.h \
    ../common/qsingleapplication/qtlockedfile.h \
    ../common/qsingleapplication/qtlocalpeer.h \
    ../common/T3kHandle.h \
    ../common/TPDPEventMultiCaster.h \
    ../common/QIniFormat.h \
    ../common/ui/QUnderlineLabel.h \
    ../common/ui/QHoverComboBox.h \
    T3kSoftlogicDlg.h \
    TabPanelWidget.h \
    TabKeyDesignWidget.h \
    TabLogicDesignWidget.h \
    TabCalibrationWidget.h \
    SoftkeyDef.h \
    SoftKey.h \
    T3kCommonData.h \
#    KeyTracker.h \
#    UserDefinedKeyCtrl.h \
#    LogicDesignerWnd.h \
#    LayoutToolBar.h \
#    KeyMapStr.h \
#    KeyEdit.h \
#    GPIOListCtrl.h \
#    GdipWndExt.h \
#    GdipTabWnd.h \
#    GdiPlusInitializer.h \
#    GdiplusExt.h \
#    DSelectModel.h \
#    DLogicGallery.h \
#    DEditGate.h \
#    DArrangeHelper.h \
#    BorderStyleEdit.h
    SoftKeyDesignToolWidget.h \
    KeyDesignWidget.h \
    SelectDeviceWidget.h \
    KeyTracker.h \
    ResizingGraphicsItem.h \
    GraphicsKeyItem.h \
    GraphicsButtonItem.h \
    EdgeResizeGrabber.h

FORMS    += \
    T3kSoftlogicDlg.ui \
    TabPanelWidget.ui \
    TabCalibrationWidget.ui \
    TabKeyDesignWidget.ui \
    TabLogicDesignWidget.ui \
    SoftKeyDesignToolWidget.ui \
    SelectDeviceWidget.ui

win32:RC_FILE = ./resources/T3kSoftlogic.rc

OTHER_FILES +=

RESOURCES += \
    T3kSoftlogic.qrc
