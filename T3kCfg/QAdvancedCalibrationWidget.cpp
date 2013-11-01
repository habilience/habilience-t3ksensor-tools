#include "QAdvancedCalibrationWidget.h"

#include <QtEvents>
#include <QPainter>
#include <QDesktopWidget>
#include <QApplication>

#include "QT3kUserData.h"
#include "QUtils.h"

QAdvancedCalibrationWidget::QAdvancedCalibrationWidget(bool bDetection, QWidget *parent) :
    QDialog(parent), m_DetectionRange(this), m_BentAdjustment(this, this)
{
    setFont( qApp->font() );

    Qt::WindowFlags flags = Qt::Tool;

#if defined(Q_OS_WIN)
    flags |= Qt::MSWindowsFixedSizeDialogHint;
#endif
    flags |= Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint;

    setWindowFlags(flags);

    m_eCurrentPart = bDetection ? spDetectionRange : spBent;

    m_pPixmap = NULL;

    connect( &m_DetectionRange, &QAutoDetectionRange::updateWidget, this, &QAdvancedCalibrationWidget::onUpdateWidget );
    connect( &m_DetectionRange, &QAutoDetectionRange::updateWidgetRect, this, &QAdvancedCalibrationWidget::onUpdateWidgetRect );
    connect( &m_DetectionRange, &QAutoDetectionRange::finishDetectionRange, this, &QAdvancedCalibrationWidget::onFinishDetectionRange );

    connect( &m_BentAdjustment, &QBentAdjustment::updateWidget, this, &QAdvancedCalibrationWidget::onUpdateWidget );
    connect( &m_BentAdjustment, &QBentAdjustment::updateWidgetRect, this, &QAdvancedCalibrationWidget::onUpdateWidgetRect );
    connect( &m_BentAdjustment, &QBentAdjustment::finishBentAdjustment, this, &QAdvancedCalibrationWidget::onFinishBentAdjustment );
}

QAdvancedCalibrationWidget::~QAdvancedCalibrationWidget()
{
}

void QAdvancedCalibrationWidget::showEvent(QShowEvent *)
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
#ifdef Q_OS_MAC
    cursor().setPos( rcPrimaryMon.center() );
#endif

    enterSettings();

    update();
}

void QAdvancedCalibrationWidget::closeEvent(QCloseEvent *)
{
    emit closeWidget();
}

void QAdvancedCalibrationWidget::keyPressEvent(QKeyEvent *evt)
{
    if( evt->key() == Qt::Key_Escape ||
            evt->key() == Qt::Key_Alt ||
            evt->key() == Qt::Key_Meta ||
            evt->key() == Qt::Key_Control )
    {
        // cancal current process : detection or bent

        switch( m_eCurrentPart )
        {
        case spDetectionRange:
            m_DetectionRange.leaveAutoRangeSetting();
            break;
        case spBent:
            m_BentAdjustment.leaveAdjustmentMode(false);
            break;
        default:
            break;
        }

        close();
    }

    QDialog::keyPressEvent(evt);
}

void QAdvancedCalibrationWidget::paintEvent(QPaintEvent *)
{
    QPainter painter;

    QRect rcClient( 0, 0, width()-1, height()-1 );

    painter.begin( this );

    switch( m_eCurrentPart )
    {
    case spDetectionRange:
        m_DetectionRange.draw( painter, rcClient );
        break;
    case spBent:
        m_BentAdjustment.draw( painter, rcClient );
        break;
    default:
        break;
    }

    painter.end();
}

void QAdvancedCalibrationWidget::enterSettings()
{
    switch( m_eCurrentPart )
    {
    case spDetectionRange:
        m_DetectionRange.enterAutoRangeSetting();
        break;
    case spBent:
        DISABLE_MSWINDOWS_TOUCH_PROPERTY;
        m_BentAdjustment.enterAdjustmentMode();
        break;
    default:
        break;
    }
}

void QAdvancedCalibrationWidget::onUpdateWidget()
{
    update();
}

void QAdvancedCalibrationWidget::onUpdateWidgetRect(QRect rc)
{
    update( rc );
}

void QAdvancedCalibrationWidget::onFinishDetectionRange(bool bRet)
{
    if( bRet )
    {
        m_eCurrentPart = spBent;
        enterSettings();
    }
    else
    {
        m_DetectionRange.leaveAutoRangeSetting();
        close();
    }
}

void QAdvancedCalibrationWidget::onFinishBentAdjustment()
{
    close();
}
