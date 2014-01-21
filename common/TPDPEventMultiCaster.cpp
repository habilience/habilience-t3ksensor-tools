#include "./QT3kDeviceREventHandler.h"


TPDPEventMultiCaster::_GC::_GC()
{
}

TPDPEventMultiCaster::_GC::~_GC()
{
        if( TPDPEventMultiCaster::s_pThis )
                delete TPDPEventMultiCaster::s_pThis;
        TPDPEventMultiCaster::s_pThis = NULL;
}
TPDPEventMultiCaster::_GC s_GC;

TPDPEventMultiCaster* TPDPEventMultiCaster::s_pThis = NULL;

TPDPEventMultiCaster::TPDPEventMultiCaster()
{
    m_pSingleListener = NULL;
}

TPDPEventMultiCaster::~TPDPEventMultiCaster()
{
    m_vEventListener.clear();
}

TPDPEventMultiCaster* TPDPEventMultiCaster::instance()
{
    if( !s_pThis )
    {
        s_pThis = new TPDPEventMultiCaster();
    }
    return s_pThis;
}

void TPDPEventMultiCaster::AddListener( ITPDPEventListener* pListener )
{
    for( int nI = 0 ; nI < m_vEventListener.count() ; nI ++ )
    {
        if( m_vEventListener.at(nI) == pListener )
            return;
    }
    m_vEventListener.push_back( pListener );
}

void TPDPEventMultiCaster::RemoveListener( ITPDPEventListener* pListener )
{
    for( int nI = 0 ; nI < m_vEventListener.count() ; nI ++ )
    {
        if( m_vEventListener.at(nI) == pListener )
        {
            m_vEventListener.remove(nI);
        }
    }
}

void TPDPEventMultiCaster::SetSingleListener(ITPDPEventListener *pListener)
{
    m_pSingleListener = pListener;
}

void TPDPEventMultiCaster::ClearSingleListener()
{
    m_pSingleListener = NULL;
}

void TPDPEventMultiCaster::OnCloseT3kDevice(T3K_HANDLE hDevice)
{
    if( m_pSingleListener )
    {
        m_pSingleListener->OnCloseT3kDevice(hDevice);
        return;
    }
    for( int nI = 0 ; nI < m_vEventListener.count() ; nI ++ )
    {
        (m_vEventListener.at(nI))->OnCloseT3kDevice(hDevice);
    }
}

void TPDPEventMultiCaster::OnOpenT3kDevice(T3K_HANDLE hDevice)
{
    if( m_pSingleListener )
    {
        m_pSingleListener->OnOpenT3kDevice(hDevice);
        return;
    }
    for( int nI = 0 ; nI < m_vEventListener.count() ; nI ++ )
    {
        (m_vEventListener.at(nI))->OnOpenT3kDevice(hDevice);
    }
}

void TPDPEventMultiCaster::OnFirmwareDownload( bool bDownload )
{
    if( m_pSingleListener )
    {
        m_pSingleListener->OnFirmwareDownload( bDownload );
        return;
    }
    for( int nI = 0 ; nI < m_vEventListener.count() ; nI ++ )
    {
        (m_vEventListener.at(nI))->OnFirmwareDownload( bDownload );
    }
}

void TPDPEventMultiCaster::OnMSG(ushort nTickTime, const char *sPartId, const char *sTxt)
{
    ResponsePart Part = GetResponsePartFromPartId( sPartId, sTxt );
    if( m_pSingleListener )
    {
        m_pSingleListener->OnMSG( Part, nTickTime, sPartId, sTxt );
        return;
    }
    for( int nI = 0 ; nI < m_vEventListener.count() ; nI ++ )
    {
        (m_vEventListener.at(nI))->OnMSG( Part, nTickTime, sPartId, sTxt );
    }
}

void TPDPEventMultiCaster::OnOBJ(ushort nTickTime, const char *sPartId, int nCamNo, T3kRangeF *pOBJ, unsigned short nNumberOfOBJ)
{
    ResponsePart Part = GetResponsePartFromPartId( sPartId, NULL );
    if( m_pSingleListener )
    {
        m_pSingleListener->OnOBJ( Part, nTickTime, sPartId, nCamNo, pOBJ, nNumberOfOBJ );
        return;
    }
    for( int nI = 0 ; nI < m_vEventListener.count() ; nI ++ )
    {
        (m_vEventListener.at(nI))->OnOBJ( Part, nTickTime, sPartId, nCamNo, pOBJ, nNumberOfOBJ );
    }
}

void TPDPEventMultiCaster::OnOBC(ushort nTickTime, const char *sPartId, int nCamNo, T3kRangeF *pOBC, unsigned short nNumberOfOBC)
{
    ResponsePart Part = GetResponsePartFromPartId( sPartId, NULL );
    if( m_pSingleListener )
    {
        m_pSingleListener->OnOBC( Part, nTickTime, sPartId, nCamNo, pOBC, nNumberOfOBC );
        return;
    }
    for( int nI = 0 ; nI < m_vEventListener.count() ; nI ++ )
    {
        (m_vEventListener.at(nI))->OnOBC( Part, nTickTime, sPartId, nCamNo, pOBC, nNumberOfOBC );
    }
}

void TPDPEventMultiCaster::OnDTC(ushort nTickTime, const char *sPartId, int nCamNo, T3kRangeF *pDTC, unsigned short nNumberOfDTC)
{
    ResponsePart Part = GetResponsePartFromPartId( sPartId, NULL );
    if( m_pSingleListener )
    {
        m_pSingleListener->OnDTC( Part, nTickTime, sPartId, nCamNo, pDTC, nNumberOfDTC );
        return;
    }
    for( int nI = 0 ; nI < m_vEventListener.count() ; nI ++ )
    {
        (m_vEventListener.at(nI))->OnDTC( Part, nTickTime, sPartId, nCamNo, pDTC, nNumberOfDTC );
    }
}

void TPDPEventMultiCaster::OnIRD(ushort nTickTime, const char *sPartId, int nCamNo, int nCount, uchar *pIRD)
{
    ResponsePart Part = GetResponsePartFromPartId( sPartId, NULL );
    if( m_pSingleListener )
    {
        m_pSingleListener->OnIRD( Part, nTickTime, sPartId, nCamNo, nCount, pIRD );
        return;
    }
    for( int nI = 0 ; nI < m_vEventListener.count() ; nI ++ )
    {
        (m_vEventListener.at(nI))->OnIRD( Part, nTickTime, sPartId, nCamNo, nCount, pIRD );
    }
}

void TPDPEventMultiCaster::OnITD(ushort nTickTime, const char *sPartId, int nCamNo, int nCount, uchar *pITD)
{
    ResponsePart Part = GetResponsePartFromPartId( sPartId, NULL );
    if( m_pSingleListener )
    {
        m_pSingleListener->OnITD( Part, nTickTime, sPartId, nCamNo, nCount, pITD );
        return;
    }
    for( int nI = 0 ; nI < m_vEventListener.count() ; nI ++ )
    {
        (m_vEventListener.at(nI))->OnITD( Part, nTickTime, sPartId, nCamNo, nCount, pITD );
    }
}

void TPDPEventMultiCaster::OnPRV(ushort nTickTime, const char *sPartId, int nWidth, int nHeight, int nBitCount, unsigned char *pBitmapBuffer)
{
    ResponsePart Part = GetResponsePartFromPartId( sPartId, NULL );
    if( m_pSingleListener )
    {
        m_pSingleListener->OnPRV( Part, nTickTime, sPartId, nWidth, nHeight, nBitCount, pBitmapBuffer );
        return;
    }
    for( int nI = 0 ; nI < m_vEventListener.count() ; nI ++ )
    {
        (m_vEventListener.at(nI))->OnPRV( Part, nTickTime, sPartId, nWidth, nHeight, nBitCount, pBitmapBuffer );
    }
}

void TPDPEventMultiCaster::OnCMD( ushort nTickTime, const char* sPartId, long lId, const char* sCmd )
{
    ResponsePart Part = GetResponsePartFromPartId( sPartId, sCmd );
    if( m_pSingleListener )
    {
        m_pSingleListener->OnCMD( Part, nTickTime, sPartId, lId, sCmd );
        return;
    }
    for( int nI = 0 ; nI < m_vEventListener.count() ; nI ++ )
    {
        (m_vEventListener.at(nI))->OnCMD( Part, nTickTime, sPartId, lId, sCmd );
    }
}

void TPDPEventMultiCaster::OnRSP( ushort nTickTime, const char* sPartId, long lId, bool bFinal, const char* sCmd )
{
    ResponsePart Part = GetResponsePartFromPartId( sPartId, sCmd );
    if( m_pSingleListener )
    {
        m_pSingleListener->OnRSP( Part, nTickTime, sPartId, lId, bFinal, sCmd );
        return;
    }
    for( int nI = 0 ; nI < m_vEventListener.count() ; nI ++ )
    {
        (m_vEventListener.at(nI))->OnRSP( Part, nTickTime, sPartId, lId, bFinal, sCmd );
    }
}

void TPDPEventMultiCaster::OnRSE( ushort nTickTime, const char* sPartId, long lId, bool bFinal, const char* sCmd )
{
    ResponsePart Part = GetResponsePartFromPartId( sPartId, sCmd );
    if( m_pSingleListener )
    {
        m_pSingleListener->OnRSE( Part, nTickTime, sPartId, lId, bFinal, sCmd );
        return;
    }
    for( int nI = 0 ; nI < m_vEventListener.count() ; nI ++ )
    {
        (m_vEventListener.at(nI))->OnRSE( Part, nTickTime, sPartId, lId, bFinal, sCmd );
    }
}

void TPDPEventMultiCaster::OnSTT(ushort nTickTime, const char *sPartId, const char *pStatus)
{
    ResponsePart Part = GetResponsePartFromPartId( sPartId, NULL );
    if( m_pSingleListener )
    {
        m_pSingleListener->OnSTT( Part, nTickTime, sPartId, pStatus );
        return;
    }
    for( int nI = 0 ; nI < m_vEventListener.count() ; nI ++ )
    {
        (m_vEventListener.at(nI))->OnSTT( Part, nTickTime, sPartId, pStatus );
    }
}

void TPDPEventMultiCaster::OnDVC( ushort nTickTime, const char* sPartId, T3kDVC& DVC )
{
    ResponsePart Part = GetResponsePartFromPartId( sPartId, NULL );
    if( m_pSingleListener )
    {
        m_pSingleListener->OnDVC( Part, nTickTime, DVC );
        return;
    }
    for( int nI = 0 ; nI < m_vEventListener.count() ; nI ++ )
    {
        (m_vEventListener.at(nI))->OnDVC( Part, nTickTime, DVC );
    }
}

void TPDPEventMultiCaster::OnTPT(ushort nTickTime, short nActualTouch, short nTouchCount, t3ktouchpoint *points)
{
    if( m_pSingleListener )
    {
        m_pSingleListener->OnTPT( nTickTime, nActualTouch, nTouchCount, points );
        return;
    }
    for( int nI = 0 ; nI < m_vEventListener.count() ; nI ++ )
    {
        (m_vEventListener.at(nI))->OnTPT( nTickTime, nActualTouch, nTouchCount, points );
    }
}

void TPDPEventMultiCaster::OnGST(ushort nTickTime, T3kGST &GST)
{
    if( m_pSingleListener )
    {
        m_pSingleListener->OnGST( nTickTime, GST );
        return;
    }
    for( int nI = 0 ; nI < m_vEventListener.count() ; nI ++ )
    {
        (m_vEventListener.at(nI))->OnGST( nTickTime, GST );
    }
}

void TPDPEventMultiCaster::OnVER(ushort nTickTime, const char *sPartId, T3kVER &Ver)
{
    ResponsePart Part = GetResponsePartFromPartId( sPartId, NULL );
    if( m_pSingleListener )
    {
        m_pSingleListener->OnVER( Part, nTickTime, sPartId, Ver );
        return;
    }
    for( int nI = 0 ; nI < m_vEventListener.count() ; nI ++ )
    {
        (m_vEventListener.at(nI))->OnVER( Part, nTickTime, sPartId, Ver );
    }
}
