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

CONFIG += static staticlib

#Define
DEFINES += _QT_COMPILER_ QUAZIP_STATIC USE_T3K_STATIC_LIBS _T3KHANDLE_INCLUDE_REMOTE

linux-g++|linux-g++-64:DEFINES += OS_LINUX

macx:DEFINES += OS_MAC

CONFIG(debug, debug|release):DEFINES += _DEBUG
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_DEBUG

#Library
INCLUDEPATH += $$PWD/../external/quazip \
                $$PWD/../external/T3kHIDLibrary/include \
                $$PWD../external/quazip/zlib/ \

DEPENDPATH += $$PWD/../external/quazip \
                $$PWD/../external/T3kHIDLibrary/include \
                $$PWD../external/quazip/zlib/ \

win32 {
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

linux-g++ {
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

    LIBS += #-L/usr/lib -lusb-1.0 \
}

linux-g++-64 {
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

    LIBS += #-L/usr/lib -lusb-1.0 \
}

macx: {
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
    ../common/ui/QUnderlineLabel.cpp \
    ../common/ui/QHoverComboBox.cpp \
    QMouseSettingWidget.cpp \
    QCalibrationSettingWidget.cpp \
    QGeneralSettingWidget.cpp \
    QMenuStripWidget.cpp \
    QMainMenuWidget.cpp \
    QCheckableButton.cpp \
    QMouseMappingTable.cpp \
    KeyMapStr.cpp \
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
    ../common/ui/QUnderlineLabel.h \
    ../common/ui/QHoverComboBox.h \
    QMouseSettingWidget.h \
    QCalibrationSettingWidget.h \
    QGeneralSettingWidget.h \
    QMenuStripWidget.h \
    QMainMenuWidget.h \
    QCheckableButton.h \
    QMouseMappingTable.h \
    KeyMapStr.h \
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
