#include "UnderlineLabel.h"

#include <QPainter>
#include <QApplication>

QUnderlineLabel::QUnderlineLabel(QWidget *parent) :
    QLabel(parent)
{
    setGeometry( 0, 0, width(), height() );

    m_nMargin = 0;
}

void QUnderlineLabel::SetIconImage(const QString &strPathName)
{
    m_strIconPathName = strPathName;
    if( m_strIconPathName.size() )
        m_nMargin = 20;
}

void QUnderlineLabel::SetIconImage(QPixmap& pxIcon)
{
    if( pxIcon.isNull() ) return;
    m_pxIcon = pxIcon.copy();
    m_nMargin = 20;
}

void QUnderlineLabel::paintEvent(QPaintEvent */*event*/)
{
    QPainter dc(this);

    int nW = width();
    int nH = height();

    QFont ftText;
    ftText.setFamily( font().family() );
    ftText.setBold( true );
    ftText.setPixelSize( nH-6 );
    dc.setFont( ftText );

    bool bR2L = QApplication::layoutDirection() == Qt::RightToLeft;
    int nTitleSP = bR2L ? -m_nMargin : m_nMargin;
    int nIconSP = bR2L ? nW-m_nMargin : 0;
    if( bR2L ) nW -= 5;
    dc.setPen( QColor(55, 100, 160) );
    dc.setRenderHint( QPainter::TextAntialiasing );
    dc.drawText( nTitleSP, 0, nW, nH-6, Qt::AlignLeft | Qt::AlignBottom, text() );

    QPen underLine( Qt::SolidLine );
    underLine.setWidth( 2 );
    underLine.setColor( QColor(153,187,232) );
    dc.setPen( underLine );

    dc.drawLine( QPoint( nTitleSP, nH-1 ), QPoint( nW+nTitleSP, nH-1 ) );

    if( !m_nMargin ) return;

    if( !m_strIconPathName.size() )
    {
        if( !m_pxIcon.isNull() )
            dc.drawPixmap( nIconSP, 0, 16, 16, m_pxIcon );
    }
    else
        dc.drawImage( nIconSP, 0, QImage( m_strIconPathName ) );
}
