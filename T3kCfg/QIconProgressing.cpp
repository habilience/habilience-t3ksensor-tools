#include "QIconProgressing.h"

#include <QImage>
#include <QPainter>
#include <QTimerEvent>

#define ICON_WIDTH      32
#define ICON_HEIGHT     32
#define ICON_COL        8
#define ICON_ROW        4

QIconProgressing::QIconProgressing(QWidget *parent) :
    QLabel(parent)
{
    m_listProgressImage.clear();

    m_nTimerProgress = 0;
    m_nCurrentImageIndex = 0;
    m_nImageCount = 0;

    QPixmap* pIconArray = new QPixmap( ":/T3kCfgRes/resources/PNG_ICON_PROGRESS.png", "PNG" );

    int nC = pIconArray->width()/ICON_WIDTH;
    int nR = pIconArray->height()/ICON_HEIGHT;
    m_nImageCount = nC * nR;

    for( int r=0; r<nR; r++ )
    {
        for( int c=0; c<nC; c++ )
        {
            m_listProgressImage.push_back( new QImage(pIconArray->copy(QRect( c*ICON_WIDTH, r*ICON_HEIGHT, ICON_WIDTH, ICON_HEIGHT ) ).toImage()) );
        }
    }
    delete pIconArray;

    Q_ASSERT( m_listProgressImage.size() == m_nImageCount );
}

QIconProgressing::~QIconProgressing()
{
    foreach( QImage* pImg, m_listProgressImage )
    {
        if( pImg )
            delete pImg;
    }

    m_listProgressImage.clear();
}

void QIconProgressing::Start()
{
    m_nCurrentImageIndex = 0;
    if( !m_nTimerProgress )
    {
        m_nTimerProgress = startTimer( 10 );
    }
}

void QIconProgressing::Complete()
{
    if( m_nTimerProgress )
    {
        killTimer( m_nTimerProgress );
        m_nTimerProgress = 0;
    }

    m_nCurrentImageIndex = 0;
}

void QIconProgressing::paintEvent(QPaintEvent *)
{
    QPainter dc;
    dc.begin( this );

    dc.fillRect( 0, 0,  width()-1, height()-1, QColor(255,255,255) );
    dc.drawImage( 0, 0, *m_listProgressImage.at(m_nCurrentImageIndex) );

    dc.end();
}

void QIconProgressing::timerEvent(QTimerEvent *evt)
{
    if( evt->timerId() == m_nTimerProgress )
    {
        if( m_nCurrentImageIndex >= m_nImageCount-1 )
            m_nCurrentImageIndex = 1;
        else
            m_nCurrentImageIndex++;

        update();
    }

    QLabel::timerEvent(evt);
}
