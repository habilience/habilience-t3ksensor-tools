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
DEFINES += _QT_COMPILER_ QUAZIP_STATIC

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
    LIBS += -lsetupapi -lole32 -luuid -lws2_32 \

    CONFIG(release, debug|release) {
        LIBS += -L$$OUT_PWD/../external/quazip/release/ -lquazip \
                -L$$PWD/../external/T3kHIDLibrary/win32/lib/ -lT3kHIDLibStatic
        PRE_TARGETDEPS += $$OUT_PWD/../external/quazip/release/quazip.lib \
                            $$PWD/../external/T3kHIDLibrary/win32/lib/T3kHIDLibStatic.lib
    }

    CONFIG(debug, debug|release) {
        LIBS += -L$$OUT_PWD/../external/quazip/debug/ -lquazipd \
                -L$$PWD/../external/T3kHIDLibrary/win32/lib/ -lT3kHIDLibStaticd
        PRE_TARGETDEPS += $$OUT_PWD/../external/quazip/debug/quazipd.lib \
                            $$PWD/../external/T3kHIDLibrary/win32/lib/T3kHIDLibStaticd.lib
    }
}

linux-g++|linux-g++-64 {
    LIBS += #-L/usr/lib -lusb-1.0 \

    CONFIG(release, debug|release) {
        LIBS += -L$$OUT_PWD/../external/quazip/ -lquazip \
                -L$$PWD/../external/T3kHIDLibrary/linux/ -lT3kHIDLibStatic

        PRE_TARGETDEPS += $$OUT_PWD/../external/quazip/libquazip.a \
                           $$PWD/../external/T3kHIDLibrary/linux/libT3kHIDLibStatic.a
    }

    CONFIG(debug, debug|release) {
        LIBS += -L$$OUT_PWD/../external/quazip/ -lquazipd \
                -L$$PWD/../external/T3kHIDLibrary/linux/ -lT3kHIDLibStaticd

        PRE_TARGETDEPS += $$OUT_PWD/../external/quazip/libquazipd.a \
                            $$PWD/../external/T3kHIDLibrary/linux/libT3kHIDLibStaticd.a
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

#Socoures
SOURCES += main.cpp\
    QMouseSettingWidget.cpp \
    QCalibrationSettingWidget.cpp \
    QGeneralSettingWidget.cpp \
    QMenuStripWidget.cpp \
    QMainMenuWidget.cpp \
    QUnderlineLabel.cpp \
    QCheckableButton.cpp \
    QMouseMappingTable.cpp \
    KeyMapStr.cpp \
    QTouchSettingWidget.cpp \
    QProfileLabel.cpp \
    QTPDPEventMultiCaster.cpp \
    T30xHandle.cpp \
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
    QHoverComboBox.cpp \
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
    QSideviewWidget.cpp \
    QOrderTouchWidget.cpp \
    QCustomDefaultSensor.cpp \
    QTabSensorStatus.cpp \
    QTabRemoteAssistance.cpp \
    QTabChat.cpp \
    QColorTabWidget.cpp \
    QColorTabBar.cpp \
    QSimpleLed.cpp

HEADERS  += \
    QMouseSettingWidget.h \
    QCalibrationSettingWidget.h \
    QGeneralSettingWidget.h \
    QMenuStripWidget.h \
    QMainMenuWidget.h \
    QUnderlineLabel.h \
    QCheckableButton.h \
    QMouseMappingTable.h \
    KeyMapStr.h \
    QTouchSettingWidget.h \
    QProfileLabel.h \
    QTPDPEventMultiCaster.h \
    stdInclude.h \
    T30xHandle.h \
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
    QHoverComboBox.h \
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
    QSideviewWidget.h \
    QOrderTouchWidget.h \
    QCustomDefaultSensor.h \
    QTabSensorStatus.h \
    QTabRemoteAssistance.h \
    QTabChat.h \
    QColorTabWidget.h \
    QColorTabBar.h \
    QSimpleLed.h

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
    QSoftKeySettingWidget.ui \
    T3kCfgWnd.ui \
    QLoadSensorDataWidget.ui \
    QLicenseWidget.ui \
    QSelectSensorWidget.ui \
    QMenuStripWidget.ui \
    QSideviewWidget.ui \
    QTabSensorStatus.ui \
    QTabRemoteAssistance.ui \
    QTabChat.ui

RESOURCES += \
    T3kCfgRes.qrc

VERSION = 1.0.0.0

win32:RC_FILE = T3kCfg.rc

OTHER_FILES += \
    T3kCfg.rc
