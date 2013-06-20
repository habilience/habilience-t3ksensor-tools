#include "QRemoteGuideWidget.h"

#include <QDesktopWidget>
#include <QPainter>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

QRemoteGuideWidget::QRemoteGuideWidget(QWidget *parent) :
    QWidget(parent)
{
    setFont( parent->font() );
    setWindowOpacity( 0.5 );

    setWindowFlags( Qt::FramelessWindowHint );

    setAttribute( Qt::WA_TransparentForMouseEvents, true );
}

void QRemoteGuideWidget::MoveWidgetToPrimary()
{
    QDesktopWidget DeskWidget;
    int nPrimary = DeskWidget.primaryScreen();
    const QRect rcPrimaryMon = DeskWidget.screenGeometry( nPrimary );
    setGeometry( rcPrimaryMon );

#ifdef Q_OS_WIN
    SetWindowPos( (HWND)winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE );
    SetForegroundWindow( (HWND)winId() );
#else
    raise();
    activateWindow();
#endif
}

void QRemoteGuideWidget::showEvent(QShowEvent *evt)
{
    MoveWidgetToPrimary();

    QWidget::showEvent(evt);
}

void QRemoteGuideWidget::paintEvent(QPaintEvent *)
{
    QPainter dc;
    dc.begin( this );

    // m_ptTarget;

    dc.end();
}
