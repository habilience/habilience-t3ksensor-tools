#include "QT3kDevice.h"

#include <QCoreApplication>
#include "QT3kDeviceEventHandler.h"
#include "QUtils.h"
#include "T3kConstStr.h"
#include <stdio.h>

#ifdef T3KDEVICE_CUSTOM
#include "PacketStructure.h"

#define MAX_RAWBUFFER           4194304     // 4M
#endif


QT3kDevice::QT3kDevice(QObject *parent) :
    QObject(parent)
{
    m_hDevice = NULL;

#ifdef Q_OS_WIN
    m_bIsVirtualDevice = false;
    m_TimerCheckVirtualDevice = 0;
#endif

    m_nInstantMode = 0;
    m_nExpireTime = 5000;
    m_dwFgstValue = 0x00;

#ifdef T3KDEVICE_CUSTOM
    m_pRawDataBuffer = NULL;
    m_nTotalRawBytes = 0;

    m_bGetBothSensorData = false;
    m_bRemoting = false;
#endif

    connect( this, SIGNAL(disconnected()), this, SLOT(onDisconnected()), Qt::QueuedConnection );
    connect( this, SIGNAL(downloadingFirmware(bool)), this, SLOT(onDownloadingFirmware(bool)), Qt::QueuedConnection );
    connect( this, SIGNAL(packetReceived()), this, SLOT(onPacketReceived()), Qt::QueuedConnection );
    connect( this, SIGNAL(packetReceivedSync()), this, SLOT(onPacketReceived()), Qt::BlockingQueuedConnection );
#ifdef T3KDEVICE_CUSTOM
    connect( this, SIGNAL(receiveRawData(void*)), QT3kDeviceEventHandler::instance(), SLOT(onReceiveRawData(void*)), Qt::QueuedConnection );
    connect( this, SIGNAL(receiveRawDataFlag(bool)), QT3kDeviceEventHandler::instance(), SLOT(onReceiveRawDataFlag(bool)), Qt::QueuedConnection );
#endif
}

QT3kDevice::~QT3kDevice()
{
    if (isOpen())
        close();

#ifdef T3KDEVICE_CUSTOM
    if( m_pRawDataBuffer )
    {
        delete[] m_pRawDataBuffer;
        m_pRawDataBuffer = NULL;
    }
#endif
}

#ifdef Q_OS_WIN
bool QT3kDevice::setFeature(void *pFeature, int nFeatureSize)
{
    if (!m_bIsVirtualDevice || !isOpen()) return false;

    if (!m_T3kVirtualDeviceHandle.SetFeature(pFeature, (unsigned long)nFeatureSize))
        return false;
    return true;
}

bool QT3kDevice::getFeature(void *pFeature, int nFeatureSize)
{
    if (!m_bIsVirtualDevice || !isOpen()) return false;

    if (!m_T3kVirtualDeviceHandle.GetFeature(pFeature, (unsigned long)nFeatureSize))
        return false;
    return true;
}

bool QT3kDevice::isVirtualDevice()
{
    return m_bIsVirtualDevice;
}

void QT3kDevice::timerEvent(QTimerEvent *evt)
{
    if (evt->type() == QEvent::Timer)
    {
        if (evt->timerId() == m_TimerCheckVirtualDevice)
        {
            FeatureCheckConnection checkConnection;
            checkConnection.ReportID = REPORTID_FEATURE_CHK_CONN;
            if (m_T3kVirtualDeviceHandle.GetFeature(&checkConnection, sizeof(FeatureCheckConnection)))
            {
                if (!checkConnection.ConnectionOK)
                {
                    T3K_DEVICE_INFO devInfo = ::T3kGetDeviceInfoFromHandle(m_hDevice);
                    QT3kDeviceEventHandler::instance()->_onDisconnected(devInfo);
                }
            }
        }
    }
}
#endif

void T3K_CALLBACK QT3kDevice::_OnT3kHandleEventsHandler( T3K_HANDLE /*hDevice*/, void * /*pContext*/ )
{
    qApp->processEvents();
}

void T3K_CALLBACK QT3kDevice::_OnT3kDisconnectHandler( T3K_HANDLE /*hDevice*/, void * pContext )
{
    QT3kDevice* pThis = (QT3kDevice*)pContext;
    emit pThis->disconnected();
}

#ifdef T3KDEVICE_CUSTOM
int  T3K_CALLBACK QT3kDevice::_OnT3kReceiveRawDataHandler( T3K_HANDLE /*hDevice*/, unsigned char* pBuffer, unsigned short nBytes, void * pContext )
{

    QT3kDevice* pThis = (QT3kDevice*)pContext;

    int nRet = pThis->onReceiveRawData( pBuffer, nBytes );

    return nRet;
}
#endif

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
        packet->length = PACKET_DEF_SIZE;
        m_PacketPool.pop_back();
        return packet;
    }
    t3kpacket* packet = (t3kpacket*)malloc(PACKET_DEF_SIZE);
    packet->length = PACKET_DEF_SIZE;
    return packet;
}

void QT3kDevice::releasePacket( t3kpacket* packet )
{
    QMutexLocker Lock(&m_PktPoolLock);
    if (m_PacketPool.size() >= PACKET_POOL_SIZE)
    {
        qDebug( "overflow packet pool" );
        free( packet );
    }
    else
    {
        m_PacketPool.push_back(packet);
    }
}

void T3K_CALLBACK QT3kDevice::_OnT3kPacketHandler( T3K_HANDLE /*hDevice*/, t3kpacket* packet, int bSync, void * pContext )
{
    QT3kDevice* pThis = (QT3kDevice*)pContext;

    {
        QMutexLocker Lock(&pThis->m_RecvPktQueLock);
        t3kpacket* packetNew = pThis->getPacket();
        if (packet->length > packetNew->length )
        {
            qDebug( "invalid packet length: %d/%d", packet->length, packetNew->length );
        }
        Q_ASSERT( packet->length <= packetNew->length );
        memcpy(packetNew, packet, packet->length);
        pThis->m_ReceivePacketQueue.push_back( packetNew );
    }

    if( bSync )
        pThis->packetReceivedSync();
    else
        emit pThis->packetReceived();
}

bool QT3kDevice::open( unsigned short nVID, unsigned short nPID, unsigned short nMI, int nDevIndex )
{
    if (isOpen())
    {
        qDebug( "QT3kDevice::open() - Already opended" );
        return true;
    }

    m_pNotify.Context = this;
    m_pNotify.fnOnDisconnect = _OnT3kDisconnectHandler;
    m_pNotify.fnOnDownloadingFirmware = _OnT3kDownloadingFirmwareHandler;
    m_pNotify.fnOnPacket = _OnT3kPacketHandler;
    m_pNotify.fnOnHandleEvents = _OnT3kHandleEventsHandler;
    m_pNotify.fnOnReceiveRawData = NULL;

    m_hDevice = ::T3kOpenDevice( nVID, nPID, nMI, nDevIndex );
    if (!m_hDevice)
    {
        qDebug( "QT3kDevice::open() - T3kOpenDevice fail: %d", ::T3kGetLastError() );
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
                        m_TimerCheckVirtualDevice = startTimer( 2000 );
                        break;
                    }
                }
            }
            close();
            return false;
        } while (false);
    }
#endif
    ::T3kSetEventNotify( m_hDevice, &m_pNotify );

    qDebug( "QT3kDevice::open() - OK" );

    return true;
}

bool QT3kDevice::isOpen()
{
    return m_hDevice != NULL ? true : false;
}

void QT3kDevice::close()
{
#ifdef Q_OS_WIN
    if (m_bIsVirtualDevice)
    {
        if (m_TimerCheckVirtualDevice)
        {
            killTimer(m_TimerCheckVirtualDevice);
            m_TimerCheckVirtualDevice = 0;
        }
        m_T3kVirtualDeviceHandle.Close();
        m_bIsVirtualDevice = false;
    }
#endif

    if (isOpen())
    {
        ::T3kCloseDevice( m_hDevice );
        m_hDevice = NULL;
    }

#ifdef Q_OS_WIN
    m_bIsVirtualDevice = false;
    m_TimerCheckVirtualDevice = 0;
#endif

    m_nInstantMode = 0;
    m_nExpireTime = 5000;
    m_dwFgstValue = 0x00;

#ifdef T3KDEVICE_CUSTOM
    m_pRawDataBuffer = NULL;
    m_nTotalRawBytes = 0;

    m_bGetBothSensorData = false;
    m_bRemoting = false;
#endif

    // free receive packet queue
    {
        t3kpacket* packet = NULL;
        QMutexLocker Lock(&m_RecvPktQueLock);
        for (int i=0 ; i<m_ReceivePacketQueue.size() ; i++)
        {
            packet = m_ReceivePacketQueue.at(i);
            releasePacket(packet);
        }
        m_ReceivePacketQueue.clear();
    }
    // free packet pool
    {
        t3kpacket* packet = NULL;
        QMutexLocker Lock(&m_PktPoolLock);
        for (int i=0 ; i<m_PacketPool.size() ; i++)
        {
            packet = m_PacketPool.at(i);
            //free( packet );
        }
        m_PacketPool.clear();
    }

    qDebug( "QT3kDevice::close()" );

}

void QT3kDevice::onDisconnected()
{
    T3K_DEVICE_INFO devInfo = ::T3kGetDeviceInfoFromHandle( m_hDevice );

    QT3kDeviceEventHandler::instance()->_onDisconnected(devInfo);
}

void QT3kDevice::onDownloadingFirmware( bool bIsDownload )
{
    T3K_DEVICE_INFO devInfo = ::T3kGetDeviceInfoFromHandle( m_hDevice );

    QT3kDeviceEventHandler::instance()->_onDownloadingFirmware(devInfo, bIsDownload);
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
    if ( !packet )
    {
        qDebug( "packet error!" );
        return;
    }

    T3K_DEVICE_INFO devInfo = ::T3kGetDeviceInfoFromHandle( m_hDevice );
    QT3kDeviceEventHandler* pEventHandler = QT3kDeviceEventHandler::instance();
    Q_ASSERT( pEventHandler );
    switch ( packet->type )
    {
    case t3ktype_msg:
            pEventHandler->_onMSG( devInfo, packet->ticktime, packet->partid,
                        packet->body.msg.message );
        break;
    case t3ktype_obj:
            pEventHandler->_onOBJ( devInfo, packet->ticktime, packet->partid, 0,
                        packet->body.obj.start_pos,
                        packet->body.obj.end_pos,
                        packet->body.obj.count );
        break;
    case t3ktype_obc:
            pEventHandler->_onOBC( devInfo, packet->ticktime, packet->partid, 0,
                        &packet->body.obc.start_pos,
                        &packet->body.obc.end_pos,
                        packet->body.obc.count );
        break;
    case t3ktype_dtc:
            pEventHandler->_onDTC( devInfo, packet->ticktime, packet->partid, 0,
                        packet->body.dtc.start_pos,
                        packet->body.dtc.end_pos,
                        packet->body.dtc.count );
        break;
    case t3ktype_ird:
            pEventHandler->_onIRD( devInfo, packet->ticktime, packet->partid,
                        packet->body.ixd.total,
                        packet->body.ixd.offset,
                        packet->body.ixd.data,
                        packet->body.ixd.count );
        break;
    case t3ktype_itd:
            pEventHandler->_onITD( devInfo, packet->ticktime, packet->partid,
                        packet->body.ixd.total,
                        packet->body.ixd.offset,
                        packet->body.ixd.data,
                        packet->body.ixd.count );
        break;
    case t3ktype_prv:
            pEventHandler->_onPRV( devInfo, packet->ticktime, packet->partid,
                        packet->body.prv.height,
                        packet->body.prv.offset_y,
                        packet->body.prv.image,
                        packet->body.prv.width );
        break;
    case t3ktype_cmd:
            pEventHandler->_onCMD( devInfo, packet->ticktime, packet->partid,
                        packet->body.cmd.id,
                        packet->body.cmd.command );
        break;
    case t3ktype_rse:
#ifdef T3KDEVICE_CUSTOM
            pEventHandler->_onRSE( devInfo, packet->ticktime, packet->partid,
                        packet->body.rsp.id,
                        packet->body.rsp.is_final,
                        packet->body.rsp.command );
        break;
#endif
    case t3ktype_rsp:
            pEventHandler->_onRSP( devInfo, packet->ticktime, packet->partid,
                        packet->body.rsp.id,
                        packet->body.rsp.is_final,
                        packet->body.rsp.command );
        break;
    case t3ktype_stt:
            pEventHandler->_onSTT( devInfo, packet->ticktime, packet->partid,
                        packet->body.stt.status );
        break;
    case t3ktype_dvc:
            pEventHandler->_onDVC( devInfo, packet->ticktime, packet->partid,
                        &packet->body.dvc );
        break;
    case t3ktype_tpt:
            pEventHandler->_onTPT( devInfo, packet->ticktime, packet->partid,
                        packet->body.tpt.touch_count,
                        packet->body.tpt.actual_touch,
                        packet->body.tpt.points );
        break;
    case t3ktype_gst:
            pEventHandler->_onGST( devInfo, packet->ticktime, packet->partid,
                        packet->body.gst.action_group,
                        packet->body.gst.action,
                        packet->body.gst.feasibleness,
                        packet->body.gst.x,
                        packet->body.gst.y,
                        packet->body.gst.w,
                        packet->body.gst.h,
                        packet->body.gst.zoom,
                        packet->body.gst.message );
        break;
    case t3ktype_ver:
            pEventHandler->_onVER( devInfo, packet->ticktime, packet->partid,
                        &packet->body.ver );
        break;
    }

    // return to pool
    releasePacket(packet);
}

T3K_DEVICE_INFO QT3kDevice::getDeviceInfo()
{
    if ( !isOpen() )
        return NULL;

    return ::T3kGetDeviceInfoFromHandle( m_hDevice );
}

static unsigned short s_nVID, s_nPID, s_nMI;
int QT3kDevice::getDeviceCount( unsigned short nVID, unsigned short nPID, unsigned short nMI )
{
    s_nVID = nVID;
    s_nPID = nPID;
    s_nMI = nMI;

#ifdef Q_OS_WIN
    if ( nVID == 0x2200 && nPID == 0xFF02 )		// Virtual HID Device for Remote Assistance
    {
        int nDevCnt = ::T3kGetDeviceCount( nVID, nPID, nMI );
        int nRealDevCnt = 0;
        t3k_hid_library::CHID VirtualDeviceChecker;
        for ( int i=0 ; i<nDevCnt ; i++ )
        {
            if ( VirtualDeviceChecker.Open( 0x2200, 0xFF02, 0, 0 ) )
            {
                FeatureCheckConnection CheckConnection;
                CheckConnection.ReportID = REPORTID_FEATURE_CHK_CONN;
                if ( VirtualDeviceChecker.GetFeature(&CheckConnection, sizeof(CheckConnection)) )
                {
                    if ( CheckConnection.ConnectionOK )
                    {
                        nRealDevCnt++;
                    }
                }
            }
        }

        return nRealDevCnt;
    }
#endif

    return ::T3kGetDeviceCount( nVID, nPID, nMI );
}

char* QT3kDevice::getDevicePath( unsigned short nVID, unsigned short nPID, unsigned short nMI, int nDevIndex )
{
    static T3K_DEVICE_INFO devInfo = NULL;
    devInfo = ::T3kGetDeviceInfo( nVID, nPID, nMI, nDevIndex );
    char* szDevPath = ::T3kGetDevInfoPath( devInfo );
    return szDevPath;
}

char* QT3kDevice::getDevicePath( int nDevIndex )
{
    static T3K_DEVICE_INFO devInfo = NULL;
    devInfo = ::T3kGetDeviceInfo( s_nVID, s_nPID, s_nMI, nDevIndex );
    if ( !devInfo ) return NULL;
    char * szDevPath = ::T3kGetDevInfoPath( devInfo );
    return szDevPath;
}

int QT3kDevice::sendCommand(const QString &cmd, bool bAsync/*=false*/, unsigned short nTimeout/*=1000*/ )
{
    if ( !isOpen() )
        return bAsync ? -1 : 0;

    static const char cstrHidModeUChar[] = { 'M', 'C', 'V', '\0', 'O', 'T', 'G', 'D', 'S', 'X' };
    static const char cstrHidModeLChar[] = { 'm', 'c', 'v', '\0', 'o', 't', 'g', 'd', 's', 'x' };

    QString strCmd = cmd;

    if ( strCmd.indexOf("instant_mode") >= 0 )
    {
        int nSP = strCmd.indexOf( '=' );
        if ( nSP >= 0 )
        {
            strCmd.remove( 0, nSP+1 );
            int nCP = strCmd.indexOf( ',' );
            if ( nCP >= 0 )
            {
                QString strInstantMode = strCmd.left(nCP);
                strInstantMode = trim(strInstantMode);

                int nInstantMode = 0x00;
                for ( int i=0 ; i<strInstantMode.size() ; i++ )
                {
                    char cChar = strInstantMode.at(i).toLatin1();
                    for ( unsigned int m=0; m<sizeof(cstrHidModeUChar)/sizeof(char) ; m++ )
                    {
                        if ( ('\0' != cChar) && (cstrHidModeUChar[m] == cChar || cstrHidModeLChar[m] == cChar) )
                        {
                            nInstantMode |= (0x01 << m);
                            break;
                        }
                    }
                }

                strCmd.remove( 0, nCP+1 );
                int nCP = strCmd.indexOf( ',' );
                int nTimeout;
                unsigned long dwFGstValue = 0x00;
                if ( nCP >= 0 )
                {
                    QString strTimeout = strCmd.left( nCP );
                    nTimeout = strTimeout.toInt( 0, 10 );
                    strCmd.remove( 0, nCP+1 );
                    dwFGstValue = strtoul( strCmd.toUtf8().data(), NULL, 16 );
                }
                else
                {
                    nTimeout = strCmd.toInt( 0, 10 );
                    dwFGstValue = (unsigned long)-1;
                }

                int nRetLen = 0;
                if ( (nRetLen = setInstantMode( nInstantMode, nTimeout, dwFGstValue )) > 0 )
                    return nRetLen;
            }
            else
            {
                QString strInstantMode = strCmd;
                strInstantMode = trim(strInstantMode);

                if ( strInstantMode.at(0) != '?' &&	// command chars
                     strInstantMode.at(0) != '!' &&
                     strInstantMode.at(0) != '*' )
                {
                    int nInstantMode = 0x00;
                    for ( int i=0 ; i<strInstantMode.size() ; i++ )
                    {
                        char cChar = strInstantMode.at(i).toLatin1();
                        for ( unsigned int m=0; m<sizeof(cstrHidModeUChar)/sizeof(char) ; m++ )
                        {
                            if ( ('\0' != cChar) && (cstrHidModeUChar[m] == cChar || cstrHidModeLChar[m] == cChar) )
                            {
                                nInstantMode |= (0x01 << m);
                                break;
                            }
                        }
                    }

                    int nTimeout = 5000;
                    unsigned long dwFGstValue = (unsigned long)-1;
                    int nRetLen = 0;
                    if( (nRetLen = setInstantMode( nInstantMode, nTimeout, dwFGstValue )) > 0 )
                        return nRetLen;
                }
            }
        }
    }
    else
    {
//		qDebug( "QT3kDevice::sendCommand - %s"), strCmd );
    }

    return ::T3kSendCommand( m_hDevice, strCmd.toLatin1(), bAsync ? 1 : 0, nTimeout );
}

bool QT3kDevice::writeBuffer( void* pBuffer, unsigned short nBytesToWrite, bool bAsync, unsigned short nTimeout )
{
    if (!isOpen())
        return false;

    return ::T3kSendBuffer( m_hDevice, (unsigned char*)pBuffer, nBytesToWrite, bAsync ? 1 : 0, nTimeout );
}

int QT3kDevice::setInstantMode( int nMode, int nExpireTime, unsigned long dwFgstValue/*=0x00*/ )
{
    if (!isOpen())
        return 0;

    m_nInstantMode = nMode;
    m_dwFgstValue = dwFgstValue;
    m_nExpireTime = nExpireTime;

    return ::T3kSetInstantMode( m_hDevice, nMode, nExpireTime, dwFgstValue );
}

bool QT3kDevice::getReportMessage()
{
    return m_nInstantMode & T3K_HID_MODE_MESSAGE ? true : false;
}

bool QT3kDevice::getReportCommand()
{
    return m_nInstantMode & T3K_HID_MODE_COMMAND ? true : false;
}

bool QT3kDevice::getReportView()
{
    return m_nInstantMode & T3K_HID_MODE_VIEW ? true : false;
}

bool QT3kDevice::getReportObject()
{
    return m_nInstantMode & T3K_HID_MODE_OBJECT ? true : false;
}

bool QT3kDevice::getReportTouchPoint()
{
    return m_nInstantMode & T3K_HID_MODE_TOUCHPNT ? true : false;
}

bool QT3kDevice::getReportGesture()
{
    return m_nInstantMode & T3K_HID_MODE_GESTURE ? true : false;
}

bool QT3kDevice::getReportDevice()
{
    return m_nInstantMode & T3K_HID_MODE_DEVICE ? true : false;
}

void QT3kDevice::setReportMessage(bool bReport)
{
    if (!isOpen())
        return;

    if (bReport)
        m_nInstantMode |= T3K_HID_MODE_MESSAGE;
    else
        m_nInstantMode &= ~T3K_HID_MODE_MESSAGE;

    ::T3kSetInstantMode( m_hDevice, m_nInstantMode, getExpireTime(), getFgstValue() );
}

void QT3kDevice::setReportCommand(bool bReport)
{
    if (!isOpen())
        return;

    if (bReport)
        m_nInstantMode |= T3K_HID_MODE_COMMAND;
    else
        m_nInstantMode &= ~T3K_HID_MODE_COMMAND;

    ::T3kSetInstantMode( m_hDevice, m_nInstantMode, getExpireTime(), getFgstValue() );
}

void QT3kDevice::setReportView(bool bReport)
{
    if (!isOpen())
        return;

    if (bReport)
        m_nInstantMode |= T3K_HID_MODE_VIEW;
    else
        m_nInstantMode &= ~T3K_HID_MODE_VIEW;

    ::T3kSetInstantMode( m_hDevice, m_nInstantMode, getExpireTime(), getFgstValue() );
}

void QT3kDevice::setReportObject(bool bReport)
{
    if (!isOpen())
        return;

    if (bReport)
        m_nInstantMode |= T3K_HID_MODE_OBJECT;
    else
        m_nInstantMode &= ~T3K_HID_MODE_OBJECT;

    ::T3kSetInstantMode( m_hDevice, m_nInstantMode, getExpireTime(), getFgstValue() );
}

void QT3kDevice::setReportTouchPoint(bool bReport)
{
    if (!isOpen())
        return;

    if (bReport)
        m_nInstantMode |= T3K_HID_MODE_TOUCHPNT;
    else
        m_nInstantMode &= ~T3K_HID_MODE_TOUCHPNT;

    ::T3kSetInstantMode( m_hDevice, m_nInstantMode, getExpireTime(), getFgstValue() );
}

void QT3kDevice::setReportGesture(bool bReport)
{
    if (!isOpen())
        return;

    if (bReport)
        m_nInstantMode |= T3K_HID_MODE_GESTURE;
    else
        m_nInstantMode &= ~T3K_HID_MODE_GESTURE;

    ::T3kSetInstantMode( m_hDevice, m_nInstantMode, getExpireTime(), getFgstValue() );
}

void QT3kDevice::setReportDevice(bool bReport)
{
    if (!isOpen())
        return;

    if (bReport)
        m_nInstantMode |= T3K_HID_MODE_DEVICE;
    else
        m_nInstantMode &= ~T3K_HID_MODE_DEVICE;

    ::T3kSetInstantMode( m_hDevice, m_nInstantMode, getExpireTime(), getFgstValue() );
}

int QT3kDevice::getLastError()
{
    return ::T3kGetLastError();
}

bool QT3kDevice::enableTouch( bool bEnable, bool bAsync/*=false*/, unsigned short nTimeout/*=1000*/ )
{
    char szBuffer[64];
    snprintf( szBuffer, 64, "%s=%d", cstrTouchEnable, bEnable ? 1 : 0 );

    return sendCommand( szBuffer, bAsync, nTimeout ) > 0 ? true : false;
}

bool QT3kDevice::enableMouse( bool bEnable )
{
    int nInstantMode = getInstantMode();
    (bEnable) ? nInstantMode &= ~T3K_HID_MODE_MOUSE_DISABLE : nInstantMode |= T3K_HID_MODE_MOUSE_DISABLE;
    return (setInstantMode(nInstantMode, getExpireTime(), getFgstValue()) > 0) ? true : false;
}

bool QT3kDevice::setCamModeSideview( int nCamNo, bool bAsync/*=false*/, unsigned short nTimeout/*=1000*/ )
{
    char szBuffer[64];
    for ( int i=0 ; i<4 ; i++ )
    {
        if ( i == nCamNo ) continue;
        if ( i < 2 )	// main camera
        {
            snprintf( szBuffer, 64, "cam%d/mode=silence", i+1 );
            if ( sendCommand( szBuffer, bAsync, nTimeout ) <= 0 ) return false;
        }
        else			// sub camera
        {
            snprintf( szBuffer, 64, "cam%d/sub/mode=silence", i+1 );
            if ( sendCommand( szBuffer, bAsync, nTimeout ) <= 0 ) return false;
        }
    }

    if ( nCamNo < 2 )	// main camera
    {
        snprintf( szBuffer, 64, "cam%d/mode=sideview", nCamNo+1 );
        if ( sendCommand( szBuffer, bAsync, nTimeout ) <= 0 ) return false;
    }
    else			// sub camera
    {
        snprintf( szBuffer, 64, "cam%d/sub/mode=sideview", nCamNo+1 );
        if ( sendCommand( szBuffer, bAsync, nTimeout ) <= 0 ) return false;
    }

    return true;
}

bool QT3kDevice::setCamModeDetection( bool bAsync/*=false*/, unsigned short nTimeout/*=1000*/ )
{
    char szBuffer[64];
    for ( int i=0 ; i<4 ; i++ )
    {
        if ( i < 2 )	// main camera
        {
            snprintf( szBuffer, 64, "cam%d/mode=detection", i+1 );
            if ( sendCommand( szBuffer, bAsync, nTimeout ) <= 0 ) return false;
        }
        else			// sub camera
        {
            snprintf( szBuffer, 64, "cam%d/sub/mode=detection", i+1 );
            if ( sendCommand( szBuffer, bAsync, nTimeout ) <= 0 ) return false;
        }
    }

    return true;
}

bool QT3kDevice::queryFirmwareVersion( unsigned short nAddrMask, int* pnMode )
{
    return ::T3kQueryFirmwareVersion( m_hDevice, nAddrMask, pnMode, 0, 300 ) == 1 ? true : false;
}


#ifdef T3KDEVICE_CUSTOM
char* QT3kDevice::getRawDataPacket( int& nRetBytes )
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

int QT3kDevice::onReceiveRawData(unsigned char * pBuffer, unsigned short nBytes)
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

void QT3kDevice::onReceiveRawDataFlag(bool bReceive)
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

    emit receiveRawDataFlag( bReceive );

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
#endif
