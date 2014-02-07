#include "QRequestHIDManager.h"

#include <QTimerEvent>

QRequestHIDManager::QRequestHIDManager(QObject* parent) :
        QObject(parent)
{
    m_nTimer = 0;
    m_pT3kHandle = NULL;
}

QRequestHIDManager::~QRequestHIDManager()
{
    Stop();
}

void QRequestHIDManager::Start( QT3kDevice* pHandle )
{
    Q_ASSERT( pHandle );
    m_pT3kHandle = pHandle;

    m_bStart = true;

    if( !m_nTimer )
        m_nTimer = startTimer( 1 );
}

void QRequestHIDManager::Pause()
{
    if( m_nTimer )
    {
        killTimer( m_nTimer );
        m_nTimer = 0;
    }
}

void QRequestHIDManager::Stop()
{
    m_bStart = false;
    if( m_nTimer )
    {
        killTimer( m_nTimer );
        m_nTimer = 0;
    }
    m_RequestItem.clear();
    m_pT3kHandle = NULL;
}

void QRequestHIDManager::AddItem( const char* szItem, QString strValue, eRequestPart ePart /*= QRequestHIDManager::MM*/ )
{
    NVData Data;
    Data.ePart = ePart;
    Data.strItem = (QString)szItem;
    Data.strValue = strValue;

    QMutexLocker Lock( &m_Mutex );
    m_RequestItem.push_back( Data );

    //qDebug( "Add Item(%d): %d %s", m_RequestItem.count(), ePart, szItem );
}

void QRequestHIDManager::RemoveItem( const char* szItem, eRequestPart ePart /*= QRequestHIDManager::MM*/ )
{
    QMutexLocker Lock( &m_Mutex );
    for( int i=0; i<m_RequestItem.count(); i++ )
    {
        const NVData& Data = m_RequestItem.at( i );
        if( Data.ePart == ePart && !Data.strItem.compare( (QString)szItem ) )
        {
            m_RequestItem.removeAt( i );

            //qDebug( "Remove Item(%d): %d %s", m_RequestItem.count(), ePart, szItem );
            break;
        }
    }
}

void QRequestHIDManager::timerEvent(QTimerEvent *evt)
{
    if( evt->timerId() == m_nTimer )
    {
        QMutexLocker Lock( &m_Mutex );
        if( m_RequestItem.count() )
        {
            QString strCmd;
            for( int i=0; i<m_RequestItem.count(); i++ )
            {
                const NVData& Data = m_RequestItem.at( i );
                switch( Data.ePart )
                {
                case MM:
                    strCmd = QString("%1%2").arg(Data.strItem).arg(Data.strValue);
                    break;
                case CM1:
                    strCmd = QString("cam1/%1%2").arg(Data.strItem).arg(Data.strValue);
                    break;
                case CM2:
                    strCmd = QString("cam2/%1%2").arg(Data.strItem).arg(Data.strValue);
                    break;
                case CM1_1:
                    strCmd = QString("cam1/sub/%1%2").arg(Data.strItem).arg(Data.strValue);
                    break;
                case CM2_1:
                    strCmd = QString("cam2/sub/%1%2").arg(Data.strItem).arg(Data.strValue);
                    break;
                default:
                    Q_ASSERT( false );
                    continue;
                }

                m_pT3kHandle->sendCommand( strCmd, true );
                qDebug( "**Retry request** : %s%s", (const char*)Data.strItem.toUtf8(), (const char*)Data.strValue.toUtf8() );
            }

            if( m_bStart )
            {
                m_bStart = false;
                killTimer( m_nTimer );
                m_nTimer = startTimer( 1000 );
            }
        }
        else
        {
            m_bStart = false;
            killTimer( m_nTimer );
            m_nTimer = 0;
            emit finish();
        }
    }

    QObject::timerEvent(evt);
}
