#ifndef TPDPEVENTMULTICASTER_H
#define TPDPEVENTMULTICASTER_H

#ifndef _T3KHANDLE_INCLUDE_REMOTE
#include "./T3kHandle.h"
#else
#include "./IncludeRemoteNotify.h"
#endif

#include <QVector>

class TPDPEventMultiCaster :
        #ifndef _T3KHANDLE_INCLUDE_REMOTE
        public ITPDPT3kNotify
        #else
        public IncludeRemoteNotify
        #endif
{
public:
    TPDPEventMultiCaster();
    ~TPDPEventMultiCaster();

    class ITPDPEventListener
    {
    public:
        ITPDPEventListener() { TPDPEventMultiCaster::instance()->AddListener(this); }
        ~ITPDPEventListener() { TPDPEventMultiCaster::instance()->RemoveListener(this); }
        virtual void OnOpenT3kDevice(T3K_HANDLE) {}
        virtual void OnCloseT3kDevice(T3K_HANDLE) {}
        virtual void OnFirmwareDownload( bool /*bDownload*/ ) {}

        virtual void OnMSG( ResponsePart /*Part*/, ushort /*nTickTime*/, const char* /*sPartId*/, const char* /*sTxt*/ ) {}
        virtual void OnOBJ( ResponsePart /*Part*/, ushort /*nTickTime*/, const char* /*sPartId*/, int /*nCamNo*/, T3kRangeF* /*pOBJ*/, unsigned short /*nNumberOfOBJ*/ ) {}
        virtual void OnOBC( ResponsePart /*Part*/, ushort /*nTickTime*/, const char* /*sPartId*/, int /*nCamNo*/, T3kRangeF* /*pOBC*/, unsigned short /*nNumberOfOBC*/ ) {}
        virtual void OnDTC( ResponsePart /*Part*/, ushort /*nTickTime*/, const char* /*sPartId*/, int /*nCamNo*/, T3kRangeF* /*pDTC*/, unsigned short /*nNumberOfDTC*/ ) {}
        virtual void OnIRD( ResponsePart /*Part*/, ushort /*nTickTime*/, const char* /*sPartId*/, int /*nCamNo*/, int /*nCount*/, uchar* /*pIRD*/ ) {}
        virtual void OnITD( ResponsePart /*Part*/, ushort /*nTickTime*/, const char* /*sPartId*/, int /*nCamNo*/, int /*nCount*/, uchar* /*pITD*/ ) {}
        virtual void OnPRV( ResponsePart /*Part*/, ushort /*nTickTime*/, const char* /*sPartId*/, int /*nWidth*/, int /*nHeight*/, int /*nBitCount*/, unsigned char* /*pBitmapBuffer*/ ) {}
        virtual void OnCMD( ResponsePart /*Part*/, ushort /*nTickTime*/, const char* /*sPartId*/, long /*lId*/, const char* /*sCmd*/ ) {}
        virtual void OnRSP( ResponsePart /*Part*/, ushort /*nTickTime*/, const char* /*sPartId*/, long /*lId*/, bool /*bFinal*/, const char* /*sCmd*/ ) {}
        virtual void OnRSE( ResponsePart /*Part*/, ushort /*nTickTime*/, const char* /*sPartId*/, long /*lId*/, bool /*bFinal*/, const char* /*sCmd*/ ) {}
        virtual void OnSTT( ResponsePart /*Part*/, ushort /*nTickTime*/, const char* /*sPartId*/, const char* /*pStatus*/ ) {}
        virtual void OnDVC( ResponsePart /*Part*/, ushort /*nTickTime*/, T3kDVC& /*DVC*/ ) {}
        virtual void OnTPT( ushort /*nTickTime*/, ushort /*nActualTouch*/, short /*nTouchCount*/, t3ktouchpoint* /*points*/ ) {}
        virtual void OnGST( ushort /*nTickTime*/, T3kGST& /*GST*/ ) {}
        virtual void OnVER( ResponsePart /*Part*/, ushort /*nTickTime*/, const char* /*sPartId*/, T3kVER& /*Ver*/ ) {}
    };

    class _GC
    {
    public:
        _GC();
        ~_GC();
    };
    friend class _GC;

    static TPDPEventMultiCaster* instance();

    void AddListener( ITPDPEventListener* pListener );
    void RemoveListener( ITPDPEventListener* pListener );

    void SetSingleListener( ITPDPEventListener* pListener );
    void ClearSingleListener();

protected:
    virtual void OnOpenT3kDevice(T3K_HANDLE hDevice);
    virtual void OnCloseT3kDevice(T3K_HANDLE hDevice);
    virtual void OnFirmwareDownload( bool bDownload );

    virtual void OnMSG( ushort nTickTime, const char* sPartId, const char* sTxt );
    virtual void OnOBJ( ushort nTickTime, const char* sPartId, int nCamNo, T3kRangeF* pOBJ, unsigned short nNumberOfOBJ );
    virtual void OnOBC( ushort nTickTime, const char* sPartId, int nCamNo, T3kRangeF* pOBC, unsigned short nNumberOfOBC );
    virtual void OnDTC( ushort nTickTime, const char* sPartId, int nCamNo, T3kRangeF* pDTC, unsigned short nNumberOfDTC );
    virtual void OnIRD( ushort nTickTime, const char* sPartId, int nCamNo, int nCount, uchar* pIRD );
    virtual void OnITD( ushort nTickTime, const char* sPartId, int nCamNo, int nCount, uchar* pITD );
    virtual void OnPRV( ushort nTickTime, const char* sPartId, int nWidth, int nHeight, int nBitCount, unsigned char* pBitmapBuffer );
    virtual void OnCMD( ushort nTickTime, const char* sPartId, long lId, const char* sCmd );
    virtual void OnRSP( ushort nTickTime, const char* sPartId, long lId, bool bFinal, const char* sCmd );
    virtual void OnRSE( ushort nTickTime, const char* sPartId, long lId, bool bFinal, const char* sCmd );
    virtual void OnSTT( ushort nTickTime, const char* sPartId, const char *pStatus );
    virtual void OnDVC( ushort nTickTime, const char* sPartId, T3kDVC& DVC );
    virtual void OnTPT( ushort nTickTime, short nActualTouch, short nTouchCount, t3ktouchpoint* points );
    virtual void OnGST( ushort nTickTime, T3kGST& GST );
    virtual void OnVER( ushort nTickTime, const char* sPartId, T3kVER& Ver );

#ifndef _T3KHANDLE_INCLUDE_REMOTE
    virtual int onReceiveRawData(void* /*pContext*/) { return 0; }
    virtual void onReceiveRawDataFlag( bool /*bReceive*/ ) {}
#endif

protected:
    QVector<ITPDPEventListener*>	m_vEventListener;
    ITPDPEventListener*             m_pSingleListener;

private:
    static TPDPEventMultiCaster*   s_pThis;
};

#endif // TPDPEVENTMULTICASTER_H
