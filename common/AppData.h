#ifndef APPDATA_H
#define APPDATA_H

#include <QString>

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

#define IDX_MM      (0)
#define IDX_CM1     (1)
#define IDX_CM2     (2)
#define IDX_CM1_1   (3)
#define IDX_CM2_1   (4)
#define IDX_MAX     (5)

struct AppData
{
    bool    bScreenShotMode;
    bool    bMaximizeToVirtualScreen;

    bool    cameraConnectionInfo[IDX_MAX];

    bool    bIsSafeMode;
    int     nCameraCount;
    bool    bIsSubCameraExist;
    QString strModelName;
    QString strFirmwareVersion;
};

extern AppData g_AppData;

#endif // APPDATA_H
