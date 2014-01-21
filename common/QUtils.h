#ifndef QSTRINGUTILS_H
#define QSTRINGUTILS_H

#include <QString>

QString lstrip(const QString& str, const char *chars);
QString rstrip(const QString& str, const char *chars);

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

QString extractLeft( QString& str, char ch );
void makeDirectory( const QString& strDir );

#ifdef Q_OS_WIN
#include <windows.h>
void setTabletPenServiceProperties( HWND hWnd );
void setTabletPenServicePropertiesWidthAllChilds( HWND hwnd );
#endif

#ifdef Q_OS_WIN
#define DISABLE_MSWINDOWS_TOUCH_PROPERTY \
    { HWND hWnd = (HWND)winId(); setTabletPenServiceProperties(hWnd); }
#define DISABLE_MSWINDOWS_TOUCH_PROPERTY_RECURSIVE \
    { HWND hWnd = (HWND)winId(); setTabletPenServicePropertiesWithAllChilds(hWnd); }
#else
#define DISABLE_MSWINDOWS_TOUCH_PROPERTY
#define DISABLE_MSWINDOWS_TOUCH_PROPERTY_RECURSIVE
#endif

#endif // QSTRINGUTILS_H
