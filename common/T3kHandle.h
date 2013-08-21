#ifndef T3KHANDLE_H
#define T3KHANDLE_H

#include <T3kHIDLib.h>
#include <HID.h>

#include <QObject>
#ifndef _T3KHANDLE_REMOVE_PRV
#include <QImage>
#endif
#include <QQueue>
#include <QMutex>
#include <QTimer>


typedef struct _T3kGST
{
    unsigned char action_group;
    unsigned char action;
    unsigned short feasibleness;
    unsigned short x;
    unsigned short y;
    unsigned short w;
    unsigned short h;
    float zoom;
    char* message;
} T3kGST;

typedef struct _T3kDVC
{
    const char* sPartId;
    unsigned char flag;
    unsigned char touch_obj_cnt;
    unsigned char mouse_buttons;
    char mouse_wheel;
    unsigned short mouse_x;
    unsigned short mouse_y;
    float touch_x;
    float touch_y;
    unsigned char keyboard_modifier;
    unsigned char keyboard_reserved;
    unsigned char* keyboard_key;
} T3kDVC;

typedef struct _T3kVER
{
    unsigned short nv;
    unsigned short minor;
    unsigned short major;
    unsigned short model;
    unsigned char date[T3K_VER_DATE_LEN + 1];
    unsigned char time[T3K_VER_TIME_LEN + 1];
} T3kVER;

typedef struct _T3kRangeI
{
    int Start;
    int End;
} T3kRangeI;

typedef struct _T3kRangeF
{
    float Start;
    float End;
} T3kRangeF;

#define REPORTID_FEATURE_CHK_CONN			10
#pragma pack(push, 1)
    typedef struct _FeatureCheckConnection
    {
        uchar			ReportID;
        uchar			ConnectionOK;
    } FeatureCheckConnection;
#pragma pack(pop)

enum ResponsePart { MM = 0, CM1, CM2, CM1_1, CM2_1 };

ResponsePart GetResponsePartFromPartId( const char* sPartId, const char* cmd );

class ITPDPT3kNotify;
class T3kHandle : public QObject
{
    Q_OBJECT

public:
    T3kHandle();
    virtual ~T3kHandle();

    enum TouchSensorModel { TSM_NONE, TSM_OLDT3000, TSM_T3000, TSM_T3100, TSM_T3200, TSM_T3500, TSM_T3900, TSM_SPEC };

    // custum
    void SetBothSensorData( bool bBoth ) { m_bGetBothSensorData = bBoth ;}

    // after open func
    bool SetNotify( ITPDPT3kNotify* pNotify );

    bool IsOpen();
    //bool Open();

    bool OpenWithVIDPID( unsigned short nVID, unsigned short nPID, unsigned short nMI = 1, int nDevIndex = 0 );
    bool Close( bool bNotify=true );
    unsigned long GetLastError();

    static int GetDeviceCount( unsigned short nVID, unsigned short nPID, unsigned short nMI = 1 );
    static char* GetDevPath( unsigned short nVID, unsigned short nPID, unsigned short nMI = 1, int nDevIndex = 0 );

    static ushort GetDeviceVID( T3K_DEVICE_INFO devInfo );
    static ushort GetDevicePID( T3K_DEVICE_INFO devInfo );

    int SendBuffer( const unsigned char* pBuffer, unsigned short nBufferSize, int bAsync, unsigned short nTimeout );
    int SendCommand( const char* lpszCmd, bool bASync=false, unsigned short lTimeout=1000 );
    int EnableTouch( bool bEnable, bool bASync=false, unsigned short nTimeout=1000 );
    int EnableMouse( bool bEnable, bool bASync=false, unsigned short nTimeout=1000 );

    int SetInstantMode(int nMode, unsigned short nExpireTime, unsigned long dwFgstValue);

    bool QueryFirmwareVersion( unsigned short wAddrMask, int* pnMode );


    int GetInstantMode() { return m_nInstantMode; }
    int GetExpireTime() { return m_nExpireTime; }
    ulong GetFGstValue() { return m_dwFgstValue; }

    void SetExpireTime( int nTime );

    bool GetReportMessage();
    bool GetReportCommand();
    bool GetReportView();
    bool GetReportObject();
    bool GetReportTouchPoint();
    bool GetReportGesture();
    bool GetReportDevice();

    void SetReportMessage( bool bReport );
    void SetReportCommand( bool bReport );
    void SetReportView( bool bReport );
    void SetReportObject( bool bReport );
    void SetReportTouchPoint( bool bReport );
    void SetReportGesture( bool bReport );
    void SetReportDevice( bool bReport );


    // HID Callback
protected:
    static void T3K_CALLBACK OnPacket( T3K_HANDLE hDevice, t3kpacket* packet, int nSync, void * pContext );
    static int T3K_CALLBACK OnReceiveRawData( T3K_HANDLE hDevice, unsigned char* pBuffer, unsigned short nBytes, void * pContext );
    static void T3K_CALLBACK OnDisconnect( T3K_HANDLE hDevice, void * pContext );
    static void T3K_CALLBACK OnDownloadingFirmware( T3K_HANDLE hDevice, int bDownload, void * pContext );
    static void T3K_CALLBACK OnHandleEvents( T3K_HANDLE hDevice, void * pContext );

public:
    void ExOnDisconnect(T3K_HANDLE hDevice);
    void ExOnPacket(t3kpacket* packet, int nSync);
    int ExOnReceiveRawData(unsigned char* pBuffer, unsigned short nBytes);
    void ExOnDownloadingFirmware(int bDownload);

    t3kpacket* GetReceivePacket();

    char* GetRawDataPacket( int& nRetBytes );

protected:
    void ConnectSignal();
    void DisconnectSignal();

protected:
    bool                                    m_bOpen;
    ITPDPT3kNotify*                         m_pNotify;

    T3K_EVENT_NOTIFY                        m_T3kNotify;
    T3K_HANDLE                              m_pT3kDevice;

    t3k_hid_library::CHID                   m_T3kVirtualDevice;
    bool                                    m_bIsVirtualDevice;
    QTimer                                  m_TimerCheckT3kVD;

    bool                                    m_bCloseNotify;

    // InstantMode
    int                                     m_nInstantMode;
    ulong                                   m_dwFgstValue;
    int                                     m_nExpireTime;

    QQueue<t3kpacket*>                      m_qRecvPacket;
    QMutex                                  m_Mutex;

    // Only RawData
    char*                                   m_pRawDataBuffer;
    int                                     m_nTotalRawBytes;
    QMutex                                  m_RawDataMutex;
    QQueue<unsigned short>                  m_qBytes;
    QQueue<char*>                           m_qBuffers;

    bool                                    m_bGetBothSensorData;
    QMutex                                  m_RemoteStatusMutex;
    bool                                    m_bRemoting;

signals:
    void Connect(T3K_HANDLE hDevice);
    void Disconnect(T3K_HANDLE hDevice);
    void Packet(void* pContext);
    // deadlock caution
    void PacketSync(void* pContext);
    //
    int ReceiveRawData(void* pContext);
    void DownloadingFirmware(int bDownload);

    void ReceiveRawDataFlag(bool bReceive);

public slots:
    void onReceiveRawDataFlag(bool bReceive);
    void onTimeout();
};

class ITPDPT3kNotify : public QObject
{
    Q_OBJECT

public:
    ITPDPT3kNotify(QObject* pParent = NULL);
    virtual ~ITPDPT3kNotify();

protected:
    virtual void OnOpenT3kDevice(T3K_HANDLE /*hDevice*/) {}
    virtual void OnCloseT3kDevice(T3K_HANDLE /*hDevice*/) {}
    virtual void OnFirmwareDownload( bool /*bDownload*/ ) {}

    virtual void OnMSG( ushort /*nTickTime*/, const char* /*sPartId*/, const char* /*sTxt*/ ) {}
    virtual void OnOBJ( ushort /*nTickTime*/, const char* /*sPartId*/, int /*nCamNo*/, T3kRangeF* /*pOBJ*/, unsigned short /*count*/ ) {}
    virtual void OnOBC( ushort /*nTickTime*/, const char* /*sPartId*/, int /*nCamNo*/, T3kRangeF* /*pOBC*/, unsigned short /*count*/ ) {}
    virtual void OnDTC( ushort /*nTickTime*/, const char* /*sPartId*/, int /*nCamNo*/, T3kRangeF* /*pDTC*/, unsigned short /*count*/ ) {}
    virtual void OnIRD( ushort /*nTickTime*/, const char* /*sPartId*/, int /*nCamNo*/, int /*nCount*/, uchar* /*pIRD*/ ) {}
    virtual void OnITD( ushort /*nTickTime*/, const char* /*sPartId*/, int /*nCamNo*/, int /*nCount*/, uchar* /*pITD*/ ) {}
#ifndef _T3KHANDLE_REMOVE_PRV
    virtual void OnPRV( ushort /*nTickTime*/, const char* /*sPartId*/, int /*nWidth*/, int /*nHeight*/, int /*nBitCount*/, unsigned char* /*pBitmapBuffer*/ ) {}
#endif
    virtual void OnCMD( ushort /*nTickTime*/, const char* /*sPartId*/, long /*lId*/, const char* /*sCmd*/ ) {}
    virtual void OnRSP( ushort /*nTickTime*/, const char* /*sPartId*/, long /*lId*/, bool /*bFinal*/, const char* /*sCmd*/ ) {}
    virtual void OnRSE( ushort /*nTickTime*/, const char* /*sPartId*/, long /*lId*/, bool /*bFinal*/, const char* /*sCmd*/ ) {}
    virtual void OnSTT( ushort /*nTickTime*/, const char* /*sPartId*/, const char* /*pStatus*/ ) {}
    virtual void OnDVC( ushort /*nTickTime*/, const char* /*sPartId*/, T3kDVC& /*DVC*/ ) {}
    virtual void OnTPT( ushort /*nTickTime*/, short /*nActualTouch*/, short /*nTouchCount*/, t3ktouchpoint* /*points*/ ) {}
    virtual void OnGST( ushort /*nTickTime*/, T3kGST& /*GST*/ ) {}
    virtual void OnVER( ushort /*nTickTime*/, const char* /*sPartId*/, T3kVER& /*Ver*/ ) {}

protected:
    // OBJ
    T3kRangeF*          m_pOBJ;
    int                 m_nMaxOBJ;
    // OBC
    T3kRangeF           m_OBC;
    int                 m_nMaxOBC;
    // DTC
    T3kRangeF*          m_pDTC;
    int                 m_nMaxDTC;
    // IRD
    int                 m_nCamIRDMax;
    uchar**             m_ppIRD;
    int*                m_pnIRD;
    // ITD
    int                 m_nCamITDMax;
    uchar**             m_ppITD;
    int*                m_pnITD;
    uchar**             m_ppChkFillITD;
#ifndef _T3KHANDLE_REMOVE_PRV
    // PRV
    QImage              m_SideView;
    int                 m_nCamPRVMax;
    uchar*              m_pFillCheck;
#endif
    // TPT
    int                 m_nMaxContact;

signals:

public slots:
    void onSensorConnect(T3K_HANDLE hDevice);
    void onSensorDisconnect(T3K_HANDLE hDevice);
    void onPacket(void* pContext);
    virtual void onDownloadingFirmware(int bDownload);
    virtual int onReceiveRawData(void* pContext) = 0;
    virtual void onReceiveRawDataFlag( bool bReceive ) = 0;
};

#endif // T3KHANDLE_H
