#include "QMyApplication.h"

#include <QTimerEvent>
#include <QKeyEvent>
#include <QFont>
#include "QUtils.h"
#include "QGUIUtils.h"


QMyApplication::QMyApplication(int &argc, char **argv) :
    QtSingleApplication(argc, argv)
{
    QFont newFont = getSystemFont(NULL);
    setFont(newFont);
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
    return QtSingleApplication::eventFilter(obj, evt);
}
