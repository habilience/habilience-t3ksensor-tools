#-------------------------------------------------
#
# Project created by QtCreator 2010-07-09T09:56:02
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = T3kCfg
CONFIG(debug, debug|release): TARGET = $$join(TARGET,,,d)

TEMPLATE = app

#Define
DEFINES += _QT_COMPILER_ QUAZIP_STATIC USE_T3K_STATIC_LIBS _T3KHANDLE_INCLUDE_REMOTE


CONFIG(debug, debug|release):DEFINES += _DEBUG
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_DEBUG

#Library
INCLUDEPATH += $$PWD/../external/quazip \
                $$PWD/../external/T3kHIDLibrary/include \
                $$PWD../external/quazip/zlib/ \

DEPENDPATH += $$PWD/../external/quazip \
                $$PWD/../external/T3kHIDLibrary/include \
                $$PWD../external/quazip/zlib/ \

QMAKE_CFLAGS_RELEASE    += -Os
QMAKE_CXXFLAGS_RELEASE  += -Os

win32 {
    CONFIG += static staticlib
    QMAKE_LFLAGS += -static

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


#################################################################################################
## linux-g++
linux-g++:QMAKE_TARGET.arch = $$QMAKE_HOST.arch
linux-g++-32:QMAKE_TARGET.arch = x86
linux-g++-64:QMAKE_TARGET.arch = x86_64

linux-g++{
    DEFINES += OS_LINUX
    contains(QMAKE_TARGET.arch, x86_64):{
        message( "building for 64bit" );
        CONFIG(debug, debug|release): OBJECTS_DIR = $$PWD/.objs_x64/debug/
        CONFIG(debug, debug|release): MOC_DIR = $$PWD/.objs_x64/debug/
        CONFIG(release, debug|release): OBJECTS_DIR = $$PWD/.objs_x64/release/
        CONFIG(release, debug|release): MOC_DIR = $$PWD/.objs_x64/release/
        CONFIG(debug, debug|release): DESTDIR = $$PWD/debug_x64
        CONFIG(release, debug|release): DESTDIR = $$PWD/release_x64

        LIBS += $$PWD/../external/T3kHIDLibrary/linux/64bit/T3kHIDLib-1.0.so.0.0.0
        QMAKE_RPATHDIR += $$PWD/../external/T3kHIDLibrary/linux/64bit

        CONFIG(release, debug|release) {
            LIBS += -L$$PWD/../external/quazip/ -lquazip_x64
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
        QMAKE_RPATHDIR += $$PWD/../external/T3kHIDLibrary/linux/32bit

        CONFIG(release, debug|release) {
            LIBS += -L$$PWD/../external/quazip/ -lquazip
        }
        CONFIG(debug, debug|release) {
            LIBS += -L$$PWD/../external/quazip/ -lquazipd
        }
    }
}

linux-g++-32{
    DEFINES += OS_LINUX
    message( "building for 32bit" );
    CONFIG(debug, debug|release): OBJECTS_DIR = $$PWD/.objs/debug/
    CONFIG(debug, debug|release): MOC_DIR = $$PWD/.objs/debug/
    CONFIG(release, debug|release): OBJECTS_DIR = $$PWD/.objs/release/
    CONFIG(release, debug|release): MOC_DIR = $$PWD/.objs/release/
    CONFIG(debug, debug|release): DESTDIR = $$PWD/debug
    CONFIG(release, debug|release): DESTDIR = $$PWD/release

    LIBS += $$PWD/../external/T3kHIDLibrary/linux/32bit/T3kHIDLib-1.0.so.0.0.0
    QMAKE_RPATHDIR += $$PWD/../external/T3kHIDLibrary/linux/32bit

    CONFIG(release, debug|release) {
        LIBS += -L$$PWD/../external/quazip/ -lquazip
    }
    CONFIG(debug, debug|release) {
        LIBS += -L$$PWD/../external/quazip/ -lquazipd
    }
}
linux-g++-64{
    DEFINES += OS_LINUX
    message( "building for 64bit" );
    CONFIG(debug, debug|release): OBJECTS_DIR = $$PWD/.objs_x64/debug/
    CONFIG(debug, debug|release): MOC_DIR = $$PWD/.objs_x64/debug/
    CONFIG(release, debug|release): OBJECTS_DIR = $$PWD/.objs_x64/release/
    CONFIG(release, debug|release): MOC_DIR = $$PWD/.objs_x64/release/
    CONFIG(debug, debug|release): DESTDIR = $$PWD/debug_x64
    CONFIG(release, debug|release): DESTDIR = $$PWD/release_x64

    LIBS += $$PWD/../external/T3kHIDLibrary/linux/64bit/T3kHIDLib-1.0.so.0.0.0
    QMAKE_RPATHDIR += $$PWD/../external/T3kHIDLibrary/linux/64bit

    CONFIG(release, debug|release) {
        LIBS += -L$$PWD/../external/quazip/ -lquazip_x64
    }
    CONFIG(debug, debug|release) {
        LIBS += -L$$PWD/../external/quazip/ -lquazipd_x64
    }
}
##
#################################################################################################


macx: {
    CONFIG += static staticlib
    DEFINES += OS_MAC

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

    LIBS += -framework CoreFoundation \
            -framework IOKit \
            -framework CoreServices \
}

#Socoures
SOURCES += main.cpp\
    ../common/T3kHandle.cpp \
    ../common/IncludeRemoteNotify.cpp \
    ../common/TPDPEventMultiCaster.cpp \
    ../common/QKeyMapStr.cpp \
    ../common/QUtils.cpp \
    ../common/ui/QUnderlineLabel.cpp \
    ../common/ui/QHoverComboBox.cpp \
    QMouseSettingWidget.cpp \
    QCalibrationSettingWidget.cpp \
    QGeneralSettingWidget.cpp \
    QMenuStripWidget.cpp \
    QMainMenuWidget.cpp \
    QCheckableButton.cpp \
    QMouseMappingTable.cpp \
    QTouchSettingWidget.cpp \
    QProfileLabel.cpp \
    QCalibrationWidget.cpp \
    QWarningWidget.cpp \
    QDiableTouchWidget.cpp \
    QAssistanceWidget.cpp \
    QEditActionWnd.cpp \
    QEditAction2WDWnd.cpp \
    QEditAction4WDWnd.cpp \
    QEditActionEDWnd.cpp \
    QIconLabel.cpp \
    QUserDefinedKeyWidget.cpp \
    QDetectionGraphView.cpp \
    QSensorSensGaugeWidget.cpp \
    QSideViewImageWidget.cpp \
    QSideViewGraphWidget.cpp \
    QSideViewWidget.cpp \
    QSimpleProgressWidget.cpp \
    QFlatTextButton.cpp \
    QSensorSettingWidget.cpp \
    QMySystemTrayIcon.cpp \
    QLangRes.cpp \
    QLangManager.cpp \
    QCustomCmdLinkButton.cpp \
    QWidgetCloseEventManager.cpp \
    QRequestHIDManager.cpp \
    QSoftKeySettingWidget.cpp \
    QSoftkey.cpp \
    QSoftkeyTableWidget.cpp \
    QSoftkeyActionKeyWidget.cpp \
    QSoftkeyActionCellWidget.cpp \
    QKeyEditWidget.cpp \
    QLegendWidget.cpp \
    T3kCfgWnd.cpp \
    QRaisePushButton.cpp \
    stdInclude.cpp \
    QSaveLogWidget.cpp \
    QPrintLogData.cpp \
    QLoadSensorDataWidget.cpp \
    QCompressToZip.cpp \
    QT3kUserData.cpp \
    QIconProgressing.cpp \
    QLicenseWidget.cpp \
    QIconToolButton.cpp \
    QSelectSensorWidget.cpp \
    QT3kLoadSideviewObject.cpp \
    QT3kHIDObject.cpp \
    QT3kLoadDetectionObject.cpp \
    QT3kLoadSensorDataObject.cpp \
    QT3kLoadEnvironmentObject.cpp \
    QRDisableScreenWidget.cpp \
    QRemoteGuideWidget.cpp \
    QTimeoutChecker.cpp \
    QOrderTouchWidget.cpp \
    QCustomDefaultSensor.cpp \
    QTabSensorStatus.cpp \
    QTabRemoteAssistance.cpp \
    QTabChat.cpp \
    QColorTabWidget.cpp \
    QColorTabBar.cpp \
    QSimpleLed.cpp \

HEADERS  += \
    ../common/T3kHandle.h \
    ../common/IncludeRemoteNotify.h \
    ../common/TPDPEventMultiCaster.h \
    ../common/QKeyMapStr.h \
    ../common/QUtils.h \
    ../common/ui/QUnderlineLabel.h \
    ../common/ui/QHoverComboBox.h \
    QMouseSettingWidget.h \
    QCalibrationSettingWidget.h \
    QGeneralSettingWidget.h \
    QMenuStripWidget.h \
    QMainMenuWidget.h \
    QCheckableButton.h \
    QMouseMappingTable.h \
    QTouchSettingWidget.h \
    QProfileLabel.h \
    stdInclude.h \
    QCalibrationWidget.h \
    QWarningWidget.h \
    QDiableTouchWidget.h \
    QAssistanceWidget.h \
    QEditActionWnd.h \
    QEditAction2WDWnd.h \
    QEditAction4WDWnd.h \
    QEditActionEDWnd.h \
    QIconLabel.h \
    QUserDefinedKeyWidget.h \
    QKeyEditWidget.h \
    QDetectionGraphView.h \
    QSensorSensGaugeWidget.h \
    QSideViewImageWidget.h \
    QSideViewGraphWidget.h \
    QSideViewWidget.h \
    QSimpleProgressWidget.h \
    QFlatTextButton.h \
    QSensorSettingWidget.h \
    QMySystemTrayIcon.h \
    QLangRes.h \
    QLangManager.h \
    QCustomCmdLinkButton.h \
    QWidgetCloseEventManager.h \
    QRequestHIDManager.h \
    QSoftKeySettingWidget.h \
    QSoftkey.h \
    QSoftkeyTableWidget.h \
    QSoftkeyActionKeyWidget.h \
    QSoftkeyActionCellWidget.h \
    QLegendWidget.h \
    T3kCfgWnd.h \
    QRaisePushButton.h \
    QSaveLogWidget.h \
    QPrintLogData.h \
    LogDataDef.h \
    QLoadSensorDataWidget.h \
    QCompressToZip.h \
    QT3kUserData.h \
    QIconProgressing.h \
    QLicenseWidget.h \
    QIconToolButton.h \
    QSelectSensorWidget.h \
    HIDStateUserDef.h \
    WindowInfoUserDef.h \
    ReportConstStr.h \
    T3kPacketDef.h \
    T3kSoftkeyDef.h \
    QT3kLoadSideviewObject.h \
    QT3kHIDObject.h \
    QT3kLoadDetectionObject.h \
    QT3kLoadSensorDataObject.h \
    QT3kLoadEnvironmentObject.h \
    QRDisableScreenWidget.h \
    QRemoteGuideWidget.h \
    QTimeoutChecker.h \
    QOrderTouchWidget.h \
    QCustomDefaultSensor.h \
    QTabSensorStatus.h \
    QTabRemoteAssistance.h \
    QTabChat.h \
    QColorTabWidget.h \
    QColorTabBar.h \
    QSimpleLed.h \

FORMS    += \
    QMouseSettingWidget.ui \
    QCalibrationSettingWidget.ui \
    QGeneralSettingWidget.ui \
    QMainMenuWidget.ui \
    QTouchSettingWidget.ui \
    QWarningWidget.ui \
    QDiableTouchWidget.ui \
    QAssistanceWidget.ui \
    QEditActionWnd.ui \
    QEditAction2WDWnd.ui \
    QEditAction4WDWnd.ui \
    QEditActionEDWnd.ui \
    QSensorSettingWidget.ui \
    QSideViewWidget.ui \
    QSoftKeySettingWidget.ui \
    T3kCfgWnd.ui \
    QLoadSensorDataWidget.ui \
    QLicenseWidget.ui \
    QSelectSensorWidget.ui \
    QMenuStripWidget.ui \
    QTabSensorStatus.ui \
    QTabRemoteAssistance.ui \
    QTabChat.ui

RESOURCES += \
    T3kCfgRes.qrc

VERSION = 1.0.0.0

win32:RC_FILE = T3kCfg.rc

OTHER_FILES += \
    T3kCfg.rc
