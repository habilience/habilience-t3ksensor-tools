#ifndef QFWDPACKET_H
#define QFWDPACKET_H

#include <T3kHIDLib.h>
#include <QObject>

#pragma pack(push, 1)

#define HID_PKT_CONT    (0x00)
#define HID_PKT_END     (0x01)
#define HID_DATA_SIZE   (61)

#define HID_PKT_SIZE    (64)

enum
{
    PKT_TYPEMASK        = 0x0FFF,

    PKT_VER             = 0,

    // dest address
    PKT_ADDR_IAP        = 0x0000,

    // part of packet
    PKT_PART_IAP  = 0x0100,
    PKT_PART_CM   = 0x0200,
    PKT_PART_MM   = 0x0300,
    PKT_BAD_ID    = 0x07FF,

    // iap pkt
    IAP_PKT_ERASE           = 0x0100,
    IAP_PKT_WRITE           = 0x0101,
    IAP_PKT_MARK_APP        = 0x0102,
    IAP_PKT_RESET           = 0x0103,
    IAP_PKT_MODE            = 0x0104,
    IAP_PKT_VER             = 0x0105,
    //IAP_PKT_NOP             = 0x0106,
    IAP_PKT_MARK_IAP        = 0x0107,
    IAP_PKT_CM_OFF          = 0x0108,
    //IAP_PKT_MODEL           = 0x0109,
    //IAP_PKT_IAP_ERASE       = 0x010A,
    //IAP_PKT_IAP_WRITE       = 0x010B,
    //IAP_PKT_CAPABILITY      = 0x010C,
    IAP_PKT_REV             = 0x0110, // hw revision
    //IAP_PKT_PAGE_ERASE      = 0x0111, // page erase
    //IAP_UPG_PKT_ERASE       = 0x0112,
    //IAP_UPG_PKT_WRITE       = 0x0113,
    //IAP_UPG_PKT_COMMIT      = 0x0114,
    //IAP_PKT_USART_TX_PIN    = 0x0115,

    MM_PKT_VER = PKT_ADDR_MM | PKT_VER

    //MM_PKT_NV_READ     = PKT_ADDR_MM | 0x0035,
    //MM_PKT_NV_WRITE    = PKT_ADDR_MM | 0x0036
};

typedef struct
{
    unsigned char id;
    unsigned char type;
    unsigned char size;
    unsigned char data[HID_DATA_SIZE];
} hid_pkt_type;

#define TX_BUF_LEN 256

typedef struct
{
    unsigned short pkt_id;
} pkt_hdr_type;


#define VER_DATE_LEN 11
#define VER_TIME_LEN 8
typedef struct
{
    unsigned short ts;
    unsigned short nv;
    unsigned short min;
    unsigned short maj;
    unsigned short model;
    char date[VER_DATE_LEN];
    char time[VER_TIME_LEN];
} mm_pkt_ver_type;

typedef struct
{
    unsigned short mode;
} iap_pkt_mode_type;

typedef struct
{
} iap_pkt_mark_app_type;

typedef struct
{
} iap_pkt_mark_iap_type;

typedef struct
{
    unsigned short ack;
} iap_pkt_ack_type;

typedef struct
{
} iap_pkt_nop_type;

typedef struct
{
    unsigned short model;
} iap_pkt_ver_type;

typedef struct
{
} iap_pkt_erase_type;

typedef struct
{
    unsigned long   addr;
    unsigned short  len;
    unsigned char   data[256];
} iap_pkt_write_type;

typedef struct
{
    unsigned short off;
    unsigned short which_cm;
} iap_pkt_cm_off_type;


typedef struct
{
    unsigned short model;
} iap_pkt_model_type;

typedef struct
{
    unsigned short which_cm;
    unsigned short pin_low;
} iap_pkt_usart_tx_pin_low_type;

typedef struct
{
    unsigned short bad_pkt_id;
} iap_pkt_bad_type;

typedef struct
{
} iap_pkt_iap_erase_type;

typedef struct
{
    unsigned long   addr;
    unsigned short  len;
    unsigned char   data[512];
} iap_pkt_iap_write_type;

typedef struct
{
    unsigned short capability;
} iap_pkt_capability_type;

typedef struct
{
    unsigned short rev;          // hw revsion
} iap_pkt_rev_type;

typedef struct
{
    unsigned short number;         //page_number;
} iap_pkt_page_erase_type;

typedef union {
    mm_pkt_ver_type                 ver;            // response
    iap_pkt_mode_type               mode;           // response
    iap_pkt_mark_app_type           mark_app;
    iap_pkt_mark_iap_type           mark_iap;
    iap_pkt_ack_type                ack;
    iap_pkt_nop_type                nop;
    iap_pkt_ver_type                iap_ver;        // response
    iap_pkt_cm_off_type             cm_off;
    //iap_pkt_model_type              model;          // response
    iap_pkt_write_type              write;
    iap_pkt_iap_write_type          iap_write;
    //iap_pkt_capability_type         iap_capability; // response
    iap_pkt_rev_type                rev;            // response
    //iap_pkt_page_erase_type         page_erase;
    //iap_pkt_usart_tx_pin_low_type   usart_tx_pin;
    iap_pkt_bad_type                bad;
} pkt_body_type;

typedef struct
{
    pkt_hdr_type    hdr;
    pkt_body_type   body;
} pkt_type;


#define CRC_END            0xF0B8

#pragma pack(pop)

class QFWDPacket : public QObject
{
    Q_OBJECT

private:
    T3K_HANDLE m_hDevice;
    T3K_EVENT_NOTIFY m_Notify;

    unsigned char m_pReceiveBuffer[512];
    unsigned int m_nReceiveBufferSize;

    typedef struct tagSensorResponse
    {
        unsigned short nFirwareMode;
        unsigned short nModelNumber;
        unsigned short nRevision;
        unsigned short nNV;
        unsigned short nMinor;
        unsigned short nMajor;
        char szDate[VER_DATE_LEN];
        char szTime[VER_TIME_LEN];
    } SensorResponse;

    SensorResponse m_SensorResponse;

protected:
    bool sendHIDPacket(hid_pkt_type* pkt);
public:
    QFWDPacket();

    bool open();
    void close();
    bool isOpen();

    int queryMode(unsigned short which);
    int queryVersion(unsigned short which);
    int queryIapVersion(unsigned short which);
    int queryIapRevision(unsigned short which);

    int markIap(unsigned short which);
    int markApp(unsigned short which);
    int reset(unsigned short which);

    int erase(unsigned short which);
    int write(unsigned short which, unsigned long address, unsigned short length, unsigned char* data);

    unsigned short getFirmwareMode() const { return m_SensorResponse.nFirwareMode; }
    unsigned short getModelNumber() const { return m_SensorResponse.nModelNumber; }
    unsigned short getRevision() const { return m_SensorResponse.nRevision; }
    unsigned short getVersionNV() const { return m_SensorResponse.nNV; }
    unsigned short getVersionMinor() const { return m_SensorResponse.nMinor; }
    unsigned short getVersionMajor() const { return m_SensorResponse.nMajor; }
    const char* getDate() const { return m_SensorResponse.szDate; }
    const char* getTime() const { return m_SensorResponse.szTime; }

    void processHIDData(unsigned char* pBuffer, unsigned short nBytes);

signals:
    void disconnected();
    void responseFromSensor( unsigned short pkt );

};

#endif // QFWDPACKET_H
