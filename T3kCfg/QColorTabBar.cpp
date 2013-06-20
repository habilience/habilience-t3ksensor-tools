#include "QColorTabBar.h"

#include <QPainter>
#include <QTimerEvent>

QColorTabBar::QColorTabBar(QWidget *parent) :
    QTabBar(parent)
{
    m_nBlinkIndex = -1;
    m_bBlinkFalg = false;
    m_nTimerBlink = 0;
}

QColorTabBar::~QColorTabBar()
{
    if( m_nTimerBlink )
        killTimer( m_nTimerBlink );
}

void QColorTabBar::BlinkTab(int nIndex)
{
    m_nBlinkIndex = nIndex;

    if( !m_nTimerBlink )
        m_nTimerBlink = startTimer( 500 );
}

void QColorTabBar::paintEvent(QPaintEvent *evt)
{
    QTabBar::paintEvent(evt);

    QPainter dc;
    dc.begin( this );

    if( m_nBlinkIndex >= 0 && m_bBlinkFalg )
    {
        QRect rcTab( tabRect( m_nBlinkIndex ) );
        dc.fillRect( rcTab, QColor(240,240,0,128) );
    }

    dc.end();
}

void QColorTabBar::timerEvent(QTimerEvent *evt)
{
    if( evt->timerId() == m_nTimerBlink )
    {
        m_bBlinkFalg = !m_bBlinkFalg;

        if( m_nBlinkIndex < 0)
        {
            killTimer( m_nTimerBlink );
            m_nTimerBlink = 0;
            m_bBlinkFalg = false;
        }

        QRect rcTab( tabRect( m_nBlinkIndex ) );
        update( rcTab );
    }

    QTabBar::timerEvent(evt);
}
