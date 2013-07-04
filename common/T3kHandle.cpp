#include "./T3kHandle.h"

#include <stdio.h>
#include <QCoreApplication>

#include "../Common/PacketStructure.h"
#define MAX_RAWBUFFER           4194304     // 4M
#define DEPTH_FORMAT32          32

//static int g_nRawDataCount = 0;

T3kHandle::T3kHandle()
{
    m_pT3kDevice = NULL;
    m_pNotify = NULL;
    m_bOpen = false;

    m_bIsVirtualDevice = false;

    m_bCloseNotify = true;
    m_nInstantMode = 0;
    m_nExpireTime = 5000;
    m_dwFgstValue = 0;

    m_qRecvPacket.clear();

    m_pRawDataBuffer = NULL;
    m_nTotalRawBytes = 0;

    m_bGetBothSensorData = false;
    m_bRemoting = false;

    ::memset( &m_T3kNotify, 0, sizeof(T3K_EVENT_NOTIFY) );
    m_T3kNotify.Context = this;
    m_T3kNotify.fnOnPacket = OnPacket;
    m_T3kNotify.fnOnReceiveRawData = NULL;
    m_T3kNotify.fnOnDownloadingFirmware = OnDownloadingFirmware;
    m_T3kNotify.fnOnDisconnect = OnDisconnect;
}

T3kHandle::~T3kHandle()
{
    if( IsOpen() )
        Close();

    DisconnectSignal();

    foreach( t3kpacket* packet, m_qRecvPacket )
    {
        if( packet )
        {
            ::free( packet );
            packet = NULL;
        }
    }
    m_qRecvPacket.clear();

    if( m_pRawDataBuffer )
    {
        delete[] m_pRawDataBuffer;
        m_pRawDataBuffer = NULL;
    }

    foreach( char* pBuf, m_qBuffers )
    {
        if( pBuf )
            delete[] pBuf;
    }
    m_qBuffers.clear();
    m_qBytes.clear();
}

t3kpacket* T3kHandle::GetReceivePacket()
{
    QMutexLocker Locker( &m_Mutex );
    if( m_qRecvPacket.isEmpty() ) return NULL;
    t3kpacket* packet = m_qRecvPacket.front();
    m_qRecvPacket.pop_front();
    return packet;
}

char* T3kHandle::GetRawDataPacket( int& nRetBytes )
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

void T3kHandle::ConnectSignal()
{
    connect( this, SIGNAL(Connect(T3K_HANDLE)), m_pNotify, SLOT(onConnect(T3K_HANDLE)), Qt::DirectConnection );
    connect( this, SIGNAL(Disconnect(T3K_HANDLE)), m_pNotify, SLOT(onDisconnect(T3K_HANDLE)), Qt::QueuedConnection );
    connect( this, SIGNAL(Packet(void*)), m_pNotify, SLOT(onPacket(void*)), Qt::QueuedConnection );
    connect( this, SIGNAL(PacketSync(void*)), m_pNotify, SLOT(onPacket(void*)), Qt::BlockingQueuedConnection );
    connect( this, SIGNAL(ReceiveRawData(void*)), m_pNotify, SLOT(onReceiveRawData(void*)), Qt::QueuedConnection );
    connect( this, SIGNAL(DownloadingFirmware(int)), m_pNotify, SLOT(onDownloadingFirmware(int)), Qt::QueuedConnection );
    connect( this, SIGNAL(ReceiveRawDataFlag(bool)), m_pNotify, SLOT(onReceiveRawDataFlag(bool)), Qt::QueuedConnection );
}

void T3kHandle::DisconnectSignal()
{
    disconnect( this, SIGNAL(Connect(T3K_HANDLE)), NULL, NULL );
    disconnect( this, SIGNAL(Disconnect(T3K_HANDLE)), NULL, NULL );
    disconnect( this, SIGNAL(Packet(void*)), NULL, NULL );
    disconnect( this, SIGNAL(PacketSync(void*)), NULL, NULL );
    disconnect( this, SIGNAL(ReceiveRawData(void*)), NULL, NULL );
    disconnect( this, SIGNAL(DownloadingFirmware(int)), NULL, NULL );
    disconnect( this, SIGNAL(ReceiveRawDataFlag(bool)), NULL, NULL );
}

bool T3kHandle::SetNotify(ITPDPT3kNotify *pNotify)
{
    m_pNotify = pNotify;

    if( !m_pT3kDevice )
        return false;

    if( pNotify )
    {
        m_T3kNotify.Context = this;
        m_T3kNotify.fnOnPacket = OnPacket;
        m_T3kNotify.fnOnReceiveRawData = NULL;
        m_T3kNotify.fnOnDownloadingFirmware = OnDownloadingFirmware;
        m_T3kNotify.fnOnDisconnect = OnDisconnect;
        m_T3kNotify.fnOnHandleEvents = OnHandleEvents;

        ::T3kSetEventNotify( m_pT3kDevice, &m_T3kNotify );
        ConnectSignal();
    }
    else
    {
        ::memset( &m_T3kNotify, 0, sizeof(T3K_EVENT_NOTIFY) );

        ::T3kSetEventNotify( m_pT3kDevice, &m_T3kNotify );
        DisconnectSignal();
    }

    return true;
}

bool T3kHandle::IsOpen()
{
    return m_pT3kDevice != NULL ? true : false;
}

bool T3kHandle::Open()
{
#ifdef HITACHI_VER
    m_pT3kDevice = ::T3kOpenDevice( HITACHI_VID, HITACHI_PID, 1, 0 );
#else
    do
    {
        int nOldT3000Cnt = ::T3kGetDeviceCount( 0xFFFF, 0x0000, 1 );
        if( nOldT3000Cnt )
        {
            m_pT3kDevice = ::T3kOpenDevice( 0xFFFF, 0x0000, 1, 0 );
            break;
        }
        int nT3000Cnt = ::T3kGetDeviceCount( 0x2200, 0x3000, 1 );
        if( nT3000Cnt )
        {
            m_pT3kDevice = ::T3kOpenDevice( 0x2200, 0x3000, 1, 0 );
            break;
        }
        int nT3100Cnt = ::T3kGetDeviceCount( 0x2200, 0x3100, 1 );
        if( nT3100Cnt )
        {
            m_pT3kDevice = ::T3kOpenDevice( 0x2200, 0x3100, 1, 0 );
            break;
        }
        int nT3200Cnt = ::T3kGetDeviceCount( 0x2200, 0x3200, 1 );
        if( nT3200Cnt )
        {
            m_pT3kDevice = ::T3kOpenDevice( 0x2200, 0x3200, 1, 0 );
            break;
        }
        int nT3500Cnt = ::T3kGetDeviceCount( 0x2200, 0x3500, 1 );
        if( nT3500Cnt )
        {
            m_pT3kDevice = ::T3kOpenDevice( 0x2200, 0x3500, 1, 0 );
            break;
        }
        int nT3900Cnt = ::T3kGetDeviceCount( 0x2200, 0x3900, 1 );
        if( nT3900Cnt )
        {
            m_pT3kDevice = ::T3kOpenDevice( 0x2200, 0x3900, 1, 0 );
            break;
        }
    } while( false );
#endif
    bool bRet = m_pT3kDevice != NULL ? true : false;
    if( bRet )
    {
        SetNotify( m_pNotify );

        emit Connect( m_pT3kDevice );
    }
    return bRet;
}

bool T3kHandle::OpenWithVIDPID( unsigned short nVID, unsigned short nPID, unsigned short nMI /*= 1*/, int nDevIndex /*= 0*/ )
{
#ifdef HITACHI_VER
    if( nVID != HITACHI_VID || nPID != HITACHI_PID ) return false;
#endif
    m_pT3kDevice = ::T3kOpenDevice( nVID, nPID, nMI, nDevIndex );

    bool bRet = m_pT3kDevice != NULL ? true : false;
    if( bRet )
    {
        SetNotify( m_pNotify );

        emit Connect( m_pT3kDevice );
    }
    //qDebug( "T3kHandle::OpenWithVIDPID - %d", bRet );
    return bRet;
}

bool T3kHandle::Close( bool bNotify )
{
    if( !m_pT3kDevice )
        return false;

    m_nInstantMode = 0;

    m_bCloseNotify = bNotify;
    ::T3kCloseDevice( m_pT3kDevice );
    m_pT3kDevice = NULL;

    return true;
}

unsigned long T3kHandle::GetLastError()
{
    if( !m_pT3kDevice )
        return 0x00;
    return ::T3kGetLastError();
}

int T3kHandle::GetDeviceCount( unsigned short nVID, unsigned short nPID, unsigned short nMI /*= 1*/ )
{
    return ::T3kGetDeviceCount( nVID, nPID, nMI );
}

char* T3kHandle::GetDevPath(unsigned short nVID, unsigned short nPID, unsigned short nMI, int nDevIndex)
{
    T3K_DEVICE_INFO devInfo = ::T3kGetDeviceInfo( nVID, nPID, nMI, nDevIndex );
    if( !devInfo ) return NULL;
    return ::T3kGetDevInfoPath( devInfo );
}

ushort T3kHandle::GetDeviceVID( T3K_DEVICE_INFO devInfo )
{
    return ::T3kGetDevInfoVendorID( devInfo );
}

ushort T3kHandle::GetDevicePID( T3K_DEVICE_INFO devInfo )
{
    return ::T3kGetDevInfoProductID( devInfo );
}

int T3kHandle::SendBuffer(const unsigned char *pBuffer, unsigned short nBufferSize, int bAsync, unsigned short nTimeout)
{
    if( !m_pT3kDevice )
        return -1;

    return ::T3kSendBuffer( m_pT3kDevice, pBuffer, nBufferSize, bAsync, nTimeout );
}

int T3kHandle::SendCommand(const char* lpszCmd, bool bASync, unsigned short nTimeout )
{
    if( !m_pT3kDevice )
        return bASync ? -1 : 0;

    //qDebug( "T3kHandle::SendCommand - %s", lpszCmd );
    return ::T3kSendCommand( m_pT3kDevice, lpszCmd, bASync, nTimeout );
}

int T3kHandle::EnableTouch(bool bEnable, bool bASync, unsigned short nTimeout )
{
    static const char cstrTouchEnable[] = "touch_enable";
    char szBuffer[64];
    sprintf( szBuffer, "%s=%d", cstrTouchEnable, bEnable ? 1 : 0 );

    return SendCommand( szBuffer, bASync, nTimeout ) > 0 ? true : false;
}

int T3kHandle::EnableMouse(bool bEnable, bool /*bASync*/, unsigned short /*nTimeout*/ )
{
    int nInstantMode = GetInstantMode();
    (bEnable) ? nInstantMode &= ~T3K_HID_MODE_MOUSE_DISABLE : nInstantMode |= T3K_HID_MODE_MOUSE_DISABLE;
    return (SetInstantMode(nInstantMode, GetExpireTime(), GetFGstValue()) > 0) ? true : false;
}

int T3kHandle::SetInstantMode(int nMode, unsigned short nExpireTime, unsigned long dwFgstValue)
{
    if( !m_pT3kDevice )
        return -1;

    if( (m_nInstantMode == nMode) &&
        (m_nExpireTime == nExpireTime) &&
        (m_dwFgstValue == dwFgstValue) ) return -1;

    m_nInstantMode = nMode;
    m_nExpireTime = nExpireTime;
    m_dwFgstValue = dwFgstValue;

    return ::T3kSetInstantMode( m_pT3kDevice, nMode, nExpireTime, dwFgstValue );
}

bool T3kHandle::QueryFirmwareVersion(unsigned short wAddrMask, int *pnMode)
{
    return ::T3kQueryFirmwareVersion( m_pT3kDevice, wAddrMask, pnMode, 0, 1000 ) == 1 ? true : false;
}

// slot
void T3kHandle::onReceiveRawDataFlag(bool bReceive)
{
    m_RemoteStatusMutex.lock();
    m_bRemoting = bReceive;
    m_RemoteStatusMutex.unlock();

    if( bReceive )
    {
        m_T3kNotify.fnOnReceiveRawData = OnReceiveRawData;

        QMutexLocker Lock( &m_RawDataMutex );
        // create buffer
        if( !m_pRawDataBuffer )
            m_pRawDataBuffer = new char[MAX_RAWBUFFER];

        m_nTotalRawBytes = 0;
        //g_nRawDataCount = 0;
    }
    else
    {
        m_T3kNotify.fnOnReceiveRawData = NULL;
    }

    emit ReceiveRawDataFlag( bReceive );

    if( !m_pT3kDevice )
        return;

    ::T3kSetEventNotify( m_pT3kDevice, &m_T3kNotify );

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

void T3kHandle::ExOnDisconnect(T3K_HANDLE hDevice)
{
    m_nInstantMode = 0;
    m_dwFgstValue = 0;
    m_nExpireTime = 5000;
    {
    QMutexLocker Lock( &m_Mutex );
    m_qRecvPacket.clear();
    }
    if( !m_pNotify || !m_bCloseNotify )
    {
        m_bCloseNotify = true;
        return;
    }
    emit Disconnect(hDevice);
}

void T3kHandle::ExOnPacket(t3kpacket *packet, int nSync)
{
    if( !m_pNotify )
        return;

    t3kpacket* pPacket = (t3kpacket*)malloc( packet->length );
    ::memcpy( pPacket, packet, packet->length );
    {
    QMutexLocker Locker( &m_Mutex );
    m_qRecvPacket.push_back( pPacket );
    }
    qDebug("ExOnPacket : %d, %d", pPacket->type, nSync );
    if( nSync )
        PacketSync( this );
    else 
        emit Packet( this );
}

int T3kHandle::ExOnReceiveRawData(unsigned char *pBuffer, unsigned short nBytes)
{
    m_RemoteStatusMutex.lock();
    bool bRemoting = m_bRemoting;
    m_RemoteStatusMutex.unlock();
    if( bRemoting )
    {
        if( !m_pNotify )
            return 0;

        {
    //        qDebug( "Recv Raw : %d - %s", nBytes, (char*)pBuffer+11 );
        QMutexLocker Locker( &m_RawDataMutex );
        Q_ASSERT( !m_pRawDataBuffer || m_nTotalRawBytes + nBytes < MAX_RAWBUFFER );

        ::memcpy( m_pRawDataBuffer+m_nTotalRawBytes, pBuffer, nBytes );
        m_nTotalRawBytes += nBytes;
        }

        emit ReceiveRawData( this );

        if( m_bGetBothSensorData )
            return 0;
        else
            return 1;
    }

    return 0;
}

void T3kHandle::ExOnDownloadingFirmware(int bDownload)
{
    if( !m_pNotify )
        return;
    emit DownloadingFirmware( bDownload );
}

void T3kHandle::OnDisconnect(T3K_HANDLE hDevice, void *pContext)
{
    ((T3kHandle*)pContext)->ExOnDisconnect( hDevice );
}

void T3kHandle::OnPacket(T3K_HANDLE /*hDevice*/, t3kpacket *packet, int nSync, void *pContext)
{
    ((T3kHandle*)pContext)->ExOnPacket( packet, nSync );
}

int T3kHandle::OnReceiveRawData(T3K_HANDLE /*hDevice*/, unsigned char *pBuffer, unsigned short nBytes, void *pContext)
{
    return ((T3kHandle*)pContext)->ExOnReceiveRawData( pBuffer, nBytes );
}

void T3kHandle::OnDownloadingFirmware(T3K_HANDLE /*hDevice*/, int bDownload, void *pContext)
{
    ((T3kHandle*)pContext)->ExOnDownloadingFirmware( bDownload );
}

void T3kHandle::OnHandleEvents(T3K_HANDLE /*hDevice*/, void */*pContext*/)
{
    qApp->processEvents();
}

void T3kHandle::SetExpireTime(int nTime)
{
    if( !m_pT3kDevice )
        return;

    m_nExpireTime = nTime;

    ::T3kSetInstantMode( m_pT3kDevice, m_nInstantMode, GetExpireTime(), GetFGstValue() );
}

bool T3kHandle::GetReportMessage()
{
    return m_nInstantMode & T3K_HID_MODE_MESSAGE ? true : false;
}

bool T3kHandle::GetReportCommand()
{
    return m_nInstantMode & T3K_HID_MODE_COMMAND ? true : false;
}

bool T3kHandle::GetReportView()
{
    return m_nInstantMode & T3K_HID_MODE_VIEW ? true : false;
}

bool T3kHandle::GetReportObject()
{
    return m_nInstantMode & T3K_HID_MODE_OBJECT ? true : false;
}

bool T3kHandle::GetReportTouchPoint()
{
    return m_nInstantMode & T3K_HID_MODE_TOUCHPNT ? true : false;
}

bool T3kHandle::GetReportGesture()
{
    return m_nInstantMode & T3K_HID_MODE_GESTURE ? true : false;
}

bool T3kHandle::GetReportDevice()
{
    return m_nInstantMode & T3K_HID_MODE_DEVICE ? true : false;
}

void T3kHandle::SetReportMessage(bool bReport)
{
    if( !m_pT3kDevice )
        return;

    if( bReport )
        m_nInstantMode |= T3K_HID_MODE_MESSAGE;
    else
        m_nInstantMode &= ~T3K_HID_MODE_MESSAGE;

    ::T3kSetInstantMode( m_pT3kDevice, m_nInstantMode, GetExpireTime(), GetFGstValue() );
}

void T3kHandle::SetReportCommand(bool bReport)
{
    if( !m_pT3kDevice )
        return;

    if( bReport )
        m_nInstantMode |= T3K_HID_MODE_COMMAND;
    else
        m_nInstantMode &= ~T3K_HID_MODE_COMMAND;

    ::T3kSetInstantMode( m_pT3kDevice, m_nInstantMode, GetExpireTime(), GetFGstValue() );
}

void T3kHandle::SetReportView(bool bReport)
{
    if( !m_pT3kDevice )
        return;

    if( bReport )
        m_nInstantMode |= T3K_HID_MODE_VIEW;
    else
        m_nInstantMode &= ~T3K_HID_MODE_VIEW;

    ::T3kSetInstantMode( m_pT3kDevice, m_nInstantMode, GetExpireTime(), GetFGstValue() );
}

void T3kHandle::SetReportObject(bool bReport)
{
    if( !m_pT3kDevice )
        return;

    if( bReport )
        m_nInstantMode |= T3K_HID_MODE_OBJECT;
    else
        m_nInstantMode &= ~T3K_HID_MODE_OBJECT;

    ::T3kSetInstantMode( m_pT3kDevice, m_nInstantMode, GetExpireTime(), GetFGstValue() );
}

void T3kHandle::SetReportTouchPoint(bool bReport)
{
    if( !m_pT3kDevice )
        return;

    if( bReport )
        m_nInstantMode |= T3K_HID_MODE_TOUCHPNT;
    else
        m_nInstantMode &= ~T3K_HID_MODE_TOUCHPNT;

    ::T3kSetInstantMode( m_pT3kDevice, m_nInstantMode, GetExpireTime(), GetFGstValue() );
}

void T3kHandle::SetReportGesture(bool bReport)
{
    if( !m_pT3kDevice )
        return;

    if( bReport )
        m_nInstantMode |= T3K_HID_MODE_GESTURE;
    else
        m_nInstantMode &= ~T3K_HID_MODE_GESTURE;

    ::T3kSetInstantMode( m_pT3kDevice, m_nInstantMode, GetExpireTime(), GetFGstValue() );
}

void T3kHandle::SetReportDevice(bool bReport)
{
    if( !m_pT3kDevice )
        return;

    if( bReport )
        m_nInstantMode |= T3K_HID_MODE_DEVICE;
    else
        m_nInstantMode &= ~T3K_HID_MODE_DEVICE;

    ::T3kSetInstantMode( m_pT3kDevice, m_nInstantMode, GetExpireTime(), GetFGstValue() );
}


// ITPDPT3kNotify
ITPDPT3kNotify::ITPDPT3kNotify(QObject *pParent) :
        QObject(pParent)
{
    // OBJ
    m_pOBJ              = NULL;
    m_nMaxOBJ           = 0;
    // OBC
    memset( &m_OBC, 0, sizeof(T3kRangeF) );
    m_nMaxOBC           = 0;
    // DTC
    m_pDTC              = NULL;
    m_nMaxDTC           = 0;
    // IRD
    m_nCamIRDMax        = 0;
    m_ppIRD             = NULL;
    m_pnIRD             = NULL;
    // ITD
    m_nCamITDMax        = 0;
    m_ppITD             = NULL;
    m_pnITD             = NULL;
    m_ppChkFillITD      = NULL;
#ifndef _T3KHANDLE_REMOVE_PRV
    // PRV
    m_nCamPRVMax        = 0;
    m_pFillCheck        = NULL;
#endif
    // TPT
    m_nMaxContact       = 0;
}

ITPDPT3kNotify::~ITPDPT3kNotify()
{
    if( m_pOBJ )
    {
        delete[] m_pOBJ;
        m_pOBJ = NULL;
    }
    if( m_pDTC )
    {
        delete[] m_pDTC;
        m_pDTC = NULL;
    }
    if( m_pnIRD )
    {
        delete[] m_pnIRD;
        m_pnIRD = NULL;
    }
    if( m_ppIRD )
    {
        delete[] m_ppIRD;
        m_ppIRD = NULL;
    }
    if( m_pnITD )
    {
        delete[] m_pnITD;
        m_pnITD = NULL;
    }
    if( m_ppITD )
    {
        delete[] m_ppITD;
        m_ppITD = NULL;
    }
    if( m_ppChkFillITD )
    {
        delete[] m_ppChkFillITD;
        m_ppChkFillITD = NULL;
    }
#ifndef _T3KHANDLE_REMOVE_PRV
    if( m_pFillCheck )
    {
        delete[] m_pFillCheck;
        m_pFillCheck = NULL;
    }
#endif
}

void ITPDPT3kNotify::onConnect(T3K_HANDLE hDevice)
{
    OnOpenT3kDevice(hDevice);
}

void ITPDPT3kNotify::onDisconnect(T3K_HANDLE hDevice)
{
    OnCloseT3kDevice(hDevice);
}

void ITPDPT3kNotify::onPacket(void* pContext)
{
    t3kpacket* packet = ((T3kHandle*)pContext)->GetReceivePacket();
    if( !packet ) return;
    switch( packet->type )
    {
    case t3ktype_msg:   // message
        OnMSG( packet->ticktime, packet->partid, packet->body.msg.message );
        break;
    case t3ktype_obj:   // touch object
        {
            if( packet->partid == NULL )
                return;

            int nCamNo = (packet->partid[3] - '1');

            int cnt = packet->body.obj.count;
            float* start_pos = packet->body.obj.start_pos;
            float* end_pos = packet->body.obj.end_pos;

            if( m_pOBJ )
            {
                if( cnt > m_nMaxOBJ )
                {
                    delete[] m_pOBJ;
                    m_pOBJ = NULL;
                    m_nMaxOBJ = cnt;
                    m_pOBJ = new T3kRangeF[m_nMaxOBJ];
                }
            }
            else
            {
                if( cnt > m_nMaxOBJ )
                {
                    m_nMaxOBJ = cnt;
                }
                if( m_nMaxOBJ < 4 )
                    m_nMaxOBJ = 4;
                m_pOBJ = new T3kRangeF[m_nMaxOBJ];
            }

            for( int i=0 ;i<m_nMaxOBJ ; i++ )
            {
                m_pOBJ[i].Start = -1;
            }

            for( int i=0 ; i<cnt && i<m_nMaxOBJ ; i++ )
            {
                m_pOBJ[i].Start	= start_pos[i];
                m_pOBJ[i].End	= end_pos[i];
            }

            OnOBJ( packet->ticktime, packet->partid, nCamNo, m_pOBJ, packet->body.obj.count );
        }
        break;
    case t3ktype_obc:   // touch object
        {
            if( packet->partid == NULL )
                return;

            int nCamNo = (packet->partid[3] - '1');

            int cnt = packet->body.obc.count;
            float start_pos = packet->body.obc.start_pos;
            float end_pos = packet->body.obc.end_pos;

            if( cnt > m_nMaxOBC )
            {
                m_nMaxOBC = cnt;
            }
            if( m_nMaxOBC < 4 )
                m_nMaxOBC = 4;

            m_OBC.Start = -1.0f;

            m_OBC.Start = start_pos;
            m_OBC.End   = end_pos;

            OnOBC( packet->ticktime, packet->partid, nCamNo, &m_OBC, packet->body.obc.count );
        }
        break;
    case t3ktype_dtc:   // detection data for graph
        {
            if( packet->partid == NULL )
                return;

            int nCamNo = (packet->partid[3] - '1');

            int cnt = packet->body.dtc.count;
            unsigned long* start_pos = packet->body.dtc.start_pos;
            unsigned long* end_pos = packet->body.dtc.end_pos;

            if( m_pDTC )
            {
                if( cnt > m_nMaxDTC )
                {
                    delete[] m_pDTC;
                    m_pDTC = NULL;
                    m_nMaxDTC = cnt;
                    m_pDTC = new T3kRangeF[m_nMaxDTC];
                }
            }
            else
            {
                if( cnt > m_nMaxDTC )
                {
                    m_nMaxDTC = cnt;
                }
                if( m_nMaxDTC < 3 )
                    m_nMaxDTC = 3;
                m_pDTC = new T3kRangeF[m_nMaxDTC];
            }

            for( int i=0 ;i<m_nMaxDTC ; i++ )
            {
                m_pDTC[i].Start = -1;
            }

            for( int i=0 ; i<cnt && i<m_nMaxDTC ; i++ )
            {
                m_pDTC[i].Start		= start_pos[i];
                m_pDTC[i].End		= end_pos[i];
            }
            OnDTC( packet->ticktime, packet->partid, nCamNo, m_pDTC, packet->body.dtc.count );
        }
        break;
    case t3ktype_ird:   // ird for graph
        {
            int nCamNo = (packet->partid[3] - '1');

            short cnt = packet->body.ixd.count;
            short offset = packet->body.ixd.offset;
            short total = packet->body.ixd.total;
            uchar* data = packet->body.ixd.data;

            if( cnt <= 0 || total <= 0 || total > 2048 )
                return;

            if( m_ppIRD )
            {
                if( nCamNo+1 > m_nCamIRDMax )
                {
                    uchar** ppNewIRD = new uchar*[nCamNo+1];
                    ::memset( ppNewIRD, 0, sizeof(uchar*) * (nCamNo+1) );
                    int* pnNewIRD = new int[nCamNo+1];
                    ::memcpy( pnNewIRD, m_pnIRD, sizeof(int)*m_nCamIRDMax );
                    ::memcpy( ppNewIRD, m_ppIRD, sizeof(uchar*)*m_nCamIRDMax );
                    delete[] m_ppIRD;
                    delete[] m_pnIRD;

                    m_ppIRD = ppNewIRD;
                    m_pnIRD = pnNewIRD;
                    m_nCamIRDMax = nCamNo+1;
                }
            }
            else
            {
                m_ppIRD = new uchar*[nCamNo+1];
                m_pnIRD = new int[nCamNo+1];
                m_nCamIRDMax = nCamNo+1;
                ::memset( m_ppIRD, 0, sizeof(uchar*) * m_nCamIRDMax );
                ::memset( m_pnIRD, 0, sizeof(int) * m_nCamIRDMax );
            }

            if( m_ppIRD[nCamNo] )
            {
                if( m_pnIRD[nCamNo] != total )
                {
                    delete[] m_ppIRD[nCamNo];
                    m_ppIRD[nCamNo] = NULL;
                }
            }
            if( !m_ppIRD[nCamNo] )
            {
                m_ppIRD[nCamNo] = new uchar[total];
                ::memset( m_ppIRD[nCamNo], 0, total*sizeof(uchar) );
                m_pnIRD[nCamNo] = total;
            }

            if( (offset + cnt) > total )
                return;

            ::memcpy( m_ppIRD[nCamNo] + offset, data, cnt );

            Q_ASSERT( m_pnIRD[nCamNo] > 0 );

            if( (offset + cnt) == total )
                OnIRD( packet->ticktime, packet->partid, nCamNo, total, m_ppIRD[nCamNo] );
        }
        break;
    case t3ktype_itd:   // itd for graph
        {
            int nCamNo = (packet->partid[3] - '1');

            short cnt = packet->body.ixd.count;
            short offset = packet->body.ixd.offset;
            short total = packet->body.ixd.total;
            uchar* data = packet->body.ixd.data;

            if( cnt <= 0 || total <= 0 || total > 2048 )
                return;

            if( m_ppITD )
            {
                if( nCamNo+1 > m_nCamITDMax )
                {
                    uchar** ppNewITD = new uchar*[nCamNo+1];
                    ::memset( ppNewITD, 0, sizeof(uchar*) * (nCamNo+1) );
                    int* pnNewITD = new int[nCamNo+1];
                    uchar** pNewFillITD = new uchar*[nCamNo+1];
                    ::memset( pNewFillITD, 0, sizeof(uchar*) * (nCamNo+1) );

                    ::memcpy( pnNewITD, m_pnITD, sizeof(int)*m_nCamITDMax );
                    ::memcpy( ppNewITD, m_ppITD, sizeof(uchar*)*m_nCamITDMax );
                    ::memcpy( pNewFillITD, m_ppChkFillITD, sizeof(uchar*)*m_nCamITDMax );
                    delete[] m_ppITD;
                    delete[] m_pnITD;
                    delete[] m_ppChkFillITD;

                    m_ppITD = ppNewITD;
                    m_pnITD = pnNewITD;
                    m_ppChkFillITD = pNewFillITD;
                    m_nCamITDMax = nCamNo+1;
                }
            }
            else
            {
                m_ppITD = new uchar*[nCamNo+1];
                m_pnITD = new int[nCamNo+1];
                m_ppChkFillITD = new uchar*[nCamNo+1];
                m_nCamITDMax = nCamNo+1;
                ::memset( m_ppITD, 0, sizeof(uchar*) * m_nCamITDMax );
                ::memset( m_pnITD, 0, sizeof(int) * m_nCamITDMax );
                ::memset( m_ppChkFillITD, 0, sizeof(uchar*) * m_nCamITDMax );
            }

            if( m_ppITD[nCamNo] )
            {
                if( m_pnITD[nCamNo] != total )
                {
                    delete[] m_ppITD[nCamNo];
                    m_ppITD[nCamNo] = NULL;
                    delete[] m_ppChkFillITD[nCamNo];
                    m_ppChkFillITD[nCamNo] = NULL;
                }
            }
            if( !m_ppITD[nCamNo] )
            {
                m_ppITD[nCamNo] = new uchar[total];
                ::memset( m_ppITD[nCamNo], 0, total*sizeof(uchar) );
                m_ppChkFillITD[nCamNo] = new uchar[total];
                ::memset( m_ppChkFillITD[nCamNo], 0, total*sizeof(uchar) );
                m_pnITD[nCamNo] = total;
            }

            if( (offset + cnt) > total )
                return;

            ::memcpy( m_ppITD[nCamNo] + offset, data, cnt );

            Q_ASSERT( m_pnITD[nCamNo] > 0 );

            ::memset( m_ppChkFillITD[nCamNo] + offset, 1, cnt );

            for( int i=0; i<total; i++ )
            {
                if( m_ppChkFillITD[nCamNo][i] == 0 )
                    return;
            }

            ::memset( m_ppChkFillITD[nCamNo], 0, sizeof(uchar)*total );

            OnITD( packet->ticktime, packet->partid, nCamNo, total, m_ppITD[nCamNo] );
        }
        break;
#ifndef _T3KHANDLE_REMOVE_PRV
    case t3ktype_prv:   // preview data for sideview
        {
            //int nCamNo = (packet->partid[3] - '1');

            short cnt = packet->body.prv.width;
            short total = packet->body.prv.height;
            short offset = packet->body.prv.offset_y;
            uchar* data = packet->body.prv.image;

            if ( m_SideView.isNull() )
            {
                jmp_rebuild:
                m_SideView = QImage( cnt, total, QImage::Format_ARGB32 );
                Q_ASSERT( !m_SideView.isNull() );
            }
            else
            {
                if ( m_SideView.width() != cnt || m_SideView.height() != total )
                {
                    delete[] m_pFillCheck;
                    m_pFillCheck = NULL;
                    goto jmp_rebuild;
                }
            }

            //int y = total / 2 - offset;
            //if ( y < 0 || y >= total )
            //	return;

            if( !m_pFillCheck )
            {
                m_pFillCheck = new uchar[total];
                ::memset( m_pFillCheck, 0, sizeof(uchar) * total );
            }

            int nCenter = (total - 1) / 2;

            int nYOffset = -offset + nCenter;
            if( nYOffset < 0 || nYOffset >= total )
            {
                return;
            }

            m_pFillCheck[nYOffset] = 1;

            for ( int ni = 0; ni < cnt; ni++ )
            {
                m_SideView.setPixel( ni, nYOffset, qRgb( data[ni],data[ni],data[ni] ) );
            }

            for( int i = 0 ; i < total ; i++ )
            {
                if( m_pFillCheck[i] == 0 )
                    return;
            }

            ::memset( m_pFillCheck, 0, sizeof(uchar) * total );
            int nWidth = m_SideView.width();
            int nHeight = m_SideView.height();
            int nBitCount = m_SideView.bytesPerLine();
            uchar* pBuffer = m_SideView.bits();

            OnPRV( packet->ticktime, packet->partid, nWidth, nHeight, nBitCount, pBuffer );
        }
        break;
#endif
    case t3ktype_cmd:   // command
        OnCMD( packet->ticktime, packet->partid, packet->body.cmd.id, packet->body.cmd.command );
        break;
    case t3ktype_rsp:   // response
        OnRSP( packet->ticktime, packet->partid, packet->body.rsp.id, packet->body.rsp.is_final, packet->body.rsp.command );
        break;
    case t3ktype_rse:   // response error
        OnRSE( packet->ticktime, packet->partid, packet->body.rsp.id, packet->body.rsp.is_final, packet->body.rsp.command );
        break;
    case t3ktype_stt:
        OnSTT( packet->ticktime, packet->partid, packet->body.stt.status );
        break;
    case t3ktype_dvc:   // device data
        {
            T3kDVC DVC;
            DVC.flag = packet->body.dvc.flag;
            DVC.touch_obj_cnt = packet->body.dvc.touch_obj_cnt;
            DVC.mouse_buttons = packet->body.dvc.mouse_buttons;
            DVC.mouse_wheel = packet->body.dvc.mouse_wheel;
            DVC.mouse_x = packet->body.dvc.mouse_x;
            DVC.mouse_y = packet->body.dvc.mouse_y;
            DVC.touch_x = packet->body.dvc.touch_x;
            DVC.touch_y = packet->body.dvc.touch_y;
            DVC.keyboard_modifier = packet->body.dvc.keyboard_modifier;
            DVC.keyboard_reserved = packet->body.dvc.keyboard_reserved;
            DVC.keyboard_key = packet->body.dvc.keyboard_key;
            OnDVC( packet->ticktime, DVC );
        }
        break;
    case t3ktype_tpt:   // touch point data
        OnTPT( packet->ticktime, packet->body.tpt.actual_touch, packet->body.tpt.touch_count, packet->body.tpt.points );
        break;
    case t3ktype_gst:   // gesture data
        {
            T3kGST GST;
            GST.action_group = packet->body.gst.action_group;
            GST.action = packet->body.gst.action;
            GST.feasibleness = packet->body.gst.feasibleness;
            GST.x = packet->body.gst.x;
            GST.y = packet->body.gst.y;
            GST.w = packet->body.gst.w;
            GST.h = packet->body.gst.h;
            GST.zoom = packet->body.gst.zoom;
            GST.message = packet->body.gst.message;
            OnGST( packet->ticktime, GST );
        }
        break;
    case t3ktype_ver:   // version information
        {
            T3kVER VER;
            VER.nv = packet->body.ver.nv;
            VER.minor = packet->body.ver.minor;
            VER.major = packet->body.ver.major;
            VER.model = packet->body.ver.model;
            memcpy( VER.date, packet->body.ver.date, sizeof(unsigned char)*T3K_VER_DATE_LEN );
            memcpy( VER.time, packet->body.ver.time, sizeof(unsigned char)*T3K_VER_TIME_LEN );
            VER.date[T3K_VER_DATE_LEN] = 0;
            VER.time[T3K_VER_TIME_LEN] = 0;
            OnVER( packet->ticktime, packet->partid, VER );
        }
        break;
    default:
        break;
    }

    ::free( packet );
}

void ITPDPT3kNotify::onDownloadingFirmware(int bDownload)
{
    OnFirmwareDownload( bDownload == 1 ? true : false );
}
