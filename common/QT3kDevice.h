#ifndef QT3KDEVICE_H
#define QT3KDEVICE_H

#include <QObject>
#include <T3kHIDLib.h>
#include <QString>
#include <QQueue>
#include <QVector>
#include <QMutex>
#include "QSingletone.h"

#ifdef Q_OS_WIN
#include <HID.h>
#define REPORTID_FEATURE_CHK_CONN			10
#pragma pack(1)
    typedef struct _FeatureCheckConnection
    {
        unsigned char   ReportID;
        unsigned char   ConnectionOK;
    } FeatureCheckConnection;
#pragma pack()
#endif

#define PACKET_DEF_SIZE     (1024)
#define PACKET_POOL_SIZE    (2000)

class QT3kDeviceEventHandler;
class QT3kDevice : public QObject, public QSingleton<QT3kDevice>
{
    Q_OBJECT
protected:
    T3K_HANDLE              m_hDevice;
private:
    T3K_EVENT_NOTIFY        m_Notify;

#ifdef Q_OS_WIN
    bool                    m_bIsVirtualDevice;
    t3k_hid_library::CHID   m_T3kVirtualDeviceHandle;
    int                     m_TimerCheckVirtualDevice;
#endif

    int             m_nInstantMode;
    int             m_nExpireTime;
    unsigned long   m_dwFgstValue;

    QQueue<t3kpacket*>  m_ReceivePacketQueue;
    QMutex              m_RecvPktQueLock;

    QVector<t3kpacket*> m_PacketPool;
    QMutex              m_PktPoolLock;

    QT3kDeviceEventHandler*     m_pT3kEventHandler;

protected:
    T3K_EVENT_NOTIFY            m_pNotify;

public:
    bool open( unsigned short nVID, unsigned short nPID, unsigned short nMI, int nDevIndex );
    void close();
    bool isOpen();
    T3K_DEVICE_INFO getDeviceInfo();

    virtual void setEventHandler( QT3kDeviceEventHandler* pHandler );

    static int getDeviceCount( unsigned short nVID, unsigned short nPID, unsigned short nMI );
    static char* getDevicePath( unsigned short nVID, unsigned short nPID, unsigned short nMI, int nDevIndex );
    static char* getDevicePath( int nDevIndex );

    int sendCommand( const QString& cmd, bool bAsync=false, unsigned short nTimeout=1000 );
    bool writeBuffer( void* pBuffer, unsigned short nBytesToWrite, bool bAsync, unsigned short nTimeout );

    int setInstantMode( int nMode, int nExpireTime, unsigned long dwFgstValue=0x00 );
    int getInstantMode() { return m_nInstantMode; }

    void setFgstValue( unsigned long dwFgstValue ) { m_dwFgstValue = dwFgstValue; }
    unsigned long getFgstValue() { return m_dwFgstValue; }
    void setExpireTime( int nExpire ) { m_nExpireTime = nExpire; }
    int getExpireTime() { return m_nExpireTime; }

    int getLastError();

    bool enableTouch( bool bEnable, bool bAsync=false, unsigned short nTimeout=1000 );
    bool enableMouse( bool bEnable );

    bool setCamModeSideview( int nCamNo, bool bAsync=false, unsigned short nTimeout=1000 );
    bool setCamModeDetection( bool bAsync=false, unsigned short nTimeout=1000 );

    bool queryFirmwareVersion( unsigned short nAddrMask, int* pnMode );

    // + instant mode helper functions
    bool getReportMessage();
    bool getReportCommand();
    bool getReportView();
    bool getReportObject();
    bool getReportTouchPoint();
    bool getReportGesture();
    bool getReportDevice();

    void setReportMessage( bool bReport );
    void setReportCommand( bool bReport );
    void setReportView( bool bReport );
    void setReportObject( bool bReport );
    void setReportTouchPoint( bool bReport );
    void setReportGesture( bool bReport );
    void setReportDevice( bool bReport );
    // - instant mode helper functions

#ifdef Q_OS_WIN
    // for virtual device
    bool setFeature( void *pFeature, int nFeatureSize );
    bool getFeature( void *pFeature, int nFeatureSize );

    bool isVirtualDevice();
    virtual void timerEvent(QTimerEvent *evt);
#else
    bool setFeature( void */*pFeature*/, int /*nFeatureSize*/ ) { return false; }
    bool getFeature( void */*pFeature*/, int /*nFeatureSize*/ ) { return false; }
    bool isVirtualDevice() { return false; }
#endif

public:
    explicit QT3kDevice(QObject *parent = 0);
    ~QT3kDevice();

protected:
    static void T3K_CALLBACK _OnT3kHandleEventsHandler( T3K_HANDLE hDevice, void * pContext );
    static void T3K_CALLBACK _OnT3kDisconnectHandler( T3K_HANDLE hDevice, void * pContext );
    static int  T3K_CALLBACK _OnT3kReceiveRawDataHandler( T3K_HANDLE hDevice, unsigned char* pBuffer, unsigned short nBytes, void * pContext );
    static void T3K_CALLBACK _OnT3kDownloadingFirmwareHandler( T3K_HANDLE hDevice, int bDownload, void * pContext );
    static void T3K_CALLBACK _OnT3kPacketHandler( T3K_HANDLE hDevice, t3kpacket* packet, int bSync, void * pContext );

    t3kpacket* getPacket();
    void releasePacket( t3kpacket* packet );

signals:
    // internal use only!
    void disconnected();
    void downloadingFirmware( bool bIsDownload );
    void packetReceived();
    void packetReceivedSync();

    int receiveRawData( unsigned char* pBuffer, unsigned short nBytes );
    void receiveRawDataFlag( bool bReceive );
    
protected slots:
    virtual void onDisconnected();
    virtual void onDownloadingFirmware( bool bIsDownload );
    virtual int onReceiveRawData(unsigned char* /*pBuffer*/, unsigned short /*nBytes*/) { return 0;}
    virtual void onReceiveRawDataFlag(bool /*bReceive*/) {}
    void onPacketReceived();
};

#endif // QT3KDEVICE_H
