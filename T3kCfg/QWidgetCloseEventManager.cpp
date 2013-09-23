#include "QWidgetCloseEventManager.h"

#include <QTimerEvent>

QWidgetCloseEventManager::_GC::_GC()
{
}

QWidgetCloseEventManager::_GC::~_GC()
{
    if( QWidgetCloseEventManager::s_pThis )
        delete QWidgetCloseEventManager::s_pThis;
    QWidgetCloseEventManager::s_pThis = NULL;
}
QWidgetCloseEventManager::_GC s_WCGC;

QWidgetCloseEventManager* QWidgetCloseEventManager::s_pThis = NULL;

QWidgetCloseEventManager::QWidgetCloseEventManager( QObject* pParent ) :
        QObject(pParent)
{    
}

QWidgetCloseEventManager::~QWidgetCloseEventManager()
{
    m_mapClosedWidget.clear();
}

QWidgetCloseEventManager* QWidgetCloseEventManager::instance()
{
    if( !s_pThis )
        s_pThis = new QWidgetCloseEventManager();
    return s_pThis;
}

bool QWidgetCloseEventManager::AddClosedWidget(QWidget *pWidget, int nTime)
{
    if( !pWidget->isWindow() ) return false;

    for( QMap<int ,QWidget*>::iterator iter = m_mapClosedWidget.begin() ; iter != m_mapClosedWidget.end() ; ++iter )
    {
        if( iter.value() == pWidget )
            return false;
    }
    int nID = startTimer( nTime );
    if( !nID ) return false;

    m_mapClosedWidget.insert( nID, pWidget );
    return true;
}

void QWidgetCloseEventManager::timerEvent(QTimerEvent *evt)
{
    for( QMap<int ,QWidget*>::iterator iter = m_mapClosedWidget.begin() ; iter != m_mapClosedWidget.end() ; ++iter )
    {
        if( evt->timerId() == iter.key() )
        {
            killTimer( iter.key() );
            QWidget* pWidget = (QWidget*)iter.value();
            if( pWidget->isActiveWindow() )
                pWidget->close();

            iter = m_mapClosedWidget.erase( iter );
            --iter;
        }
    }

    QObject::timerEvent(evt);
}
