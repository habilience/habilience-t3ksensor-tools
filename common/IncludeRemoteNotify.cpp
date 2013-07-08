#include "IncludeRemoteNotify.h"

#include "PacketStructure.h"


IncludeRemoteNotify::IncludeRemoteNotify()
{
    m_pSocket = NULL;
    m_bRemoteMode = false;
}

void IncludeRemoteNotify::SetSocket(QTcpSocket *socket)
{
    m_pSocket = socket;
}

void IncludeRemoteNotify::SendRemoteNotifyPacket(int nType)
{
    if( m_pSocket == NULL || m_pSocket->state() != QAbstractSocket::ConnectedState ) return;

    RHeaderPkt packet;
    packet.nType = nType;
    packet.nPktSize = sizeof(RHeaderPkt);

    /*qint64 nRet = */m_pSocket->write( (const char*)&packet, packet.nPktSize );
}

void IncludeRemoteNotify::SendRemoteRawDataPacket(int nType, const char *pData, qint64 nDataSize)
{
    Q_ASSERT( pData != NULL && nDataSize != 0 );

    if( m_pSocket == NULL || m_pSocket->state() != QAbstractSocket::ConnectedState ) return;

    int nPacketSize = sizeof(RHeaderPkt) + nDataSize + sizeof(short);
    char* pBuffer = new char[nPacketSize];
    RRawDataPkt* packet = (RRawDataPkt*)pBuffer;
    packet->Header.nType = nType;
    packet->Header.nPktSize = nPacketSize;
    memcpy( packet->data, pData, nDataSize );

    /*qint64 nRet = */m_pSocket->write( (const char*)pBuffer, nPacketSize );

    delete[] pBuffer;

//    if( nRet < 0 )
//    {
//        // error
//    }
}

void IncludeRemoteNotify::onSensorDisconnect(T3K_HANDLE hDevice)
{
    // send to remote
    if( m_bRemoteMode )
        SendRemoteNotifyPacket( Client | NotifySensorDisconnected );

    OnCloseT3kDevice(hDevice);
}

int IncludeRemoteNotify::onReceiveRawData(void* pContext)
{
    // send to remote
    if( m_bRemoteMode )
    {
        if( m_pSocket == NULL || m_pSocket->state() != QAbstractSocket::ConnectedState ) return 0;

        int nTotalBytes = 0;
        do
        {
            char* rawData = ((T3kHandle*)pContext)->GetRawDataPacket( nTotalBytes );
            if( nTotalBytes == 0 || rawData == NULL ) break;

            Q_ASSERT( nTotalBytes < MAX_RAWDATA_BLOCK+1 );

            int nPacketSize = sizeof(RHeaderPkt) + nTotalBytes + sizeof(short);
            char* pBuffer = new char[nPacketSize];
            ::memset( pBuffer, 9, nPacketSize );
            RRawDataPkt* packet = (RRawDataPkt*)pBuffer;
            packet->Header.nType = Client | TranSensorRawData;
            packet->Header.nPktSize = nPacketSize;
            //packet->nTotalBlockCount = ++g_nRawDataCount;           // del
            ::memcpy( packet->data, rawData, nTotalBytes );

            qint64 nRet = nPacketSize;
            char* pSendBuf = pBuffer;
            qint64 nSend = 0;
            while( nPacketSize )
            {
//                qDebug( "write %02x %02x %02x %02x",
//                        (unsigned char)pSendBuf[0], (unsigned char)pSendBuf[1],
//                        (unsigned char)pSendBuf[2], (unsigned char)pSendBuf[3] );
                nRet = m_pSocket->write( (const char*)pSendBuf, nPacketSize );
                if ( nRet <= 0 ) break;
                nSend += nRet;
                nPacketSize -= nRet;
                pSendBuf += nRet;
            }

            //qDebug( "Write : %d, %d %d", nSend, g_nRawDataCount, packet->nTotalBlockCount );
            //qDebug( "Write : %d %s", packet->Header.nPktSize );
            //qDebug( "Write : %d %d %s", ((RRawDataPkt*)pBuffer)->Header.nType, ((RRawDataPkt*)pBuffer)->Header.nPktSize, (char*)(pBuffer+17) );
            delete[] pBuffer;
            delete[] rawData;

        } while( nTotalBytes > 0 );
    }

    return 0;
}

void IncludeRemoteNotify::onReceiveRawDataFlag(bool bReceive)
{
    m_bRemoteMode = bReceive;
}

void IncludeRemoteNotify::onDownloadingFirmware(int bDownload)
{
    // send to remote
    if( m_bRemoteMode )
        SendRemoteRawDataPacket( Client | NotifyFWDownloading, (const char*)&bDownload, sizeof(int) );

    OnFirmwareDownload( bDownload == 1 ? true : false );
}
