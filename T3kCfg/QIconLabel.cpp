#include "QIconLabel.h"

#include <QPainter>

QIconLabel::QIconLabel(QWidget *parent) :
    QWidget(parent)
{
}

void QIconLabel::SetIconImage(QString strImagePathName)
{
    m_strImagePathName = strImagePathName;
    update();
}

void QIconLabel::paintEvent(QPaintEvent */*evt*/)
{
    QPainter dc;
    dc.begin( this );

    dc.fillRect( 0, 0,  width()-1, height()-1, QColor(255,255,255) );
    dc.drawImage( 0, 0, QImage( m_strImagePathName ) );

    dc.end();
}
