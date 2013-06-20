#include "QSensorSensGaugeWidget.h"

#include <QPainter>

QSensorSensGaugeWidget::QSensorSensGaugeWidget(QWidget *parent) :
    QWidget(parent)
{
    m_nStepCount = 30;
    m_nStep = 0;
    m_nColorLevel = 0;
}

void QSensorSensGaugeWidget::paintEvent(QPaintEvent */*evt*/)
{
    QPainter dc;
    dc.begin( this );

    dc.setRenderHint( QPainter::Antialiasing );

    QRectF rectBody( 0, 0, width()-1, height()-1 );

    dc.fillRect( rectBody, QColor(245, 245, 245) );

    rectBody.adjust( 2.0, 2.0, -2.0, -2.0 );
    QPen RectPen( QColor(160, 160, 160, 100) );
    RectPen.setWidth( 2 );
    dc.setPen( RectPen );
    dc.drawRect( rectBody );
    RectPen.setColor( QColor(160,160,160) );
    RectPen.setWidth( 1 );
    dc.setPen( RectPen );
    dc.drawRect( rectBody );

    rectBody.adjust( 1.0, 2.0, -1.0, -2.0 );

    float fItemH = rectBody.height() / m_nStepCount;
    QRectF rectItem;

    rectItem.moveLeft( rectBody.x() );
    rectItem.setHeight( fItemH );
    rectItem.setWidth( rectBody.width() );

    int nLevel = m_nColorLevel;
    for( int i=0 ; i<m_nStepCount ; i++ )
    {
        rectItem.moveTop( rectBody.bottom() - fItemH*(i+1) );

        if( i > m_nStep )
        {
            DrawItem( dc, rectItem, -1 );
        }
        else
        {
            DrawItem( dc, rectItem, nLevel );
        }
    }

    dc.end();
}

void QSensorSensGaugeWidget::SetStep( int nStep )
{
    if( nStep != m_nStep )
    {
        m_nStep = nStep;
        update();
    }
}

QPainterPath QSensorSensGaugeWidget::CreateRoundRect( const QRect& rect, int nRadius )
{
    //nRadius = qMin(rect.width(), rect.height()) / 2;
    int nDiameter = nRadius << 1;

    int x1, y1, x2, y2;
    rect.getCoords(&x1, &y1, &x2, &y2);

    QRect arcRect( x1, y1, nDiameter, nDiameter );

    QPainterPath path;
    path.moveTo(x1, y1+nRadius);
    path.arcTo(arcRect, 180.0, -90.0);
    path.lineTo(x2-nRadius,y1);
    arcRect.moveTo( x2-nDiameter, y1);
    path.arcTo(arcRect, 90.0, -90.0);
    path.lineTo(x2,y2-nRadius);
    arcRect.moveTo(x2-nDiameter, y2-nDiameter);
    path.arcTo(arcRect, 0.0, -90.0);
    path.lineTo(x1+nRadius,y2);
    arcRect.moveTo(x1,y2-nDiameter);
    path.arcTo(arcRect, -90.0, -90.0);
    path.lineTo(x1, y1+nRadius);

    path.closeSubpath();

    return path;
}

QPainterPath QSensorSensGaugeWidget::CreateRoundRect( const QRectF& rect, int nRadiusLT, int nRadiusRT, int nRadiusRB, int nRadiusLB )
{
    QPainterPath roundRect;
    if( nRadiusLT == 0 )
    {
        roundRect.moveTo( rect.left(), rect.top() );
        roundRect.lineTo( rect.right() - (nRadiusRT<<1), rect.top() );
    }
    else
        roundRect.arcTo( QRectF(rect.left(), rect.top(), (qreal)(nRadiusLT<<1), (qreal)(nRadiusLT<<1)), 180, 90 );

    if( nRadiusRT == 0 )
    {
        roundRect.lineTo( rect.right(), rect.top() );
        roundRect.lineTo( rect.right(), rect.bottom()-(float)(nRadiusRB<<1) );
    }
    else
        roundRect.arcTo( QRectF(rect.right() - (float)(nRadiusRT<<1), rect.top(), (float)(nRadiusRT<<1), (float)(nRadiusRT<<1)), 270, 90 );

    if( nRadiusRB == 0 )
    {
        roundRect.lineTo( rect.right(), rect.bottom() );
        roundRect.lineTo( rect.left()-(float)(nRadiusLB<<1), rect.bottom() );
    }
    else
        roundRect.arcTo( QRectF(rect.right() - (float)(nRadiusRB<<1), rect.bottom() -(float)(nRadiusRB<<1), (float)(nRadiusRB<<1), (float)(nRadiusRB<<1)), 0, 90 );

    if( nRadiusLB == 0 )
    {
        roundRect.lineTo( rect.left(), rect.bottom() );
        roundRect.lineTo( rect.left(), rect.top()-(float)(nRadiusLT<<1) );
    }
    else
        roundRect.arcTo( QRectF(rect.left(), rect.bottom() -(float)(nRadiusLB<<1), (float)(nRadiusLB<<1), (float)(nRadiusLB<<1)), 90, 90 );

    roundRect.closeSubpath();

    return roundRect;
}

void QSensorSensGaugeWidget::DrawItem( QPainter& pDC, QRectF rectItem, int nLevel )
{
    rectItem.adjust( 1.0, 0.5, -1.0, -0.5 );

    QPainterPath pItemPath = CreateRoundRect( rectItem.toRect(), 2 );
    pDC.drawPath( pItemPath );

    QRectF rectH = rectItem;
    rectH.setHeight( rectH.height() / 2.f );
    rectH.adjust( 1.0, 0.0, -1.0, 0.0 );
    QPainterPath pItemHighlightPath = CreateRoundRect( rectH.toRect(), 2 );

    int nR = 0, nG = 0, nB = 0;
    switch( nLevel )
    {
    case 3:
        nR = 10;
        nG = 220;
        nB = 10;
        break;
    case 2:
        nR = 100;
        nG = 220;
        nB = 10;
        break;
    case 1:
        nR = 220;
        nG = 100;
        nB = 10;
        break;
    case 0:
        nR = 220;
        nG = 10;
        nB = 10;
        break;
    case -1:
        nR = 220;
        nG = 220;
        nB = 220;
        break;
    }

    pDC.fillPath( pItemPath, QBrush( QColor(nR, nG, nB, 200) ) );
    pDC.fillPath( pItemHighlightPath, QBrush( QColor(255, 255, 255, 50) ) );

    QPen PathPen( QColor(0,0,0,60) );
    PathPen.setWidthF( 1.5 );
    pDC.drawPath( pItemPath );
}
