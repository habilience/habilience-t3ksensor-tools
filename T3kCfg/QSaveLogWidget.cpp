#include "QSaveLogWidget.h"

#include "QLoadSensorDataWidget.h"

#include <QDesktopWidget>
#include <QtEvents>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

QSaveLogWidget::QSaveLogWidget(T3kHandle*& pHandle, QWidget *parent) :
    QDialog(parent), m_pT3kHandle(pHandle)
{
    setWindowFlags( Qt::Dialog | Qt::FramelessWindowHint );

    // Qt 5.1.1 bug not work
#ifdef _DEBUG
    setWindowOpacity( 0.0 );
#else
    setWindowOpacity( 0.4 );
#endif
    QPalette CurPalette = palette();
    CurPalette.setColor( QPalette::Window, Qt::black );
    setPalette( CurPalette );

    setFont( parent->font() );

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

    //setGeometry( rcFullScreen );

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
