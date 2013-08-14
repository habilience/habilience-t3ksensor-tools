#include "QGraphSensitivityCtrl.h"

#include <QPainter>

QGraphSensitivityCtrl::QGraphSensitivityCtrl(QWidget *parent) :
    QLabel(parent)
{
    m_nStepCount = 30;
    m_nStep = 1;
    m_nColorLevel = 0;
}

void QGraphSensitivityCtrl::setColorLevel( int level )
{
    level < 0 ? level = 0 : 0;
    level > MAX_COLOR_LEVEL ? level = MAX_COLOR_LEVEL : 0;
    if( m_nColorLevel != level )
        update();
    m_nColorLevel = level;
}

void QGraphSensitivityCtrl::setStep( int step )
{
    if (step != m_nStep)
    {
        m_nStep = step;
        update();
    }
}

void QGraphSensitivityCtrl::setRange( int max )
{
    m_nStepCount = max;
}

int QGraphSensitivityCtrl::getRange() const
{
    return m_nStepCount;
}

void QGraphSensitivityCtrl::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.save();
    QRect rcBody(0, 0, width(), height());

    p.setRenderHint(QPainter::Antialiasing);

    p.fillRect( rcBody, QColor(245,245,245) );

    rcBody.adjust( 2, 2, -2, -2 );

    p.setPen( QPen(QColor(160, 160, 160, 100), 2) );
    p.drawRect( rcBody );
    p.setPen( QPen(QColor(160, 160, 160), 1) );
    p.drawRect( rcBody );

    rcBody.adjust( 1, 2, -1, 0 );

    float fItemH = (float)rcBody.height() / m_nStepCount;
    QRectF rcItem;

    rcItem.setX( rcBody.x() );
    rcItem.setWidth( rcBody.width() );

    int nLevel = m_nColorLevel;
    for (int i=0 ; i<m_nStepCount ; i++ )
    {
        rcItem.setY( rcBody.bottom() - fItemH * (i+1) );
        rcItem.setHeight( fItemH );

        if (i > m_nStep || !isEnabled())
            drawItem( p, rcItem, -1 );
        else
            drawItem( p, rcItem, nLevel );
    }

    p.restore();
}

void QGraphSensitivityCtrl::drawItem(QPainter &p, QRectF rcItem, int nLevel)
{
    rcItem.adjust( 0.5, 0.5, -0.5, -0.5 );

    int nR=0, nG=0, nB=0;
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

    p.setBrush( QBrush(QColor(nR, nG, nB, 200)) );
    p.setPen( Qt::NoPen );
    p.drawRoundRect( rcItem, 2, 2 );

    QRectF rcHighlight = rcItem;
    rcHighlight.setHeight(rcItem.height()/2);
    rcHighlight.adjust( 1.0, 0.0, -1.0, 0.0 );

    p.setBrush( QBrush(QColor(255, 255, 255, 50)) );
    p.drawRoundRect( rcHighlight, 2, 2 );

    p.setPen( QPen( QBrush(QColor(0,0,0,60)), 1.5f ) );
    p.drawRoundRect( rcItem, 2, 2 );
}
