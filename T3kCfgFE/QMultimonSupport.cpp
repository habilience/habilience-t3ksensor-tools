#include "QMultimonSupport.h"
#include <QDesktopWidget>
#include <QApplication>

QMultimonSupport::QMultimonSupport()
{
    m_nCurrentMonitorIndex = 0;
}

int QMultimonSupport::getMonitorCount()
{
    QDesktopWidget* desktop = QApplication::desktop();
    return desktop->screenCount();
}

QSize QMultimonSupport::getMonitorResolution( int nIndex )
{
    QDesktopWidget* desktop = QApplication::desktop();
    QRect rc = desktop->screenGeometry(nIndex);
    return rc.size();
}

QRect QMultimonSupport::getMonitorPosition( int nIndex )
{
    QDesktopWidget* desktop = QApplication::desktop();
    return desktop->screenGeometry(nIndex);
}

int QMultimonSupport::getPrimaryMonitorIndex()
{
    QDesktopWidget* desktop = QApplication::desktop();
    return desktop->primaryScreen();
}

void QMultimonSupport::selectMonitor( int nIndex )
{
    if ( nIndex < 0 || nIndex >= getMonitorCount())
        m_nCurrentMonitorIndex = getPrimaryMonitorIndex();
    else
        m_nCurrentMonitorIndex = nIndex;
}

int QMultimonSupport::getSelectedMonitor()
{
    return m_nCurrentMonitorIndex;
}

QRect QMultimonSupport::getVirtualScreen()
{
    QDesktopWidget* desktop = QApplication::desktop();

    QRect rcMerge(0,0,0,0);
    for ( int i=0 ; i<desktop->screenCount(); i++ )
    {
        QRect rc = desktop->screenGeometry(i);
        if (rcMerge.isEmpty())
            rcMerge = rc;
        else
        {
            rcMerge = rcMerge.united(rc);
        }
    }

    return rcMerge;
}
