#include "QMyApplication.h"

#include <QTimerEvent>
#include <QKeyEvent>

QMyApplication::QMyApplication(int &argc, char **argv) :
    QApplication(argc, argv)
{
    installEventFilter(this);
}

void QMyApplication::setMonitoringMouseMovement(bool bMonitoring)
{
    m_bWatchMouseMovement = bMonitoring;
}

bool QMyApplication::eventFilter(QObject *obj, QEvent *evt)
{
    if ( m_bWatchMouseMovement )
    {
        if ( evt->type() == QEvent::MouseMove )
        {
            emit mouseMoved();
        }
    }
    return QApplication::eventFilter(obj, evt);
}
