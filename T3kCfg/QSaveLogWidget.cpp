#include "QSaveLogWidget.h"

#include "QLoadSensorDataWidget.h"

#include <QDesktopWidget>
#include <QtEvents>
#include <QApplication>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

QSaveLogWidget::QSaveLogWidget(QT3kDevice*& pHandle, QWidget *parent) :
    QDialog(parent), m_pT3kHandle(pHandle)
{
    setWindowFlags( Qt::FramelessWindowHint );
    setWindowModality( Qt::ApplicationModal );
    setModal( true );

    // until Qt 5.2.1 bug (only windows 32bit) -> changed Qt 5.2.0
#ifdef _DEBUG
    setWindowOpacity( 0.0 );
#else
    setWindowOpacity( 0.4 );
#endif
    QPalette CurPalette = palette();
    CurPalette.setColor( QPalette::Window, Qt::black );
    setPalette( CurPalette );

    setFont( qApp->font() );

    m_nTimer = 0;
}

QSaveLogWidget::~QSaveLogWidget()
{
}

void QSaveLogWidget::showEvent(QShowEvent *)
{
    QDesktopWidget DeskWidget;
    QRect rcFullScreen;
    for( int i=0; i<DeskWidget.screenCount(); i++ )
    {
        QRect rc( DeskWidget.screenGeometry( i ) );
        rcFullScreen.setLeft( rcFullScreen.left() < rc.left() ? rcFullScreen.left() : rc.left() );
        rcFullScreen.setTop( rcFullScreen.top() < rc.top() ? rcFullScreen.top() : rc.top() );
        rcFullScreen.setRight( rcFullScreen.right() > rc.right() ? rcFullScreen.right() : rc.right() );
        rcFullScreen.setBottom( rcFullScreen.bottom() > rc.bottom() ? rcFullScreen.bottom() : rc.bottom() );
    }

#ifndef _DEBUG
    setGeometry( rcFullScreen );
#endif

#ifdef Q_OS_WIN
    SetWindowPos( (HWND)winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE );
    SetForegroundWindow( (HWND)winId() );
#else
    raise();
    activateWindow();
#endif

    m_nTimer = startTimer( 1 );
}

void QSaveLogWidget::keyPressEvent(QKeyEvent *evt)
{
    if( evt->type() == QEvent::KeyPress )
    {
        if( evt->key() == Qt::Key_Escape || evt->key() == Qt::Key_Control || evt->key() == Qt::Key_Meta || evt->key() == Qt::Key_Alt )
        {
            return;
        }
    }

    QWidget::keyPressEvent(evt);
}

void QSaveLogWidget::timerEvent(QTimerEvent *evt)
{
    if( evt->timerId() == m_nTimer )
    {
        killTimer( m_nTimer );
        m_nTimer = 0;
        QLoadSensorDataWidget Widget( m_pT3kHandle, this );
        Widget.exec();
        QDialog::accept();
    }
}
