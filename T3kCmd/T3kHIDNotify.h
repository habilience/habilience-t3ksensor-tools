#ifndef T3KHIDNOTIFY_H
#define T3KHIDNOTIFY_H

#include "../common/T3kHandle.h"

class T3kHIDNotify : public ITPDPT3kNotify
{
public:
    T3kHIDNotify();
    virtual ~T3kHIDNotify() {}

    class IT3kEventListener
    {
    public:
        IT3kEventListener() { T3kHIDNotify::Instance()->SetEvnetListener(this); }
        ~IT3kEventListener() { T3kHIDNotify::Instance()->SetEvnetListener(NULL); }
        virtual void OnOpenT3kDevice(T3K_HANDLE /*hDevice*/) {}
        virtual void OnCloseT3kDevice(T3K_HANDLE /*hDevice*/) {}
        virtual void OnFirmwareDownload( bool /*bDownload*/ ) {}

        virtual void OnMSG( short /*nTickTime*/, const char* /*sPartId*/, const char* /*sTxt*/ ) {}
        virtual void OnOBJ( short /*nTickTime*/, const char* /*sPartId*/, int /*nCamNo*/, T3kRangeF* /*pOBJ*/, unsigned short /*nNumberOfOBJ*/ ) {}
        virtual void OnOBC( short /*nTickTime*/, const char* /*sPartId*/, int /*nCamNo*/, T3kRangeF* /*pOBC*/, unsigned short /*nNumberOfOBC*/ ) {}
        virtual void OnDTC( short /*nTickTime*/, const char* /*sPartId*/, int /*nCamNo*/, T3kRangeF* /*pDTC*/, unsigned short /*nNumberOfDTC*/ ) {}
        virtual void OnIRD( short /*nTickTime*/, const char* /*sPartId*/, int /*nCamNo*/, int /*nCount*/, uchar* /*pIRD*/ ) {}
        virtual void OnITD( short /*nTickTime*/, const char* /*sPartId*/, int /*nCamNo*/, int /*nCount*/, uchar* /*pITD*/ ) {}
        virtual void OnPRV( short /*nTickTime*/, const char* /*sPartId*/, int /*nWidth*/, int /*nHeight*/, int /*nBitCount*/, unsigned char* /*pBitmapBuffer*/ ) {}
        virtual void OnCMD( short /*nTickTime*/, const char* /*sPartId*/, long /*lId*/, const char* /*sCmd*/ ) {}
        virtual void OnRSP( short /*nTickTime*/, const char* /*sPartId*/, long /*lId*/, bool /*bFinal*/, const char* /*sCmd*/ ) {}
        virtual void OnRSE( short /*nTickTime*/, const char* /*sPartId*/, long /*lId*/, bool /*bFinal*/, const char* /*sCmd*/ ) {}
        virtual void OnSTT( short /*nTickTime*/, const char* /*sPartId*/, const char* /*pStatus*/ ) {}
        //virtual void OnDVC( short /*nTickTime*/, T3kDVC& /*DVC*/ ) {}
        virtual void OnTPT( short /*nTickTime*/, short /*nActualTouch*/, short /*nTouchCount*/, t3ktouchpoint* /*points*/ ) {}
        virtual void OnGST( short /*nTickTime*/, T3kGST& /*GST*/ ) {}
        virtual void OnVER( short /*nTickTime*/, const char* /*sPartId*/, T3kVER& /*Ver*/ ) {}
    };

    class _GC
    {
    public:
        _GC();
        ~_GC();
    };
    friend class _GC;

    static T3kHIDNotify* Instance();

    void SetEvnetListener( IT3kEventListener* listener );

protected:
    virtual void OnOpenT3kDevice(T3K_HANDLE hDevice);
    virtual void OnCloseT3kDevice(T3K_HANDLE hDevice);
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

    virtual int onReceiveRawData(void* /*pContext*/) { return 0; }
    virtual void onReceiveRawDataFlag( bool /*bReceive*/ ) {}

protected:
    IT3kEventListener*          m_EventListner;

private:
    static T3kHIDNotify* s_pThis;
};

#endif // T3KHIDNOTIFY_H
