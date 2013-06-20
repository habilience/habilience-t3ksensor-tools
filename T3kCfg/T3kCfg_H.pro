#-------------------------------------------------
#
# Project created by QtCreator 2012-02-20T09:56:02
#
#-------------------------------------------------

QT       += core gui

TARGET = T3kCfg_H
TEMPLATE = app

#Define
DEFINES += _QT_COMPILER_ \
            HITACHI_VER \
            HITACHI_VID=0x2200 HITACHI_PID=0x3100

linux-g++:DEFINES += OS_LINUX
linux-g++-64:DEFINES += OS_LINUX

macx:DEFINES += OS_MAC

CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT
CONFIG(debug, debug|release):DEFINES += _DEBUG

#Library
linux-g++:CONFIG(release, debug|release):QMAKE_RPATHDIR += /usr/share/T3kShare/lib
linux-g++-64:CONFIG(release, debug|release):QMAKE_RPATHDIR += /usr/share/T3kShare/lib

linux-g++:CONFIG(debug, debug|release):QMAKE_RPATHDIR += ../quazip-build-desktop/ \
                                                            ../../WindowApp/T3kHIDLib/build/Linux/out/lib
linux-g++-64:CONFIG(debug, debug|release):QMAKE_RPATHDIR += ../quazip-build-desktop/ \
                                                            ../../WindowApp/T3kHIDLib/build/Linux/out/lib

INCLUDEPATH += ../quazip/zlib/ \
               ../../WindowApp/T3kHIDLib/T3kHIDLib/Include/

win32:LIBS += -lsetupapi -lole32 -luuid \
                ../quazip/release/quazip.dll \
                ../../WindowApp/T3kHIDLib/build/Win32/Out/Dll/Lib/T3kHIDLib.lib

linux-g++:LIBS += -L/usr/lib -lusb-1.0 \
                ../quazip-build-desktop/libquazip.so.1 \
                ../../WindowApp/T3kHIDLib/build/Linux/out/lib/T3kHIDLib-1.0.so.0.0.0

linux-g++-64:LIBS += -L/usr/lib -lusb-1.0 \
                ../quazip-build-desktop/libquazip.so.1 \
                ../../WindowApp/T3kHIDLib/build/Linux/out/lib/T3kHIDLib-1.0.so.0.0.0

macx:LIBS += -framework CoreFoundation \
            -framework IOKit \
            ../quazip-build-desktop/libquazip.1.dylib \

CONFIG(release, debug|release):macx:LIBS += ../../WindowApp/T3kHIDLib/build/OSX/T3kHIDLib/build/Release/libT3kHIDLib.dylib
CONFIG(debug, debug|release):macx:LIBS += ../../WindowApp/T3kHIDLib/build/OSX/T3kHIDLib/build/Debug/libT3kHIDLib.dylib

CONFIG(debug, debug|release):macx:QMAKE_POST_LINK += \
                                   install_name_tool -change libquazip.1.dylib \
                                   @executable_path/../../../../quazip-build-desktop/libquazip.1.dylib \
                                   ../T3kCfg-build-desktop/T3kCfg.app/Contents/MacOS/T3kCfg &&\
                                    install_name_tool -change /libT3kHIDLib.dylib \
                                   @executable_path/../../../../../WindowApp/T3kHIDLib/build/OSX/T3kHIDLib/build/Debug/libT3kHIDLib.dylib \
                                   ../T3kCfg-build-desktop/T3kCfg.app/Contents/MacOS/T3kCfg

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
    QSensorCamDiagDlg.cpp \
    QEditActionWnd.cpp \
    QEditAction2WDWnd.cpp \
    QEditAction4WDWnd.cpp \
    QEditActionEDWnd.cpp \
    QTabSideView.cpp \
    QTabDetection.cpp \
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
    QSelectSensorWidget.cpp

win32:SOURCES +=

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
    QSensorCamDiagDlg.h \
    QEditActionWnd.h \
    QEditAction2WDWnd.h \
    QEditAction4WDWnd.h \
    QEditActionEDWnd.h \
    QTabSideView.h \
    QTabDetection.h \
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
    T3kSoftkeyDef.h

win32:HEADERS +=

FORMS    += \
    QMouseSettingWidget.ui \
    QCalibrationSettingWidget.ui \
    QGeneralSettingWidget.ui \
    QMainMenuWidget.ui \
    QTouchSettingWidget.ui \
    QWarningWidget.ui \
    QDiableTouchWidget.ui \
    QSensorCamDiagDlg.ui \
    QEditActionWnd.ui \
    QEditAction2WDWnd.ui \
    QEditAction4WDWnd.ui \
    QEditActionEDWnd.ui \
    QTabSideView.ui \
    QTabDetection.ui \
    QSensorSettingWidget.ui \
    QSoftKeySettingWidget.ui \
    T3kCfgWnd.ui \
    QLoadSensorDataWidget.ui \
    QLicenseWidget.ui \
    QSelectSensorWidget.ui

win32:FORMS +=

RESOURCES += \
    T3kCfgRes.qrc

VERSION = 1.0.0.0

win32:RC_FILE = T3kCfg.rc

OTHER_FILES += \
    T3kCfg.rc
