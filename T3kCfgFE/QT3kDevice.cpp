#include "QT3kDevice.h"

#include <QCoreApplication>

QT3kDevice::QT3kDevice(QObject *parent) :
    QObject(parent)
{
    m_hDevice = NULL;

#ifdef Q_OS_WIN
    m_bIsVirtualDevice = false;
#endif

    m_nInstantMode = 0;
    m_nExpireTime = 5000;
    m_dwFgstValue = 0x00;

    connect( this, SIGNAL(disconnected()), this, SLOT(onDisconnected()), Qt::QueuedConnection );
    connect( this, SIGNAL(downloadingFirmware(bool)), this, SLOT(onDownloadingFirmware(bool)), Qt::QueuedConnection );
    connect( this, SIGNAL(packetReceived()), this, SLOT(onPacketReceived()), Qt::QueuedConnection );
}

void T3K_CALLBACK QT3kDevice::_OnT3kHandleEventsHandler( T3K_HANDLE /*hDevice*/, void * /*pContext*/ )
{
    qApp->processEvents();
}

void T3K_CALLBACK QT3kDevice::_OnT3kDisconnectHandler( T3K_HANDLE /*hDevice*/, void * pContext )
{
    QT3kDevice* pThis = (QT3kDevice*)pContext;
    emit pThis->disconnected();
}

//int  T3K_CALLBACK QT3kDevice::_OnT3kReceiveRawDataHandler( T3K_HANDLE hDevice, unsigned char* pBuffer, unsigned short nBytes, void * pContext );

void T3K_CALLBACK QT3kDevice::_OnT3kDownloadingFirmwareHandler( T3K_HANDLE /*hDevice*/, int bDownload, void * pContext )
{
    QT3kDevice* pThis = (QT3kDevice*)pContext;
    emit pThis->downloadingFirmware(bDownload == 1 ? true : false);
}

t3kpacket* QT3kDevice::getPacket()
{
    QMutexLocker Lock(&m_PktPoolLock);
    if (m_PacketPool.size() > 0)
    {
        t3kpacket* packet = m_PacketPool.back();
        m_PacketPool.pop_back();
        return packet;
    }
    t3kpacket* packet = (t3kpacket*)malloc(PACKET_DEF_SIZE);
    return packet;
}

void QT3kDevice::releasePacket( t3kpacket* packet )
{
    QMutexLocker Lock(&m_PktPoolLock);
    if (m_PacketPool.size() >= PACKET_POOL_SIZE)
    {
        free( packet );
    }
    m_PacketPool.push_back(packet);
}

void T3K_CALLBACK QT3kDevice::_OnT3kPacketHandler( T3K_HANDLE /*hDevice*/, t3kpacket* packet, int /*bSync*/, void * pContext )
{
    QT3kDevice* pThis = (QT3kDevice*)pContext;

    {
        QMutexLocker Lock(&pThis->m_RecvPktQueLock);
        t3kpacket* packetNew = pThis->getPacket();
        Q_ASSERT( packet->length >= packetNew->length );
        memcpy(packetNew, packet, packet->length);
        pThis->m_ReceivePacketQueue.push_back( packetNew );
    }

    emit pThis->packetReceived();
}

bool QT3kDevice::open( unsigned short nVID, unsigned short nPID, unsigned short nMI, int nDevIndex )
{
    if (isOpen())
    {
        qDebug( "QT3kDevice: Already opended" );
        return true;
    }

    T3K_EVENT_NOTIFY notify;
    notify.Context = this;
    notify.fnOnDisconnect = _OnT3kDisconnectHandler;
    notify.fnOnDownloadingFirmware = _OnT3kDownloadingFirmwareHandler;
    notify.fnOnPacket = _OnT3kPacketHandler;
    notify.fnOnHandleEvents = _OnT3kHandleEventsHandler;
    notify.fnOnReceiveRawData = NULL;

    m_hDevice = ::T3kOpenDevice( nVID, nPID, nMI, nDevIndex );
    if (!m_hDevice)
    {
        qDebug( "T3kOpenDevice fail" );
        close();
        return false;
    }

#ifdef Q_OS_WIN
    m_bIsVirtualDevice = false;

    if ( nVID == 0x2200 && nPID == 0xFF02 )     // virtual hid device for remote assistance
    {
        do
        {
            if (m_T3kVirtualDeviceHandle.Open( nVID, nPID, 0, 0 ))
            {
                FeatureCheckConnection checkConnection;
                checkConnection.ReportID = REPORTID_FEATURE_CHK_CONN;
                if (m_T3kVirtualDeviceHandle.GetFeature(&checkConnection, sizeof(checkConnection)))
                {
                    if (checkConnection.ConnectionOK)
                    {
                        m_bIsVirtualDevice = true;
                        // TODO: startTimer 2s
                        break;
                    }
                }
            }
            close();
            return false;
        } while (false);
    }
#endif
    ::T3kSetEventNotify( m_hDevice, &notify );

    qDebug( "open device OK" );

    return true;
}

bool QT3kDevice::isOpen()
{
    return false;
}

void QT3kDevice::close()
{

}

void QT3kDevice::onDisconnected()
{

}

void QT3kDevice::onDownloadingFirmware( bool bIsDownload )
{

}

void QT3kDevice::onPacketReceived()
{
    t3kpacket* packet = NULL;
    // get packet
    {
        QMutexLocker Lock(&m_RecvPktQueLock);
        if (m_ReceivePacketQueue.size() <= 0)
            return;

        packet = m_ReceivePacketQueue.front();
        m_ReceivePacketQueue.pop_front();
    }

    // parse packet!!


    // return to pool
    releasePacket(packet);
}
