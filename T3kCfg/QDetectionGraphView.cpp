#include "QDetectionGraphView.h"

#include <QPainter>
#include <QWheelEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QScrollBar>

#define ZOOM_STEP		(1.5f)

QDetectionGraphView::QDetectionGraphView(QWidget *parent) :
    QScrollArea(parent)
{
    setWidgetResizable( true );
    setMouseTracking( true );

    m_fZoomMax = 5.f;
    m_fZoomMin = 1.f;

    m_fZoomRatioW = 1.f;
    m_fZoomRatioH = 1.f;
    m_nBaseWidth = 0;
    m_nBaseHeight = 0;

    m_pITD = m_pIRD = NULL;
    m_nIRD = 0;
    m_bStopUpdate = false;

    m_nThreshold = 50;

    m_nRangeLeft = m_nRangeRight = 0;

    m_pImageCanvas = m_pImageGraph = m_pImageDetection = NULL;

    m_nNumberOfDTC = 0;

    m_nAutoOffset = -1;

    QSize sizeTotal;
    sizeTotal.setWidth( 0 );
    sizeTotal.setHeight( 0 );

    connect( horizontalScrollBar(), SIGNAL(actionTriggered(int)), this, SLOT(OnHScrollActionTrig(int)) );
    connect( verticalScrollBar(), SIGNAL(actionTriggered(int)), this, SLOT(OnVScrollActionTrig(int)) );

    //startTimer( 350 );
}

QDetectionGraphView::~QDetectionGraphView()
{
    if( m_pITD )
        delete[] m_pITD;
    m_pITD = NULL;
    if( m_pIRD )
        delete[] m_pIRD;
    m_pIRD = NULL;

    if( m_pImageCanvas )
    {
        delete m_pImageCanvas;
        m_pImageCanvas = NULL;
    }
    if( m_pImageGraph )
    {
        delete m_pImageGraph;
        m_pImageGraph = NULL;
    }
    if( m_pImageDetection )
    {
        delete m_pImageDetection;
        m_pImageDetection = NULL;
    }
}

void QDetectionGraphView::resizeEvent(QResizeEvent *evt)
{
    QSize szSize = evt->size();
    if( szSize.width() <= 0 && szSize.height() <= 0 ) return;

    // OnSize
    int nSWW, nSWH;
    GetScrollBarDimension( nSWW, nSWH );

    m_nBaseWidth = width();// + nSWW;
    m_nBaseHeight = height() + nSWH;

//    qDebug( tr("OnSize: (%dx%d)\r\n"), m_nBaseWidth, m_nBaseHeight );

    setUpdatesEnabled( false );

    if( m_pImageCanvas ) delete m_pImageCanvas;
    m_pImageCanvas = new QPixmap( m_nBaseWidth, m_nBaseHeight );
    QPainter pG( m_pImageCanvas );
    pG.fillRect( 0, 0, m_nBaseWidth, m_nBaseHeight, Qt::white );

    //    int nScaledW = int(m_nBaseWidth*m_fZoomRatioW+.5f);
    //    int nScaledH = int(m_nBaseHeight*m_fZoomRatioH+.5f);
    if( m_pImageDetection )
    {
        if( m_pImageDetection->width() != m_nBaseWidth || m_pImageDetection->height() != m_nBaseHeight )
        {
            delete m_pImageDetection;
            m_pImageDetection = new QPixmap( m_nBaseWidth, m_nBaseHeight );//nScaledW, nScaledH );
        }
    }
    else
    {
        m_pImageDetection = new QPixmap( m_nBaseWidth, m_nBaseHeight );//nScaledW, nScaledH );
    }

    m_pImageDetection->fill( Qt::transparent );

    if( m_pImageGraph ) delete m_pImageGraph;
    m_pImageGraph = new QPixmap( m_nBaseWidth, m_nBaseHeight );//nScaledW, nScaledH );
    QPainter pF( m_pImageGraph );
    pF.fillRect( 0, 0, m_nBaseWidth, m_nBaseHeight, Qt::white );//nScaledW, nScaledH );

    DrawGraph( pF );

    setUpdatesEnabled( true );
}

void QDetectionGraphView::DrawGraph( QPainter& dc )
{
    QRectF rectBody( 0.0, 0.0, (int)(m_nBaseWidth*m_fZoomRatioW+.5f)-1.f, (int)(m_nBaseHeight*m_fZoomRatioH+.5f)-1.f );

    dc.setPen( QColor(176,196,222) );
    dc.drawRect( QRectF(0.f, 0.f, (float)m_nBaseWidth-1.f, (float)m_nBaseHeight-1.f) );

    m_rectGraph = rectBody;
    m_rectGraph.adjust( 2.0, 1.0, -2.0, -1.0 );

    DrawIRD( dc, m_rectGraph );
    //DrawITD( g, m_rectGraph );

    DrawRange( dc, m_rectGraph );

    DrawGrid( dc, rectBody );
}

void QDetectionGraphView::SetDetectionRange( int nLeft, int nRight )
{
    m_nRangeLeft = nLeft;
    m_nRangeRight = nRight;
}

void QDetectionGraphView::DrawRange( QPainter& dc, const QRectF& rectGraph )
{
    dc.setRenderHint( QPainter::NonCosmeticDefaultPen );  //g.SetSmoothingMode( SmoothingModeNone );
    QPen RangePen( QColor(240, 80, 80) );
    RangePen.setStyle( Qt::DashDotLine );

    QPoint ptScroll = GetDeviceScrollPosition();

    int nLeft = m_nRangeLeft * m_nIRD / 0xffff;
    int nRight = m_nRangeRight * m_nIRD / 0xffff;

    float fHorzW = rectGraph.width() / m_nIRD;

    float fLeft = rectGraph.x() - ptScroll.x() + fHorzW*(nLeft-1);
    float fRight = rectGraph.x() - ptScroll.x() + fHorzW*(nRight-1);

    dc.setPen( RangePen );
    if( (m_nRangeLeft > 0) && fLeft >= 0.f )
    {
        dc.drawLine( fLeft, rectGraph.y(), fLeft, rectGraph.bottom() );
    }
    if( (m_nRangeRight < 0xffff) && fRight <= (float)m_nBaseWidth )
    {
        dc.drawLine( fRight, rectGraph.y(), fRight, rectGraph.bottom() );
    }
}

QPoint QDetectionGraphView::GetDeviceScrollPosition()
{
    QPoint pt(horizontalScrollBar()->value(), verticalScrollBar()->value());
    Q_ASSERT(pt.x() >= 0 && pt.y() >= 0);
    return pt;
}

void QDetectionGraphView::DrawITD( QPainter& dc, const QRectF& rectGraph )
{
    if( !m_pITD )
        return;

    QRectF rectView = rectGraph;

    float fHorzW = rectGraph.width() / m_nIRD;

    QPoint ptScroll = GetDeviceScrollPosition();

    QPointF pointX1, pointX2;
    QPainterPath PointPath;

    rectView.translate( -ptScroll );

    pointX1.setX( rectView.x() );
    pointX1.setY( rectView.bottom() );
    pointX2.setX( rectView.x() );
    pointX2.setY( rectView.bottom() );

    if( pointX2.x() < (int)(-fHorzW) )
    {
        pointX2.setX( 0 );
        pointX1.setX( 0 );
    }

    PointPath.moveTo( pointX1 ); PointPath.lineTo( pointX2 );

    int nMinI = int( (-fHorzW - rectView.x()) / fHorzW ) - 1;
    if( nMinI < 0 ) nMinI = 0;

    for( int i=nMinI ; i<m_nIRD-2 ; i++ )
    {
        uchar nITD1 = m_pITD[i] * m_nThreshold / 0x100;
        uchar nITD2 = m_pITD[i+1] * m_nThreshold / 0x100;
        pointX1.setX( i*fHorzW + rectView.x() );
        pointX1.setY( rectView.height() - (nITD1 * rectView.height() / 0xff) );
        pointX2.setX( i*fHorzW + rectView.x() );
        pointX2.setY( rectView.height() - (nITD2 * rectView.height() / 0xff) );

        if( pointX2.x() < (int)(-fHorzW) ) continue;
        if( pointX1.x() > m_nBaseWidth+fHorzW )
            break;

        PointPath.lineTo( pointX1 ); PointPath.lineTo( pointX2 );
    }

    pointX1.setX( (m_nIRD-2)*fHorzW + rectView.x() );
    pointX1.setY( rectView.bottom() );
    pointX2.setX( (m_nIRD-2)*fHorzW + rectView.x() );
    pointX2.setY( rectView.bottom() );

    if( pointX1.x() > m_nBaseWidth )
    {
        pointX2.setX( (float)m_nBaseWidth );
        pointX1.setY( (float)m_nBaseWidth );
    }
    PointPath.lineTo( pointX1 ); PointPath.lineTo( pointX2 );

    QPen LinePen( QColor(50, 0, 0, 0) );
    QBrush GraphBrush( QColor(255, 255, 255) );

    //g.SetClip( Rect(ptScroll.x-1, ptScroll.y-1, m_nBaseWidth+2, m_nBaseHeight+2) );

    PointPath.setFillRule( Qt::WindingFill );
    dc.setBrush( GraphBrush );
    dc.setPen( LinePen );
    dc.drawPath( PointPath );

    //g.ResetClip();
}

void QDetectionGraphView::DrawIRD( QPainter& dc, const QRectF& rectGraph )
{
    if( !m_pIRD )
        return;

    QRectF rectView = rectGraph;

    float fHorzW = rectView.width() / m_nIRD;

    QPoint ptScroll = GetDeviceScrollPosition();

    QPointF pointX1, pointX2;
    QPainterPath PointPath;

    rectView.translate( -ptScroll );

    pointX1.setX( rectView.x() );
    pointX1.setY( rectView.bottom() );
    pointX2.setX( rectView.x() );
    pointX2.setY( rectView.bottom() );
    if( pointX2.x() < (int)(-fHorzW) )
    {
        pointX2.setX( 0 );
        pointX1.setX( 0 );
    }
    PointPath.moveTo( pointX1 ); PointPath.lineTo( pointX2 );

    int nMinI = int( (-fHorzW - rectView.x()) / fHorzW ) - 1;
    if( nMinI < 0 ) nMinI = 0;

    for( int i=nMinI ; i<m_nIRD-2 ; i++ )
    {
        pointX1.setX( i*fHorzW + rectView.x() );
        pointX1.setY( rectView.bottom() - (m_pIRD[i] * rectView.height() / 0xff) );
        pointX2.setX( i*fHorzW + rectView.x() );
        pointX2.setY( rectView.bottom() - (m_pIRD[i+1] * rectView.height() / 0xff) );

        if( pointX2.x() < (int)(-fHorzW) ) continue;
        if( pointX1.x() > m_nBaseWidth+fHorzW )
            break;

        PointPath.lineTo( pointX1 ); PointPath.lineTo( pointX2 );
    }

    pointX1.setX( (m_nIRD-2)*fHorzW + rectView.x() );
    pointX1.setY( rectView.bottom() );
    pointX2.setX( (m_nIRD-2)*fHorzW + rectView.x() );
    pointX2.setY( rectView.bottom() );
    if( pointX1.x() > m_nBaseWidth )
    {
        pointX2.setX( (float)m_nBaseWidth );
        pointX1.setX( (float)m_nBaseWidth );
    }
    PointPath.lineTo( pointX1 ); PointPath.lineTo( pointX2 );

    QPen LinePen( QColor(100, 100, 100) );
    QBrush GraphBrush( QColor(160, 160, 160) );

    //g.SetClip( Rect(ptScroll.x-1, ptScroll.y-1, m_nBaseWidth+2, m_nBaseHeight+2) );

    PointPath.setFillRule( Qt::WindingFill );
    dc.setBrush( GraphBrush );
    dc.setPen( LinePen );
    dc.drawPath( PointPath );

    //g.ResetClip();
}

void QDetectionGraphView::DrawDTC( QPainter& dc, const QRectF& rectGraph, QPoint* pObj )
{
    if( m_nNumberOfDTC == 0 ) return;

    QPoint ptScroll = GetDeviceScrollPosition();

    float fHorzW = rectGraph.width() / m_nIRD;
    QRectF rectDetect;
    rectDetect.moveTop( rectGraph.y() );
    rectDetect.setHeight( rectGraph.height() );

    for( int i=0 ; i<m_nNumberOfDTC ; i++ )
    {
        if( pObj[i].x() != -1 )
        {
            rectDetect.moveLeft( pObj[i].x()*fHorzW + rectGraph.x() - ptScroll.x() );
            float fDW = (pObj[i].y()*fHorzW - pObj[i].x()*fHorzW);
            fDW < 1.f ? fDW = 1.f : 0;
            rectDetect.setWidth( fDW );

            if( rectDetect.x()+fHorzW < 0.f ) continue;
            if( rectDetect.x() > (float)m_nBaseWidth+fHorzW ) continue;

            //qDebug( "\\ DTC %d(%d) : %f %f %f %f", i, m_nNumberOfDTC, rectDetect.x(), rectDetect.y(), rectDetect.width(), rectDetect.height() );
            dc.fillRect( rectDetect, QColor(255,180,180, 128) );
            dc.setPen( QColor(255, 75, 75) );
            dc.drawRect( rectDetect );
        }
        else
        {
            break;
        }
    }
}

void QDetectionGraphView::SetCaption( QString& strLeft, QString& strRight )
{
    m_strCaptionLeft = strLeft;
    m_strCaptionRight = strRight;
}

void QDetectionGraphView::DrawGrid( QPainter& dc, const QRectF& rectBody )
{
    dc.setRenderHint( QPainter::NonCosmeticDefaultPen );  //g.SetSmoothingMode( SmoothingModeNone );

    QPen GridPen( QColor(245,245,245) );
    QPen ShadowPen( QColor(169,169,169) );
    GridPen.setStyle( Qt::DashLine );
    ShadowPen.setStyle( Qt::DashLine );

    float fGridH = rectBody.height() / 10.f;
    for( int i=1 ; i<10 ; i++ )
    {
        float fGridY = fGridH * i;
        dc.setPen( ShadowPen );
        dc.drawLine( 1.f, fGridY+1.5f, m_nBaseWidth-1.f, fGridY+1.5f );
        dc.setPen( GridPen );
        dc.drawLine( 0.f, fGridY, m_nBaseWidth-1.f, fGridY );
    }


    QFont ftCaption( font() );
#if defined(Q_OS_MAC) || defined(Q_OS_LINUX)
    ftCaption.setPointSize( ftCaption.pointSize()-2 );
    dc.setFont( ftCaption );
#endif

    QRectF rectCaption( rectBody.x()+5, rectBody.y()+5, 80.f, 40.f );

    QString strText( QString("Auto Offset : %1").arg(m_nAutoOffset) );
    QFontMetrics ftMetrics( ftCaption );
    int nW = ftMetrics.width( strText );
    int nH = ftMetrics.height();
    rectCaption.setWidth( (qreal)nW+5 );
    rectCaption.setHeight( (qreal)nH );

    dc.fillRect( rectCaption, Qt::white );
    dc.setPen( QColor(80,80,80) );
    dc.drawText( rectCaption, strText );

#if defined(Q_OS_MAC) || defined(Q_OS_LINUX)
    dc.setFont( font() );
#endif
/*
    // left / right caption
    FontFamily fntFamily( L"Arial" );
    Font fntCaption( &fntFamily, 10.f, FontStyleRegular, UnitPixel );

    StringFormat stringFormat;
    stringFormat.SetLineAlignment( StringAlignmentNear );
    stringFormat.SetAlignment( StringAlignmentNear );
    stringFormat.SetTrimming( StringTrimmingNone );
    RectF rectCaption( (float)rectBody.X+5, (float)rectBody.Y+5, 80.f, 40.f );
    RectF rectBB;
    g.MeasureString( m_strCaptionLeft, -1, &fntCaption, rectCaption, &stringFormat, &rectBB );
    g.FillRectangle( &SolidBrush( Color(255, 255, 255) ), rectBB );
    g.DrawString( m_strCaptionLeft, -1, &fntCaption, rectCaption, &stringFormat, &SolidBrush( Color(80, 80, 80) ) );

    rectCaption.X = (float)rectBody.GetRight() - 5 - 80;
    stringFormat.SetAlignment( StringAlignmentFar );
    g.MeasureString( m_strCaptionRight, -1, &fntCaption, rectCaption, &stringFormat, &rectBB );
    g.FillRectangle( &SolidBrush( Color(255, 255, 255) ), rectBB );
    g.DrawString( m_strCaptionRight, -1, &fntCaption, rectCaption, &stringFormat, &SolidBrush( Color(80, 80, 80) ) );*/
}

void QDetectionGraphView::ClearGraph()
{
    if( m_pIRD )
    {
        delete[] m_pIRD;
        m_pIRD = NULL;
    }

    if( m_pITD )
    {
        delete[] m_pITD;
        m_pITD = 0;
    }
    m_nIRD = 0;

    UpdateGraph();
}

void QDetectionGraphView::SetGraphData( int nIRD, uchar* pIRD, uchar* pITD )
{
    if( m_nIRD != nIRD )
    {
        m_nIRD = nIRD;
        if( m_pIRD ) delete[] m_pIRD;
        m_pIRD = new uchar[ m_nIRD ];
        if( m_pITD ) delete[] m_pITD;
        m_pITD = new uchar[ m_nIRD ];
    }

    memcpy( m_pIRD, pIRD, sizeof(uchar) * m_nIRD );
    memcpy( m_pITD, pITD, sizeof(uchar) * m_nIRD );
}

void QDetectionGraphView::UpdateDetectionData( QPoint* pObjs, int nNumberOfDTC )
{
    m_nNumberOfDTC = nNumberOfDTC;

    // XTODO(hckim):
    {
        QRect rcClient;
        rcClient.setLeft( 0 );
        rcClient.setTop( 0 );
        rcClient.setRight( rcClient.left() + m_nBaseWidth );//int(m_nBaseWidth*m_fZoomRatioW+.5f);
        rcClient.setBottom( rcClient.top() + m_nBaseHeight );//int(m_nBaseHeight*m_fZoomRatioH+.5f);

        if( m_pImageDetection &&
           ( m_pImageDetection->width() != rcClient.width() ||
            m_pImageDetection->height() != rcClient.height() ) )
        {
            delete m_pImageDetection;
            m_pImageDetection = NULL;
        }

        if( !m_pImageDetection )
        {
            m_pImageDetection = new QPixmap( rcClient.width(), rcClient.height() );
        }
        m_pImageDetection->fill( Qt::transparent );

        QPainter* pG = new QPainter( m_pImageDetection );   // ????????????
//        Graphics* pG = Graphics::FromImage( m_pImageDetection );
        if( pG )
        {
            //pG->setCompositionMode( QPainter::CompositionMode_DestinationIn );
            DrawDTC( *pG, m_rectGraph, pObjs );

            delete pG;
        }
    }

    viewport()->update();
}

void QDetectionGraphView::RedrawGraph()
{
    if( !m_bUpdateGraph )
    {
        UpdateGraph();
        m_bUpdateGraph = true;
    }
}

void QDetectionGraphView::wheelEvent(QWheelEvent *evt)
{
    QPoint pt( evt->pos() );
    if( evt->modifiers() == Qt::ControlModifier )
    {
        m_bStopUpdate = true;
        setUpdatesEnabled( false );
        // zoom !
        float fZoom = m_fZoomRatioW;
        if( evt->delta() > 0 )
            fZoom *= ZOOM_STEP;
        else
            fZoom /= ZOOM_STEP;

        ZoomTo( pt, fZoom, 1.f );
        setUpdatesEnabled( true );
        UpdateGraph();
        m_bStopUpdate = false;
    }
}

void QDetectionGraphView::GetScrollBarDimension( int& nSWW, int &nSWH )
{
    if( !horizontalScrollBar()->isVisible() )
        nSWW = 0;
    else
        nSWW = horizontalScrollBar()->width();    // ::GetSystemMetrics(SM_CXHSCROLL);

    if( !verticalScrollBar()->isVisible() )
        nSWH = 0;
    else
        nSWH = horizontalScrollBar()->height();   // ::GetSystemMetrics(SM_CYHSCROLL);

//    QSize szNSb, szRange;
//    QRect rcView( 0, 0, width()-1, height()-1 );
//    QPoint ptMove;

//    GetScrollBarState( QSize(rcView.width(), rcView.height()), szNSb, szRange, ptMove, false );
//    if( !szNSb.width() ) nSWW = 0; if( !szNSb.height() ) nSWH = 0;
}

void QDetectionGraphView::MousePan( QPoint& point )
{
    QPoint ptScroll = GetDeviceScrollPosition();
    QSize szNSb, szRange;
    //QPoint ptMoveT;

    QSize sizeTotal;
    sizeTotal.setWidth( int(m_nBaseWidth) );
    sizeTotal.setHeight( int(m_nBaseHeight) );

//    GetScrollBarState( sizeTotal, szNSb, szRange, ptMoveT, false );
//    if( szRange.width() < 0 ) szRange.setWidth( 0 );
//    if( szRange.height() < 0 ) szRange.setHeight( 0 );

    if( !horizontalScrollBar()->isVisible() )
        szRange.setWidth( 0 );
    else
        szRange.setWidth( horizontalScrollBar()->width() );    // ::GetSystemMetrics(SM_CXHSCROLL);

    if( !verticalScrollBar()->isVisible() )
        szRange.setHeight( 0 );
    else
        szRange.setHeight( horizontalScrollBar()->height() );

    if( szRange.width() == 0 && szRange.height() == 0 )
            return;

    QPoint ptMove( point );
    ptMove -= m_ptViewLastMouse;

    ptScroll -= ptMove;
    if( szNSb.width() )
        horizontalScrollBar()->setValue( ptScroll.x() );
    if( szNSb.height() )
        verticalScrollBar()->setValue( ptScroll.y() );

    m_ptViewLastMouse = point;

    verticalScrollBar()->show();
    horizontalScrollBar()->show();
    verticalScrollBar()->update();      //UpdateBars();
    horizontalScrollBar()->update();
    UpdateGraph();
    viewport()->update();
}

void QDetectionGraphView::mouseMoveEvent(QMouseEvent *evt)
{
    if( evt->buttons() == Qt::LeftButton )
    {
        if( m_ptViewLastMouse.x() == -1 &&  m_ptViewLastMouse.y() == -1 )
        {
            QScrollArea::mouseMoveEvent(evt);
            return;
        }

        QPoint pPoint = evt->pos();
        MousePan( pPoint );
    }
    QScrollArea::mouseMoveEvent(evt);
}

void QDetectionGraphView::mousePressEvent(QMouseEvent *evt)
{
    if( evt->button() == Qt::LeftButton )
    {
//        if( ::GetCapture() != m_hWnd ) ::SetCapture( m_hWnd );

        m_ptViewLastMouse = evt->pos();

        //int nSWW, nSWH;
        //GetScrollBarDimension( nSWW, nSWH );
        //if( nSWW != 0 || nSWH != 0 )
        //	m_bStopUpdate = TRUE;
    }
    QScrollArea::mousePressEvent(evt);
}

void QDetectionGraphView::ZoomIn()
{
    m_bStopUpdate = true;
    setUpdatesEnabled( false );

    float fZoomW = m_fZoomRatioW;
    fZoomW *= ZOOM_STEP;

    QRect rc( 0, 0, width()-1, height()-1 );

    ZoomTo( rc.center(), fZoomW, 1.f );

    setUpdatesEnabled( true );

    UpdateGraph();

    m_bStopUpdate = false;
}

void QDetectionGraphView::ZoomOut()
{
    m_bStopUpdate = true;
    setUpdatesEnabled( false );

    float fZoomW = m_fZoomRatioW;
    fZoomW /= ZOOM_STEP;

    QRect rc( 0, 0, width()-1, height()-1 );

    ZoomTo( rc.center(), fZoomW, 1.f );

    setUpdatesEnabled( true );

    UpdateGraph();

    m_bStopUpdate = false;
}

void QDetectionGraphView::ResetZoom()
{
    m_bStopUpdate = true;
    setUpdatesEnabled( false );
    SetZoomRatio( 1.f, 1.f, false );
    setUpdatesEnabled( true );
    UpdateGraph();
    m_bStopUpdate = false;
}

void QDetectionGraphView::DisplayAutoOffset(int nOffset)
{
    m_nAutoOffset = nOffset;
}

void QDetectionGraphView::mouseReleaseEvent(QMouseEvent *evt)
{
    if( evt->button() == Qt::LeftButton )
    {
//        if ( ::GetCapture() == m_hWnd )
//            ::ReleaseCapture();

        m_ptViewLastMouse = evt->pos();
        m_ptViewLastMouse.setX( -1 );
        m_ptViewLastMouse.setY( -1 );

        //m_bStopUpdate = FALSE;
    }
}

#ifndef INT_MAX
#define INT_MAX       2147483647    /* maximum (signed) int value */
#endif
void QDetectionGraphView::OnVScrollActionTrig(int nAction)
{
    int y = verticalScrollBar()->value();
    int yOrig = y;

    switch( nAction )
    {
    case QAbstractSlider::SliderNoAction:
        return;
    case QAbstractSlider::SliderSingleStepAdd:
        y += verticalScrollBar()->singleStep();
        break;
    case QAbstractSlider::SliderSingleStepSub:
        y -= verticalScrollBar()->singleStep();
        break;
    case QAbstractSlider::SliderPageStepAdd:
        y += verticalScrollBar()->pageStep();
        break;
    case QAbstractSlider::SliderPageStepSub:
        y -= verticalScrollBar()->pageStep();
        break;
    case QAbstractSlider::SliderToMinimum:
        y = 0;
        break;
    case QAbstractSlider::SliderToMaximum:
        y = INT_MAX;
        break;
    case QAbstractSlider::SliderMove:
        break;
    default:
        return;
    }

    int nVScrollSize = y - yOrig;

    int nVOrig, nVCurVal;
    nVOrig = nVCurVal = verticalScrollBar()->value();
    int yMax = verticalScrollBar()->maximum();
    nVCurVal += nVScrollSize;
    if (nVCurVal < 0)
        nVCurVal = 0;
    else if (nVCurVal > yMax)
        nVCurVal = yMax;

    if (nVCurVal != nVOrig)
        verticalScrollBar()->setValue( nVCurVal );

    UpdateGraph();
    viewport()->update();
}

void QDetectionGraphView::OnHScrollActionTrig(int nAction)
{
    int x = horizontalScrollBar()->value();
    int xOrig = x;

    switch( nAction )
    {
    case QAbstractSlider::SliderNoAction:
        return;
    case QAbstractSlider::SliderSingleStepAdd:
        x += horizontalScrollBar()->singleStep();
        break;
    case QAbstractSlider::SliderSingleStepSub:
        x -= horizontalScrollBar()->singleStep();
        break;
    case QAbstractSlider::SliderPageStepAdd:
        x += horizontalScrollBar()->pageStep();
        break;
    case QAbstractSlider::SliderPageStepSub:
        x -= horizontalScrollBar()->pageStep();
        break;
    case QAbstractSlider::SliderToMinimum:
        x = 0;
        break;
    case QAbstractSlider::SliderToMaximum:
        x = INT_MAX;
        break;
    case QAbstractSlider::SliderMove:
        break;
    default:
        return;
    }

    int nXScrollSize = x - xOrig;

    int nHOrig, nHCurVal;
    nHOrig = nHCurVal = horizontalScrollBar()->value();
    int xMax = horizontalScrollBar()->maximum();
    nHCurVal += nXScrollSize;
    if (nHCurVal < 0)
        nHCurVal = 0;
    else if (nHCurVal > xMax)
        nHCurVal = xMax;

    qDebug( "Move Scroll Value : %d", nHCurVal );
    if ( nHCurVal != nHOrig)
        horizontalScrollBar()->setValue( nHCurVal );

    UpdateGraph();
    viewport()->update();
}

void QDetectionGraphView::ZoomTo( QPoint ptScreen, float fZoomW, float fZoomH )
{
    QRect rcClient ( 0, 0, width()-1, height()-1 );
    QPoint ptCenter( rcClient.center() );

    setUpdatesEnabled( false );

    QPoint ptScrl( GetDeviceScrollPosition() );
    float fCenterX = float(ptCenter.x() + ptScrl.x()) / m_fZoomRatioW;
    float fCenterY = float(ptCenter.y() + ptScrl.y()) / m_fZoomRatioH;

    float fOldScrnX = float(ptScreen.x() + ptScrl.x()) / m_fZoomRatioW;
    float fOldScrnY = float(ptScreen.y() + ptScrl.y()) / m_fZoomRatioH;

    float fOldX = float(ptScreen.x() - ptCenter.x());
    float fOldY = float(ptScreen.y() - ptCenter.y());

    SetZoomRatio( fZoomW, fZoomH, false );

    float fNewCenterX = fCenterX * m_fZoomRatioW;
    float fNewCenterY = fCenterY * m_fZoomRatioH;

    float fNewScrnX = fOldScrnX * m_fZoomRatioW;
    float fNewScrnY = fOldScrnY * m_fZoomRatioH;

    float fNewX = fNewScrnX - fNewCenterX;
    float fNewY = fNewScrnY - fNewCenterY;

    float fMoveX = fNewCenterX - (fOldX - fNewX);
    float fMoveY = fNewCenterY - (fOldY - fNewY);

//    qDebug( "MoveTo : %f, %f / NewCenter : %f, %f / Ratio : %f, %f / ptCenter : %d, %d / ptScrl : %d, %d", fMoveX, fMoveY, fNewCenterX, fNewCenterY, m_fZoomRatioW, m_fZoomRatioH, ptCenter.x(), ptCenter.y(), ptScrl.x(), ptScrl.y() );
    MoveTo( QPoint((int)(fMoveX+.5f), (int)(fMoveY+.5f)) );

    setUpdatesEnabled( true );
}

void QDetectionGraphView::MoveTo( QPoint ptGraph )
{
    QRect rcClient( 0, 0, width()-1, height()-1 );
    int nWidth = int(rcClient.width()-1);
    int nHeight = int(rcClient.height()-1);

    int nHalfWidth = nWidth >> 1;
    int nHalfHeight = nHeight >> 1;
    QPoint ptDevice( ptGraph );

    QSize szScroll( int(m_nBaseWidth * m_fZoomRatioW + .5f), int(m_nBaseHeight * m_fZoomRatioH + .5f) );

    int nScrollX = ptDevice.x() - nHalfWidth;
    int nScrollY = ptDevice.y() - nHalfHeight;

    int nAvaliableScrollX = szScroll.width() - nWidth;
    int nAvaliableScrollY = szScroll.height() - nHeight;

    nScrollX > nAvaliableScrollX ? nScrollX = nAvaliableScrollX : 0;
    nScrollX < 0 ? nScrollX = 0 : 0;
    nScrollY > nAvaliableScrollY ? nScrollY = nAvaliableScrollY : 0;
    nScrollY < 0 ? nScrollY = 0 : 0;

    horizontalScrollBar()->setValue( nScrollX );
    verticalScrollBar()->setValue( nScrollY );
}

void QDetectionGraphView::SetZoomRatio( float fZoomW, float fZoomH, bool bInvalidate/*=TRUE*/ )
{
//    ASSERT( fZoomH == 1.f );
    if( fZoomW > 1.f )
    {
        int nSWH = horizontalScrollBar()->height();//   ::GetSystemMetrics(SM_CYHSCROLL);
        fZoomH = (float)(m_nBaseHeight-nSWH-1) / m_nBaseHeight;
    }
    // to limit zoom-range
    fZoomW = CalcValidZoom( fZoomW );
    //fZoomH = CalcValidZoom( fZoomH );

    {
        //CRect rc;
        //GetClientRect( rc );

        //SetRedraw( FALSE );
        QSize sizeTotal( int(m_nBaseWidth * fZoomW + .5f), int(m_nBaseHeight * fZoomH + .5f) );
        qDebug( "Scroll Size: (%dx%d) %dx%d\r\n", m_nBaseWidth, m_nBaseHeight, sizeTotal.width(), sizeTotal.height() );

        horizontalScrollBar()->setPageStep( viewport()->width() );
        horizontalScrollBar()->setRange( 0, sizeTotal.width()-viewport()->width() );

        if( fZoomW == 1.f && fZoomH == 1.f )
            horizontalScrollBar()->setRange( 0, 0 );

        //SetRedraw( TRUE );
    }

    int nSWW, nSWH;
    GetScrollBarDimension( nSWW, nSWH );

    m_fZoomRatioW = fZoomW;
    m_fZoomRatioH = 1.f;//fZoomH;

    if( bInvalidate ) viewport()->update();
}

void QDetectionGraphView::paintEvent(QPaintEvent */*evt*/)
{
    QPainter dc( viewport() );

    QRect rcClient( 0, 0, width()-1, height()-1 );

    if( m_nBaseWidth == 0 )
    {
        m_nBaseWidth = rcClient.width();
    }
    if( m_nBaseHeight == 0 )
    {
        m_nBaseHeight = rcClient.height();
    }

//    int nGW = int(m_nBaseWidth*m_fZoomRatioW+.5f);
//    int nGH = int(m_nBaseHeight*m_fZoomRatioH+.5f);

    QRect rcCanvas(0, 0, m_nBaseWidth, m_nBaseHeight);

//    QPoint ptScroll( GetDeviceScrollPosition() );

    QPainter gCanvas( m_pImageCanvas );
#if 0
    gCanvas.drawImage( QRect(rcCanvas.left(), rcCanvas.top(), rcCanvas.width(), rcCanvas.height()),
                       m_pImageGraph->toImage(),
                       QRect(rcCanvas.left()/*+ptScroll.x*/, rcCanvas.top()/*+ptScroll.y*/, rcCanvas.width(), rcCanvas.height() ) );
#else
    gCanvas.drawPixmap( rcCanvas,
                        *m_pImageGraph,
                        rcCanvas );
#endif

    if( m_nNumberOfDTC != 0 )
    {
#if 0
        gCanvas.drawImage( QRect(rcCanvas.left(), rcCanvas.top(), rcCanvas.width(), rcCanvas.height()),
                           m_pImageDetection->toImage(),
                           QRect(rcCanvas.left()/*+ptScroll.x*/, rcCanvas.top()/*+ptScroll.y*/, rcCanvas.width(), rcCanvas.height()) );
#else
        gCanvas.drawPixmap( rcCanvas,
                           *m_pImageDetection,
                           rcCanvas );
#endif
    }

    //dc.drawImage( QRect(rcClient.left(), rcClient.top(), rcClient.width(), rcClient.height()), m_pImageCanvas->toImage()  );
    dc.drawPixmap( rcClient, *m_pImageCanvas  );
}

void QDetectionGraphView::OnTimer( int /*nTimerId*/ )
{
    if( !m_bStopUpdate && m_bUpdateGraph )
    {
        m_bUpdateGraph = false;

        viewport()->update();
    }

    //QScrollArea::timerEvent(evt);
}

void QDetectionGraphView::UpdateGraph()
{
    QRect rcClient( 0, 0, width()-1, height()-1 );

    if( m_nBaseWidth == 0 )
        m_nBaseWidth = rcClient.width();
    if( m_nBaseHeight == 0 )
        m_nBaseHeight = rcClient.height();

    rcClient.moveLeft( 0 );
    rcClient.moveTop( 0 );
    //rcClient.right = rcClient.left + int(m_nBaseWidth*m_fZoomRatioW+.5f);
    //rcClient.bottom = rcClient.top + int(m_nBaseHeight*m_fZoomRatioH+.5f);

    if( m_pImageGraph &&
        (m_pImageGraph->width() != rcClient.width() || m_pImageGraph->height() != rcClient.height()) )
    {
        qDebug( "Graph( zoom: %.4f, %.4f, (%dx%d) width: %d, height: %d )\r\n", m_fZoomRatioW, m_fZoomRatioH, m_nBaseWidth, m_nBaseHeight, rcClient.width(), rcClient.height() );

        delete m_pImageGraph;
        m_pImageGraph = NULL;
    }

    if( !m_pImageGraph )
    {
        m_pImageGraph = new QPixmap( rcClient.width(), rcClient.height() );
        //m_pImageGraph->fill( Qt::transparent );
    }

    QPainter  g( m_pImageGraph );
    g.fillRect( rcClient, Qt::white );

    DrawGraph( g );
}
