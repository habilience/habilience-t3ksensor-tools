#include "QSimpleLed.h"

#include <QPainter>


QSimpleLed::QSimpleLed(QWidget *parent) :
    QLabel(parent)
{
    m_eLEDStatus = SDisconnect;
    m_pixImage.load( ":/T3kCfgRes/resources/PNG_NETWORK_DISCONNECT.png", "PNG" );
}

void QSimpleLed::SetStatus(eLEDStatus eStatus)
{
    switch( eStatus )
    {
    case SDisconnect:
    default:
        m_pixImage.load( ":/T3kCfgRes/resources/PNG_NETWORK_DISCONNECT.png", "PNG" );
        break;
    case SConnectOFF:
        m_pixImage.load( ":/T3kCfgRes/resources/PNG_NETWORK_OFF.png", "PNG" );
        break;
    case SConnectON:
        m_pixImage.load( ":/T3kCfgRes/resources/PNG_NETWORK_ON.png", "PNG" );
        break;
    }

    update();
}

void QSimpleLed::paintEvent(QPaintEvent *)
{
    QPainter dc;
    dc.begin( this );

    QRect rcClient( 0, 0, width(), height() );

    dc.drawPixmap( rcClient, m_pixImage );

    dc.end();
}
