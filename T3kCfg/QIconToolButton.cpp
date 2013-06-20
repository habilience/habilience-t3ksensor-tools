#include "QIconToolButton.h"
#include <QPaintEvent>
#include <QPainter>
#include <QImage>
#include <QApplication>

#define MAX_WIDTH           340
#define MAX_HEIGHT          34

QIconToolButton::QIconToolButton(QWidget *parent) :
    QToolButton(parent)
{
}

void QIconToolButton::setIcon(const QIcon &/*icon*/)
{
    return;
}

void QIconToolButton::ChangeIcon(QString strFilePahtName)
{
    m_IconImage = QImage( strFilePahtName );

    int nWidth = m_IconImage.width();
    int nHeight = m_IconImage.height();
    if( nHeight > MAX_HEIGHT )
    {
        m_IconImage = m_IconImage.scaledToHeight( MAX_HEIGHT );
        nHeight = m_IconImage.height();
        nWidth = m_IconImage.width();
    }
    if( nWidth > MAX_WIDTH )
    {
        m_IconImage = m_IconImage.scaledToWidth( MAX_WIDTH );
        nWidth = m_IconImage.width();
        nHeight = m_IconImage.height();
    }

    setGeometry( x(), y(), nWidth+10, height() );
    setMinimumWidth( nWidth+10 );
}

void QIconToolButton::SetHidenBtnAction(bool bHidenAction)
{
    m_bHidenAction = bHidenAction;
    if( m_bHidenAction )
    {
        setCursor( Qt::ArrowCursor );
    }
    else
    {
        setCursor( Qt::PointingHandCursor );
    }
}

void QIconToolButton::paintEvent(QPaintEvent *evt)
{
    if( !m_bHidenAction )
        QToolButton::paintEvent(evt);

    QPainter dc;
    dc.begin( this );

    int nY = m_IconImage.height() < height() ? (int)(double(height()-m_IconImage.height())/2.0f +.5f) : 0;

    dc.drawImage( QApplication::layoutDirection() == Qt::RightToLeft ? width()-m_IconImage.width()-5 : 5, nY, m_IconImage );

    dc.end();
}
