#ifndef STDINCLUDE_H
#define STDINCLUDE_H

#define ID_FLATMENU_HOME                            	(0x200)
#define ID_FLATMENU_MOUSE_SETTING			(0x100)
#define ID_FLATMENU_CALIBRATION				(0x101)
#define ID_FLATMENU_GENERAL_SETTING			(0x102)
#define ID_FLATMENU_REMOTE_ASSIST			(0x103)

#define WM_FLATMENU_GET_STATUS				(WM_USER+0x300)

#ifdef Q_OS_WIN
#define T3KFE_DIALOG_CLASSNAME			L"Habilience T3000 Factory-Edition Dialog"
#else
#define T3KFE_DIALOG_CLASSNAME			"Habilience T3000 Factory-Edition Dialog"
#endif

enum FlatMenuStatus { MenuHome = 0, MenuMouseSetting, MenuCalibrationSetting, MenuSensorSetting, MenuSoftkeySetting, MenuGeneralSetting, MenuRemote };

#ifdef Q_OS_WIN
#include "../../WindowApp/Common/T30xConstStr.h"
#else
#include "Common/T30xConstStr.h"
#endif

#define RoundP1(A)          (float)((long int) (((A)+.05) * 10))/10;

#define MM_MIN_SUPPORT_FIRMWARE_VERSION                 2.2
#define MM_LAST_SUPPORT_FRIMWARE_VERSION                2.4

#define MM_MIN_SUPPORT_MACMARGIN_N_INVERTWHEEL_VERSION  2.3
#define MM_MIN_SUPPORT_STT_VERSION                      2.52//2.6
#define MM_MIN_SUPPORT_DISPLAYORICMD_VERSION            2.52

#define QProfileName1                                   "General"
#define QProfileName2                                   "Windows 7"
#define QProfileName3                                   "Mac OS X"
#define QProfileName4                                   "Linux (GNOME)"
#define QProfileName5                                   "User Defined"

typedef struct _ST_SHAREDMEMORY
{
    char szDuplicateRuns;
} ST_SHAREDMEMORY;

#include <QWidget>
void genAdjustButtonWidgetForWinAndX11(QWidget *rootWidget);

#endif // STDINCLUDE_H
