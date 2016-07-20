#ifndef APPDATA_H
#define APPDATA_H

#include <QString>
#include <QColor>

#include "T3KCamDef.h"

struct RangeI
{
    int start;
    int end;
};

struct RangeF
{
    float start;
    float end;
};

//#define IDX_MM      (0)
//#define IDX_CM1     (1)
//#define IDX_CM2     (2)
//#define IDX_CM1_1   (3)
//#define IDX_CM2_1   (4)
//#define IDX_MAX     (5)

const QColor    s_clrNormalBorderColor(112, 154, 209);
const QColor    s_clrNormalBgColor(242, 245, 251);

const QColor    s_clrModifyBorderColor(153, 0, 48);
const QColor    s_clrModifyBgColor(255, 240, 245);

//const QString   sCam1("cam1/");
//const QString   sCam2("cam2/");
//const QString   sCam1_1("cam1/sub/");
//const QString   sCam2_1("cam2/sub/");

struct AppData
{
    bool    bScreenShotMode;
    bool    bMaximizeToVirtualScreen;

    bool    cameraConnectionInfo[IDX_MAX];

    bool    bIsSafeMode;
    int     nCameraCount;               // total count
    int     nSubCameraCount;            // sub count
    QString strModelName;
    QString strFirmwareVersion;
    bool    bUpgradeFW;
    int     nUpgradeDeviceListIdx;
    bool    bDelayStart;
    bool    bDevelop;
};

extern AppData g_AppData;

#endif // APPDATA_H
