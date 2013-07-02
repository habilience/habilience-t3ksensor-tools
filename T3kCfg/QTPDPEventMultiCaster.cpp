#include "stdInclude.h"
#include "QTPDPEventMultiCaster.h"

inline ResponsePart GetResponsePartFromPartId( const char* sPartId, const char* cmd )
{
    ResponsePart Part = MM;

    const char* szCmd = cmd;

    QString strPartID( sPartId );
    int nPartIdIdx = (int)strlen( sPartId )-1;
    if( strPartID.contains( '-' ) )
        nPartIdIdx -= 2;

    switch( sPartId[nPartIdIdx] )
    {
    default:
        if( szCmd != NULL )
        {
            if ( strstr(szCmd, cstrCam1) == szCmd )
            {
                szCmd += (sizeof(cstrCam1)-1);
                Part = CM1;
            }

            if ( strstr(szCmd, cstrCam2) == szCmd )
            {
                szCmd += (sizeof(cstrCam2)-1);
                Part = CM2;
            }
        }
        else
        {
            Part = MM;
        }
        break;
    case '1':
        Part = strPartID.contains( '-' ) ? CM1_1 : CM1;
        break;
    case '2':
        Part = strPartID.contains( '-' ) ? CM2_1 : CM2;
        break;
    }

    return Part;
}

QTPDPEventMultiCaster::_GC::_GC()
{
}

QTPDPEventMultiCaster::_GC::~_GC()
{
        if( QTPDPEventMultiCaster::s_pThis )
                delete QTPDPEventMultiCaster::s_pThis;
        QTPDPEventMultiCaster::s_pThis = NULL;
}
QTPDPEventMultiCaster::_GC s_GC;

QTPDPEventMultiCaster* QTPDPEventMultiCaster::s_pThis = NULL;

QTPDPEventMultiCaster::QTPDPEventMultiCaster()
{
    m_pSingleListener = NULL;
}

QTPDPEventMultiCaster::~QTPDPEventMultiCaster()
{
    m_vEventListener.clear();
}

QTPDPEventMultiCaster* QTPDPEventMultiCaster::GetPtr()
{
    if( !s_pThis )
    {
        s_pThis = new QTPDPEventMultiCaster();
    }
    return s_pThis;
}

void QTPDPEventMultiCaster::AddListener( ITPDPEventListener* pListener )
{
    for( int nI = 0 ; nI < m_vEventListener.count() ; nI ++ )
    {
        if( m_vEventListener.at(nI) == pListener )
            return;
    }
    m_vEventListener.push_back( pListener );
}

void QTPDPEventMultiCaster::RemoveListener( ITPDPEventListener* pListener )
{
    for( int nI = 0 ; nI < m_vEventListener.count() ; nI ++ )
    {
        if( m_vEventListener.at(nI) == pListener )
        {
            m_vEventListener.remove(nI);
        }
    }
}

void QTPDPEventMultiCaster::SetSingleListener(ITPDPEventListener *pListener)
{
    m_pSingleListener = pListener;
}

void QTPDPEventMultiCaster::ClearSingleListener()
{
    m_pSingleListener = NULL;
}

void QTPDPEventMultiCaster::OnCloseT3kDevice()
{
    if( m_pSingleListener )
    {
        m_pSingleListener->OnCloseT3kDevice();
        return;
    }
    for( int nI = 0 ; nI < m_vEventListener.count() ; nI ++ )
    {
        (m_vEventListener.at(nI))->OnCloseT3kDevice();
    }
}

void QTPDPEventMultiCaster::OnOpenT3kDevice()
{
    if( m_pSingleListener )
    {
        m_pSingleListener->OnOpenT3kDevice();
        return;
    }
    for( int nI = 0 ; nI < m_vEventListener.count() ; nI ++ )
    {
        (m_vEventListener.at(nI))->OnOpenT3kDevice();
    }
}

void QTPDPEventMultiCaster::OnFirmwareDownload( bool bDownload )
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

void QTPDPEventMultiCaster::OnMSG(short nTickTime, const char *sPartId, const char *sTxt)
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

void QTPDPEventMultiCaster::OnOBJ(short nTickTime, const char *sPartId, int nCamNo, T3kRangeF *pOBJ, unsigned short nNumberOfOBJ)
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

void QTPDPEventMultiCaster::OnOBC(short nTickTime, const char *sPartId, int nCamNo, T3kRangeF *pOBC, unsigned short nNumberOfOBC)
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

void QTPDPEventMultiCaster::OnDTC(short nTickTime, const char *sPartId, int nCamNo, T3kRangeF *pDTC, unsigned short nNumberOfDTC)
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

void QTPDPEventMultiCaster::OnIRD(short nTickTime, const char *sPartId, int nCamNo, int nCount, uchar *pIRD)
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

void QTPDPEventMultiCaster::OnITD(short nTickTime, const char *sPartId, int nCamNo, int nCount, uchar *pITD)
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

void QTPDPEventMultiCaster::OnPRV(short nTickTime, const char *sPartId, int nWidth, int nHeight, int nBitCount, unsigned char *pBitmapBuffer)
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

void QTPDPEventMultiCaster::OnCMD( short nTickTime, const char* sPartId, long lId, const char* sCmd )
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

void QTPDPEventMultiCaster::OnRSP( short nTickTime, const char* sPartId, long lId, bool bFinal, const char* sCmd )
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

void QTPDPEventMultiCaster::OnRSE( short nTickTime, const char* sPartId, long lId, bool bFinal, const char* sCmd )
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

void QTPDPEventMultiCaster::OnSTT(short nTickTime, const char *sPartId, const char *pStatus)
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

//void QTPDPEventMultiCaster::OnDVC( short nTickTime, T3kDVC& DVC )
//{

//}

void QTPDPEventMultiCaster::OnTPT(short nTickTime, short nActualTouch, short nTouchCount, t3ktouchpoint *points)
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

void QTPDPEventMultiCaster::OnGST(short nTickTime, T3kGST &GST)
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

void QTPDPEventMultiCaster::OnVER(short nTickTime, const char *sPartId, T3kVER &Ver)
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
