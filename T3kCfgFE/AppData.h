#ifndef APPDATA_H
#define APPDATA_H

#include <QString>
#include <QColor>

const QColor    s_clrNormalBorderColor(112, 154, 209);
const QColor    s_clrNormalBgColor(242, 245, 251);

const QColor    s_clrModifyBorderColor(153, 0, 48);
const QColor    s_clrModifyBgColor(255, 240, 245);

const QString   sCam1("cam1/");
const QString   sCam2("cam2/");
const QString   sCam1_1("cam1/sub/");
const QString   sCam2_1("cam2/sub/");

struct AppData
{
    bool    bScreenShotMode;
    bool    bMaximizeToVirtualScreen;

    bool    bIsSafeMode;
    int     nCameraCount;
    bool    bIsSubCameraExist;
    QString strModelName;
    QString strFirmwareVersion;
};

extern AppData g_AppData;

#endif // APPDATA_H
