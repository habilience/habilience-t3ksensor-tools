#include "QSimpleProgressWidget.h"

#include <QPainter>

QSimpleProgressWidget::QSimpleProgressWidget(QWidget *parent) :
    QProgressBar(parent)
{
    m_nMax = 100;
    m_nMin = 0;
    m_nPos = 0;
}

void QSimpleProgressWidget::SetPos( int nPos )
{
    if( nPos < m_nMin ) nPos = m_nMin;
    if( nPos > m_nMax ) nPos = m_nMax;
    m_nPos = nPos;
    update();
}

void QSimpleProgressWidget::paintEvent(QPaintEvent */*evt*/)
{
    QPainter dc;
    dc.begin( this );

    dc.setRenderHint( QPainter::Antialiasing );

    QRectF rectBody( 0, 0, width()-1, height()-1 );

//    QRectF rectBody( (float)rcClient.left, (float)rcClient.top, (float)rcClient.Width()-1.f, (float)rcClient.Height()-1.f );

    dc.fillRect( rectBody, QColor(245,245,245) );

    rectBody.adjust( 2.0,2.0,-2.0,-2.0 );
    if( m_nMax == m_nPos )
    {
        QPen drawPen( m_nMax == m_nPos ? QColor(60, 84, 109, 142) : QColor(100, 160, 160, 160) );
        drawPen.setWidthF( 2.0 );
        dc.setPen( drawPen );
        dc.drawRect( rectBody );
    }
    dc.setPen( QColor(160,160,160) );
    dc.drawRect( rectBody );

    rectBody.adjust( 1.0,1.0,-2.0,-2.0 );

    QRectF rectProgress;
    rectProgress = rectBody;
    rectProgress.setWidth( (float)(m_nPos - m_nMin) * rectBody.width() / (m_nMax - m_nMin) );

    dc.fillRect( rectProgress, m_nMax == m_nPos ? QColor(69, 114, 207) : QColor(84, 109, 142) );

    dc.end();
}
