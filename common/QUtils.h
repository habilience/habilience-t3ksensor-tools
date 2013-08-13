#ifndef QSTRINGUTILS_H
#define QSTRINGUTILS_H

#include <QString>
#include <QDir>

inline QString lstrip(const QString& str, const char *chars)
{
    QByteArray byteArray = str.toUtf8();
    for (int n=0 ; n<byteArray.size() ; n++)
    {
        if (0 == strchr(chars, byteArray.at(n)))
        {
            byteArray = byteArray.right(byteArray.size()-n);
            break;
        }
    }
    return QString(byteArray);
}

inline QString rstrip(const QString& str, const char *chars)
{
    QByteArray byteArray = str.toUtf8();
    int n = byteArray.size() - 1;
    for (; n >= 0; --n)
    {
        if (0 == strchr(chars, byteArray.at(n)))
        {
            byteArray = byteArray.left(n+1);
            break;
        }
    }
    return QString(byteArray);
}

inline QString lstrip(const QString &str)
{
    return lstrip(str, " \t\r\n");
}

inline QString rstrip(const QString &str)
{
    return rstrip(str, " \t\r\n");
}

inline QString trim( const QString& str, const char* chars)
{
    return lstrip( rstrip(str, chars), chars );
}

inline QString trim( const QString& str)
{
    return lstrip( rstrip(str, " \t\r\n"), " \t\r\n" );
}

inline QString extractLeft( QString& str, char ch )
{
    QString strExtracted;
    str = trim(str);

    if ( str.isEmpty() ) return strExtracted;

    int nSP = str.indexOf( ch );
    if ( nSP >= 0 )
    {
        strExtracted = str.left( nSP );
        str.remove( 0, nSP+1 );
        str = lstrip( str, " ");
        return strExtracted;
    }

    strExtracted = str;
    str = "";
    return strExtracted;
}

inline void makeDirectory( const QString& strDir )
{
    QDir dir;
    dir.mkpath(strDir);
}

#ifdef Q_OS_WIN
#include <windows.h>
#define MICROSOFT_TABLETPENSERVICE_PROPERTY "MicrosoftTabletPenServiceProperty"
#define TABLET_DISABLE_PRESSANDHOLD        0x00000001
#define TABLET_DISABLE_PENTAPFEEDBACK      0x00000008
#define TABLET_DISABLE_PENBARRELFEEDBACK   0x00000010
#define TABLET_DISABLE_TOUCHUIFORCEON      0x00000100
#define TABLET_DISABLE_TOUCHUIFORCEOFF     0x00000200
#define TABLET_DISABLE_TOUCHSWITCH         0x00008000
#define TABLET_DISABLE_FLICKS              0x00010000
#define TABLET_ENABLE_FLICKSONCONTEXT      0x00020000
#define TABLET_ENABLE_FLICKLEARNINGMODE    0x00040000
#define TABLET_DISABLE_SMOOTHSCROLLING     0x00080000
#define TABLET_DISABLE_FLICKFALLBACKKEYS   0x00100000
#define TABLET_ENABLE_MULTITOUCHDATA       0x01000000


const unsigned long dwHwndTabletProperty =
    TABLET_DISABLE_PRESSANDHOLD | // disables press and hold (right-click) gesture
    TABLET_DISABLE_PENTAPFEEDBACK | // disables UI feedback on pen up (waves)
    TABLET_DISABLE_PENBARRELFEEDBACK | // disables UI feedback on pen button down (circle)
    TABLET_DISABLE_FLICKS; // disables pen flicks (back, forward, drag down, drag up)

inline void setTabletPenServiceProperties( HWND hWnd )
{
    ATOM atom = ::GlobalAddAtomA(MICROSOFT_TABLETPENSERVICE_PROPERTY);
    ::SetPropA(hWnd, MICROSOFT_TABLETPENSERVICE_PROPERTY, reinterpret_cast<HANDLE>(dwHwndTabletProperty));
    ::GlobalDeleteAtom(atom);
}

inline void setTabletPenServicePropertiesWidthAllChilds( HWND hwnd )
{
    setTabletPenServiceProperties(hwnd);

    HWND hwndC = ::GetWindow(hwnd, GW_CHILD);
    while ( hwndC )
    {
        setTabletPenServicePropertiesWidthAllChilds(hwndC);
        hwndC = ::GetWindow(hwndC, GW_HWNDNEXT);
    }
}
#endif

#ifdef Q_OS_WIN
#define DISABLE_MSWINDOWS_TOUCH_PROPERTY \
    { HWND hWnd = (HWND)winId(); setTabletPenServiceProperties(hWnd); }
#define DISABLE_MSWINDOWS_TOUCH_PROPERTY_RECURSIVE \
    { HWND hWnd = (HWND)winId(); setTabletPenServicePropertiesWidthAllChilds(hWnd); }
#else
#define DISABLE_MSWINDOWS_TOUCH_PROPERTY
#define DISABLE_MSWINDOWS_TOUCH_PROPERTY_RECURSIVE
#endif

#endif // QSTRINGUTILS_H