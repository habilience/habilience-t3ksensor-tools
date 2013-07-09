#include "QFWDPacket.h"
#include <stddef.h>

static int T3K_CALLBACK _onReceiveHIDData( T3K_HANDLE /*hDevice*/, unsigned char* pBuffer, unsigned short nBytes, void * pContext )
{
    QFWDPacket* pThis = (QFWDPacket*)pContext;
    pThis->processHIDData(pBuffer, nBytes);
    return 1;
}

static void T3K_CALLBACK _onDisconnect( T3K_HANDLE /*hDevice*/, void * pContext )
{
    QFWDPacket* pThis = (QFWDPacket*)pContext;
    emit pThis->disconnected();
}

QFWDPacket::QFWDPacket()
{
    m_hDevice = NULL;
    m_nReceiveBufferSize = 0;

    ::memset( &m_Notify, 0, sizeof(T3K_EVENT_NOTIFY) );
    m_Notify.Context = this;
    m_Notify.fnOnReceiveRawData = _onReceiveHIDData;
    m_Notify.fnOnDisconnect = _onDisconnect;
}

bool QFWDPacket::open()
{
    if (m_hDevice != NULL)
    {
        return false;
    }

    struct DEVICE_INFO {
        unsigned short VID;
        unsigned short PID;
        unsigned short MI;
    };

    DEVICE_INFO T3kDeviceInfos[] = {
        {0xffff, 0x3000, 1},        // old T3000 app
        {0x2200, 0x3000, 1},        // new T3000 app
        {0x2200, 0x3100, 1},        // T3100 app
        {0x2200, 0x3200, 1},        // T3200 app
        {0x2200, 0x3500, 1},        // T3500 app
        {0x2200, 0x3900, 1},        // T3500 app
        {0x2200, 0x0001, 0},        // T3100 iap
        {0x2200, 0x0002, 0},        // T3100 iap upg
        {0x2200, 0x0003, 0},        // T3000 iap
        {0x2200, 0x0004, 0},        // T3000 iap upg
        {0x2200, 0x3201, 0},        // T3200 iap
        {0x2200, 0x3202, 0},        // T3200 iap upg
        {0x2200, 0x3501, 0},        // T3500 iap
        {0x2200, 0x3502, 0},        // T3500 iap upg
        {0x2200, 0x3901, 0},        // T3900 iap
        {0x2200, 0x3902, 0}         // T3900 iap upg
    };

    for ( unsigned int i=0 ; i<sizeof(T3kDeviceInfos)/sizeof(DEVICE_INFO) ; i++ )
    {
        m_hDevice = ::T3kOpenDevice(T3kDeviceInfos[i].VID, T3kDeviceInfos[i].PID, T3kDeviceInfos[i].MI, 0);
        if (m_hDevice)
        {
            ::T3kSetEventNotify(m_hDevice, &m_Notify);
            ::T3kLockFirmwareDownload(m_hDevice);
            break;
        }
    }

    return m_hDevice != NULL ? true : false;
}

void QFWDPacket::close()
{
    if (m_hDevice == NULL)
        return;
    ::T3kUnlockFirmwareDownload(m_hDevice);
    ::T3kCloseDevice(m_hDevice);
    m_hDevice = NULL;
}

bool QFWDPacket::isOpen()
{
    return m_hDevice != NULL ? true : false;
}

bool QFWDPacket::sendHIDPacket( hid_pkt_type* pkt )
{
    // must be async
    return T3kSendBuffer(m_hDevice, (const unsigned char*)pkt, HID_PKT_SIZE, 1, 0) != 0 ? true : false;
}

int QFWDPacket::queryMode(unsigned short which)
{
    pkt_type* pkt_ptr;
    switch (which)
    {
    case PKT_ADDR_CM1:
    case PKT_ADDR_CM1|PKT_ADDR_CM_SUB:
    case PKT_ADDR_CM2:
    case PKT_ADDR_CM2|PKT_ADDR_CM_SUB:
    case PKT_ADDR_MM:
        break;
    default:
        return -1;
    }

    hid_pkt_type hid_pkt;

    hid_pkt.id = 0x01;
    hid_pkt.type = HID_PKT_END;

    pkt_ptr = (pkt_type*)hid_pkt.data;

    pkt_ptr->hdr.pkt_id = IAP_PKT_MODE | which;
    pkt_ptr->body.mode.mode = MODE_UNKNOWN;
    unsigned short pkt_length = sizeof(pkt_hdr_type) + sizeof(iap_pkt_mode_type);
    pkt_length = T3kCalculateCRC( (unsigned char*)pkt_ptr, pkt_length );
    hid_pkt.size = (unsigned char) pkt_length;

    if (!sendHIDPacket( &hid_pkt ))
        return -1;

    return pkt_ptr->hdr.pkt_id;
}

int QFWDPacket::queryVersion(unsigned short which)
{
    pkt_type* pkt_ptr;
    switch (which)
    {
    case PKT_ADDR_CM1:
    case PKT_ADDR_CM1|PKT_ADDR_CM_SUB:
    case PKT_ADDR_CM2:
    case PKT_ADDR_CM2|PKT_ADDR_CM_SUB:
    case PKT_ADDR_MM:
        break;
    default:
        return -1;
    }

    hid_pkt_type hid_pkt;

    hid_pkt.id = 0x01;
    hid_pkt.type = HID_PKT_END;

    pkt_ptr = (pkt_type*)hid_pkt.data;

    pkt_ptr->hdr.pkt_id = PKT_VER | which;
    unsigned short pkt_length = sizeof(pkt_hdr_type) + sizeof(mm_pkt_ver_type);
    pkt_length = T3kCalculateCRC( (unsigned char*)pkt_ptr, pkt_length );
    hid_pkt.size = (unsigned char) pkt_length;

    if (!sendHIDPacket( &hid_pkt ))
        return -1;

    return pkt_ptr->hdr.pkt_id;
}

int QFWDPacket::queryIapVersion(unsigned short which)
{
    pkt_type* pkt_ptr;
    switch (which)
    {
    case PKT_ADDR_CM1:
    case PKT_ADDR_CM1|PKT_ADDR_CM_SUB:
    case PKT_ADDR_CM2:
    case PKT_ADDR_CM2|PKT_ADDR_CM_SUB:
    case PKT_ADDR_MM:
        break;
    default:
        return -1;
    }

    hid_pkt_type hid_pkt;

    hid_pkt.id = 0x01;
    hid_pkt.type = HID_PKT_END;

    pkt_ptr = (pkt_type*)hid_pkt.data;

    pkt_ptr->hdr.pkt_id = IAP_PKT_VER | which;
    unsigned short pkt_length = sizeof(pkt_hdr_type) + sizeof(iap_pkt_ver_type);
    pkt_length = T3kCalculateCRC( (unsigned char*)pkt_ptr, pkt_length );
    hid_pkt.size = (unsigned char) pkt_length;

    if (!sendHIDPacket( &hid_pkt ))
        return -1;

    return pkt_ptr->hdr.pkt_id;
}

int QFWDPacket::queryIapRevision(unsigned short which)
{
    pkt_type* pkt_ptr;
    switch (which)
    {
    case PKT_ADDR_CM1:
    case PKT_ADDR_CM1|PKT_ADDR_CM_SUB:
    case PKT_ADDR_CM2:
    case PKT_ADDR_CM2|PKT_ADDR_CM_SUB:
    case PKT_ADDR_MM:
        break;
    default:
        return -1;
    }

    hid_pkt_type hid_pkt;

    hid_pkt.id = 0x01;
    hid_pkt.type = HID_PKT_END;

    pkt_ptr = (pkt_type*)hid_pkt.data;

    pkt_ptr->hdr.pkt_id = IAP_PKT_REV | which;
    unsigned short pkt_length = sizeof(pkt_hdr_type) + sizeof(iap_pkt_rev_type);
    pkt_length = T3kCalculateCRC( (unsigned char*)pkt_ptr, pkt_length );
    hid_pkt.size = (unsigned char) pkt_length;

    if (!sendHIDPacket( &hid_pkt ))
        return -1;

    return pkt_ptr->hdr.pkt_id;
}

int QFWDPacket::markIap(unsigned short which)
{
    pkt_type* pkt_ptr;
    switch (which)
    {
    case PKT_ADDR_CM1:
    case PKT_ADDR_CM1|PKT_ADDR_CM_SUB:
    case PKT_ADDR_CM2:
    case PKT_ADDR_CM2|PKT_ADDR_CM_SUB:
    case PKT_ADDR_MM:
        break;
    default:
        return -1;
    }

    hid_pkt_type hid_pkt;

    hid_pkt.id = 0x01;
    hid_pkt.type = HID_PKT_END;

    pkt_ptr = (pkt_type*)hid_pkt.data;

    pkt_ptr->hdr.pkt_id = IAP_PKT_MARK_IAP | which;
    unsigned short pkt_length = sizeof(pkt_hdr_type) + sizeof(iap_pkt_mark_iap_type);
    pkt_length = T3kCalculateCRC( (unsigned char*)pkt_ptr, pkt_length );
    hid_pkt.size = (unsigned char) pkt_length;

    if (!sendHIDPacket( &hid_pkt ))
        return -1;

    return pkt_ptr->hdr.pkt_id;
}

int QFWDPacket::markApp(unsigned short which)
{
    pkt_type* pkt_ptr;
    switch (which)
    {
    case PKT_ADDR_CM1:
    case PKT_ADDR_CM1|PKT_ADDR_CM_SUB:
    case PKT_ADDR_CM2:
    case PKT_ADDR_CM2|PKT_ADDR_CM_SUB:
    case PKT_ADDR_MM:
        break;
    default:
        return -1;
    }

    hid_pkt_type hid_pkt;

    hid_pkt.id = 0x01;
    hid_pkt.type = HID_PKT_END;

    pkt_ptr = (pkt_type*)hid_pkt.data;

    pkt_ptr->hdr.pkt_id = IAP_PKT_MARK_APP | which;
    unsigned short pkt_length = sizeof(pkt_hdr_type) + sizeof(iap_pkt_mark_app_type);
    pkt_length = T3kCalculateCRC( (unsigned char*)pkt_ptr, pkt_length );
    hid_pkt.size = (unsigned char) pkt_length;

    if (!sendHIDPacket( &hid_pkt ))
        return -1;

    return pkt_ptr->hdr.pkt_id;
}

int QFWDPacket::reset(unsigned short which)
{
    pkt_type* pkt_ptr;
    switch (which)
    {
    case PKT_ADDR_CM1:
    case PKT_ADDR_CM1|PKT_ADDR_CM_SUB:
    case PKT_ADDR_CM2:
    case PKT_ADDR_CM2|PKT_ADDR_CM_SUB:
    case PKT_ADDR_MM:
        break;
    default:
        return -1;
    }

    hid_pkt_type hid_pkt;

    hid_pkt.id = 0x01;
    hid_pkt.type = HID_PKT_END;

    pkt_ptr = (pkt_type*)hid_pkt.data;

    pkt_ptr->hdr.pkt_id = IAP_PKT_RESET | which;
    unsigned short pkt_length = sizeof(pkt_hdr_type);
    pkt_length = T3kCalculateCRC( (unsigned char*)pkt_ptr, pkt_length );
    hid_pkt.size = (unsigned char) pkt_length;

    if (!sendHIDPacket( &hid_pkt ))
        return -1;

    return pkt_ptr->hdr.pkt_id;
}

int QFWDPacket::erase(unsigned short which)
{
    pkt_type* pkt_ptr;
    switch (which)
    {
    case PKT_ADDR_CM1:
    case PKT_ADDR_CM1|PKT_ADDR_CM_SUB:
    case PKT_ADDR_CM2:
    case PKT_ADDR_CM2|PKT_ADDR_CM_SUB:
    case PKT_ADDR_MM:
        break;
    default:
        return -1;
    }

    hid_pkt_type hid_pkt;

    hid_pkt.id = 0x01;
    hid_pkt.type = HID_PKT_END;

    pkt_ptr = (pkt_type*)hid_pkt.data;

    pkt_ptr->hdr.pkt_id = IAP_PKT_ERASE | which;
    unsigned short pkt_length = sizeof(pkt_hdr_type) + sizeof(iap_pkt_erase_type);
    pkt_length = T3kCalculateCRC( (unsigned char*)pkt_ptr, pkt_length );
    hid_pkt.size = (unsigned char) pkt_length;

    if (!sendHIDPacket( &hid_pkt ))
        return -1;

    return pkt_ptr->hdr.pkt_id;
}

int QFWDPacket::write(unsigned short which, unsigned long address, unsigned short length, unsigned char* data)
{
    switch( which )
    {
    case PKT_ADDR_MM:
    case PKT_ADDR_CM1:
    case PKT_ADDR_CM2:
    case PKT_ADDR_CM1 | PKT_ADDR_CM_SUB:
    case PKT_ADDR_CM2 | PKT_ADDR_CM_SUB:
        break;
    default:
        return -1;
    }

    pkt_type write_pkt;
    write_pkt.hdr.pkt_id = IAP_PKT_WRITE | which;
    write_pkt.body.write.addr = address;
    write_pkt.body.write.len = length;
    memcpy( write_pkt.body.write.data, data, length );
    unsigned short pkt_length = T3kCalculateCRC( (unsigned char*)&write_pkt, sizeof(pkt_hdr_type)+sizeof(iap_pkt_write_type) );

    unsigned char* data_ptr = (unsigned char*) &write_pkt;

    hid_pkt_type hid_pkt;

    hid_pkt.id = 0x01;
    hid_pkt.type = HID_PKT_CONT;
    hid_pkt.size = HID_DATA_SIZE;

    int hid_pkt_cnt = (pkt_length - 1) / HID_DATA_SIZE + 1;

    for ( int i=0 ; i<hid_pkt_cnt-1 ; i++ )
    {
        memcpy( hid_pkt.data, data_ptr, HID_DATA_SIZE );
        if ( !sendHIDPacket( &hid_pkt ) )
            return -1;

        data_ptr += HID_DATA_SIZE;
        pkt_length -= HID_DATA_SIZE;
    }

    hid_pkt.type = HID_PKT_END;
    hid_pkt.size = (unsigned char)pkt_length;
    memcpy( hid_pkt.data, data_ptr, pkt_length );
    if ( !sendHIDPacket( &hid_pkt ) )
        return -1;

    return write_pkt.hdr.pkt_id;
}

void QFWDPacket::processHIDData(unsigned char* pBuffer, unsigned short nBytes)
{
#pragma pack(push, 1)
    typedef struct
    {
        unsigned short     pkt_id;
        unsigned short     ticktime;
    } pkt_type_dummy;
#pragma pack(pop)

    if ( nBytes != HID_PKT_SIZE )
    {
        qDebug( "abnormal packet size: recv %d bytes", nBytes );
        return;
    }

    hid_pkt_type* hid_pkt = (hid_pkt_type*)pBuffer;
    pkt_hdr_type* pkt_hdr = NULL;

    unsigned short nCRC = 0;
    static int nCRCError = 0;

    switch (hid_pkt->type)
    {
    case HID_PKT_CONT:
        if (m_nReceiveBufferSize+hid_pkt->size >= 512)
        {
            qDebug( "receive buffer overflow: %d", m_nReceiveBufferSize+hid_pkt->size);
            m_nReceiveBufferSize = 0;
            break;
        }
        memcpy( m_pReceiveBuffer+m_nReceiveBufferSize, hid_pkt->data, hid_pkt->size );
        m_nReceiveBufferSize += hid_pkt->size;
        break;
    case HID_PKT_END:
        if (m_nReceiveBufferSize+hid_pkt->size >= 512)
        {
            qDebug( "receive buffer overflow: %d", m_nReceiveBufferSize+hid_pkt->size);
            m_nReceiveBufferSize = 0;
            break;
        }
        memcpy( m_pReceiveBuffer+m_nReceiveBufferSize, hid_pkt->data, hid_pkt->size );

        m_nReceiveBufferSize += hid_pkt->size;

        nCRC = T3kCheckCRC( m_pReceiveBuffer, m_nReceiveBufferSize );

        if ( nCRC != CRC_END )
        {
            qDebug( "CRC Error - %d", ++nCRCError );
            m_nReceiveBufferSize = 0;
            break;
        }

        pkt_hdr = (pkt_hdr_type*)m_pReceiveBuffer;

        if ((pkt_hdr->pkt_id & PKT_TYPEMASK) == PKT_BAD_ID)
        {
            qDebug( "bad pkt id: %x", pkt_hdr->pkt_id);
            m_nReceiveBufferSize = 0;
            break;
        }

        pkt_type* rx_pkt = (pkt_type*)m_pReceiveBuffer;

        //qDebug( "-> pkt_id: %x, length: %d", rx_pkt->hdr.pkt_id, m_nReceiveBufferSize );

        switch (rx_pkt->hdr.pkt_id & PKT_TYPEMASK)
        {
        case IAP_PKT_MODE:
            m_SensorResponse.nFirwareMode = rx_pkt->body.mode.mode;
            break;
        case IAP_PKT_VER:
            m_SensorResponse.nModelNumber = rx_pkt->body.iap_ver.model;
            break;
        case IAP_PKT_REV:
            m_SensorResponse.nRevision = rx_pkt->body.rev.rev;
            break;
        case PKT_VER:
            m_SensorResponse.nNV = rx_pkt->body.ver.nv;
            m_SensorResponse.nMinor = rx_pkt->body.ver.min;
            m_SensorResponse.nMajor = rx_pkt->body.ver.maj;
            m_SensorResponse.nModelNumber = rx_pkt->body.ver.model;
            memcpy( m_SensorResponse.szDate, rx_pkt->body.ver.date, VER_DATE_LEN );
            m_SensorResponse.szDate[VER_DATE_LEN] = 0;
            memcpy( m_SensorResponse.szTime, rx_pkt->body.ver.time, VER_TIME_LEN );
            m_SensorResponse.szTime[VER_TIME_LEN] = 0;
            break;
        }

        emit responseFromSensor(rx_pkt->hdr.pkt_id);

        m_nReceiveBufferSize = 0;
        break;
    }

}
