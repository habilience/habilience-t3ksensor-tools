#ifndef QTPDPEVENTMULTICASTER_H
#define QTPDPEVENTMULTICASTER_H

#include "../common/IncludeRemoteNotify.h"

#include <QVector>

enum ResponsePart { MM = 0, CM1, CM2, CM1_1, CM2_1 };

class QTPDPEventMultiCaster : public IncludeRemoteNotify
{
public:
    QTPDPEventMultiCaster();
    ~QTPDPEventMultiCaster();

    class ITPDPEventListener
    {
    public:
        ITPDPEventListener() { QTPDPEventMultiCaster::GetPtr()->AddListener(this); }
        ~ITPDPEventListener() { QTPDPEventMultiCaster::GetPtr()->RemoveListener(this); }
        virtual void OnOpenT3kDevice() {}
        virtual void OnCloseT3kDevice() {}
        virtual void OnFirmwareDownload( bool /*bDownload*/ ) {}

        virtual void OnMSG( ResponsePart /*Part*/, short /*nTickTime*/, const char* /*sPartId*/, const char* /*sTxt*/ ) {}
        virtual void OnOBJ( ResponsePart /*Part*/, short /*nTickTime*/, const char* /*sPartId*/, int /*nCamNo*/, T3kRangeF* /*pOBJ*/, unsigned short /*nNumberOfOBJ*/ ) {}
        virtual void OnOBC( ResponsePart /*Part*/, short /*nTickTime*/, const char* /*sPartId*/, int /*nCamNo*/, T3kRangeF* /*pOBC*/, unsigned short /*nNumberOfOBC*/ ) {}
        virtual void OnDTC( ResponsePart /*Part*/, short /*nTickTime*/, const char* /*sPartId*/, int /*nCamNo*/, T3kRangeF* /*pDTC*/, unsigned short /*nNumberOfDTC*/ ) {}
        virtual void OnIRD( ResponsePart /*Part*/, short /*nTickTime*/, const char* /*sPartId*/, int /*nCamNo*/, int /*nCount*/, uchar* /*pIRD*/ ) {}
        virtual void OnITD( ResponsePart /*Part*/, short /*nTickTime*/, const char* /*sPartId*/, int /*nCamNo*/, int /*nCount*/, uchar* /*pITD*/ ) {}
        virtual void OnPRV( ResponsePart /*Part*/, short /*nTickTime*/, const char* /*sPartId*/, int /*nWidth*/, int /*nHeight*/, int /*nBitCount*/, unsigned char* /*pBitmapBuffer*/ ) {}
        virtual void OnCMD( ResponsePart /*Part*/, short /*nTickTime*/, const char* /*sPartId*/, long /*lId*/, const char* /*sCmd*/ ) {}
        virtual void OnRSP( ResponsePart /*Part*/, short /*nTickTime*/, const char* /*sPartId*/, long /*lId*/, bool /*bFinal*/, const char* /*sCmd*/ ) {}
        virtual void OnRSE( ResponsePart /*Part*/, short /*nTickTime*/, const char* /*sPartId*/, long /*lId*/, bool /*bFinal*/, const char* /*sCmd*/ ) {}
        virtual void OnSTT( ResponsePart /*Part*/, short /*nTickTime*/, const char* /*sPartId*/, const char* /*pStatus*/ ) {}
        virtual void OnDVC( ResponsePart /*Part*/, short /*nTickTime*/, T3kDVC& /*DVC*/ ) {}
        virtual void OnTPT( short /*nTickTime*/, short /*nActualTouch*/, short /*nTouchCount*/, t3ktouchpoint* /*points*/ ) {}
        virtual void OnGST( short /*nTickTime*/, T3kGST& /*GST*/ ) {}
        virtual void OnVER( ResponsePart /*Part*/, short /*nTickTime*/, const char* /*sPartId*/, T3kVER& /*Ver*/ ) {}
    };

    class _GC
    {
    public:
        _GC();
        ~_GC();
    };
    friend class _GC;

    static QTPDPEventMultiCaster* GetPtr();

    void AddListener( ITPDPEventListener* pListener );
    void RemoveListener( ITPDPEventListener* pListener );

    void SetSingleListener( ITPDPEventListener* pListener );
    void ClearSingleListener();

protected:
    virtual void OnOpenT3kDevice();
    virtual void OnCloseT3kDevice();
    virtual void OnFirmwareDownload( bool bDownload );

    virtual void OnMSG( short nTickTime, const char* sPartId, const char* sTxt );
    virtual void OnOBJ( short nTickTime, const char* sPartId, int nCamNo, T3kRangeF* pOBJ, unsigned short nNumberOfOBJ );
    virtual void OnOBC( short nTickTime, const char* sPartId, int nCamNo, T3kRangeF* pOBC, unsigned short nNumberOfOBC );
    virtual void OnDTC( short nTickTime, const char* sPartId, int nCamNo, T3kRangeF* pDTC, unsigned short nNumberOfDTC );
    virtual void OnIRD( short nTickTime, const char* sPartId, int nCamNo, int nCount, uchar* pIRD );
    virtual void OnITD( short nTickTime, const char* sPartId, int nCamNo, int nCount, uchar* pITD );
    virtual void OnPRV( short nTickTime, const char* sPartId, int nWidth, int nHeight, int nBitCount, unsigned char* pBitmapBuffer );
    virtual void OnCMD( short nTickTime, const char* sPartId, long lId, const char* sCmd );
    virtual void OnRSP( short nTickTime, const char* sPartId, long lId, bool bFinal, const char* sCmd );
    virtual void OnRSE( short nTickTime, const char* sPartId, long lId, bool bFinal, const char* sCmd );
    virtual void OnSTT( short nTickTime, const char* sPartId, const char *pStatus );
    //virtual void OnDVC( short nTickTime, T3kDVC& DVC );
    virtual void OnTPT( short nTickTime, short nActualTouch, short nTouchCount, t3ktouchpoint* points );
    virtual void OnGST( short nTickTime, T3kGST& GST );
    virtual void OnVER( short nTickTime, const char* sPartId, T3kVER& Ver );

protected:
    QVector<ITPDPEventListener*>	m_vEventListener;
    ITPDPEventListener*             m_pSingleListener;

private:
    static QTPDPEventMultiCaster*   s_pThis;
};

#endif // QTPDPEVENTMULTICASTER_H
