#include "QT3kDeviceR.h"

#include "QT3kDeviceREventHandler.h"
#include "PacketStructure.h"

#define MAX_RAWBUFFER           4194304     // 4M


QT3kDeviceR::QT3kDeviceR(QObject *parent) :
    QT3kDevice(parent)
{
    m_pT3kREventHandler = NULL;

    m_pRawDataBuffer = NULL;
    m_nTotalRawBytes = 0;

    m_bGetBothSensorData = false;
    m_bRemoting = false;
}

char* QT3kDeviceR::getRawDataPacket( int& nRetBytes )
{
    QMutexLocker Locker( &m_RawDataMutex );

    if( !m_pRawDataBuffer || !m_nTotalRawBytes )
    {
        nRetBytes = 0;
        return NULL;
    }

    int nReturnLength = m_nTotalRawBytes > MAX_RAWDATA_BLOCK ? MAX_RAWDATA_BLOCK : m_nTotalRawBytes;
    m_nTotalRawBytes -= nReturnLength;

    char* pBuffer = new char[nReturnLength];
    ::memcpy( pBuffer, m_pRawDataBuffer, nReturnLength );

    if( m_nTotalRawBytes )
        ::memmove( m_pRawDataBuffer, m_pRawDataBuffer+nReturnLength, m_nTotalRawBytes );

    nRetBytes = nReturnLength;

    return pBuffer;
}

void QT3kDeviceR::setEventHandler( QT3kDeviceREventHandler* pHandler )
{
    m_pT3kREventHandler = pHandler;

    disconnect( this, &QT3kDeviceR::receiveRawData, NULL, NULL );
    disconnect( this, &QT3kDeviceR::receiveRwDataFlag, NULL, NULL );

    if( m_pT3kREventHandler )
    {
        connect( this, &QT3kDeviceR::receiveRawData, m_pT3kREventHandler, &QT3kDeviceREventHandler::onReceiveRawData, Qt::QueuedConnection );
        connect( this, &QT3kDeviceR::receiveRwDataFlag, m_pT3kREventHandler, &QT3kDeviceREventHandler::onReceiveRawDataFlag, Qt::QueuedConnection );
    }

    QT3kDevice::setEventHandler( pHandler );
}

int QT3kDeviceR::onReceiveRawData(unsigned char * pBuffer, unsigned short nBytes)
{
    m_RemoteStatusMutex.lock();
    bool bRemoting = m_bRemoting;
    m_RemoteStatusMutex.unlock();
    if( bRemoting )
    {
//        if( !m_pNotify )
//            return 0;

        {
        QMutexLocker Locker( &m_RawDataMutex );
        Q_ASSERT( !m_pRawDataBuffer || m_nTotalRawBytes + nBytes < MAX_RAWBUFFER );

        ::memcpy( m_pRawDataBuffer+m_nTotalRawBytes, pBuffer, nBytes );
        m_nTotalRawBytes += nBytes;
        }

        // windnsoul signal
        emit receiveRawData( this );

        if( m_bGetBothSensorData )
            return 0;
        else
            return 1;
    }

    return 0;
}

void QT3kDeviceR::onReceiveRawDataFlag(bool bReceive)
{
    m_RemoteStatusMutex.lock();
    m_bRemoting = bReceive;
    m_RemoteStatusMutex.unlock();

    if( bReceive )
    {
        m_pNotify.fnOnReceiveRawData = _OnT3kReceiveRawDataHandler;

        QMutexLocker Lock( &m_RawDataMutex );
        // create buffer
        if( !m_pRawDataBuffer )
            m_pRawDataBuffer = new char[MAX_RAWBUFFER];

        m_nTotalRawBytes = 0;
        //g_nRawDataCount = 0;
    }
    else
    {
        m_pNotify.fnOnReceiveRawData = NULL;
    }

    m_pT3kREventHandler->m_bRemoteMode = bReceive;
    //emit receiveRwDataFlag( bReceive );

    if( !m_hDevice )
        return;

    ::T3kSetEventNotify( m_hDevice, &m_pNotify );

    if( !bReceive )
    {
        QMutexLocker Lock( &m_RawDataMutex );
        foreach( char* pBuf, m_qBuffers )
        {
            if( pBuf )
                delete[] pBuf;
        }
        m_qBuffers.clear();
        m_qBytes.clear();

        // create buffer
        if( m_pRawDataBuffer )
        {
            delete[] m_pRawDataBuffer;
            m_pRawDataBuffer = NULL;
        }

        m_nTotalRawBytes = 0;
    }
}
