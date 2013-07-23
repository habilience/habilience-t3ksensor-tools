#ifndef QMULTIMONSUPPORT_H
#define QMULTIMONSUPPORT_H

#include <QVector>
#include <QRect>
#include <QSize>
#include "QSingletone.h"

class QMultimonSupport : public QSingleton<QMultimonSupport>
{
private:
    int     m_nCurrentMonitorIndex;
public:
    int getMonitorCount();
    QSize getMonitorResolution( int nIndex );
    QRect getMonitorPosition( int nIndex );
    int getPrimaryMonitorIndex();

    void selectMonitor( int nIndex );
    int getSelectedMonitor();

    QRect getVirtualScreen();

public:
    QMultimonSupport(void);
};


#endif // QMULTIMONSUPPORT_H
