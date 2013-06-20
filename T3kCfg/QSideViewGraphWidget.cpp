#include "QSideViewGraphWidget.h"

#include <QPainter>

QSideViewGraphWidget::QSideViewGraphWidget(QWidget *parent) :
    QWidget(parent)
{
    m_pData = NULL;
//    m_pBitmap = NULL;
    m_nCnt = 0;
}

QSideViewGraphWidget::~QSideViewGraphWidget()
{
    if( m_pData )
    {
        delete[] m_pData;
        m_pData = NULL;
    }
/*    if( m_pBitmap )
        delete m_pBitmap;
    m_pBitmap = NULL;*/
}

void QSideViewGraphWidget::SetData( const uchar* pData, const int nCnt )
{
    if( m_pData == NULL ||
        m_nCnt != nCnt )
    {
        if( m_pData ) delete[] m_pData;
        m_pData = new uchar[nCnt];
        m_nCnt = nCnt;
    }

    memcpy( m_pData, pData, sizeof(uchar)*m_nCnt );
}

void QSideViewGraphWidget::UpdateGraph()
{
    update();
}

void QSideViewGraphWidget::paintEvent(QPaintEvent */*evt*/)
{
    QPainter dc;
    dc.begin( this );

    QRect rcClient( 0, 0, width()-1, height()-1 );

    dc.fillRect( rcClient, Qt::black );
    OnDraw( &dc );

    dc.end();

    /*
    if( m_MemDC.GetSafeHdc() == NULL )
    {
        m_MemDC.CreateCompatibleDC( &dc );
    }

    bool bInit = false;
    if( !m_pBitmap )
    {
        m_pBitmap = new CBitmap;
        ASSERT( m_pBitmap );
        VERIFY( m_pBitmap->CreateCompatibleBitmap( &dc, rcClient.Width(), rcClient.Height() ) );
        bInit = TRUE;
    }

    ASSERT( m_pBitmap );
    CBitmap* pOldBitmap = m_MemDC.SelectObject( m_pBitmap );
    if( bInit )
        m_MemDC.FillSolidRect( rcClient, RGB(0, 0, 0) );

    OnDraw( &m_MemDC );

    m_MemDC.SetMapMode( MM_TEXT );
    m_MemDC.SetViewportOrg( 0, 0 );

    dc.BitBlt( rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(),
        &m_MemDC, 0, 0, SRCCOPY );

    m_MemDC.SelectObject( pOldBitmap );
    */
}

void QSideViewGraphWidget::OnDraw( QPainter* pDC )
{
    QRect rc( 0, 0, width()-1, height()-1 );

    if( m_nCnt == 0 || m_pData == NULL )
        return;

    float fStepW = (float)rc.width() / m_nCnt;

    pDC->setPen( QColor(255,80,80) );

    for( int i=1 ; i<m_nCnt ; i++ )
    {
        int nY1 = rc.height() - rc.height() * m_pData[i-1] / 0xff;
        int nY2 = rc.height() - rc.height() * m_pData[i] / 0xff;
        pDC->drawLine( int(rc.left() + fStepW * (i-1) + fStepW/2.f +.5f), nY1, int(rc.left() + fStepW * i + fStepW/2.f +.5f), nY2 );
    }
}
