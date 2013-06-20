#include "QLegendWidget.h"

#include <QPainter>

QLegendWidget::QLegendWidget(QWidget *parent) :
    QWidget(parent)
{
    setFont( parent->font() );
}

void QLegendWidget::paintEvent(QPaintEvent */*evt*/)
{
    QPainter dc(this);
    QRect rcClient( 0, 0, width()-1, height()-1 );
    dc.setPen( QColor(100,100,100,240) );
    dc.setFont( font() );
    dc.setRenderHint( QPainter::TextAntialiasing );

    dc.drawText( rcClient, Qt::AlignVCenter | Qt::AlignRight, m_strText );
}
