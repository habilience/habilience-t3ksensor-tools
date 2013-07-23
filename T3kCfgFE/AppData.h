#ifndef APPDATA_H
#define APPDATA_H

#include <QString>

struct AppData
{
    bool bScreenShotMode;
    bool bMaximizeToVirtualScreen;

    bool bIsSafeMode;
    int  nCameraCount;
    bool bIsSubCameraExist;
    QString strModelName;
    QString strFirmwareVersion;
};

extern AppData g_AppData;

#endif // APPDATA_H
