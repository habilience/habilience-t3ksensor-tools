#include "QAdvancedCalibrationWidget.h"

#include <QtEvents>
#include <QPainter>
#include <QDesktopWidget>
#include <QApplication>

#include "QT3kUserData.h"
#include "QUtils.h"
#include "QAutoRangeCompleteDialog.h"
#include "QConfigData.h"


QAdvancedCalibrationWidget::QAdvancedCalibrationWidget(bool bDetection, QWidget *parent) :
    QWidget(parent), m_DetectionRange(this), m_BentAdjustment(this, this)
{
    setFont( qApp->font() );

    Qt::WindowFlags flags = Qt::Widget;

#if defined(Q_OS_WIN)
    flags |= Qt::MSWindowsFixedSizeDialogHint;
#endif
    flags |= Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint;

    setWindowFlags(flags);

    //setFixedSize( 600, 400 );

    m_eCurrentPart = bDetection ? spDetectionRange : spBent;

    m_pPixmap = NULL;

    connect( &m_DetectionRange, &QAutoDetectionRange::updateWidget, this, &QAdvancedCalibrationWidget::onUpdateWidget );
    connect( &m_DetectionRange, &QAutoDetectionRange::updateWidgetRect, this, &QAdvancedCalibrationWidget::onUpdateWidgetRect );
    connect( &m_DetectionRange, &QAutoDetectionRange::finishDetectionRange, this, &QAdvancedCalibrationWidget::onFinishDetectionRange );
    connect( &m_DetectionRange, &QAutoDetectionRange::showProgressDialog, this, &QAdvancedCalibrationWidget::onShowDetectionWaitDialog, Qt::QueuedConnection );

    connect( &m_BentAdjustment, &QBentAdjustment::updateWidget, this, &QAdvancedCalibrationWidget::onUpdateWidget );
    connect( &m_BentAdjustment, &QBentAdjustment::updateWidgetRect, this, &QAdvancedCalibrationWidget::onUpdateWidgetRect );
    connect( &m_BentAdjustment, &QBentAdjustment::finishBentAdjustment, this, &QAdvancedCalibrationWidget::onFinishBentAdjustment );

    onChangeLanguage();
}

QAdvancedCalibrationWidget::~QAdvancedCalibrationWidget()
{
}

void QAdvancedCalibrationWidget::showEvent(QShowEvent *)
{
    QDesktopWidget DeskWidget;
    int nPrimary = DeskWidget.primaryScreen();
    const QRect rcPrimaryMon = DeskWidget.screenGeometry( nPrimary );

    move( rcPrimaryMon.left(), rcPrimaryMon.top() );

#ifdef Q_OS_MAC
    cursor().setPos( rcPrimaryMon.center() );

    setGeometry( rcPrimaryMon );
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

    QWidget::keyPressEvent(evt);
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

    QFont ft( font() );
    ft.setPixelSize( rcClient.height() / 30 );
    ft.setWeight(QFont::Black);

    painter.setFont( ft );
    QFontMetricsF fm( ft );
    qreal dW = fm.width( m_strEscapeMsg );
    qreal dH = fm.height();
    QRectF rcText( rcClient.center().x() - (dW / 2.f),
                  rcClient.height() / 4 - (dH / 2.f),
                  dW, dH );

    QPen penText( Qt::SolidLine );
    penText.setColor( QColor(160,160,160) );
    painter.setPen( penText );
    painter.drawText( rcText, m_strEscapeMsg, QTextOption(Qt::AlignCenter) );

    painter.end();
}

void QAdvancedCalibrationWidget::onChangeLanguage()
{
    if( !winId() ) return;

    QLangRes& Res = QLangManager::instance()->getResource();

    m_strEscapeMsg = Res.getResString( QString::fromUtf8("ADVANCED"), QString::fromUtf8("MSG_ESCAPE") );
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

void QAdvancedCalibrationWidget::onShowDetectionWaitDialog()
{
    QAutoRangeCompleteDialog AutoRangeCompleteDialog( this );
    AutoRangeCompleteDialog.exec();
}
