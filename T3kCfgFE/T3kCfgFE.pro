#-------------------------------------------------
#
# Project created by QtCreator 2013-06-20T16:47:53
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = T3kCfgFE
CONFIG(debug, debug|release): TARGET = $$join(TARGET,,,d)

TEMPLATE = app

#CONFIG += static staticlib
#QMAKE_LFLAGS += -static

DEFINES += USE_T3K_STATIC_LIBS QUAZIP_STATIC

linux-g++:DEFINES += OS_LINUX
macx:DEFINES += OS_MAC

CONFIG(debug, debug|release):DEFINES += _DEBUG
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

#Library
INCLUDEPATH +=  $$PWD/../external/quazip \
                $$PWD/../external/T3kHIDLibrary/include \
                $$PWD/../external/quazip/zlib/ \

DEPENDPATH +=   $$PWD/../external/quazip \
                $$PWD/../external/T3kHIDLibrary/lib \
                $$PWD/../external/quazip/zlib/ \


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
    LIBS += $$PWD/../external/T3kHIDLibrary/linux/64bit/T3kHIDLib-1.0.so.0.0.0
    QMAKE_RPATHDIR += $$PWD/../external/T3kHIDLibrary/linux/64bit

    CONFIG(release, debug|release) {
        LIBS += -L$$PWD/../external/quazip/ -lquazip \
    }

    CONFIG(debug, debug|release) {
        LIBS += -L$$PWD/../external/quazip/ -lquazipd \
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

SOURCES += main.cpp\
        dialog.cpp \
    ../common/QUtils.cpp \
    QStyleButton.cpp \
    QT3kDevice.cpp \
    QT3kDeviceEventHandler.cpp \
    QCmdAsyncManagerCtrl.cpp \
    QLogSystem.cpp \
    QSensorInitDataCfg.cpp \
    QMultimonSupport.cpp \
    QLangRes.cpp \
    QIni.cpp \
    QLangZipFile.cpp \
    QLangManager.cpp \
    QInitDataIni.cpp \
    QSelectSensorDataDialog.cpp \
    QShowMessageBox.cpp \
    QEnterFileNameDialog.cpp \
    QFileNameEdit.cpp \
    QSelectDeviceDialog.cpp \
    QDataProgressDialog.cpp \
    QSideviewDialog.cpp \
    QDetectionDialog.cpp \
    QBentAdjustmentDialog.cpp \
    QTouchSettingDialog.cpp \
    QAreaSettingForm.cpp \
    QDetectionGraphForm.cpp \
    QBorderStyleEdit.cpp \
    QUnderlineTitle.cpp \
    QMyApplication.cpp \
    QGraphSensitivityCtrl.cpp \
    QDetectionGraphWidget.cpp \
    QEventRedirection.cpp \
    QBentCfgParam.cpp \
    QArrowButton.cpp \
    t3kcomdef.cpp \
    QPointClipper.cpp \
    QBentProgressDialog.cpp \
    QCalcCamValue.cpp \
    QAutoRangeCompleteDialog.cpp \
    QGestureProfileDialog.cpp \
    QColorTabWidget.cpp \
    QGestureMappingTable.cpp \
    QKeyMapStr.cpp \
    QEditActionEnableDialog.cpp \
    QEditActionKey4WayDialog.cpp \
    QEditActionKey2WayDialog.cpp \
    QEditActionKey1Dialog.cpp \
    QUserDefinedKeyCtrl.cpp \
    QKeyEdit.cpp \
    QRemoteTouchMarkDialog.cpp

HEADERS  += dialog.h \
    ../common/QUtils.h \
    QStyleButton.h \
    QT3kDevice.h \
    QT3kDeviceEventHandler.h \
    AppData.h \
    QCmdAsyncManagerCtrl.h \
    QLogSystem.h \
    QSensorInitDataCfg.h \
    QMultimonSupport.h \
    QSingletone.h \
    QLangRes.h \
    QIni.h \
    QLangZipFile.h \
    QLangManager.h \
    QInitDataIni.h \
    QSelectSensorDataDialog.h \
    QShowMessageBox.h \
    QEnterFileNameDialog.h \
    QFileNameEdit.h \
    QSelectDeviceDialog.h \
    t3kcomdef.h \
    QDataProgressDialog.h \
    QSideviewDialog.h \
    QDetectionDialog.h \
    QBentAdjustmentDialog.h \
    QTouchSettingDialog.h \
    QAreaSettingForm.h \
    QDetectionGraphForm.h \
    QBorderStyleEdit.h \
    QUnderlineTitle.h \
    QMyApplication.h \
    QGraphSensitivityCtrl.h \
    QDetectionGraphWidget.h \
    QEventRedirection.h \
    QBentCfgParam.h \
    QArrowButton.h \
    QPointClipper.h \
    QBentProgressDialog.h \
    QCalcCamValue.h \
    QAutoRangeCompleteDialog.h \
    QGestureProfileDialog.h \
    QColorTabWidget.h \
    QGestureMappingTable.h \
    QKeyMapStr.h \
    QEditActionEnableDialog.h \
    QEditActionKey4WayDialog.h \
    QEditActionKey2WayDialog.h \
    QEditActionKey1Dialog.h \
    QUserDefinedKeyCtrl.h \
    QKeyEdit.h \
    QRemoteTouchMarkDialog.h

FORMS    += dialog.ui \
    QSelectSensorDataDialog.ui \
    QEnterFileNameDialog.ui \
    QSelectDeviceDialog.ui \
    QDataProgressDialog.ui \
    QSideviewDialog.ui \
    QDetectionDialog.ui \
    QBentAdjustmentDialog.ui \
    QTouchSettingDialog.ui \
    QAreaSettingForm.ui \
    QDetectionGraphForm.ui \
    QBentProgressDialog.ui \
    QAutoRangeCompleteDialog.ui \
    QGestureProfileDialog.ui \
    QEditActionEnableDialog.ui \
    QEditActionKey4WayDialog.ui \
    QEditActionKey2WayDialog.ui \
    QEditActionKey1Dialog.ui \
    QRemoteTouchMarkDialog.ui

RESOURCES += \
    T3kCfgFE.qrc

win32:RC_FILE = T3kCfgFE.rc

macx:ICON =

OTHER_FILES += \
    T3kCfgFE.rc
