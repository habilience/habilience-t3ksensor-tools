#include "stdInclude.h"

#include "Common/nv.h"

#include "QCalibrationWidget.h"

#include <QKeyEvent>
#include <QPainter>
#include <QDesktopWidget>

#include <QCursor>

#include <QApplication>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#define MAC_MARGIN 2470.0f
#define STD_SCALE 32767.0f
#define MAC_SCALE 27827.0f // STD_SCALE-MAC_MARGIN*2

QCalibrationWidget::QCalibrationWidget(QT3kDevice*& pHandle, QWidget *parent) :
    QWidget(parent), m_pT3kHandle(pHandle)
{
    m_fScreenMargin = 0.f;
    m_bDigitizerMode = false;
    m_nManualCaliPointX = 0;
    m_nManualCaliPointY = 0;

    m_nTimerUpdate = 0;
    m_bUpdateFlag = false;

    m_bUnderVersion = false;
    m_paryCaliPoints = NULL;

    setFocusPolicy( Qt::StrongFocus );
    setMouseTracking( true );
    setWindowFlags( Qt::Popup | Qt::FramelessWindowHint );

    onChangeLanguage();
}

QCalibrationWidget::~QCalibrationWidget()
{
    if( m_paryCaliPoints )
    {
        delete[] m_paryCaliPoints;
        m_paryCaliPoints = NULL;
    }
}

void QCalibrationWidget::onChangeLanguage()
{
    if( !winId() ) return;

    m_strTitle = QLangManager::instance()->getResource().getResString( QString::fromUtf8("CALIBRATION SETTING"), QString::fromUtf8("TEXT_CALIBRATE") );
}

void QCalibrationWidget::TPDP_OnMSG(T3K_DEVICE_INFO /*devInfo*/, ResponsePart Part, unsigned short /*ticktime*/, const char *partid, const char *txt)
{
    if( Part == MM && QString(partid).compare("CAL") == 0 )
    {
        QString strTxt( txt );
        int nPoints = m_bUnderVersion ? UNDER_VER_CALI_PNTS : SUPPORT_VER_CALI_PNTS;
        if( m_bDigitizerMode && strTxt.contains("CAL_AXIS_COORD") )
        {
            QDesktopWidget DeskWidget;
            int nPrimary = DeskWidget.primaryScreen();
            const QRect rcPrimaryMon = DeskWidget.screenGeometry( nPrimary );

            int nS = strTxt.indexOf( ":" );
            int nE = strTxt.indexOf( "," );
            m_nManualCaliPointX = strTxt.mid( nS+1, nE-1-nS ).trimmed().toInt() * rcPrimaryMon.width() / STD_SCALE;
            nS = nE+1;
            m_nManualCaliPointY = strTxt.mid( nS ).trimmed().toInt() * rcPrimaryMon.height() / STD_SCALE;

            if( !m_nTimerUpdate )
                m_nTimerUpdate = startTimer( 500 );
            update();

        }
        if( strTxt.contains("CAL_AXIS_OK") )
        {
            int nPos = strTxt.indexOf( ":" );
            int nIndex = strTxt.mid( nPos+1 ).trimmed().toInt();

            qDebug( "Calibration Point OK: %d", nIndex );
            if( nIndex >= 0 && nIndex < nPoints )
                m_paryCaliPoints[nIndex] = true;
        }
        else if( strTxt.contains("CAL_AXIS_RESET") )
        {
            int nPos = strTxt.indexOf( ":" );
            int nIndex = strTxt.mid( nPos+1 ).trimmed().toInt();

            qDebug( "Calibration Point Reset: %d", nIndex );
            if( nIndex >= 0 && nIndex < nPoints )
                m_paryCaliPoints[nIndex] = false;
        }
    }
}

bool QCalibrationWidget::ShowWindow( bool bShow, int nUsbConfigMode, float fScreenMargin/*=0*/, int nMacMargin/*=0*/, float fMMVersion/*=0.0*/ )
{
    m_bDigitizerMode = nUsbConfigMode == 0x04 ? true : false;
    m_nManualCaliPointX = 0;
    m_nManualCaliPointY = 0;

    if( !m_paryCaliPoints )
        m_paryCaliPoints = new bool[QCALI_POINTS];
    ::memset( m_paryCaliPoints, 0, sizeof(bool)*QCALI_POINTS );
    if( bShow )
    {
        switch( nMacMargin )
        {
        case -1:
            m_fScreenMargin = 0;
            break;
        case 0:
            m_fScreenMargin = fScreenMargin;
            break;
        case 1:
            {
                float fMargin =  ((fScreenMargin*MAC_SCALE/100.0f) + MAC_MARGIN) / STD_SCALE;
                m_fScreenMargin = fMargin*100.0f;
            }
            break;
        default:
            break;
        }

        show();

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

        m_pT3kHandle->setReportMessage( true );

        if( fMMVersion >= MM_MIN_SUPPORT_FIRMWARE_VERSION && fMMVersion <= MM_LAST_SUPPORT_FRIMWARE_VERSION )
        {
            m_bUnderVersion = true;
            QApplication::setOverrideCursor( QCursor(QPixmap(":/T3kCfgRes/resources/CURSOR_BIG_CROSS.png")) );
        }
        else
        {
            m_bUnderVersion = false;
            QApplication::setOverrideCursor(QCursor(QPixmap(":/T3kCfgRes/resources/PNG_NULL_CURSOR.png")));
        }
    }
    else
    {
        m_pT3kHandle->setReportMessage( false );
        QWidget::hide();
        QApplication::restoreOverrideCursor();
    }

    return true;
}

void QCalibrationWidget::EscapeCalibrationMode()
{
    if( m_nTimerUpdate )
    {
        killTimer( m_nTimerUpdate );
        m_nTimerUpdate = 0;
    }
    m_pT3kHandle->sendCommand( QString("%1%2").arg(cstrCalibrationMode).arg(MODE_CALIBRATION_NONE), true );
}

void QCalibrationWidget::paintEvent(QPaintEvent */*evt*/)
{
    if( m_bUnderVersion ) return;
    QPainter dc;
    dc.begin( this );

    QRect rcClient( x(), y(), width(), height() );

    dc.setRenderHint( QPainter::Antialiasing );
    dc.fillRect( rcClient, QColor(255,255,255) );

    if( m_fScreenMargin == 0.f ) return;

    int nMarginX = (int)( rcClient.width() * m_fScreenMargin / 100.f + .5 );
    int nMarginY = (int)( rcClient.height() * m_fScreenMargin / 100.f + .5 );

    QRect rcBody = rcClient;
    rcBody.adjust( nMarginX, nMarginY, -nMarginX, -nMarginY );

    QPoint ptC = rcClient.center();

    int nPoints = 0;

    QPoint* ptX = NULL;
    if( m_bUnderVersion )
    {
         nPoints = UNDER_VER_CALI_PNTS;
         ptX = new QPoint[nPoints];
         ptX[0] = QPoint(rcBody.right(), rcBody.top());     ptX[1] = QPoint(rcBody.right(), ptC.y());
         ptX[2] = QPoint(rcBody.right(), rcBody.bottom());  ptX[3] = QPoint(ptC.x(), rcBody.bottom());
         ptX[4] = QPoint(rcBody.left(), rcBody.bottom());   ptX[5] = QPoint(rcBody.left(), ptC.y());
         ptX[6] = QPoint(rcBody.left(), rcBody.top());      ptX[7] = QPoint(ptC.x(), rcBody.top());
         ptX[8] = QPoint(ptC.x(), ptC.y());
    }
    else
    {
        nPoints = SUPPORT_VER_CALI_PNTS;
        ptX = new QPoint[nPoints];

        ptX[0] = QPoint(rcBody.right(), rcBody.top());      ptX[1] = QPoint(rcBody.right(), rcBody.bottom());
        ptX[2] = QPoint(rcBody.left(), rcBody.bottom());    ptX[3] = QPoint(rcBody.left(), rcBody.top());
    }

    int nHalfLength = rcClient.width() / 50;
    QPen penNormal( QColor(140, 140, 140) );
    penNormal.setWidthF( 3.f );
    QPen penActive( QColor(255, 50, 50) );
    penActive.setWidthF( 3.f );
    QPen penActiveBox( QColor(230, 230, 230) );
    penActiveBox.setWidthF( 20.f );
    QPen penNon( QBrush(QColor(200,200,200,128)), 3.f, Qt::DashLine );
    QPen penFinished( QBrush(QColor(140, 140, 140)) ,5.f, Qt::SolidLine );
    QRect rcBox;
    QPoint ptCursor( m_ptCur );
    if( m_bDigitizerMode )
        ptCursor = QPoint(m_nManualCaliPointX, m_nManualCaliPointY);
    ptCursor = mapFrom( this, ptCursor );

    for( int i=0 ; i<nPoints ; i++ )
    {
        rcBox.setRect( ptX[i].x()-20, ptX[i].y()-20, 40, 40 );

        if( m_paryCaliPoints[i] )
        {
            dc.setPen( penFinished );

            dc.drawLine( ptX[i].x()-nHalfLength, ptX[i].y(), ptX[i].x()+nHalfLength, ptX[i].y() );
            dc.drawLine( ptX[i].x(), ptX[i].y()-nHalfLength, ptX[i].x(), ptX[i].y()+nHalfLength );
        }
        else
        {
            if( m_bDigitizerMode ? (rcBox.contains( ptCursor ) && m_bUpdateFlag) : rcBox.contains( ptCursor ) )
            {
                dc.setPen( penActiveBox );
                dc.drawEllipse( ptX[i].x()-20, ptX[i].y()-20, 40, 40 );
                dc.setPen( penActive );
            }
            else
            {
                dc.setPen( penNon );
            }

            dc.drawLine( ptX[i].x()-nHalfLength, ptX[i].y(), ptX[i].x()+nHalfLength, ptX[i].y() );
            dc.drawLine( ptX[i].x(), ptX[i].y()-nHalfLength, ptX[i].x(), ptX[i].y()+nHalfLength );
        }
    }

    dc.end();
}

void QCalibrationWidget::mouseMoveEvent(QMouseEvent *evt)
{
    m_ptCur = evt->pos();
    update();

    QWidget::mouseMoveEvent(evt);
}

void QCalibrationWidget::keyPressEvent(QKeyEvent *evt)
{
    if( evt->type() == QEvent::KeyPress )
    {
        if( evt->key() == Qt::Key_Escape )
        {
            QWidget::hide();
            EscapeCalibrationMode();
            QApplication::restoreOverrideCursor();
            return;
        }

        if( evt->key() == Qt::Key_Control || evt->key() == Qt::Key_Meta || evt->key() == Qt::Key_Alt )
        {
            QWidget::hide();
            EscapeCalibrationMode();
            QApplication::restoreOverrideCursor();
            return;
        }
    }
    QWidget::keyPressEvent(evt);
}

void QCalibrationWidget::showEvent(QShowEvent *evt)
{
    setFocus();

    QWidget::showEvent(evt);
}

void QCalibrationWidget::focusOutEvent(QFocusEvent *evt)
{
    QWidget::hide();
    EscapeCalibrationMode();
    QApplication::restoreOverrideCursor();

    QWidget::focusOutEvent(evt);
}

void QCalibrationWidget::timerEvent(QTimerEvent *evt)
{
    if( evt->timerId() )
    {
        m_bUpdateFlag = !m_bUpdateFlag;
        update();
    }

    QWidget::timerEvent(evt);
}
