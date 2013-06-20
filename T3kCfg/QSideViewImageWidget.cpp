#include "QSideViewImageWidget.h"

#include <QPainter>

QSideViewImageWidget::QSideViewImageWidget(QWidget *parent) :
    QLabel(parent)
{
    m_bmpImage = NULL;
    m_nProgress = 0;
    m_nOrignImageHeight = 0;
    m_bSimpleDetection = false;
    m_bNoCam = false;

    m_nAutoLine = -1;
}

QSideViewImageWidget::~QSideViewImageWidget()
{
    if( m_bmpImage )
    {
        delete m_bmpImage;
        m_bmpImage = NULL;
    }
}

void QSideViewImageWidget::SetImage( QImage* bmp )
{
    QRect rcClient( 0,0,width()-1,height()-1 );

    rcClient.adjust( 1, 1, -1, -1 );

    if( !m_bmpImage || m_bmpImage->isNull() )
    {
        m_bmpImage = new QImage( rcClient.width(), rcClient.height(), QImage::Format_RGB32 );
        m_bmpImage->fill( Qt::black );
    }
    else
    {
        if( m_bmpImage->width() != rcClient.width() ||
                m_bmpImage->height() != rcClient.height() )
        {
            delete m_bmpImage;
            m_bmpImage = NULL;
            m_bmpImage = new QImage( rcClient.width(), rcClient.height(), QImage::Format_RGB32 );
            m_bmpImage->fill( Qt::black );
        }
    }

    QPainter dc( m_bmpImage );

    if( !bmp )
    {
        QString str( text() );

        dc.fillRect( rcClient, Qt::black );

        dc.setPen( Qt::white );
        dc.drawText( rcClient, str, QTextOption( Qt::AlignVCenter | Qt::AlignHCenter ) );
        m_nOrignImageHeight = 0;
    }
    else
    {
        dc.drawImage( rcClient, *bmp, QRect( 0, 7, bmp->width(), bmp->height()-14 ) );
        m_nOrignImageHeight = bmp->height()-14;
    }

    update();
}

void QSideViewImageWidget::SetProgress( int nPos )
{
    if( nPos > 100 ) nPos = 100;
    if( nPos < 0 ) nPos = 0;

    if( nPos != m_nProgress )
    {
        if( nPos == 100 || nPos == 0 )
        {
            update();
        }
    }

    m_nProgress = nPos;
}

void QSideViewImageWidget::paintEvent(QPaintEvent */*evt*/)
{
    QPainter dc;
    dc.begin( this );

    QRect rcClient( 0,0,width()-1,height()-1 );

    QPen BorderPen( QColor(84, 109, 142) );
    BorderPen.setWidth( 2 );

    if( m_nProgress == 100 )
    {
        dc.setPen( BorderPen );
        dc.drawRect( rcClient );
    }
    else
    {
        dc.setPen( Qt::lightGray );
        dc.drawRect( rcClient );
    }

    dc.setPen( Qt::black );
    rcClient.adjust( 1, 1, -1, -1 );

    if( m_bmpImage && !m_bmpImage->isNull() )
    {
        dc.drawImage( rcClient, *m_bmpImage );

        // draw split-line
        if( !m_bNoCam && m_nOrignImageHeight != 0 )
        {
            int nStep = m_nOrignImageHeight;
            float fStepH = (float)rcClient.height() / nStep;
            if( fStepH >= 2.f )
            {
                for( int i=1 ; i<nStep ; i++ )
                {
                    if( i == (nStep/2) || i == (nStep/2+1) ) continue;
                    int nY = (int)(rcClient.top() + i*fStepH + .5f);
                    dc.drawLine( rcClient.left(), nY, rcClient.right(), nY );
                }
            }

            int nY1 = (int)(rcClient.top() + (nStep/2)*fStepH + .5f);
            int nY2 = (int)(rcClient.top() + (nStep/2+2)*fStepH + .5f);
            int nYC = (int)(rcClient.top() + (nStep/2+1)*fStepH + .5f);

            dc.setPen( Qt::black );
            dc.drawLine( rcClient.left(), nYC, rcClient.right(), nYC );
            dc.drawLine( rcClient.left(), nY1, rcClient.right(), nY1 );

            dc.setPen( QColor(255,50,50) );

            int nDrawY = 0;
            int nDrawH = 0;

            if( m_strModelName.compare( "T3000" ) == 0 ||
                m_strModelName.compare( "C3000" ) == 0 )
            {
                nDrawY = nY1;
                nDrawH = (nY2-nY1);
                dc.drawRect( rcClient.left(), nDrawY, rcClient.width(), nDrawH );
            }
            else
            {
                if( m_bSimpleDetection )
                {
                    nDrawY = nY1;
                    nDrawH = (nYC-nY1);
                    dc.drawRect( rcClient.left(), nDrawY, rcClient.width(), nDrawH );
                }
                else
                {
                    nDrawY = nY1 - fStepH + .5f;
                    nDrawH = (nY2-nY1) + fStepH + .5f;
                    dc.drawRect( rcClient.left(), nDrawY, rcClient.width(), nDrawH );
                }
            }

            if( m_nAutoLine > 0 )
            {
                dc.setPen( Qt::cyan );
                dc.drawRect( rcClient.left(), nDrawY+(int)((m_nAutoLine*fStepH)+.5f), rcClient.width(), nDrawH );
            }
        }
    }
    else
    {
        QString str( text() );

        dc.fillRect( rcClient, Qt::black );

        dc.setPen( Qt::white );
        dc.drawText( rcClient, str, QTextOption( Qt::AlignVCenter | Qt::AlignHCenter ) );
    }

    dc.end();
}
