#ifndef T30XHANDLE_H
#define T30XHANDLE_H

#include <T3kHIDLib.h>

#include <QObject>
#include <QImage>
#include <QQueue>
#include <QMutex>


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


class ITPDPT30xNotify;
class T30xHandle : public QObject
{
    Q_OBJECT

public:
    T30xHandle();
    virtual ~T30xHandle();

    enum TouchSensorModel { TSM_NONE, TSM_OLDT3000, TSM_T3000, TSM_T3100, TSM_T3200, TSM_T3500, TSM_T3900, TSM_SPEC };

    // custum
    void SetBothSensorData( bool bBoth ) { m_bGetBothSensorData = bBoth ;}

    // after open func
    bool SetNotify( ITPDPT30xNotify* pNotify );

    bool IsOpen();
    bool Open();

    bool OpenWithVIDPID( unsigned short nVID, unsigned short nPID, unsigned short nMI = 1, int nDevIndex = 0 );
    bool Close( bool bNotify=true );
    unsigned long GetLastError();

    static int GetDeviceCount( unsigned short nVID, unsigned short nPID, unsigned short nMI = 1 );
    static char* GetDevPath( unsigned short nVID, unsigned short nPID, unsigned short nMI = 1, int nDevIndex = 0 );

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
    void ExOnDisconnect();
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
    ITPDPT30xNotify*                        m_pNotify;

    T3K_EVENT_NOTIFY                        m_T30xNotify;
    T3K_HANDLE                              m_pT3kDevice;

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
    void Connect();
    void Disconnect();
    void Packet(void* pContext);
    // deadlock caution
    void PacketSync(void* pContext);
    //
    int ReceiveRawData(void* pContext);
    void DownloadingFirmware(int bDownload);

    void ReceiveRawDataFlag(bool bReceive);

public slots:
    void onReceiveRawDataFlag(bool bReceive);

};

class ITPDPT30xNotify : public QObject
{
    Q_OBJECT

public:
    ITPDPT30xNotify(QObject* pParent = NULL);
    virtual ~ITPDPT30xNotify();

protected:
    virtual void OnOpenT30xDevice() {}
    virtual void OnCloseT30xDevice() {}
    virtual void OnFirmwareDownload( bool /*bDownload*/ ) {}

    virtual void OnMSG( short /*nTickTime*/, const char* /*sPartId*/, const char* /*sTxt*/ ) {}
    virtual void OnOBJ( short /*nTickTime*/, const char* /*sPartId*/, int /*nCamNo*/, T3kRangeF* /*pOBJ*/, unsigned short /*count*/ ) {}
    virtual void OnOBC( short /*nTickTime*/, const char* /*sPartId*/, int /*nCamNo*/, T3kRangeF* /*pOBC*/, unsigned short /*count*/ ) {}
    virtual void OnDTC( short /*nTickTime*/, const char* /*sPartId*/, int /*nCamNo*/, T3kRangeF* /*pDTC*/, unsigned short /*count*/ ) {}
    virtual void OnIRD( short /*nTickTime*/, const char* /*sPartId*/, int /*nCamNo*/, int /*nCount*/, uchar* /*pIRD*/ ) {}
    virtual void OnITD( short /*nTickTime*/, const char* /*sPartId*/, int /*nCamNo*/, int /*nCount*/, uchar* /*pITD*/ ) {}
    virtual void OnPRV( short /*nTickTime*/, const char* /*sPartId*/, int /*nWidth*/, int /*nHeight*/, int /*nBitCount*/, unsigned char* /*pBitmapBuffer*/ ) {}
    virtual void OnCMD( short /*nTickTime*/, const char* /*sPartId*/, long /*lId*/, const char* /*sCmd*/ ) {}
    virtual void OnRSP( short /*nTickTime*/, const char* /*sPartId*/, long /*lId*/, bool /*bFinal*/, const char* /*sCmd*/ ) {}
    virtual void OnRSE( short /*nTickTime*/, const char* /*sPartId*/, long /*lId*/, bool /*bFinal*/, const char* /*sCmd*/ ) {}
    virtual void OnSTT( short /*nTickTime*/, const char* /*sPartId*/, const char* /*pStatus*/ ) {}
    virtual void OnDVC( short /*nTickTime*/, T3kDVC& /*DVC*/ ) {}
    virtual void OnTPT( short /*nTickTime*/, short /*nActualTouch*/, short /*nTouchCount*/, t3ktouchpoint* /*points*/ ) {}
    virtual void OnGST( short /*nTickTime*/, T3kGST& /*GST*/ ) {}
    virtual void OnVER( short /*nTickTime*/, const char* /*sPartId*/, T3kVER& /*Ver*/ ) {}

protected:
    void SendRemoteNotifyPacket( int nType );
    void SendRemoteRawDataPacket( int nType, const char* pData, qint64 nDataSize );

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
    // PRV
    QImage              m_SideView;
    int                 m_nCamPRVMax;
    uchar*              m_pFillCheck;
    // TPT
    int                 m_nMaxContact;

    // Remote
    bool                m_bRemoteMode;

signals:

public slots:
    void onConnect();
    void onSensorDisconnect();
    void onPacket(void* pContext);
    int onReceiveRawData(void* pContext);
    void onDownloadingFirmware(int bDownload);

    void onReceiveRawDataFlag( bool bReceive );
};

#endif // T30XHANDLE_H
