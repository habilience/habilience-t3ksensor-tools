#include "QGUIUtils.h"

#include <QApplication>
#include <QDir>
#include <QSettings>
#include <QWidget>

#ifdef Q_OS_WIN
#include <windows.h>
QFont::Weight weightFromInteger(long weight)
{
    if (weight < 400)
        return QFont::Light;
    if (weight < 600)
        return QFont::Normal;
    if (weight < 700)
        return QFont::DemiBold;
    if (weight < 800)
        return QFont::Bold;
    return QFont::Black;
}
#endif

QFont getSystemFont(QWidget* widget)
{
    QFont fontSystem;
#if  defined(Q_OS_WIN) && !defined(T3K_DEFULAT_FONT)
    NONCLIENTMETRICSW ncm;
    ncm.cbSize = FIELD_OFFSET(NONCLIENTMETRICS, lfMessageFont) + sizeof(LOGFONT);
    SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0);
    QString strFont;
    strFont = QString::fromWCharArray(ncm.lfMessageFont.lfFaceName);
    //strFont = "Arial";
    fontSystem = QFont(strFont);
    if (ncm.lfMessageFont.lfWeight != FW_DONTCARE)
        fontSystem.setWeight(weightFromInteger(ncm.lfMessageFont.lfWeight));

    QSettings s("HKEY_CURRENT_USER\\Control Panel\\Desktop", QSettings::NativeFormat);
    const int clearTypeEnum = 2;
    if ( clearTypeEnum == s.value("FontSmoothingType",1) )
    {
        fontSystem.setStyleStrategy(QFont::PreferAntialias);
    }
    if (widget == NULL)
    {
        fontSystem.setPointSizeF( qApp->font().pointSizeF() );
    }
    else
    {
        fontSystem.setPointSizeF( widget->font().pointSizeF() );
    }
#else
    if (widget == NULL)
        fontSystem = qApp->font();
    else
        fontSystem = widget->font();
#endif
    return fontSystem;
}

#ifdef Q_OS_WIN
typedef struct _CustomdevicemodeW {
    WCHAR  dmDeviceName[CCHDEVICENAME];
    WORD dmSpecVersion;
    WORD dmDriverVersion;
    WORD dmSize;
    WORD dmDriverExtra;
    DWORD dmFields;
    union {
      /* printer only fields */
      struct {
        short dmOrientation;
        short dmPaperSize;
        short dmPaperLength;
        short dmPaperWidth;
        short dmScale;
        short dmCopies;
        short dmDefaultSource;
        short dmPrintQuality;
      };
      /* display only fields */
      struct {
        POINTL dmPosition;
        DWORD  dmDisplayOrientation;
        DWORD  dmDisplayFixedOutput;
      };
    };
    short dmColor;
    short dmDuplex;
    short dmYResolution;
    short dmTTOption;
    short dmCollate;
    WCHAR  dmFormName[CCHFORMNAME];
    WORD   dmLogPixels;
    DWORD  dmBitsPerPel;
    DWORD  dmPelsWidth;
    DWORD  dmPelsHeight;
    union {
        DWORD  dmDisplayFlags;
        DWORD  dmNup;
    };
    DWORD  dmDisplayFrequency;
#if(WINVER >= 0x0400)
    DWORD  dmICMMethod;
    DWORD  dmICMIntent;
    DWORD  dmMediaType;
    DWORD  dmDitherType;
    DWORD  dmReserved1;
    DWORD  dmReserved2;
#if (WINVER >= 0x0500) || (_WIN32_WINNT >= _WIN32_WINNT_NT4)
    DWORD  dmPanningWidth;
    DWORD  dmPanningHeight;
#endif
#endif /* WINVER >= 0x0400 */
} CT_DEVMODEW;

int getOrientation()
{
    CT_DEVMODEW dm;
    ZeroMemory(&dm, sizeof(dm));
    dm.dmSize = sizeof(dm);

    if( 0 != ::EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, (PDEVMODEW)&dm) )
    {
        if ( dm.dmFields & DM_DISPLAYORIENTATION )
            return (unsigned long)dm.dmDisplayOrientation;
    }

    return 0xFFFFFFFF;
}
#elif defined(Q_OS_LINUX)
//#include <X11/Xlib.h>
//#include <X11/extensions/Xrandr.h>
int getOrientation()
{
    return 0xFFFFFFFF;
//    Display *dpy;
//    dpy = XOpenDisplay (NULL);
//    if (!dpy) return 0xFFFFFFFF;

//    int screen = DefaultScreen(dpy);

//    Rotation rotation, current_rotation;

//    rotation = XRRRotations(dpy, screen, &current_rotation);

//    int orientation = 0xFFFFFFFF;
//    switch (current_rotation)
//    {
//        case RR_Rotate_0:
//            orientation = 0;
//            break;
//        case RR_Rotate_90:
//            orientation = 1;
//            break;
//        case RR_Rotate_180:
//            orientation = 2;
//            break;
//        case RR_Rotate_270:
//            orientation = 3;
//            break;
//        default:
//            break;
//    }

//    return orientation;
}
#elif defined(Q_OS_MAC)
#include <ApplicationServices/ApplicationServices.h>
int getOrientation()
{
    const int MAX_DISPLAYS = 16;
    CGDisplayErr dErr;
    CGDisplayCount displayCount;
    CGDisplayCount maxDisplay = MAX_DISPLAYS;
    CGDirectDisplayID onlineDisplays[MAX_DISPLAYS];
    CGDirectDisplayID mainDisplay = CGMainDisplayID();
    dErr = CGGetOnlineDisplayList(maxDisplay, onlineDisplays, &displayCount);
    if (dErr != kCGErrorSuccess)
        return 0xFFFFFFFF;

    for (CGDisplayCount i=0; i<displayCount; i++)
    {
        CGDirectDisplayID dID = onlineDisplays[i];
        if (mainDisplay == dID)
        {
            int fRotation = CGDisplayRotation(dID);
            return fRotation / 90;
        }
    }
    return 0xFFFFFFFF;
}
#endif
