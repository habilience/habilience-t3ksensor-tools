#include "QUnderlineTitle.h"

#include <QPainter>

QUnderlineTitle::QUnderlineTitle(QWidget *parent) :
    QLabel(parent)
{
}

void QUnderlineTitle::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    p.save();
    QRect rcBody(0, 0, width()-1, height()-1);

    QString strText = text();


    QRect rcIcon(0,0,0,0);
    QRect rcText = rcBody;
    if (!m_imgIcon.isNull())
    {
        rcIcon.setLeft(rcText.x());
        rcIcon.setTop(rcText.y() + (rcText.height()-m_imgIcon.height())/2);
        const int nOffsetX = 4;
        rcIcon.setWidth(m_imgIcon.width());
        rcIcon.setHeight(m_imgIcon.height());

        p.drawImage( rcIcon, m_imgIcon );
        rcIcon.setWidth(12);
        rcIcon.setHeight(12);

        rcText.setLeft( rcIcon.x() + rcIcon.width() + nOffsetX );
        rcText.setWidth( rcText.width() - (rcIcon.x() + rcIcon.width() + nOffsetX) );
    }

    QFont fntText = font();
    fntText.setBold(true);

    p.setFont(fntText);

    if (isEnabled())
        p.setPen(QColor(55, 100, 160));
    else
        p.setPen(QColor(100, 100, 100));
    int flags = Qt::AlignLeft|Qt::AlignVCenter|Qt::TextSingleLine;
    p.drawText( rcText, flags, strText );
    QRect rcBB;
    p.drawText( rcText, flags, strText, &rcBB );

    QRect rcUnderLine;
    rcUnderLine.setLeft( rcBody.x() + rcIcon.x() + rcIcon.width() + (m_imgIcon.isNull() ? 0 : 5) );
    rcUnderLine.setTop( rcBB.bottom() + 1 );
    rcUnderLine.setWidth( rcBody.width() );
    rcUnderLine.setHeight( 2 );

    p.fillRect( rcUnderLine, isEnabled() ? QColor(0x99, 0xBB, 0xE8) : QColor(0xBE, 0xBE, 0xBE) );

    p.restore();

}

void QUnderlineTitle::setIconImage(const QString& strIconPath)
{
    m_imgIcon = QImage(strIconPath);
}
