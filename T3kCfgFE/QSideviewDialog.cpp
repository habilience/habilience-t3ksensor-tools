#include "QSideviewDialog.h"
#include "ui_QSideviewDialog.h"
#include <QPainter>
#include "dialog.h"
#include <QCloseEvent>
#include <QTimerEvent>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QDesktopWidget>
#include "QShowMessageBox.h"
#include "QLogSystem.h"
#include "QT3kDevice.h"

#include "QUtils.h"
#include "T3kConstStr.h"

#include "QSensorInitDataCfg.h"

#include "QMyApplication.h"

#include "conf.h"

#define MAIN_TAG "MAIN"
#define RES_TAG "SIDEVIEW"

#define PREVIEW_COUNT_DOWN      (10*2)      // 10 secs by 500m timer

QSideviewDialog::QSideviewDialog(Dialog *parent) :
    QDialog(parent),
    m_pMainDlg(parent),
    ui(new Ui::QSideviewDialog),
    m_EventRedirect(this)
{
    ui->setupUi(this);
    QT3kDevice* pDevice = QT3kDevice::instance();

    Qt::WindowFlags flags = Qt::Tool;

    if (!pDevice->isVirtualDevice())
    {
#if defined(Q_OS_WIN)
        flags |= Qt::MSWindowsFixedSizeDialogHint;
#endif
        flags |= Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint;
    }

    if (g_AppData.bScreenShotMode)
    {
        flags &= ~Qt::WindowStaysOnTopHint;
    }

    setWindowFlags(flags);
    setAttribute(Qt::WA_DeleteOnClose);

    m_bIsModified = false;
    m_TimerPreviewCountDown = 0;
    m_TimerNoData = 0;
    m_TimerRefreshAutoOffset = 0;
    m_bIsNoData = true;
    m_bDrawNoData = false;
    m_nCurrentCameraIndex = -1;

    m_nDetectionLine = 0;
    m_nAutoOffset = 0;

    m_pImgSideview = NULL;
    m_pImgTempBuffer = NULL;

    m_bSimpleDetection = false;

    m_rcUpdateImage = QRect(0,0,0,0);

    m_bSyncMode = false;

    LOG_I( "Enter [Sideview]" );

    onChangeLanguage();

    ui->cmdAsyncMngr->setT3kDevice( pDevice );
    ui->btnRemoteSideview->setVisible( pDevice->isVirtualDevice() ? true : false );
    ui->btnCam1->setChecked(true);
    ui->btnCam2->setChecked(false);
    ui->btnCam1_1->setChecked(false);
    ui->btnCam2_1->setChecked(false);

    ui->btnLight2Dec->setVisible(false);
    ui->btnLight2Inc->setVisible(false);
    ui->txtEdtLight2->setVisible(false);
    ui->btnLight3Dec->setVisible(false);
    ui->btnLight3Inc->setVisible(false);
    ui->txtEdtLight3->setVisible(false);

    if (g_AppData.strModelName.compare( "T3k A" ) == 0)
    {
        ui->lblAmbientLights->setVisible(false);
        ui->btnLight1Dec->setVisible(false);
        ui->btnLight1Inc->setVisible(false);
        ui->txtEdtLight1->setVisible(false);
    }
    else
    {
        ui->lblAmbientLights->setVisible(true);
        ui->btnLight1Dec->setVisible(true);
        ui->btnLight1Inc->setVisible(true);
        ui->txtEdtLight1->setVisible(true);
    }

    connect( ui->cmdAsyncMngr, SIGNAL(asyncFinished(bool,int)), SLOT(onCmdAsyncMngrFinished(bool,int)));

    setViewMode( true );
    pDevice->sendCommand("cam1/mode=sideview", true);
    m_nCurrentCameraIndex = IDX_CM1;

    requestSensorData( cmdLoadFactoryDefault, false );

    if (!pDevice->isVirtualDevice())
    {
        m_TimerPreviewCountDown = startTimer( 500 );
        m_nPreviewCountDown = PREVIEW_COUNT_DOWN;
        g_pApp->setMonitoringMouseMovement(true);
        QLangRes& res = QLangManager::getResource();
        QString strBtnCaption = res.getResString(MAIN_TAG, "BTN_CAPTION_CLOSE") + "("+ QString::number(m_nPreviewCountDown/2) + ")";
        ui->btnClose->setText( strBtnCaption );
    }

    connect(g_pApp, SIGNAL(mouseMoved()), SLOT(onGlobalMouseMoved()));

    resetEditColors();

    ui->btnCam1_1->setVisible( g_AppData.nSubCameraCount > 0 ? true : false );
    ui->btnCam2_1->setVisible( g_AppData.nSubCameraCount > 0 ? true : false );

    installEventFilter(this);

    connect( ui->txtEdtDetectLine, SIGNAL(editModified(QBorderStyleEdit*,int,double)), SLOT(onEditModified(QBorderStyleEdit*,int,double)) );
    connect( ui->txtEdtLight1, SIGNAL(editModified(QBorderStyleEdit*,int,double)), SLOT(onEditModified(QBorderStyleEdit*,int,double)) );
    connect( ui->txtEdtLight2, SIGNAL(editModified(QBorderStyleEdit*,int,double)), SLOT(onEditModified(QBorderStyleEdit*,int,double)) );
    connect( ui->txtEdtLight3, SIGNAL(editModified(QBorderStyleEdit*,int,double)), SLOT(onEditModified(QBorderStyleEdit*,int,double)) );

    ui->txtEdtDetectLine->setAlignment(Qt::AlignCenter);
    ui->txtEdtLight1->setAlignment(Qt::AlignCenter);
    ui->txtEdtLight2->setAlignment(Qt::AlignCenter);
    ui->txtEdtLight3->setAlignment(Qt::AlignCenter);

    ui->btnCam1->setEnabled(g_AppData.cameraConnectionInfo[IDX_CM1]);
    ui->btnCam2->setEnabled(g_AppData.cameraConnectionInfo[IDX_CM2]);
    ui->btnCam1_1->setEnabled(g_AppData.cameraConnectionInfo[IDX_CM1_1]);
    ui->btnCam2_1->setEnabled(g_AppData.cameraConnectionInfo[IDX_CM2_1]);

    m_EventRedirect.installEventListener(this);
    installEventFilter(&m_EventRedirect);

    //ui->btnClose->setFocus();
    ui->btnCam1->setFocus();
}

QSideviewDialog::~QSideviewDialog()
{
    if (m_pImgTempBuffer)
    {
        delete[] m_pImgTempBuffer;
        m_pImgTempBuffer = NULL;
    }
    if (m_pImgSideview)
    {
        delete m_pImgSideview;
        m_pImgSideview = NULL;
    }

    m_pMainDlg->onCloseMenu();
    delete ui;

    LOG_I( "Exit [Sideview]" );
}

void QSideviewDialog::setViewMode( bool bViewMode )
{
    qDebug( "setViewMode: %s", bViewMode ? "true" : "false" );
    int nMode = T3K_HID_MODE_COMMAND;
#ifdef MONITORING_EXPIRED_MODE
    nMode |= T3K_HID_MODE_MESSAGE;
#endif

    if (bViewMode)
        nMode |= T3K_HID_MODE_VIEW;

    qDebug( "setViewMode: %x", nMode );

    m_pMainDlg->setInstantMode( nMode );
}

void QSideviewDialog::onChangeLanguage()
{
    QLangRes& res = QLangManager::getResource();

    static bool s_bIsR2L = false;
    bool bIsR2L = res.isR2L();
    if ( bIsR2L )
        setLayoutDirection( Qt::RightToLeft );
    else
        setLayoutDirection( Qt::LeftToRight );

    setWindowTitle( res.getResString(MAIN_TAG, "BTN_CAPTION_SIDE_VIEW") );

    QString strCamera = res.getResString( MAIN_TAG, "TEXT_CAMERA" );

    ui->btnCam1->setText( strCamera + " 1" );
    ui->btnCam2->setText( strCamera + " 2" );
    ui->btnCam1_1->setText( strCamera + " 1-1" );
    ui->btnCam2_1->setText( strCamera + " 2-1" );

    ui->lblDetectionLine->setText( res.getResString( RES_TAG, "TEXT_DETECTION_LINE") );
    ui->lblAmbientLights->setText( res.getResString( RES_TAG, "TEXT_IR_BAR") );
    ui->btnRemoteSideview->setText( res.getResString( RES_TAG, "BTN_CAPTION_REMOTE_SHARE_SIDEVIEW") );

    ui->btnReset->setText( res.getResString( MAIN_TAG, "BTN_CAPTION_RESET") );

    ui->btnRefresh->setText( res.getResString( MAIN_TAG, "BTN_CAPTION_REFRESH") );
    ui->btnSave->setText( res.getResString( MAIN_TAG, "BTN_CAPTION_SAVE") );
    ui->btnClose->setText( res.getResString( MAIN_TAG, "BTN_CAPTION_CLOSE") );

    if (bIsR2L != s_bIsR2L)
    {
        // TODO: adjust ui
    }

    s_bIsR2L = bIsR2L;
}

bool QSideviewDialog::canClose()
{
    if (m_bIsModified)
    {
        QLangRes& res = QLangManager::getResource();
        QString strPrompt = res.getResString( MAIN_TAG, "TEXT_WARNING_SENSOR_DATA_IS_CHANGED" );
        QString strTitle = res.getResString( MAIN_TAG, "TEXT_WARNING_MESSAGE_TITLE" );
        int nRet = showMessageBox( this,
            strPrompt,
            strTitle,
            QMessageBox::Question, QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel, QMessageBox::Yes );
        if (nRet == QMessageBox::Yes)
        {
            enableAllControls(false);
            if (!requestSensorData(cmdWriteToFactoryDefault, true))
            {
                strPrompt = res.getResString( MAIN_TAG, "TEXT_ERROR_WRITE_FACTORY_DEFAULT_FAILURE" );
                strTitle = res.getResString( MAIN_TAG, "TEXT_ERROR_MESSAGE_TITLE" );
                nRet = showMessageBox( this,
                    strPrompt,
                    strTitle,
                    QMessageBox::Critical, QMessageBox::Ok, QMessageBox::Ok );
                enableAllControls(true);
                ui->btnClose->setFocus();
                return false;
            }
            enableAllControls(true);
        }
        else if (nRet == QMessageBox::No)
        {
            requestSensorData( cmdLoadFactoryDefault, true );
        }
        else
        {
            ui->btnClose->setEnabled(true);
            ui->btnClose->setFocus();
            return false;
        }
    }

    return true;
}

QString QSideviewDialog::getCameraPrefix( int nCameraIndex )
{
    QString strCameraPrefix = "";
    switch (nCameraIndex)
    {
    case IDX_CM1:
        strCameraPrefix = sCam1;
        break;
    case IDX_CM2:
        strCameraPrefix = sCam2;
        break;
    case IDX_CM1_1:
        strCameraPrefix = sCam1_1;
        break;
    case IDX_CM2_1:
        strCameraPrefix = sCam2_1;
        break;
    }

    return strCameraPrefix;
}

void QSideviewDialog::setSideview( int nCameraIndex, bool setMode )
{
    QString strCameraPrefix = getCameraPrefix(nCameraIndex);

    m_nCurrentCameraIndex = nCameraIndex;

    m_bIsNoData = true;
    m_bDrawNoData = false;

    ui->btnDetectLineDn->setEnabled(false);
    ui->btnDetectLineUp->setEnabled(false);
    ui->txtEdtDetectLine->setEnabled(false);
    ui->btnLight1Dec->setEnabled(false);
    ui->btnLight1Inc->setEnabled(false);
    ui->txtEdtLight1->setEnabled(false);
    ui->btnLight2Dec->setEnabled(false);
    ui->btnLight2Inc->setEnabled(false);
    ui->txtEdtLight2->setEnabled(false);
    ui->btnLight3Dec->setEnabled(false);
    ui->btnLight3Inc->setEnabled(false);
    ui->txtEdtLight3->setEnabled(false);
    ui->lblAmbientLights->setEnabled(false);
    ui->lblDetectionLine->setEnabled(false);

    if (m_TimerNoData)
        killTimer(m_TimerNoData);
    m_TimerNoData = startTimer(1500);

    ui->txtEdtDetectLine->setText("");
    ui->txtEdtLight1->setText("");
    ui->txtEdtLight2->setText("");
    ui->txtEdtLight3->setText("");

    if (setMode)
    {
        QString strCmd;
        strCmd = strCameraPrefix + "mode=sideview";
        QT3kDevice::instance()->sendCommand( strCmd, false );

        if (m_pImgSideview)
        {
            m_pImgSideview->fill(QColor(0,0,0));
            if (m_pImgTempBuffer)
            {
                memset( m_pImgTempBuffer, 0, sizeof(uchar)*m_pImgSideview->width()*m_pImgSideview->height() );
            }
        }
    }

    requestSensorData( cmdRefresh, false );

    update();
}

void QSideviewDialog::drawSideviewImage( QPainter& p, const QRect& rcImageDst, int nScaleY )
{
    p.drawImage( rcImageDst, *m_pImgSideview, m_pImgSideview->rect() );

    p.setPen( Qt::black );
    p.drawRect( rcImageDst );

    for (int y=rcImageDst.top() ; y<rcImageDst.bottom() ; y+=nScaleY)
    {
        p.drawLine( rcImageDst.left(), y, rcImageDst.right(), y );
    }

    if (m_strCMModelName.indexOf("3000") >= 0)  // C3000
    {
        QRect rc;
        if (!m_bSimpleDetection)
        {
            rc.setLeft( rcImageDst.left() );
            rc.setTop( rcImageDst.top() + rcImageDst.height() / 2 - nScaleY/2 );
            rc.setWidth( rcImageDst.width() );
            rc.setHeight( nScaleY * 2 );
        }
        else
        {
            rc.setLeft( rcImageDst.left() );
            rc.setTop( rcImageDst.top() + rcImageDst.height() / 2 - nScaleY/2 );
            rc.setWidth( rcImageDst.width() );
            rc.setHeight( nScaleY );
        }

        if (m_nAutoOffset != 0)
        {
            QRect rcOffset = rc;
            rcOffset.adjust( 0, m_nAutoOffset * nScaleY, 0, m_nAutoOffset * nScaleY );
            p.setPen( QColor(5,250,250) );
            p.drawRect( rcOffset );
        }

        p.setPen( QColor(255,50,50) );
        p.drawRect( rc );
    }
    else
    {
        QRect rc;
        if (!m_bSimpleDetection)
        {
            rc.setLeft( rcImageDst.left() );
            rc.setTop( rcImageDst.top() + rcImageDst.height() / 2 - nScaleY/2 - nScaleY );
            rc.setWidth( rcImageDst.width() );
            rc.setHeight( nScaleY + nScaleY * 2 );
        }
        else
        {
            rc.setLeft( rcImageDst.left() );
            rc.setTop( rcImageDst.top() + rcImageDst.height() / 2 - nScaleY/2 );
            rc.setWidth( rcImageDst.width() );
            rc.setHeight( nScaleY );
        }

        if (m_nAutoOffset != 0)
        {
            QRect rcOffset = rc;
            rcOffset.adjust( 0, m_nAutoOffset * nScaleY, 0, m_nAutoOffset * nScaleY );
            p.setPen( QColor(5,250,250) );
            p.drawRect( rcOffset );
        }

        p.setPen( QColor(255,50,50) );
        p.drawRect( rc );
    }
}

#define GRAPH_DTC_RANGE_LEFT (12000)
#define GRAPH_DTC_MAX (0xffff)
#define GRAPH_DTC_RANGE_RIGHT (GRAPH_DTC_MAX-12000)
#define LIGHT_GOOD_MIN  20 // 20%
#define LIGHT_SOSO_MIN  15 // 15%
void QSideviewDialog::drawGraph(QPainter& p, const QRect& rcGraphDst)
{
    int nRangeLeft = m_pImgSideview->width() * GRAPH_DTC_RANGE_LEFT / GRAPH_DTC_MAX + 1;
    int nRangeRight = m_pImgSideview->width() * GRAPH_DTC_RANGE_RIGHT / GRAPH_DTC_MAX - 1;

    bool bGood = true;
    bool bSoso = true;

    int nCenterY = (m_pImgSideview->height()/2);
    int nOffsetY1 = (nCenterY-1) * m_pImgSideview->width();
    int nOffsetY2 = nCenterY * m_pImgSideview->width();
    int nOffsetY3 = (nCenterY+1) * m_pImgSideview->width();

    uchar* pScanLine1 = m_pImgTempBuffer+nOffsetY1;
    uchar* pScanLine2 = m_pImgTempBuffer+nOffsetY2;
    uchar* pScanLine3 = m_pImgTempBuffer+nOffsetY3;

    if ( (m_strCMModelName.indexOf("3000") >= 0) || m_bSimpleDetection )    // T3000 or simple detection
    {
        pScanLine3 = pScanLine2;
    }
    if ( m_bSimpleDetection )
        pScanLine1 = pScanLine2;

    static QPoint* lines = NULL;
    static int lineCnt = 0;
    if (!lines || lineCnt != m_pImgSideview->width())
    {
        if (lines) delete[] lines;
        lines = new QPoint[m_pImgSideview->width()];
        lineCnt = m_pImgSideview->width();
    }

    QPoint pt;
    uchar px;
    double scaleX = (double)rcGraphDst.width() / m_pImgSideview->width();
    for ( int x=0 ; x<m_pImgSideview->width() ; x++ )
    {
        px = qMax( qMax(pScanLine1[x], pScanLine2[x]), pScanLine3[x] );
        pt.setX( (int)(x * scaleX + 0.5) + rcGraphDst.left() );
        pt.setY( rcGraphDst.bottom() - px * rcGraphDst.height() / 0xff );
        lines[x] = pt;

        if ( x >= nRangeLeft && x <= nRangeRight )
        {
            if ( px < 0xff * LIGHT_GOOD_MIN / 100 )
                bGood = false;
            if ( px < 0xff * LIGHT_SOSO_MIN / 100 )
                bSoso = false;
        }
    }

    p.setPen( Qt::black );
    p.drawPolyline( lines, lineCnt );

    if (bGood)
        p.setPen(QPen(QColor(0,0,255), 3));
    else if (bSoso)
        p.setPen(QColor(0,0,255));
    else
        p.setPen(QColor(100,100,100));
    p.drawRect( rcGraphDst );
}

void QSideviewDialog::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QRect rcBody(0, 0, width()-1, height()-1);
    p.fillRect( rcBody, Qt::white );

    if (g_AppData.bIsSafeMode)
    {
        m_pMainDlg->drawSafeMode(rcBody, p);
    }

    p.save();
    QRect rcRightPanel = ui->rightPanel->geometry();

    if (m_bIsNoData)
    {
        if (m_bDrawNoData)
        {
            QLangRes& res = QLangManager::getResource();
            QString strText = res.getResString(RES_TAG, "TEXT_NO_DATA");
            int flags = Qt::AlignHCenter|Qt::AlignVCenter|Qt::TextSingleLine;
            p.setPen(QColor(255, 0, 0));
            p.drawText( rcRightPanel, flags, strText );
        }
    }
    else
    {
        if (m_pImgSideview)
        {
            QRect rcSrc = m_pImgSideview->rect();

            QRect rcImageDst = rcRightPanel;
            const int nSpaceY = 5;
            int nScaleY = ((rcImageDst.height()-nSpaceY) / 2) / rcSrc.height();
            if (nScaleY > 5) nScaleY = 5;
            if (nScaleY < 1) nScaleY = 1;
            rcImageDst.setHeight( rcSrc.height() * nScaleY );

            QRect rcGraphDst = rcImageDst;
            rcGraphDst.adjust( 0, rcImageDst.height()+nSpaceY, 0, rcImageDst.height()+nSpaceY );

            drawSideviewImage( p, rcImageDst, nScaleY );

            drawGraph( p, rcGraphDst );

            m_rcUpdateImage = rcImageDst;
            m_rcUpdateImage = m_rcUpdateImage.united( rcGraphDst );
            m_rcUpdateImage.adjust(-3, -3, 3, 3+70);
        }
    }

    p.restore();
}

void QSideviewDialog::timerEvent(QTimerEvent *evt)
{
    if (evt->timerId() == m_TimerPreviewCountDown)
    {
        if( m_nPreviewCountDown >= 0 )
        {
            QLangRes& res = QLangManager::getResource();
            QString strBtnCaption = res.getResString(MAIN_TAG, "BTN_CAPTION_CLOSE") + "("+ QString::number(m_nPreviewCountDown/2) + ")";
            ui->btnClose->setText( strBtnCaption );
        }

        if (m_nPreviewCountDown == PREVIEW_COUNT_DOWN/2)
        {
            LOG_I( "Auto select Cam2 by Timer" );
            on_btnCam2_clicked();
        }
        if (m_nPreviewCountDown == 0)
        {
            LOG_I( "Auto exit by Timer" );
            killTimer( m_TimerPreviewCountDown );
            m_TimerPreviewCountDown = 0;
            close();
        }
        m_nPreviewCountDown--;
    }
    else if (evt->timerId() == m_TimerNoData)
    {
        killTimer(m_TimerNoData);
        m_TimerNoData = 0;
        m_bDrawNoData = true;
        update();
    }
    else if (evt->timerId() == m_TimerRefreshAutoOffset)
    {
        QString strPrefix = getCameraPrefix(m_nCurrentCameraIndex);
        QT3kDevice::instance()->sendCommand( strPrefix + cstrDetectionLine + "?", true );
    }
}
#include <QDebug>
void QSideviewDialog::showEvent(QShowEvent *)
{
    if( g_AppData.bScreenShotMode ) return;

    QDesktopWidget DeskWidget;
    int nPrimary = DeskWidget.primaryScreen();
    QRect rcPrimaryMon = DeskWidget.screenGeometry( nPrimary );

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
}

void QSideviewDialog::closeEvent(QCloseEvent *evt)
{
    if (!canClose())
    {
        evt->ignore();
    }
    else
    {
        onClose();
    }
}

void QSideviewDialog::onClose()
{
    if (m_TimerPreviewCountDown)
    {
        killTimer(m_TimerPreviewCountDown);
        m_TimerPreviewCountDown = 0;
    }
    if (m_TimerNoData)
    {
        killTimer(m_TimerNoData);
        m_TimerNoData = 0;
    }
    if (m_TimerRefreshAutoOffset)
    {
        killTimer(m_TimerRefreshAutoOffset);
        m_TimerRefreshAutoOffset = 0;
    }

    QT3kDevice* pDevice = QT3kDevice::instance();
    pDevice->sendCommand( "cam1/mode=detection", true );
    pDevice->sendCommand( "cam2/mode=detection", true );
    if (g_AppData.nSubCameraCount > 0)
    {
        pDevice->sendCommand( "cam1/sub/mode=detection", true );
        pDevice->sendCommand( "cam2/sub/mode=detection", true );
    }
    m_pMainDlg->setInstantMode(T3K_HID_MODE_COMMAND);

    g_pApp->setMonitoringMouseMovement(false);
    disconnect(g_pApp, SIGNAL(mouseMoved()), this, SLOT(onGlobalMouseMoved()));
}

bool QSideviewDialog::onKeyPress(QKeyEvent */*evt*/)
{
    stopPreviewCountDown();
    return false;
}

bool QSideviewDialog::onKeyRelease(QKeyEvent *evt)
{
    if ( (evt->key() == Qt::Key_Enter) ||
         (evt->key() == Qt::Key_Return) )
    {
        QWidget* pWidget = focusWidget();
        if (pWidget && pWidget->objectName().indexOf("txtEdt") >= 0)
        {
            pWidget->clearFocus();
            return true;
        }
    }
    else if (evt->key() == Qt::Key_Left)
    {
        if (m_TimerPreviewCountDown && (m_nPreviewCountDown < (PREVIEW_COUNT_DOWN-1)) )
        {
            stopPreviewCountDown();
        }

        QPushButton* pBtns[] = {
            ui->btnCam1,
            ui->btnCam2,
            ui->btnCam1_1,
            ui->btnCam2_1
        };

        int nIdx = m_nCurrentCameraIndex -2; // - 1 - 1
        if( nIdx >= 0 )
            pBtns[nIdx]->click();
    }
    else if (evt->key() == Qt::Key_Right)
    {
        if (m_TimerPreviewCountDown && (m_nPreviewCountDown < (PREVIEW_COUNT_DOWN-1)) )
        {
            stopPreviewCountDown();
        }

        QPushButton* pBtns[] = {
            ui->btnCam1,
            ui->btnCam2,
            ui->btnCam1_1,
            ui->btnCam2_1
        };

        int nIdx = m_nCurrentCameraIndex; // - 1 + 1
        if( nIdx < (g_AppData.nSubCameraCount > 0) ? 4 : 2 )
            pBtns[nIdx]->click();
    }
    else if (evt->key() == Qt::Key_Up)
    {
        if (m_TimerPreviewCountDown && (m_nPreviewCountDown < (PREVIEW_COUNT_DOWN-1)) )
        {
            stopPreviewCountDown();
        }

        on_btnDetectLineUp_clicked();
    }
    else if (evt->key() == Qt::Key_Down)
    {
        if (m_TimerPreviewCountDown && (m_nPreviewCountDown < (PREVIEW_COUNT_DOWN-1)) )
        {
            stopPreviewCountDown();
        }

        on_btnDetectLineDn_clicked();
    }
    return false;
}

bool QSideviewDialog::onMouseWheel(QWheelEvent *evt)
{
    int delta = evt->delta();
    if (delta > 0)
    {
        LOG_I( "From Mouse Shortcut(WHEEL UP)" );
        on_btnDetectLineUp_clicked();
    }
    else
    {
        LOG_I( "From Mouse Shortcut(WHEEL DN)" );
        on_btnDetectLineDn_clicked();
    }
    return false;
}

void QSideviewDialog::onRButtonClicked()
{
    if (m_TimerPreviewCountDown && (m_nPreviewCountDown < (PREVIEW_COUNT_DOWN-1)) )
    {
        stopPreviewCountDown();
    }

    LOG_I( "From Mouse Shortcut(RBUTTON CLICK)" );

    int nNextCameraIndex = -1;
    for (int i=m_nCurrentCameraIndex+1 ; i<IDX_MAX ; i++)
    {
        if (g_AppData.cameraConnectionInfo[i])
        {
            nNextCameraIndex = i;
            break;
        }
    }

    if (nNextCameraIndex < 0)
        nNextCameraIndex = IDX_CM1;

    switch (nNextCameraIndex)
    {
    case IDX_CM1:
        on_btnCam1_clicked();
        break;
    case IDX_CM2:
        on_btnCam2_clicked();
        break;
    case IDX_CM1_1:
        on_btnCam1_1_clicked();
        break;
    case IDX_CM2_1:
        on_btnCam2_1_clicked();
        break;
    }
}

bool QSideviewDialog::onRButtonDblClicked()
{
    LOG_I( "From Mouse Shortcut(RBUTTON DOUBLE CLICK)" );
    on_btnSave_clicked();
    return true;
}

void QSideviewDialog::onGlobalMouseMoved()
{
    if (m_TimerPreviewCountDown && (m_nPreviewCountDown < (PREVIEW_COUNT_DOWN-1)) )
    {
        stopPreviewCountDown();
    }
}

void QSideviewDialog::onEditModified(QBorderStyleEdit* pEdit, int nValue, double /*dValue*/)
{
    if (pEdit == ui->txtEdtDetectLine)
    {
        QString strCam = getCameraPrefix(m_nCurrentCameraIndex);

        QString strCmd = strCam + cstrDetectionLine + QString::number(nValue);
        QT3kDevice::instance()->sendCommand(strCmd, true);

        setModifyEditColor( ui->txtEdtDetectLine );
        m_bIsModified = true;
    }
    else if (pEdit == ui->txtEdtLight1)
    {
        int nLight1 = ui->txtEdtLight1->toPlainText().toInt();
        int nLight2 = ui->txtEdtLight2->toPlainText().toInt();
        int nLight3 = ui->txtEdtLight3->toPlainText().toInt();

        nLight1 = nValue;

        adjustAmbientLight(nLight1, nLight2, nLight3);

        setModifyEditColor( ui->txtEdtLight1 );

        m_bIsModified = true;
    }
    else if (pEdit == ui->txtEdtLight2)
    {
        int nLight1 = ui->txtEdtLight1->toPlainText().toInt();
        int nLight2 = ui->txtEdtLight2->toPlainText().toInt();
        int nLight3 = ui->txtEdtLight3->toPlainText().toInt();

        nLight2 = nValue;

        adjustAmbientLight(nLight1, nLight2, nLight3);

        setModifyEditColor( ui->txtEdtLight1 );

        m_bIsModified = true;
    }
    else if (pEdit == ui->txtEdtLight3)
    {
        int nLight1 = ui->txtEdtLight1->toPlainText().toInt();
        int nLight2 = ui->txtEdtLight2->toPlainText().toInt();
        int nLight3 = ui->txtEdtLight3->toPlainText().toInt();

        nLight3 = nValue;

        adjustAmbientLight(nLight1, nLight2, nLight3);

        setModifyEditColor( ui->txtEdtLight1 );

        m_bIsModified = true;
    }
}

void QSideviewDialog::stopPreviewCountDown()
{
    if (m_TimerPreviewCountDown)
    {
        killTimer(m_TimerPreviewCountDown);
        m_TimerPreviewCountDown = 0;
        g_pApp->setMonitoringMouseMovement(false);

        QLangRes& res = QLangManager::getResource();
        ui->btnClose->setText( res.getResString(MAIN_TAG, "BTN_CAPTION_CLOSE") );
    }
}

void QSideviewDialog::reject()
{
    LOG_I( "From Keyboard(ESC)" );
    ui->btnClose->setEnabled(false);
    close();
}

void QSideviewDialog::accept()
{
    close();
}

bool QSideviewDialog::requestSensorData( RequestCmd cmd, bool bWait )
{
    setViewMode( false );
    if (ui->cmdAsyncMngr->isStarted())
    {
        ui->cmdAsyncMngr->stop();
        ui->cmdAsyncMngr->resetCommands();
    }

    switch (cmd)
    {
    case cmdInitialize:
        sensorReset();
        break;
    case cmdLoadFactoryDefault:
        sensorLoadFactoryDefault();
        break;
    case cmdRefresh:
        sensorRefresh();
        break;
    case cmdWriteToFactoryDefault:
        m_bSyncMode = true;
        if( !sensorWriteToFactoryDefault() )
        {
            m_bSyncMode = false;
            return false;
        }
        m_bSyncMode = false;
        break;
    default:
        setViewMode( true );
        return false;
    }

    QEventLoop loop;
    if (bWait)
    {
        loop.connect( ui->cmdAsyncMngr, SIGNAL(asyncFinished(bool,int)), SLOT(quit()));
    }

    ui->cmdAsyncMngr->start( bWait ? 6000 : (unsigned int)-1 );

    if (bWait)
    {
        loop.exec();
    }

    bool bResult = ui->cmdAsyncMngr->getLastResult();

    if (bResult && (cmd == cmdInitialize || cmd == cmdWriteToFactoryDefault))
    {
        resetEditColors();
        m_bIsModified = false;
    }

    return bResult;
}

void QSideviewDialog::onCmdAsyncMngrFinished(bool, int)
{
    qDebug( "onCmdAsyncMngrFinished" );
    setViewMode(true);
}

void QSideviewDialog::sensorReset()
{
    QString strSensorCmd;

    strSensorCmd = sCam1 + cstrDetectionLine;
    resetDataWithInitData( strSensorCmd );
    strSensorCmd = sCam1 + cstrAmbientLight;
    resetDataWithInitData( strSensorCmd );
    strSensorCmd = sCam2 + cstrDetectionLine;
    resetDataWithInitData( strSensorCmd );
    strSensorCmd = sCam2 + cstrAmbientLight;
    resetDataWithInitData( strSensorCmd );

    if (g_AppData.nSubCameraCount > 0)
    {
        strSensorCmd = sCam1_1 + cstrDetectionLine;
        resetDataWithInitData( strSensorCmd );
        strSensorCmd = sCam1_1 + cstrAmbientLight;
        resetDataWithInitData( strSensorCmd );
        strSensorCmd = sCam2_1 + cstrDetectionLine;
        resetDataWithInitData( strSensorCmd );
        strSensorCmd = sCam2_1 + cstrAmbientLight;
        resetDataWithInitData( strSensorCmd );
    }

    resetDataWithInitData( cstrAutoTuning, false );
}

void QSideviewDialog::sensorLoadFactoryDefault()
{
    QString strSensorCmd;

    strSensorCmd = sCam1 + cstrDetectionLine + "*";
    ui->cmdAsyncMngr->insertCommand(strSensorCmd);
    strSensorCmd = sCam1 + cstrAmbientLight + "*";
    ui->cmdAsyncMngr->insertCommand(strSensorCmd);
    strSensorCmd = sCam2 + cstrDetectionLine + "*";
    ui->cmdAsyncMngr->insertCommand(strSensorCmd);
    strSensorCmd = sCam2 + cstrAmbientLight + "*";
    ui->cmdAsyncMngr->insertCommand(strSensorCmd);

    if( g_AppData.nSubCameraCount > 0 )
    {
        strSensorCmd = sCam1_1 + cstrDetectionLine + "*";
        ui->cmdAsyncMngr->insertCommand(strSensorCmd);
        strSensorCmd = sCam1_1 + cstrAmbientLight + "*";
        ui->cmdAsyncMngr->insertCommand(strSensorCmd);
        strSensorCmd = sCam2_1 + cstrDetectionLine + "*";
        ui->cmdAsyncMngr->insertCommand(strSensorCmd);
        strSensorCmd = sCam2_1 + cstrAmbientLight + "*";
        ui->cmdAsyncMngr->insertCommand(strSensorCmd);
    }

    sensorRefresh();
}

void QSideviewDialog::sensorRefresh()
{
    QString strSensorCmd;

    QString strCameraPrefix = getCameraPrefix(m_nCurrentCameraIndex);

    strSensorCmd = strCameraPrefix + cstrFirmwareVersion + "?";
    ui->cmdAsyncMngr->insertCommand(strSensorCmd);
    strSensorCmd = strCameraPrefix + cstrDetectionLine + "?";
    ui->cmdAsyncMngr->insertCommand(strSensorCmd);
    strSensorCmd = strCameraPrefix + cstrAmbientLight + "?";
    ui->cmdAsyncMngr->insertCommand(strSensorCmd);

    strSensorCmd = QString(cstrSimpleDetection) + "?";
    ui->cmdAsyncMngr->insertCommand(strSensorCmd);
    strSensorCmd = QString(cstrAutoTuning) + "?";
    ui->cmdAsyncMngr->insertCommand(strSensorCmd);
}

bool QSideviewDialog::sensorWriteToFactoryDefault()
{
    QString strSensorCmd;

    m_strSyncCmdValue.clear();
    strSensorCmd = sCam1 + cstrDetectionLine + "?";
    int nTry = 3;
    while( nTry-- > 0 && QT3kDevice::instance()->sendCommand( strSensorCmd, false, 1000 ) == 0 )
        continue;

    if( nTry < 0 || m_strSyncCmdValue.isEmpty() )
        return false;

    strSensorCmd = sCam1 + cstrDetectionLine + "**";
    ui->cmdAsyncMngr->insertCommand(strSensorCmd);
    strSensorCmd = sCam1 + cstrDetectionLine + m_strSyncCmdValue;
    ui->cmdAsyncMngr->insertCommand(strSensorCmd);
    strSensorCmd = sCam1 + cstrDetectionLine + "!";
    ui->cmdAsyncMngr->insertCommand(strSensorCmd);

    strSensorCmd = sCam1 + cstrAmbientLight + "!";
    ui->cmdAsyncMngr->insertCommand(strSensorCmd);

    m_strSyncCmdValue.clear();
    strSensorCmd = sCam2 + cstrDetectionLine + "?";
    nTry = 3;
    while( nTry-- > 0 && QT3kDevice::instance()->sendCommand( strSensorCmd, false, 1000 ) == 0 )
        continue;

    if( nTry < 0 || m_strSyncCmdValue.isEmpty() )
        return false;

    strSensorCmd = sCam2 + cstrDetectionLine + "**";
    ui->cmdAsyncMngr->insertCommand(strSensorCmd);
    strSensorCmd = sCam2 + cstrDetectionLine + m_strSyncCmdValue;
    ui->cmdAsyncMngr->insertCommand(strSensorCmd);
    strSensorCmd = sCam2 + cstrDetectionLine + "!";
    ui->cmdAsyncMngr->insertCommand(strSensorCmd);

    strSensorCmd = sCam2 + cstrAmbientLight + "!";
    ui->cmdAsyncMngr->insertCommand(strSensorCmd);

    if( g_AppData.nSubCameraCount > 0 )
    {
        m_strSyncCmdValue.clear();
        strSensorCmd = sCam1_1 + cstrDetectionLine + "?";
        nTry = 3;
        while( nTry-- > 0 && QT3kDevice::instance()->sendCommand( strSensorCmd, false, 1000 ) == 0 )
            continue;

        if( nTry < 0 || m_strSyncCmdValue.isEmpty() )
            return false;

        strSensorCmd = sCam1_1 + cstrDetectionLine + "**";
        ui->cmdAsyncMngr->insertCommand(strSensorCmd);
        strSensorCmd = sCam1_1 + cstrDetectionLine + m_strSyncCmdValue;
        ui->cmdAsyncMngr->insertCommand(strSensorCmd);
        strSensorCmd = sCam1_1 + cstrDetectionLine + "!";
        ui->cmdAsyncMngr->insertCommand(strSensorCmd);

        strSensorCmd = sCam1_1 + cstrAmbientLight + "!";
        ui->cmdAsyncMngr->insertCommand(strSensorCmd);

        m_strSyncCmdValue.clear();
        strSensorCmd = sCam2_1 + cstrDetectionLine + "?";
        nTry = 3;
        while( nTry-- > 0 && QT3kDevice::instance()->sendCommand( strSensorCmd, false, 1000 ) == 0 )
            continue;

        if( nTry < 0 || m_strSyncCmdValue.isEmpty() )
            return false;

        strSensorCmd = sCam2_1 + cstrDetectionLine + "**";
        ui->cmdAsyncMngr->insertCommand(strSensorCmd);
        strSensorCmd = sCam2_1 + cstrDetectionLine + m_strSyncCmdValue;
        ui->cmdAsyncMngr->insertCommand(strSensorCmd);
        strSensorCmd = sCam2_1 + cstrDetectionLine + "!";
        ui->cmdAsyncMngr->insertCommand(strSensorCmd);

        strSensorCmd = sCam2_1 + cstrAmbientLight + "!";
        ui->cmdAsyncMngr->insertCommand(strSensorCmd);
    }

    return true;
}

void QSideviewDialog::resetDataWithInitData( const QString& strCmd, bool bWithFactoryDefault/*=true*/)
{
    QString strSensorCmd;
    QString strValue;
    QString strC = strCmd;
    strC = trim( strC, "=" );
    if ( QSensorInitDataCfg::instance()->isLoaded() )
    {
        if ( QSensorInitDataCfg::instance()->getValue( strC, strValue ) )
        {
            strSensorCmd = strC + "=" + strValue;
            ui->cmdAsyncMngr->insertCommand(strSensorCmd);
            if ( bWithFactoryDefault )
            {
                strSensorCmd = strC + "=!";
                ui->cmdAsyncMngr->insertCommand(strSensorCmd);
            }
            return;
        }
    }

    if ( bWithFactoryDefault )
    {
        strSensorCmd = strC + "=**";
    }
    else
    {
        strSensorCmd = strC + "=*";
    }
    ui->cmdAsyncMngr->insertCommand(strSensorCmd);
}

void QSideviewDialog::resetEditColors()
{
    ui->txtEdtLight1->setColor(s_clrNormalBorderColor, s_clrNormalBgColor );
    ui->txtEdtLight2->setColor(s_clrNormalBorderColor, s_clrNormalBgColor );
    ui->txtEdtLight3->setColor(s_clrNormalBorderColor, s_clrNormalBgColor );

    ui->txtEdtDetectLine->setColor(s_clrNormalBorderColor, s_clrNormalBgColor );

    ui->txtEdtLight1->setFloatStyle(false);
    ui->txtEdtLight2->setFloatStyle(false);
    ui->txtEdtLight3->setFloatStyle(false);

    ui->txtEdtDetectLine->setFloatStyle(false);

    ui->txtEdtLight1->update();
    ui->txtEdtLight2->update();
    ui->txtEdtLight3->update();
    ui->txtEdtDetectLine->update();
}

void QSideviewDialog::setModifyEditColor(QBorderStyleEdit* pEdit)
{
    pEdit->setColor(s_clrModifyBorderColor, s_clrModifyBgColor);
    pEdit->update();
}

void QSideviewDialog::setModifyAllEditColors()
{
    QBorderStyleEdit* edits[] = {
        ui->txtEdtDetectLine, ui->txtEdtLight1, ui->txtEdtLight2, ui->txtEdtLight3
    };
    for ( int i=0 ; i<(int)(sizeof(edits)/sizeof(QBorderStyleEdit*)) ; i++ )
    {
        edits[i]->setColor(s_clrModifyBorderColor, s_clrModifyBgColor);
        edits[i]->update();
    }
}

void QSideviewDialog::enableAllControls( bool bEnable )
{
    setEnabled(bEnable);
}

void QSideviewDialog::initImageBuffer( int width, int height )
{
    if (m_pImgTempBuffer)
        delete m_pImgTempBuffer;
    m_pImgTempBuffer = new uchar[width * height];

    memset( m_pImgTempBuffer, 0, sizeof(uchar)*width*height );

    m_pImgSideview = new QImage( width, height, QImage::Format_RGB32 );
    m_pImgSideview->fill(QColor(0,0,0));
}

void QSideviewDialog::TPDP_OnPRV(T3K_DEVICE_INFO /*devInfo*/, ResponsePart Part, unsigned short /*ticktime*/, const char */*partid*/, int total, int offset, const unsigned char *data, int cnt)
{
    if (m_bIsNoData)
    {
        int nCameraIndex = getIndexFromPart(Part);
        if ( m_nCurrentCameraIndex == nCameraIndex )
        {
            m_bIsNoData = false;
            if (m_TimerNoData)
            {
                killTimer(m_TimerNoData);
                m_TimerNoData = 0;
            }
            ui->btnDetectLineDn->setEnabled(true);
            ui->btnDetectLineUp->setEnabled(true);
            ui->txtEdtDetectLine->setEnabled(true);
            ui->btnLight1Dec->setEnabled(true);
            ui->btnLight1Inc->setEnabled(true);
            ui->txtEdtLight1->setEnabled(true);
            ui->btnLight2Dec->setEnabled(true);
            ui->btnLight2Inc->setEnabled(true);
            ui->txtEdtLight2->setEnabled(true);
            ui->btnLight3Dec->setEnabled(true);
            ui->btnLight3Inc->setEnabled(true);
            ui->txtEdtLight3->setEnabled(true);
            ui->lblAmbientLights->setEnabled(true);
            ui->lblDetectionLine->setEnabled(true);
            update();
        }
    }

    if ( m_pImgSideview && ((m_pImgSideview->width() != cnt) || (m_pImgSideview->height() != total)) )
    {
        delete m_pImgSideview;
        m_pImgSideview = NULL;
    }

    if (!m_pImgSideview)
    {
        initImageBuffer( cnt, total );
    }

    bool bUpdate = false;

#ifndef Q_OS_LINUX
    if (offset == 0)
#endif
        bUpdate = true;

    offset += total/2;
    if ( offset >= 0 && offset < total )
    {
        int nOffsetY = offset * cnt;
        memcpy( m_pImgTempBuffer + nOffsetY, data, cnt );
    }

    if (bUpdate)
    {
        int nW = cnt;
        int nH = total;
        int nOffsetY = 0;
        for ( int y=0 ; y<nH ; y++ )
        {
            QRgb* pRgbLine = (QRgb*)m_pImgSideview->scanLine(y);
            nOffsetY = y * nW;
            int g;
            for (int i=0 ; i<m_pImgSideview->width(); i++ )
            {
                g = m_pImgTempBuffer[nOffsetY+i];
                pRgbLine[i] = qRgb(g, g, g);
            }
        }
        if (m_rcUpdateImage.isEmpty())
            update();
        else
            update(m_rcUpdateImage);
    }
}

void QSideviewDialog::TPDP_OnRSP(T3K_DEVICE_INFO /*devInfo*/, ResponsePart Part, unsigned short /*ticktime*/, const char */*partid*/, int /*id*/, bool /*bFinal*/, const char *cmd)
{
    int nCameraIndex = getIndexFromPart(Part);
    if ( strstr(cmd, cstrModeSideview) == cmd )
    {
        if ( nCameraIndex != m_nCurrentCameraIndex )
        {
            ui->btnCam1->setChecked(false);
            ui->btnCam2->setChecked(false);
            ui->btnCam1_1->setChecked(false);
            ui->btnCam2_1->setChecked(false);
            switch (nCameraIndex)
            {
            case IDX_CM1:
                ui->btnCam1->setChecked(true);
                break;
            case IDX_CM2:
                ui->btnCam2->setChecked(true);
                break;
            case IDX_CM1_1:
                ui->btnCam1_1->setChecked(true);
                break;
            case IDX_CM2_1:
                ui->btnCam2_1->setChecked(true);
                break;
            }
            setSideview(nCameraIndex, false);
        }
    }

    if ( nCameraIndex == m_nCurrentCameraIndex || m_bSyncMode )
    {
        if ( strstr(cmd, cstrDetectionLine) == cmd )
        {
            cmd += sizeof(cstrDetectionLine) - 1;
            int nDetectionLine = atoi(cmd);
            const char * szC = strstr(cmd, ",");
            if ( szC )
                m_nAutoOffset = atoi(szC + 1);
            else
                m_nAutoOffset = 0;

            //if ( m_nDetectionLine != nDetectionLine )
            {
                m_nDetectionLine = nDetectionLine;
                if (!ui->txtEdtDetectLine->isEditing())
                    ui->txtEdtDetectLine->setText( QString::number(m_nDetectionLine) );
            }
            m_strSyncCmdValue = QString::number(m_nDetectionLine);
        }
        else if ( strstr(cmd, cstrAmbientLight) == cmd )
        {
            cmd += sizeof(cstrAmbientLight) - 1;
            const char * value2 = strchr(cmd, ',') + 1;
            int n1 = atoi(cmd);
            int n2 = atoi(value2);
            value2 = strchr(value2, ',') + 1;
            int n3 = atoi(value2);

            if (!ui->txtEdtLight1->isEditing())
                ui->txtEdtLight1->setText( QString::number(n1) );
            if (!ui->txtEdtLight2->isEditing())
                ui->txtEdtLight2->setText( QString::number(n2) );
            if (!ui->txtEdtLight3->isEditing())
                ui->txtEdtLight3->setText( QString::number(n3) );
        }
        else if ( strstr(cmd, cstrFirmwareVersion) == cmd )
        {
            char szVer[255];
            strncpy( szVer, cmd + sizeof(cstrFirmwareVersion) - 1, 255 );

            QString strCMVer = szVer;

            int nS = strCMVer.indexOf(' ');
            if (nS >= 0)
            {
                m_strCMModelName = strCMVer.right( strCMVer.size() - 1 - nS );
                update();
            }
        }
    }

    if ( strstr(cmd, cstrSimpleDetection) == cmd )
    {
        const char* pC = cmd + sizeof(cstrSimpleDetection) - 1;
        int nI = atoi(pC);
        m_bSimpleDetection = (nI == 1 ? true : false);
    }
    else if ( strstr(cmd, cstrAutoTuning) == cmd )
    {
        cmd += sizeof(cstrAutoTuning) - 1;
        int n1 = atoi(cmd);

        bool bAutoOffset = (n1 == 1 ? true : false);
        if (m_TimerRefreshAutoOffset)
        {
            killTimer(m_TimerRefreshAutoOffset);
            m_TimerRefreshAutoOffset = 0;
        }
        if (bAutoOffset)
        {
            m_TimerRefreshAutoOffset = startTimer(4000);
        }
    }
}

void QSideviewDialog::TPDP_OnMSG(T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short /*ticktime*/, const char *partid, const char *txt)
{
    if ( strstr(partid, "TXT") == partid )
    {
        if ( strstr(txt, "expired mode") == txt )
        {
            LOG_I( "sensor expired mode => reset sideview" );
            qDebug( "sensor expired mode => reset sideview" );
            setSideview( m_nCurrentCameraIndex, true );
        }
    }
}

void QSideviewDialog::on_btnClose_clicked()
{
    LOG_B( "Exit" );
    ui->btnClose->setEnabled(false);
    close();
}

void QSideviewDialog::on_btnReset_clicked()
{
    LOG_B( "Reset" );

    QLangRes& res = QLangManager::getResource();
    QString strPrompt = res.getResString( MAIN_TAG, "TEXT_WARNING_RESET_ALL_VALUES" );
    QString strTitle = res.getResString( MAIN_TAG, "TEXT_WARNING_MESSAGE_TITLE" );
    int nRet = showMessageBox( this,
        strPrompt,
        strTitle,
        QMessageBox::Warning, QMessageBox::Yes|QMessageBox::No, QMessageBox::No );

    if (nRet != QMessageBox::Yes)
        return;

    enableAllControls( false );
    if ( !requestSensorData(cmdInitialize, true) )
    {
        strPrompt = res.getResString( MAIN_TAG, "TEXT_ERROR_RESET_ALL_VALUES" );
        strTitle = res.getResString( MAIN_TAG, "TEXT_ERROR_MESSAGE_TITLE" );
        nRet = showMessageBox( this,
            strPrompt,
            strTitle,
            QMessageBox::Critical, QMessageBox::Ok, QMessageBox::Ok );

        enableAllControls( true );
        ui->btnReset->setFocus();
        return;
    }
    enableAllControls( true );
    ui->btnReset->setFocus();
}

void QSideviewDialog::on_btnRefresh_clicked()
{
    LOG_B( "Refresh" );

    requestSensorData( cmdRefresh, false );
}

void QSideviewDialog::on_btnSave_clicked()
{
    LOG_B( "Save" );

    setEnabled(false);
    setModifyAllEditColors();

    enableAllControls(false);

    if (!requestSensorData(cmdWriteToFactoryDefault, true))
    {
        QLangRes& res = QLangManager::getResource();
        QString strPrompt = res.getResString( MAIN_TAG, "TEXT_ERROR_WRITE_FACTORY_DEFAULT_FAILURE" );
        QString strTitle = res.getResString( MAIN_TAG, "TEXT_ERROR_MESSAGE_TITLE" );
        showMessageBox( this,
            strPrompt,
            strTitle,
            QMessageBox::Critical, QMessageBox::Ok, QMessageBox::Ok );

        enableAllControls( true );
        setEnabled( true );
        ui->btnSave->setFocus();
        return;
    }
    enableAllControls( true );

    setEnabled( true );
    ui->btnSave->setFocus();
    close();
}

void QSideviewDialog::on_btnCam1_clicked()
{
    LOG_B( "Cam1" );
    ui->btnCam1->setFocus();
    ui->btnCam1->setChecked(true);
    ui->btnCam2->setChecked(false);
    ui->btnCam1_1->setChecked(false);
    ui->btnCam2_1->setChecked(false);

    if (m_nCurrentCameraIndex != IDX_CM1)
        setSideview(IDX_CM1, true);
}

void QSideviewDialog::on_btnCam2_clicked()
{
    LOG_B( "Cam2" );
    ui->btnCam2->setFocus();
    ui->btnCam1->setChecked(false);
    ui->btnCam2->setChecked(true);
    ui->btnCam1_1->setChecked(false);
    ui->btnCam2_1->setChecked(false);

    if (m_nCurrentCameraIndex != IDX_CM2)
        setSideview(IDX_CM2, true);
}

void QSideviewDialog::on_btnCam1_1_clicked()
{
    LOG_B( "Cam1-1" );
    ui->btnCam1_1->setFocus();
    ui->btnCam1->setChecked(false);
    ui->btnCam2->setChecked(false);
    ui->btnCam1_1->setChecked(true);
    ui->btnCam2_1->setChecked(false);

    if (m_nCurrentCameraIndex != IDX_CM1_1)
        setSideview(IDX_CM1_1, true);
}

void QSideviewDialog::on_btnCam2_1_clicked()
{
    LOG_B( "Cam2-1" );
    ui->btnCam2_1->setFocus();
    ui->btnCam1->setChecked(false);
    ui->btnCam2->setChecked(false);
    ui->btnCam1_1->setChecked(false);
    ui->btnCam2_1->setChecked(true);

    if (m_nCurrentCameraIndex != IDX_CM2_1)
        setSideview(IDX_CM2_1, true);
}

void QSideviewDialog::on_btnDetectLineUp_clicked()
{
    LOG_B( "Detection Line \"+\"" );

    int nDetectionLine = ui->txtEdtDetectLine->toPlainText().toInt();
    nDetectionLine++;

    QString strCam = getCameraPrefix(m_nCurrentCameraIndex);

    QString strCmd = strCam + cstrDetectionLine + QString::number(nDetectionLine);
    QT3kDevice::instance()->sendCommand(strCmd, true);

    setModifyEditColor( ui->txtEdtDetectLine );

    m_bIsModified = true;
}

void QSideviewDialog::on_btnDetectLineDn_clicked()
{
    LOG_B( "Detection Line \"-\"" );

    int nDetectionLine = ui->txtEdtDetectLine->toPlainText().toInt();
    nDetectionLine--;

    QString strCam = getCameraPrefix(m_nCurrentCameraIndex);

    QString strCmd = strCam + cstrDetectionLine + QString::number(nDetectionLine);
    QT3kDevice::instance()->sendCommand(strCmd, true);

    setModifyEditColor( ui->txtEdtDetectLine );

    m_bIsModified = true;
}

void QSideviewDialog::adjustAmbientLight(int nLight1, int nLight2, int nLight3)
{
    QT3kDevice* pDevice = QT3kDevice::instance();
    QString strCmd;

    strCmd = sCam1 + cstrAmbientLight + QString::number(nLight1) + "," + QString::number(nLight2) + "," + QString::number(nLight3);
    pDevice->sendCommand( strCmd );
    strCmd = sCam2 + cstrAmbientLight + QString::number(nLight1) + "," + QString::number(nLight2) + "," + QString::number(nLight3);
    pDevice->sendCommand( strCmd );
    if (g_AppData.nSubCameraCount > 0)
    {
        strCmd = sCam1_1 + cstrAmbientLight + QString::number(nLight1) + "," + QString::number(nLight2) + "," + QString::number(nLight3);
        pDevice->sendCommand( strCmd );
        strCmd = sCam2_1 + cstrAmbientLight + QString::number(nLight1) + "," + QString::number(nLight2) + "," + QString::number(nLight3);
        pDevice->sendCommand( strCmd );
    }
}

void QSideviewDialog::on_btnLight1Dec_clicked()
{
    LOG_B( "Ambient Light \"-\"" );

    int nLight1 = ui->txtEdtLight1->toPlainText().toInt();
    int nLight2 = ui->txtEdtLight2->toPlainText().toInt();
    int nLight3 = ui->txtEdtLight3->toPlainText().toInt();

    if ( nLight1 <= 15 )
        nLight1 -= 1;
    else if ( nLight1 <= 50 )
        nLight1 -= 5;
    else
        nLight1 -= 10;
    if ( nLight1 < 0 )
        nLight1 = 0;

    adjustAmbientLight(nLight1, nLight2, nLight3);

    setModifyEditColor( ui->txtEdtLight1 );

    m_bIsModified = true;
}

void QSideviewDialog::on_btnLight1Inc_clicked()
{
    LOG_B( "Ambient Light \"+\"" );

    int nLight1 = ui->txtEdtLight1->toPlainText().toInt();
    int nLight2 = ui->txtEdtLight2->toPlainText().toInt();
    int nLight3 = ui->txtEdtLight3->toPlainText().toInt();

    if ( nLight1 < 15 )
        nLight1 += 1;
    else if ( nLight1 < 50 )
        nLight1 += 5;
    else
        nLight1 += 10;
    if ( nLight1 > 100 )
        nLight1 = 100;

    adjustAmbientLight(nLight1, nLight2, nLight3);

    setModifyEditColor( ui->txtEdtLight1 );

    m_bIsModified = true;
}

void QSideviewDialog::on_btnLight2Dec_clicked()
{
    LOG_B( "Ambient Light2 \"-\"" );

    int nLight1 = ui->txtEdtLight1->toPlainText().toInt();
    int nLight2 = ui->txtEdtLight2->toPlainText().toInt();
    int nLight3 = ui->txtEdtLight3->toPlainText().toInt();

    if ( nLight2 <= 15 )
        nLight2 -= 1;
    else if ( nLight2 <= 50 )
        nLight2 -= 5;
    else
        nLight2 -= 10;
    if ( nLight2 < 0 )
        nLight2 = 0;

    adjustAmbientLight(nLight1, nLight2, nLight3);

    setModifyEditColor( ui->txtEdtLight2 );

    m_bIsModified = true;
}

void QSideviewDialog::on_btnLight2Inc_clicked()
{
    LOG_B( "Ambient Light2 \"+\"" );

    int nLight1 = ui->txtEdtLight1->toPlainText().toInt();
    int nLight2 = ui->txtEdtLight2->toPlainText().toInt();
    int nLight3 = ui->txtEdtLight3->toPlainText().toInt();

    if ( nLight2 < 15 )
        nLight2 += 1;
    else if ( nLight2 < 50 )
        nLight2 += 5;
    else
        nLight2 += 10;
    if ( nLight2 > 100 )
        nLight2 = 100;

    adjustAmbientLight(nLight1, nLight2, nLight3);

    setModifyEditColor( ui->txtEdtLight2 );

    m_bIsModified = true;
}

void QSideviewDialog::on_btnLight3Dec_clicked()
{
    LOG_B( "Ambient Light3 \"-\"" );

    int nLight1 = ui->txtEdtLight1->toPlainText().toInt();
    int nLight2 = ui->txtEdtLight2->toPlainText().toInt();
    int nLight3 = ui->txtEdtLight3->toPlainText().toInt();

    if ( nLight3 <= 15 )
        nLight3 -= 1;
    else if ( nLight3 <= 50 )
        nLight3 -= 5;
    else
        nLight3 -= 10;
    if ( nLight3 < 0 )
        nLight3 = 0;

    adjustAmbientLight(nLight1, nLight2, nLight3);

    setModifyEditColor( ui->txtEdtLight3 );

    m_bIsModified = true;
}

void QSideviewDialog::on_btnLight3Inc_clicked()
{
    LOG_B( "Ambient Light3 \"+\"" );

    int nLight1 = ui->txtEdtLight1->toPlainText().toInt();
    int nLight2 = ui->txtEdtLight2->toPlainText().toInt();
    int nLight3 = ui->txtEdtLight3->toPlainText().toInt();

    if ( nLight3 < 15 )
        nLight3 += 1;
    else if ( nLight3 < 50 )
        nLight3 += 5;
    else
        nLight3 += 10;
    if ( nLight3 > 100 )
        nLight3 = 100;

    adjustAmbientLight(nLight1, nLight2, nLight3);

    setModifyEditColor( ui->txtEdtLight3 );

    m_bIsModified = true;
}

void QSideviewDialog::on_btnRemoteSideview_clicked()
{
    bool bRemoteShareSideview = ui->btnRemoteSideview->isChecked();
    LOG_B( "[Remote] Share Sideview: %s", bRemoteShareSideview ? "true" : "false" );

    QT3kDevice* pDevice = QT3kDevice::instance();
    if ( pDevice->isVirtualDevice() )
    {
        FeatureScreenMode feature;
        feature.ReportID = REPORTID_FEATURE_SCREEN_MODE;
        feature.ScreenMode = bRemoteShareSideview ? 1 : 0;
        pDevice->setFeature( &feature, sizeof(FeatureScreenMode) );
    }
}
