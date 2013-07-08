#include "T3kHIDNotify.h"

T3kHIDNotify::_GC::_GC()
{
}

T3kHIDNotify::_GC::~_GC()
{
    if( T3kHIDNotify::s_pThis )
        delete T3kHIDNotify::s_pThis;
    T3kHIDNotify::s_pThis = NULL;
}
T3kHIDNotify::_GC s_GC;

T3kHIDNotify* T3kHIDNotify::s_pThis = NULL;


T3kHIDNotify::T3kHIDNotify()
{
    m_EventListner = NULL;
}

T3kHIDNotify* T3kHIDNotify::Instance()
{
    if( !s_pThis )
        s_pThis = new T3kHIDNotify();
    return s_pThis;
}

void T3kHIDNotify::SetEvnetListener(IT3kEventListener *listener)
{
    m_EventListner = listener;
}

void T3kHIDNotify::OnCloseT3kDevice(T3K_HANDLE hDevice)
{
    if( m_EventListner != NULL )
        m_EventListner->OnCloseT3kDevice(hDevice);
}

void T3kHIDNotify::OnOpenT3kDevice(T3K_HANDLE hDevice)
{
    if( m_EventListner != NULL )
        m_EventListner->OnOpenT3kDevice(hDevice);
}

void T3kHIDNotify::OnFirmwareDownload( bool bDownload )
{
    if( m_EventListner != NULL )
        m_EventListner->OnFirmwareDownload( bDownload );
}

void T3kHIDNotify::OnMSG(ushort nTickTime, const char *sPartId, const char *sTxt)
{
    if( m_EventListner != NULL )
        m_EventListner->OnMSG( nTickTime, sPartId, sTxt );
}

void T3kHIDNotify::OnOBJ(ushort nTickTime, const char *sPartId, int nCamNo, T3kRangeF *pOBJ, unsigned short nNumberOfOBJ)
{
    if( m_EventListner != NULL )
        m_EventListner->OnOBJ( nTickTime, sPartId, nCamNo, pOBJ, nNumberOfOBJ );
}

void T3kHIDNotify::OnOBC(ushort nTickTime, const char *sPartId, int nCamNo, T3kRangeF *pOBC, unsigned short nNumberOfOBC)
{
    if( m_EventListner != NULL )
        m_EventListner->OnOBC( nTickTime, sPartId, nCamNo, pOBC, nNumberOfOBC );
}

void T3kHIDNotify::OnDTC(ushort nTickTime, const char *sPartId, int nCamNo, T3kRangeF *pDTC, unsigned short nNumberOfDTC)
{
    if( m_EventListner != NULL )
        m_EventListner->OnDTC( nTickTime, sPartId, nCamNo, pDTC, nNumberOfDTC );
}

void T3kHIDNotify::OnIRD(ushort nTickTime, const char *sPartId, int nCamNo, int nCount, uchar *pIRD)
{
    if( m_EventListner != NULL )
        m_EventListner->OnIRD( nTickTime, sPartId, nCamNo, nCount, pIRD );
}

void T3kHIDNotify::OnITD(ushort nTickTime, const char *sPartId, int nCamNo, int nCount, uchar *pITD)
{
    if( m_EventListner != NULL )
        m_EventListner->OnITD( nTickTime, sPartId, nCamNo, nCount, pITD );
}

void T3kHIDNotify::OnPRV(ushort nTickTime, const char *sPartId, int nWidth, int nHeight, int nBitCount, unsigned char *pBitmapBuffer)
{
    if( m_EventListner != NULL )
        m_EventListner->OnPRV( nTickTime, sPartId, nWidth, nHeight, nBitCount, pBitmapBuffer );
}

void T3kHIDNotify::OnCMD( ushort nTickTime, const char* sPartId, long lId, const char* sCmd )
{
    if( m_EventListner != NULL )
        m_EventListner->OnCMD( nTickTime, sPartId, lId, sCmd );
}

void T3kHIDNotify::OnRSP( ushort nTickTime, const char* sPartId, long lId, bool bFinal, const char* sCmd )
{
    if( m_EventListner != NULL )
        m_EventListner->OnRSP( nTickTime, sPartId, lId, bFinal, sCmd );
}

void T3kHIDNotify::OnRSE( ushort nTickTime, const char* sPartId, long lId, bool bFinal, const char* sCmd )
{
    if( m_EventListner != NULL )
        m_EventListner->OnRSE( nTickTime, sPartId, lId, bFinal, sCmd );
}

void T3kHIDNotify::OnSTT(ushort nTickTime, const char *sPartId, const char *pStatus)
{
    if( m_EventListner != NULL )
        m_EventListner->OnSTT( nTickTime, sPartId, pStatus );
}

//void T3kHIDNotify::OnDVC( ushort nTickTime, T3kDVC& DVC )
//{

//}

void T3kHIDNotify::OnTPT(ushort nTickTime, short nActualTouch, short nTouchCount, t3ktouchpoint *points)
{
    if( m_EventListner != NULL )
        m_EventListner->OnTPT( nTickTime, nActualTouch, nTouchCount, points );
}

void T3kHIDNotify::OnGST(ushort nTickTime, T3kGST &GST)
{
    if( m_EventListner != NULL )
        m_EventListner->OnGST( nTickTime, GST );
}

void T3kHIDNotify::OnVER(ushort nTickTime, const char *sPartId, T3kVER &Ver)
{
    if( m_EventListner != NULL )
        m_EventListner->OnVER( nTickTime, sPartId, Ver );
}
