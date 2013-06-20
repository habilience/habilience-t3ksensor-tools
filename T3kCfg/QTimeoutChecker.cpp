#include "QTimeoutChecker.h"

#include "QTimerEvent"

QTimeoutChecker::QTimeoutChecker(QObject *parent) :
    QObject(parent)
{
    m_nTimerID = 0;
}

QTimeoutChecker::~QTimeoutChecker()
{
    Stop();
}

int QTimeoutChecker::Start(int nTimerout)
{
    int nTimerID = startTimer( nTimerout );
    Q_ASSERT( nTimerID > 0 );

    m_vID.push_back( nTimerID );

    return nTimerID;
}

void QTimeoutChecker::Stop( int nID /*=-1*/ )
{
    if( nID )
    {
        if( m_vID.contains( nID ) )
        {
            killTimer( nID );
            m_vID.remove( m_vID.indexOf( nID ) );
        }
    }
    else
    {
        for( QVector<int>::iterator iter = m_vID.begin(); iter != m_vID.end(); ++iter )
        {
            Q_ASSERT( (*iter) > 0 );
            Stop( (*iter) );
        }
        m_vID.clear();
    }
}

bool QTimeoutChecker::IsRunning(int nID)
{
    return m_vID.contains( nID );
}

void QTimeoutChecker::timerEvent(QTimerEvent *evt)
{
    int nID = evt->timerId();
    if( m_vID.contains( nID ) )
    {
        emit TimeOut( nID );
    }

    QObject::timerEvent(evt);
}
