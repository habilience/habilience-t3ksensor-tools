#include "QOrderTouchWidget.h"

#include <QDesktopWidget>
#include <QPainter>
#include <QtEvents>
#include <QLangManager.h>
#include <QApplication>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#define MAKR_RADIUS       50


QOrderTouchWidget::QOrderTouchWidget(QWidget *parent) :
    QWidget(parent)
{
    if( parent )
        setFont( parent->font() );

    setFocusPolicy( Qt::StrongFocus );
    setMouseTracking( true );
    setWindowFlags( Qt::Popup | Qt::FramelessWindowHint );

    m_bShowMark = false;
    m_bOnScreen = false;
    m_bTouch = false;
    m_bFlickerMark = false;

    m_nPercent = 0;

    m_nTimerFlickerMark = 0;
    m_bHideCursor = false;

    QPixmap pmpArrow( ":/T3kCfgRes/resources/PNG_DIAGONAL_ARROW.png" );
    int nW = pmpArrow.width()/4;
    int nH = pmpArrow.height();
    for( int i=0; i<4; i++ )
        m_ArrowImage[i] = pmpArrow.copy( i*nW, 0, nW, nH );
}

QOrderTouchWidget::~QOrderTouchWidget()
{
    if( m_nTimerFlickerMark )
    {
        killTimer( m_nTimerFlickerMark );
        m_nTimerFlickerMark = 0;
    }

    if( m_bHideCursor )
        QApplication::restoreOverrideCursor();
}

void QOrderTouchWidget::Init()
{
    QDesktopWidget DeskWidget;
    int nPrimary = DeskWidget.primaryScreen();
    const QRect rcPrimaryMon = DeskWidget.screenGeometry( nPrimary );

    setGeometry( rcPrimaryMon );
    setWindowOpacity( 1.0 );

#ifdef Q_OS_WIN
    SetWindowPos( (HWND)winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE );
    SetForegroundWindow( (HWND)winId() );
#else
    raise();
    activateWindow();
#endif
#ifdef Q_OS_MAC
    cursor().setPos( rcPrimaryMon.center() );
#endif
}

void QOrderTouchWidget::paintEvent(QPaintEvent *)
{
    QPainter dc;
    dc.begin( this );

    dc.fillRect( 0, 0, width(), height(), Qt::white );

    int nW = width();
    int nH = height();
    int nStepW = nW / 50 + 1;
    int nStepH = nH / 50 + 1;

    QPen penGrid( QBrush(QColor(220, 220, 220, 128)) ,2.f, Qt::DashLine );
    dc.setPen( penGrid );
    for( int i=1; i<nStepW; i++ )
        dc.drawLine( i*50, 0, i*50, nH );

    for( int i=1; i<nStepH; i++ )
        dc.drawLine( 0, i*50, nW, i*50 );

    dc.setRenderHint( QPainter::Antialiasing );
    if( m_bShowMark )
    {
        if( m_bOnScreen )
        {
            if( m_bTouch )
            {
                QPen penTouchOK( QBrush(QColor(140, 140, 140)) ,5.f, Qt::SolidLine );
                dc.setPen( penTouchOK );
            }
            else
            {
                if( m_bFlickerMark )
                {
                    QPen penEllipse( QColor(230, 230, 230) );
                    penEllipse.setWidthF( 20.f );
                    dc.setPen( penEllipse );
                    dc.drawEllipse( m_ptOrder, 30, 30 );

                    QPen penActive( QColor(255, 50, 50) );
                    penActive.setWidthF( 3.f );
                    dc.setPen( penActive );
                }
                else
                {
                    QPen penCross( QBrush(QColor(200,200,200,128)), 3.f, Qt::DashLine );
                    dc.setPen( penCross );
                }
            }

            if( !m_bTouch && m_nPercent > 0 && m_nPercent <= 100 )
            {
                const QPen oldPen = dc.pen();

                QRectF rc( m_ptOrder.x()-40, m_ptOrder.y()-40, 80, 80 );
                QPen penArc( QColor(240,220,220,128) );
                dc.setPen( penArc );
                qreal nAngle = (3.6*m_nPercent);

                QPainterPath ArcProgPath;
                ArcProgPath.moveTo( rc.center().x(), rc.top() );
                ArcProgPath.arcTo( rc, 90, -nAngle );
                ArcProgPath.lineTo( rc.center().x(), rc.center().y() );
                //ArcProgPath.lineTo( rc.center().x(), rc.top() );

                QRectF rcIner( rc.center().x()-20, rc.center().y()-20, 40, 40 );
                ArcProgPath.arcTo( rcIner, 90, -nAngle );
                ArcProgPath.lineTo( rcIner.center().x(), rcIner.center().y() );
                ArcProgPath.lineTo( rc.center().x(), rc.top() );

                ArcProgPath.closeSubpath();
                dc.fillPath( ArcProgPath, QBrush(QColor(240,128,128,200)) );
                dc.drawPath( ArcProgPath );

                dc.setPen( oldPen );
            }

            dc.drawLine( m_ptOrder.x()-MAKR_RADIUS, m_ptOrder.y(), m_ptOrder.x()+MAKR_RADIUS, m_ptOrder.y() );
            dc.drawLine( m_ptOrder.x(), m_ptOrder.y()-MAKR_RADIUS, m_ptOrder.x(), m_ptOrder.y()+MAKR_RADIUS );
        }
        else
        {
            Q_ASSERT( (m_ptOrder.x() == 0 || m_ptOrder.x() == 1) || (m_ptOrder.y() == 0 || m_ptOrder.y() == 1) );

            int nIdx = (m_ptOrder.x()*2) + (m_ptOrder.y()*1);

            if( m_bFlickerMark )
            {
                dc.drawPixmap( (width() * m_ptOrder.x()) - m_ptOrder.x() * m_ArrowImage[nIdx].width(),
                               (height() * m_ptOrder.y()) - m_ptOrder.y() * m_ArrowImage[nIdx].height(),
                               m_ArrowImage[nIdx].width(), m_ArrowImage[nIdx].height(), m_ArrowImage[nIdx] );
            }
        }
    }

    QPen penNomal( Qt::darkGray );
    dc.setPen( penNomal );

    QLangRes& Res = QLangManager::instance()->getResource();

    QString str;
    if( m_bOnScreen )
        str = Res.getResString( QString::fromUtf8("ASSISTANCE"), QString::fromUtf8("TEXT_HELP_MSG") );
    else
        str = Res.getResString( QString::fromUtf8("ASSISTANCE"), QString::fromUtf8("TEXT_HELP_ARROW") );

    QFont ft( font() );
    ft.setPointSize( ft.pointSize()*2 );
    ft.setWeight( QFont::Bold );

    dc.setFont( ft );

    QFontMetrics fm( ft );

    int nX = width() / 2 - fm.width( str ) / 2;
    int nY = height() / 2 - fm.height() / 2;

    dc.setRenderHint( QPainter::TextAntialiasing );
    dc.drawText( nX, nY, str );

    dc.end();
}

void QOrderTouchWidget::timerEvent(QTimerEvent *evt)
{
    if( evt->timerId() == m_nTimerFlickerMark )
    {
        m_bFlickerMark = !m_bFlickerMark;
        QRect rc;
        int nUpdateRadius = MAKR_RADIUS+10;
        if( m_bOnScreen )
            rc.setRect( m_ptOrder.x()-nUpdateRadius, m_ptOrder.y()-nUpdateRadius, nUpdateRadius*2, nUpdateRadius*2 );
        else
            rc.setRect( (width() * m_ptOrder.x()) - m_ptOrder.x() * 100,
                        (height() * m_ptOrder.y()) - m_ptOrder.y() * 100, 100, 100 );
        update( rc );
    }

    QWidget::timerEvent(evt);
}

void QOrderTouchWidget::hideEvent(QHideEvent *evt)
{
    if( m_nTimerFlickerMark )
    {
        killTimer( m_nTimerFlickerMark );
        m_nTimerFlickerMark = 0;
    }

    if( m_bHideCursor )
    {
        m_bHideCursor = false;
        QApplication::restoreOverrideCursor();
    }

    QWidget::hideEvent(evt);
}

void QOrderTouchWidget::onOrderTouch(bool bShowMark, bool bOnScreen, bool bTouch, short nX, short nY, int nPercent)
{
    m_bShowMark = bShowMark;
    m_bOnScreen = bOnScreen;
    m_bTouch = bTouch;

    m_nPercent = nPercent;

    if( m_bShowMark )
    {
        if( !m_nTimerFlickerMark )
            m_nTimerFlickerMark = startTimer( 300 );

        show();

        if( !m_bHideCursor )
        {
            m_bHideCursor = true;
            QApplication::setOverrideCursor(QCursor(QPixmap(":/T3kCfgRes/resources/PNG_NULL_CURSOR.png")));
        }
    }
    else
    {
        hide();
        return;
    }

    int nW = width();
    int nH = height();

    int nUpdateRadius = MAKR_RADIUS+10;
    QRect rcOld;
    if( m_bOnScreen )
        rcOld.setRect( m_ptOrder.x()-nUpdateRadius, m_ptOrder.y()-nUpdateRadius, nUpdateRadius*2, nUpdateRadius*2 );
    else
        rcOld.setRect( (width() * m_ptOrder.x()) - m_ptOrder.x() * 100,
                    (height() * m_ptOrder.y()) - m_ptOrder.y() * 100, 100, 100 );

    if( m_bOnScreen )
    {
        m_ptOrder.setX( (int)((double)(nX * nW) / 32767.f + .5f) );
        m_ptOrder.setY( (int)((double)(nY * nH) / 32767.f + .5f) );
    }
    else
    {
        m_ptOrder.setX( nX );
        m_ptOrder.setY( nY );
    }

    QRect rc;
    if( m_bOnScreen )
        rc.setRect( m_ptOrder.x()-nUpdateRadius, m_ptOrder.y()-nUpdateRadius, nUpdateRadius*2, nUpdateRadius*2 );
    else
        rc.setRect( (width() * m_ptOrder.x()) - m_ptOrder.x() * 100,
                    (height() * m_ptOrder.y()) - m_ptOrder.y() * 100, 100, 100 );

    update( rcOld );
    update( rc );
}
