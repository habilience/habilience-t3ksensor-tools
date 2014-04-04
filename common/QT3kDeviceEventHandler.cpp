#include "./QT3kDeviceEventHandler.h"
#include "./T3kConstStr.h"

#ifdef T3KDEVICE_CUSTOM
#include "PacketStructure.h"
#endif


#define PARTID_LENGTH		(8)
ResponsePart getResponsePartFromPartId( const char* partid, const char* cmd )
{
    ResponsePart Part = MM;

    const char* szCmd = cmd;

    int nPartIdIdx = (int)strlen( partid )-1;

    if( partid[nPartIdIdx-1] == '-' )
    {
        nPartIdIdx -= 2;
    }

    switch( partid[nPartIdIdx] )
    {
    default:
        if( szCmd != NULL )
        {
            if ( strstr(szCmd, cstrCam1) == szCmd )
            {
                szCmd += (sizeof(cstrCam1)-1);
                Part = CM1;
                break;
            }

            if ( strstr(szCmd, cstrCam2) == szCmd )
            {
                szCmd += (sizeof(cstrCam2)-1);
                Part = CM2;
                break;
            }

            // TODO : sub ?
            const char cstrSub[] = "sub/";
            if( strstr(szCmd, cstrSub) == szCmd )
            {
                break;
            }
        }
        else
        {
            Part = MM;
        }
        break;
    case '1':
        {
            int nCamSub = 0;
            if( PARTID_LENGTH > nPartIdIdx+2 )
                nCamSub = (int)(partid[nPartIdIdx + 2]-'0');

            if( nCamSub > 0 )
                Part = CM1_1;
            else
                Part = CM1;
        }
        break;
    case '2':
        {
            int nCamSub = 0;
            if( PARTID_LENGTH > nPartIdIdx+2 )
                nCamSub = (int)(partid[nPartIdIdx + 2]-'0');

            if( nCamSub > 0 )
                Part = CM2_1;
            else
                Part = CM2;
        }
        break;
    }

    return Part;
}

QT3kDeviceEventHandler::QT3kDeviceEventHandler(QObject *parent) :
    QObject(parent)
{
#ifdef T3KDEVICE_CUSTOM
    m_pSocket = NULL;
    m_bRemoteMode = false;
#endif
}

QT3kDeviceEventHandler::~QT3kDeviceEventHandler()
{
    m_EventListener.clear();
}

void QT3kDeviceEventHandler::addListener(IListener* l)
{
    for (int i=0 ; i<m_EventListener.size() ; i++ )
    {
        if (m_EventListener.at(i) == l)
            return;
    }
    m_EventListener.push_back(l);
}

void QT3kDeviceEventHandler::removeListener(IListener* l)
{
    for (int i=0 ; i<m_EventListener.size() ; i++ )
    {
        if (m_EventListener.at(i) == l)
        {
            m_EventListener.remove(i);
            return;
        }
    }
}

void QT3kDeviceEventHandler::_onDisconnected( T3K_DEVICE_INFO devInfo )
{
#ifdef T3KDEVICE_CUSTOM
    // send to remote
    if( m_bRemoteMode )
        sendRemoteNotifyPacket( Client | NotifySensorDisconnected );
#endif

    for (int i=0 ; i<m_EventListener.size() ; i++)
    {
        m_EventListener.at(i)->TPDP_OnDisconnected(devInfo);
    }
}

void QT3kDeviceEventHandler::_onDownloadingFirmware( T3K_DEVICE_INFO devInfo, bool bIsDownload )
{
#ifdef T3KDEVICE_CUSTOM
    // send to remote
    if( m_bRemoteMode )
        sendRemoteRawDataPacket( Client | NotifyFWDownloading, (const char*)&bIsDownload, sizeof(int) );
#endif

    for (int i=0 ; i<m_EventListener.size() ; i++)
    {
        m_EventListener.at(i)->TPDP_OnDownloadingFirmware(devInfo, bIsDownload);
    }
}

void QT3kDeviceEventHandler::_onMSG( T3K_DEVICE_INFO devInfo, unsigned short ticktime,
             const char* partid, const char * txt )
{
    ResponsePart Part = getResponsePartFromPartId( partid, txt );

    for (int i=0 ; i<m_EventListener.size() ; i++)
    {
        m_EventListener.at(i)->TPDP_OnMSG(devInfo, Part, ticktime, partid, txt);
    }
}

void QT3kDeviceEventHandler::_onOBJ( T3K_DEVICE_INFO devInfo, unsigned short ticktime,
             const char* partid, unsigned char * layerid, float * start_pos, float * end_pos, int cnt )
{
    ResponsePart Part = getResponsePartFromPartId( partid, NULL );

    for (int i=0 ; i<m_EventListener.size() ; i++)
    {
        m_EventListener.at(i)->TPDP_OnOBJ(devInfo, Part, ticktime, partid, layerid, start_pos, end_pos, cnt);
    }
}

void QT3kDeviceEventHandler::_onOBC( T3K_DEVICE_INFO devInfo, unsigned short ticktime,
             const char* partid, unsigned char * layerid, float * start_pos, float * end_pos, int cnt )
{
    ResponsePart Part = getResponsePartFromPartId( partid, NULL );

    for (int i=0 ; i<m_EventListener.size() ; i++)
    {
        m_EventListener.at(i)->TPDP_OnOBC(devInfo, Part, ticktime, partid, layerid, start_pos, end_pos, cnt);
    }
}

void QT3kDeviceEventHandler::_onDTC( T3K_DEVICE_INFO devInfo, unsigned short ticktime,
             const char* partid, unsigned char * layerid, unsigned long * start_pos, unsigned long * end_pos, int cnt )
{
    ResponsePart Part = getResponsePartFromPartId( partid, NULL );

    for (int i=0 ; i<m_EventListener.size() ; i++)
    {
        m_EventListener.at(i)->TPDP_OnDTC(devInfo, Part, ticktime, partid, layerid, start_pos, end_pos, cnt);
    }
}

void QT3kDeviceEventHandler::_onIRD( T3K_DEVICE_INFO devInfo, unsigned short ticktime,
             const char* partid, int total, int offset, const unsigned char * data, int cnt )
{
    ResponsePart Part = getResponsePartFromPartId( partid, NULL );

    for (int i=0 ; i<m_EventListener.size() ; i++)
    {
        m_EventListener.at(i)->TPDP_OnIRD(devInfo, Part, ticktime, partid, total, offset, data, cnt);
    }
}

void QT3kDeviceEventHandler::_onITD( T3K_DEVICE_INFO devInfo, unsigned short ticktime,
             const char* partid, int total, int offset, const unsigned char * data, int cnt )
{
    ResponsePart Part = getResponsePartFromPartId( partid, NULL );

    for (int i=0 ; i<m_EventListener.size() ; i++)
    {
        m_EventListener.at(i)->TPDP_OnITD(devInfo, Part, ticktime, partid, total, offset, data, cnt);
    }
}

void QT3kDeviceEventHandler::_onPRV( T3K_DEVICE_INFO devInfo, unsigned short ticktime,
             const char* partid, int total, int offset, const unsigned char * data, int cnt )
{
    ResponsePart Part = getResponsePartFromPartId( partid, NULL );

    for (int i=0 ; i<m_EventListener.size() ; i++)
    {
        m_EventListener.at(i)->TPDP_OnPRV(devInfo, Part, ticktime, partid, total, offset, data, cnt);
    }
}

void QT3kDeviceEventHandler::_onTXE( T3K_DEVICE_INFO devInfo, unsigned short ticktime,
             const char* partid, int err_bytes )
{
    ResponsePart Part = getResponsePartFromPartId( partid, NULL );

    for (int i=0 ; i<m_EventListener.size() ; i++)
    {
        m_EventListener.at(i)->TPDP_OnTXE(devInfo, Part, ticktime, partid, err_bytes);
    }
}

void QT3kDeviceEventHandler::_onRXE( T3K_DEVICE_INFO devInfo, unsigned short ticktime,
             const char* partid, int err_bytes )
{
    ResponsePart Part = getResponsePartFromPartId( partid, NULL );

    for (int i=0 ; i<m_EventListener.size() ; i++)
    {
        m_EventListener.at(i)->TPDP_OnRXE(devInfo, Part, ticktime, partid, err_bytes);
    }
}

void QT3kDeviceEventHandler::_onCMD( T3K_DEVICE_INFO devInfo, unsigned short ticktime,
             const char* partid, int id, const char * cmd )
{
    ResponsePart Part = getResponsePartFromPartId( partid, cmd );

    for (int i=0 ; i<m_EventListener.size() ; i++)
    {
        m_EventListener.at(i)->TPDP_OnCMD(devInfo, Part, ticktime, partid, id, cmd);
    }
}

void QT3kDeviceEventHandler::_onRSP( T3K_DEVICE_INFO devInfo, unsigned short ticktime,
             const char* partid, int id, bool bFinal, const char * cmd )
{
    ResponsePart Part = getResponsePartFromPartId( partid, cmd );

    for (int i=0 ; i<m_EventListener.size() ; i++)
    {
        m_EventListener.at(i)->TPDP_OnRSP(devInfo, Part, ticktime, partid, id, bFinal, cmd);
    }
}

#ifdef T3KDEVICE_CUSTOM
void QT3kDeviceEventHandler::_onRSE( T3K_DEVICE_INFO devInfo, unsigned short ticktime,
             const char* partid, int id, bool bFinal, const char * cmd )
{
    ResponsePart Part = getResponsePartFromPartId( partid, cmd );

    for (int i=0 ; i<m_EventListener.size() ; i++)
    {
        m_EventListener.at(i)->TPDP_OnRSE(devInfo, Part, ticktime, partid, id, bFinal, cmd);
    }
}
#endif

void QT3kDeviceEventHandler::_onSTT( T3K_DEVICE_INFO devInfo, unsigned short ticktime,
             const char* partid, const char * status )
{
    ResponsePart Part = getResponsePartFromPartId( partid, status );

    for (int i=0 ; i<m_EventListener.size() ; i++)
    {
        m_EventListener.at(i)->TPDP_OnSTT(devInfo, Part, ticktime, partid, status);
    }
}

void QT3kDeviceEventHandler::_onDVC( T3K_DEVICE_INFO devInfo, unsigned short ticktime,
             const char* partid, t3kpacket::_body::_dvc * device )
{
    ResponsePart Part = getResponsePartFromPartId( partid, NULL );

    for (int i=0 ; i<m_EventListener.size() ; i++)
    {
        m_EventListener.at(i)->TPDP_OnDVC(devInfo, Part, ticktime, partid, device);
    }
}

void QT3kDeviceEventHandler::_onTPT( T3K_DEVICE_INFO devInfo, unsigned short ticktime,
             const char* partid, int count, int guess_cnt, t3ktouchpoint* points )
{
    ResponsePart Part = getResponsePartFromPartId( partid, NULL );

    for (int i=0 ; i<m_EventListener.size() ; i++)
    {
        m_EventListener.at(i)->TPDP_OnTPT(devInfo, Part, ticktime, partid, count, guess_cnt, points);
    }
}

void QT3kDeviceEventHandler::_onGST( T3K_DEVICE_INFO devInfo, unsigned short ticktime,
             const char* partid, unsigned char cActionGroup, unsigned char cAction, unsigned short wFeasibleness,
             unsigned short x, unsigned short y, unsigned short w, unsigned short h, float fZoom, const char* msg )
{
    ResponsePart Part = getResponsePartFromPartId( partid, NULL );

    for (int i=0 ; i<m_EventListener.size() ; i++)
    {
        m_EventListener.at(i)->TPDP_OnGST(devInfo, Part, ticktime, partid, cActionGroup, cAction, wFeasibleness, x, y, w, h, fZoom, msg);
    }
}

void QT3kDeviceEventHandler::_onVER( T3K_DEVICE_INFO devInfo, unsigned short ticktime, const char* partid, t3kpacket::_body::_ver* ver )
{
    ResponsePart Part = getResponsePartFromPartId( partid, NULL );

    for (int i=0 ; i<m_EventListener.size() ; i++)
    {
        m_EventListener.at(i)->TPDP_OnVER(devInfo, Part, ticktime, partid, ver);
    }
}

#ifdef T3KDEVICE_CUSTOM
void QT3kDeviceEventHandler::setSocket(QTcpSocket *socket)
{
    m_pSocket = socket;
}

void QT3kDeviceEventHandler::sendRemoteNotifyPacket(int nType)
{
    if( m_pSocket == NULL || m_pSocket->state() != QAbstractSocket::ConnectedState ) return;

    RHeaderPkt packet;
    packet.nType = nType;
    packet.nPktSize = sizeof(RHeaderPkt);

    /*qint64 nRet = */m_pSocket->write( (const char*)&packet, packet.nPktSize );
}

void QT3kDeviceEventHandler::sendRemoteRawDataPacket(int nType, const char *pData, qint64 nDataSize)
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

int QT3kDeviceEventHandler::onReceiveRawData(void* /*pContext*/)
{
    // send to remote
    if( m_bRemoteMode )
    {
        if( m_pSocket == NULL || m_pSocket->state() != QAbstractSocket::ConnectedState ) return 0;

        int nTotalBytes = 0;
        do
        {
            char* rawData = QT3kDevice::instance()->getRawDataPacket( nTotalBytes );
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

void QT3kDeviceEventHandler::onReceiveRawDataFlag(bool bReceive)
{
    m_bRemoteMode = bReceive;
}
#endif
