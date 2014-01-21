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
