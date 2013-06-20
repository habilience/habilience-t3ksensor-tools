#include "QRDisableScreenWidget.h"

#include <QPainter>

QRDisableScreenWidget::QRDisableScreenWidget(QWidget *parent) :
    QWidget(parent)
{
    setFont( parent->font() );

    //setAttribute( Qt::WA_NoSystemBackground );

    setFixedSize( 620, 440 );
    setWindowOpacity( 0.0 );
}

void QRDisableScreenWidget::paintEvent(QPaintEvent *)
{
    QPainter dc;
    dc.begin( this );

    QRect rcClient( 0, 0, width(), height() );

    dc.fillRect( rcClient, QColor(0,0,0 ,100) );
    QFont ft( font() );
    ft.setPointSize( 20 );
    dc.setFont( ft );
    dc.drawText( 0, 370, width(), 20, Qt::AlignCenter, "Remote..." );

    dc.end();
}
