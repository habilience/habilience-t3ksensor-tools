#include "QTouchSettingDialog.h"
#include "ui_QTouchSettingDialog.h"
#include "dialog.h"
#include <QPainter>
#include <QCloseEvent>
#include <QDesktopWidget>
#include "QT3kDevice.h"

#include "QLogSystem.h"
#include "QShowMessageBox.h"
#include "T3kConstStr.h"
#include "QUtils.h"
#include "QSensorInitDataCfg.h"
#include "QGestureProfileDialog.h"

#include "conf.h"

#define NV_DEF_TIME_A              400
#define NV_DEF_TIME_A_RANGE_START  100
#define NV_DEF_TIME_A_RANGE_END    800
    // time_l
#define NV_DEF_TIME_L              1000
#define NV_DEF_TIME_L_RANGE_START  800
#define NV_DEF_TIME_L_RANGE_END    3000

#define NV_DEF_WHEEL_SENSITIVITY					(-0xff/4)
#define NV_DEF_WHEEL_SENSITIVITY_RANGE_START		(-0xff)
#define NV_DEF_WHEEL_SENSITIVITY_RANGE_END			(0xff)

#define NV_DEF_ZOOM_SENSITIVITY						(0xff/4)
#define NV_DEF_ZOOM_SENSITIVITY_RANGE_START			(-0xff)
#define NV_DEF_ZOOM_SENSITIVITY_RANGE_END			(0xff)

QTouchSettingDialog::QTouchSettingDialog(Dialog *parent) :
    QDialog(parent), m_pMainDlg(parent),
    ui(new Ui::QTouchSettingDialog),
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

    LOG_I( "Enter [Touch Settings]" );
    onChangeLanguage();

    m_EventRedirect.installEventListener(this);
    installEventFilter(&m_EventRedirect);

    m_bIsModified = false;

    ui->horzSliderTap->setRange( NV_DEF_TIME_A_RANGE_START/100, NV_DEF_TIME_A_RANGE_END/100 );
    ui->horzSliderTap->setTickInterval(1);
    ui->horzSliderTap->setValue( NV_DEF_TIME_A/100 );
    ui->horzSliderLongTap->setRange( NV_DEF_TIME_L_RANGE_START/100, NV_DEF_TIME_L_RANGE_END/100 );
    ui->horzSliderLongTap->setTickInterval(1);
    ui->horzSliderLongTap->setValue( NV_DEF_TIME_L/100 );
    ui->horzSliderWheel->setRange( NV_DEF_WHEEL_SENSITIVITY_RANGE_START, NV_DEF_WHEEL_SENSITIVITY_RANGE_END );
    ui->horzSliderWheel->setTickInterval(1);
    ui->horzSliderWheel->setValue( NV_DEF_WHEEL_SENSITIVITY );
    ui->horzSliderZoom->setRange( NV_DEF_ZOOM_SENSITIVITY_RANGE_START, NV_DEF_ZOOM_SENSITIVITY_RANGE_END );
    ui->horzSliderZoom->setTickInterval(1);
    ui->horzSliderZoom->setValue( NV_DEF_ZOOM_SENSITIVITY );

    ui->btnFull->setEnabled(false);
    ui->btnDigitizerOnly->setEnabled(false);

    resetEditColors();

    //QRect rcBody = ui->frameTM->frameGeometry();
    QRect rcBody = ui->frameTM->geometry();
    rcBody.adjust( -rcBody.left(), -rcBody.top(), -rcBody.left(), -rcBody.top() );

    QRect rcEdit = ui->txtEdtTMLeft->geometry();
    QRect rcBtn = ui->btnTMLeftDec->geometry();
    int nOffsetX = rcEdit.width() + rcBtn.width()/2 + 1;
    int nOffsetY = rcEdit.height() + 1;

    rcBody.adjust( nOffsetX, nOffsetY, -nOffsetX, -nOffsetY );
    ui->txtEdtTMLeft->move( rcBody.left()-rcEdit.width()/2, rcBody.center().y()-rcEdit.height()/2 );
    ui->txtEdtTMLeft->resize( rcEdit.width(), rcEdit.height() );
    rcEdit = ui->txtEdtTMLeft->geometry();
    ui->btnTMLeftDec->move( rcEdit.left()-rcBtn.width(), rcEdit.top() );
    ui->btnTMLeftDec->resize( rcBtn.width(), rcBtn.height() );
    ui->btnTMLeftInc->move( rcEdit.right(), rcEdit.top() );
    ui->btnTMLeftInc->resize( rcBtn.width(), rcBtn.height() );

    ui->txtEdtTMRight->move( rcBody.right()-rcEdit.width()/2, rcBody.center().y()-rcEdit.height()/2 );
    ui->txtEdtTMRight->resize( rcEdit.width(), rcEdit.height() );
    rcEdit = ui->txtEdtTMRight->geometry();
    ui->btnTMRightDec->move( rcEdit.left()-rcBtn.width(), rcEdit.top() );
    ui->btnTMRightDec->resize( rcBtn.width(), rcBtn.height() );
    ui->btnTMRightInc->move( rcEdit.right(), rcEdit.top() );
    ui->btnTMRightInc->resize( rcBtn.width(), rcBtn.height() );

    ui->txtEdtTMTop->move( rcBody.center().x()-rcEdit.width()/2, rcBody.top()-rcEdit.height()*3/4 );
    ui->txtEdtTMTop->resize( rcEdit.width(), rcEdit.height() );
    rcEdit = ui->txtEdtTMTop->geometry();
    ui->btnTMTopDec->move( rcEdit.left()-rcBtn.width(), rcEdit.top() );
    ui->btnTMTopDec->resize( rcBtn.width(), rcBtn.height() );
    ui->btnTMTopInc->move( rcEdit.right(), rcEdit.top() );
    ui->btnTMTopInc->resize( rcBtn.width(), rcBtn.height() );

    ui->txtEdtTMBottom->move( rcBody.center().x()-rcEdit.width()/2, rcBody.bottom()-rcEdit.height()*1/4 );
    ui->txtEdtTMBottom->resize( rcEdit.width(), rcEdit.height() );
    rcEdit = ui->txtEdtTMBottom->geometry();
    ui->btnTMBottomDec->move( rcEdit.left()-rcBtn.width(), rcEdit.top() );
    ui->btnTMBottomDec->resize( rcBtn.width(), rcBtn.height() );
    ui->btnTMBottomInc->move( rcEdit.right(), rcEdit.top() );
    ui->btnTMBottomInc->resize( rcBtn.width(), rcBtn.height() );

    ui->cmdAsyncMngr->setT3kDevice( pDevice );

    connect( ui->cmdAsyncMngr, SIGNAL(asyncFinished(bool,int)), SLOT(onCmdAsyncMngrFinished(bool,int)));

    setViewMode( true );

    requestSensorData( cmdLoadFactoryDefault, false );

    QBorderStyleEdit* edits[] = {
        ui->txtEdtTMLeft,
        ui->txtEdtTMTop,
        ui->txtEdtTMRight,
        ui->txtEdtTMBottom,
        ui->txtEdtTap,
        ui->txtEdtLongTap,
        ui->txtEdtWheel,
        ui->txtEdtZoom
    };

    for (int i=0 ; i<(int)(sizeof(edits)/sizeof(QBorderStyleEdit*)) ; i++)
    {
        if (i < 4)
        {
            edits[i]->setAlignment(Qt::AlignCenter);
            edits[i]->setMaxTextLength(4);
            edits[i]->setFloatStyle(true);
        }
        else
        {
            edits[i]->setAlignment(Qt::AlignRight);
            edits[i]->setMaxTextLength(3);
            edits[i]->setFloatStyle(false);
        }

        edits[i]->setText("");

        edits[i]->installEventFilter(&m_EventRedirect);

        connect( edits[i], SIGNAL(editModified(QBorderStyleEdit*,int,double)), SLOT(onEditModified(QBorderStyleEdit*,int,double)) );
    }

    ui->btnMouse->setEnabled(true);
    ui->btnMultitouch->setEnabled(true);

    m_pMainDlg->setInstantMode( T3K_HID_MODE_COMMAND|T3K_HID_MODE_TOUCHPNT );

    ui->btnClose->setFocus();
}

QTouchSettingDialog::~QTouchSettingDialog()
{
    m_pMainDlg->onCloseMenu();
    LOG_I( "Exit [Touch Settings]" );
    delete ui;
}

void QTouchSettingDialog::setViewMode( bool bViewMode )
{
    qDebug( "setViewMode: %s", bViewMode ? "true" : "false" );
    int nMode = T3K_HID_MODE_COMMAND;

    if (bViewMode)
        nMode |= T3K_HID_MODE_TOUCHPNT;

    qDebug( "setViewMode: %x", nMode );

    m_pMainDlg->setInstantMode( nMode );
}

#define MAIN_TAG    "MAIN"
#define RES_TAG     "TOUCH SETTING"
void QTouchSettingDialog::onChangeLanguage()
{
    QLangRes& res = QLangManager::getResource();

    static bool s_bIsR2L = false;
    bool bIsR2L = res.isR2L();
    if ( bIsR2L )
        setLayoutDirection( Qt::RightToLeft );
    else
        setLayoutDirection( Qt::LeftToRight );

    setWindowTitle( res.getResString(MAIN_TAG, "BTN_CAPTION_TOUCH_SETTING") );

    ui->lblTouchAreaMarginTitle->setText( res.getResString(RES_TAG, "TEXT_TOUCH_AREA_MARGIN") );
    ui->lblTimeSettingTitle->setText( res.getResString(RES_TAG, "TEXT_TIME_SETTING") );
    ui->lblWheelZoomTitle->setText( res.getResString(RES_TAG, "TEXT_WHEEL_ZOOM_SENS_SETTING") );
    ui->lblInputModeTitle->setText( res.getResString(RES_TAG, "TEXT_INPUT_MODE_SETTING") );
    ui->lblUsbCfgModeTitle->setText( res.getResString(RES_TAG, "TEXT_USB_CONFIG_MODE_SETTING") );
    ui->lblAreaSettingTitle->setText( res.getResString(RES_TAG, "TEXT_AREA_SETTING") );

    ui->lblTap->setText( res.getResString(RES_TAG, "TEXT_TAP") );
    ui->lblLongTap->setText( res.getResString(RES_TAG, "TEXT_LONG_TAP") );
    ui->lblWheel->setText( res.getResString(RES_TAG, "TEXT_WHEEL") );
    ui->lblZoom->setText( res.getResString(RES_TAG, "TEXT_ZOOM") );
    ui->gbAutoSelect->setTitle( res.getResString(RES_TAG, "BTN_CAPTION_INPUT_MODE_AUTO_SELECT") );
    ui->btnMouse->setText( res.getResString(RES_TAG, "BTN_CAPTION_INPUT_MODE_MOUSE") );
    ui->btnMultitouch->setText( res.getResString(RES_TAG, "BTN_CAPTION_INPUT_MODE_MULTITOUCH") );
    ui->btnFull->setText( res.getResString(RES_TAG, "BTN_CAPTION_USB_CFG_FULL") );
    ui->btnDigitizerOnly->setText( res.getResString(RES_TAG, "BTN_CAPTION_USB_CFG_DIGITIZER_ONLY") );
    ui->btnGestureProfile->setText( res.getResString(RES_TAG, "BTN_CAPTION_GESTURE_PROFILE") );

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

bool QTouchSettingDialog::canClose()
{
    if (m_bIsModified || ui->widgetAreaSetting->isModified())
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

void QTouchSettingDialog::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QRect rcBody(0, 0, width(), height());
    p.fillRect( rcBody, Qt::white );

    drawTouchMargin( p );

    if (g_AppData.bIsSafeMode)
    {
        m_pMainDlg->drawSafeMode(rcBody, p);
    }
}

void QTouchSettingDialog::drawTouchMargin( QPainter& p )
{
    QPoint pos = ui->frameTM->parentWidget()->mapToGlobal(ui->frameTM->pos());
    pos = mapFromGlobal(pos);

    QRect rcScreen(pos, ui->frameTM->size());

    rcScreen.adjust( +10, 0, -10, 0 );
    QRect rcBody = rcScreen;

    QRect rcEdit = ui->txtEdtTMLeft->geometry();

    int nOffsetX = rcEdit.width() + 3;
    int nOffsetY = rcEdit.height() * 4 / 3 + 3;

    const int nOffsetXY = 15;

    rcScreen.adjust( nOffsetX, nOffsetY, -nOffsetX, -nOffsetY );

    QPen outlinePen( QBrush(QColor(154,183,222)), 2.f );
    QPen innerlinePen( QColor(255,255,255) );
    QBrush bodyBrush( QColor(221,231,244) );

    p.setBrush( bodyBrush );
    p.setPen( outlinePen );
    p.drawRect( rcScreen );

    p.setPen( innerlinePen );
    rcScreen.adjust( 1, 1, -2, -2 );
    p.drawRect( rcScreen );

    rcScreen.adjust( -1, -1, 2, 2 );

    p.setRenderHint(QPainter::Antialiasing);

    drawHorzArrow( p, QPoint(rcBody.left(), rcScreen.top() + nOffsetXY), QPoint(rcScreen.left(), rcScreen.top() + nOffsetXY),
                   QPoint(rcBody.left(), rcScreen.top()), QPoint(rcBody.left(), rcScreen.bottom()) );
    drawHorzArrow( p, QPoint(rcScreen.right(), rcScreen.top() + nOffsetXY), QPoint(rcBody.right(), rcScreen.top() + nOffsetXY),
                   QPoint(rcBody.right(), rcScreen.top()), QPoint(rcBody.right(), rcScreen.bottom()) );
    drawVertArrow( p, QPoint(rcScreen.left() + nOffsetXY, rcBody.top()), QPoint(rcScreen.left() + nOffsetXY, rcScreen.top()),
                   QPoint(rcScreen.left(), rcBody.top()), QPoint(rcScreen.right(), rcBody.top()) );
    drawVertArrow( p, QPoint(rcScreen.left() + nOffsetXY, rcScreen.bottom()), QPoint(rcScreen.left() + nOffsetXY, rcBody.bottom()),
                   QPoint(rcScreen.left(), rcBody.bottom()), QPoint(rcScreen.right(), rcBody.bottom()) );

    //p.setBackgroundMode( Qt::TransparentMode );
    p.setPen( QColor(52, 98, 160) );
    QLangRes& res = QLangManager::getResource();
    QString strScreen = res.getResString( RES_TAG, "TEXT_SCREEN" );
    int flags = Qt::AlignCenter|Qt::AlignVCenter|Qt::TextSingleLine;
    p.drawText( rcBody, flags, strScreen );
}

void QTouchSettingDialog::drawHorzArrow( QPainter& p, QPoint pt1, QPoint pt2, QPoint pt3, QPoint pt4 )
{
    QColor clrArrow(120,120,120);
    QBrush arrowBrush( clrArrow );
    QPen arrowPen( QBrush(clrArrow), 1.f, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin );
    QPen guidePen( QBrush(QColor(100,100,100)), 1.f, Qt::DotLine );

    p.setPen( arrowPen );
    p.setBrush( arrowBrush );

    p.drawLine( pt1, pt2 );
    const int nHeadSize = 5;

    // Draw the arrow head
    QPainterPath path1;
    path1.moveTo( pt1 );
    path1.lineTo( pt1.x()+nHeadSize, pt1.y()+nHeadSize*2.f/3 );
    path1.lineTo( pt1.x()+nHeadSize, pt1.y()-nHeadSize*2.f/3 );
    path1.lineTo( pt1 );

    p.drawPath( path1 );

    QPainterPath path2;

    path2.moveTo( pt2 );
    path2.lineTo( pt2.x()-nHeadSize, pt2.y()+nHeadSize*2.f/3 );
    path2.lineTo( pt2.x()-nHeadSize, pt2.y()-nHeadSize*2.f/3 );
    path2.lineTo( pt2 );

    p.drawPath( path2 );

    p.setPen( guidePen );
    p.drawLine( pt3, pt4 );
}

void QTouchSettingDialog::drawVertArrow( QPainter& p, QPoint pt1, QPoint pt2, QPoint pt3, QPoint pt4 )
{
    QColor clrArrow(120,120,120);
    QBrush arrowBrush( clrArrow );
    QPen arrowPen( QBrush(clrArrow), 1.f, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin );
    QPen guidePen( QBrush(QColor(100,100,100)), 1.f, Qt::DotLine );

    p.setPen( arrowPen );
    p.setBrush( arrowBrush );

    p.drawLine( pt1, pt2 );
    const int nHeadSize = 5;

    // Draw the arrow head
    QPainterPath path1;
    path1.moveTo( pt1 );
    path1.lineTo( pt1.x()-nHeadSize*2.f/3, pt1.y()+nHeadSize );
    path1.lineTo( pt1.x()+nHeadSize*2.f/3, pt1.y()+nHeadSize );
    path1.lineTo( pt1 );

    p.drawPath( path1 );

    QPainterPath path2;

    path2.moveTo( pt2 );
    path2.lineTo( pt2.x()-nHeadSize*2.f/3, pt2.y()-nHeadSize );
    path2.lineTo( pt2.x()+nHeadSize*2.f/3, pt2.y()-nHeadSize );
    path2.lineTo( pt2 );

    p.drawPath( path2 );

    p.setPen( guidePen );
    p.drawLine( pt3, pt4 );
}

void QTouchSettingDialog::showEvent(QShowEvent *)
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
}

void QTouchSettingDialog::closeEvent(QCloseEvent *evt)
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

void QTouchSettingDialog::onClose()
{
    m_pMainDlg->setInstantMode(T3K_HID_MODE_COMMAND);
}

void QTouchSettingDialog::reject()
{
    close();
}

void QTouchSettingDialog::accept()
{
    close();
}

void QTouchSettingDialog::onEditModified(QBorderStyleEdit* pEdit, int /*nValue*/, double /*dValue*/)
{
    setModifyEditColor( pEdit );
    m_bIsModified = true;

    QT3kDevice* pDevice = QT3kDevice::instance();

    char szValue[256];
    QString strCmd;
    if ( (pEdit == ui->txtEdtTMLeft)
         || (pEdit == ui->txtEdtTMTop)
         || (pEdit == ui->txtEdtTMRight)
         || (pEdit == ui->txtEdtTMBottom) )
    {
        LOG_I( "Manual Edit Touch Margin" );
        unsigned char l, t, r, b;
        l = (unsigned char)ui->txtEdtTMLeft->toPlainText().toFloat() * 10;
        t = (unsigned char)ui->txtEdtTMTop->toPlainText().toFloat() * 10;
        r = (unsigned char)ui->txtEdtTMRight->toPlainText().toFloat() * 10;
        b = (unsigned char)ui->txtEdtTMBottom->toPlainText().toFloat() * 10;

        snprintf( szValue, 256, "%02x%02x%02x%02x", l, t, r, b );
        strCmd = QString(cstrFactorialScreenMargin) + szValue;
        pDevice->sendCommand( strCmd, true );
    }
    else if (pEdit == ui->txtEdtTap)
    {
        LOG_I( "Manual Edit Time Tap" );
        int value = ui->txtEdtTap->toPlainText().toInt();
        strCmd = QString(cstrTimeA) + QString::number(value);
        pDevice->sendCommand( strCmd );

    }
    else if (pEdit == ui->txtEdtLongTap)
    {
        LOG_I( "Manual Edit Time Long Tap" );
        int value = ui->txtEdtLongTap->toPlainText().toInt();
        strCmd = QString(cstrTimeL) + QString::number(value);
        pDevice->sendCommand( strCmd );
    }
    else if (pEdit == ui->txtEdtWheel)
    {
        LOG_I( "Manual Edit Wheel Sensitivity" );
        int value = ui->txtEdtWheel->toPlainText().toInt();
        strCmd = QString(cstrWheelSensitivity) + QString::number(value);
        pDevice->sendCommand( strCmd );
    }
    else if (pEdit == ui->txtEdtZoom)
    {
        LOG_I( "Manual Edit Zoom Sensitivity" );
        int value = ui->txtEdtZoom->toPlainText().toInt();
        strCmd = QString(cstrZoomSensitivity) + QString::number(value);
        pDevice->sendCommand( strCmd );
    }
}

bool QTouchSettingDialog::requestSensorData( RequestCmd cmd, bool bWait )
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
        sensorWriteToFactoryDefault();
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

void QTouchSettingDialog::onCmdAsyncMngrFinished(bool, int)
{
    qDebug( "onCmdAsyncMngrFinished" );
    setViewMode(true);
}

void QTouchSettingDialog::onModifiedProfile()
{
    m_bIsModified = true;
}

void QTouchSettingDialog::sensorReset()
{
    resetDataWithInitData( cstrAreaC );
    resetDataWithInitData( cstrAreaD );
    resetDataWithInitData( cstrAreaM );
    resetDataWithInitData( cstrAreaP );
    resetDataWithInitData( cstrFactorialScreenMargin );
    resetDataWithInitData( cstrTimeA );
    resetDataWithInitData( cstrTimeL );
    resetDataWithInitData( cstrWheelSensitivity );
    resetDataWithInitData( cstrZoomSensitivity );
    resetDataWithInitData( cstrInputMode, false );
    resetDataWithInitData( cstrUsbConfigMode, false );

    resetDataWithInitData( cstrMouseProfile1 );
    resetDataWithInitData( cstrMouseProfile2 );
}

void QTouchSettingDialog::sensorLoadFactoryDefault()
{
    QString strSensorCmd;

    strSensorCmd = QString(cstrAreaC) + "*";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = QString(cstrAreaD) + "*";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = QString(cstrAreaM) + "*";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = QString(cstrAreaP) + "*";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = QString(cstrFactorialScreenMargin) + "?";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = QString(cstrTimeA) + "*";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = QString(cstrTimeL) + "*";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = QString(cstrWheelSensitivity) + "*";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = QString(cstrZoomSensitivity) + "*";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = QString(cstrInputMode) + "?";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = QString(cstrUsbConfigMode) + "?";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );

    strSensorCmd = QString(cstrMouseProfile1) + "*";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = QString(cstrMouseProfile2) + "*";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
}

void QTouchSettingDialog::sensorRefresh()
{
    QString strSensorCmd;

    strSensorCmd = QString(cstrAreaC) + "?";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = QString(cstrAreaD) + "?";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = QString(cstrAreaM) + "?";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = QString(cstrAreaP) + "?";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = QString(cstrFactorialScreenMargin) + "?";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = QString(cstrTimeA) + "?";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = QString(cstrTimeL) + "?";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = QString(cstrWheelSensitivity) + "?";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = QString(cstrZoomSensitivity) + "?";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = QString(cstrInputMode) + "?";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = QString(cstrUsbConfigMode) + "?";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
}

void QTouchSettingDialog::sensorWriteToFactoryDefault()
{
    QString strSensorCmd;

    strSensorCmd = QString(cstrAreaC) + "!";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = QString(cstrAreaD) + "!";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = QString(cstrAreaM) + "!";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = QString(cstrAreaP) + "!";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = QString(cstrFactorialScreenMargin) + "?";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = QString(cstrTimeA) + "!";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = QString(cstrTimeL) + "!";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = QString(cstrWheelSensitivity) + "!";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = QString(cstrZoomSensitivity) + "!";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = QString(cstrInputMode) + "?";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = QString(cstrUsbConfigMode) + "?";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );

    strSensorCmd = QString(cstrMouseProfile1) + "!";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = QString(cstrMouseProfile2) + "!";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
}

void QTouchSettingDialog::resetDataWithInitData( const QString& sCmd, bool bWithFactoryDefault/*=true*/)
{
    QString strCmd = sCmd;
    QString strValue;
    QString strSensorCmd;

    strCmd = rstrip(strCmd, "=");
    if ( QSensorInitDataCfg::instance()->isLoaded() )
    {
        if ( QSensorInitDataCfg::instance()->getValue( strCmd, strValue ) )
        {
            strSensorCmd = strCmd + "=" + strValue;
            ui->cmdAsyncMngr->insertCommand( strSensorCmd );
            if ( bWithFactoryDefault )
            {
                strSensorCmd = strCmd + "=!";
                ui->cmdAsyncMngr->insertCommand( strSensorCmd );
            }
            return;
        }
    }

    if ( bWithFactoryDefault )
    {
        strSensorCmd = strCmd + "=**";
    }
    else
    {
        strSensorCmd = strCmd + "=*";
    }
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
}

void QTouchSettingDialog::enableAllControls( bool bEnable )
{
    setEnabled(bEnable);
}

void QTouchSettingDialog::resetEditColors()
{
    QBorderStyleEdit* edits[] = {
        ui->txtEdtTap, ui->txtEdtLongTap, ui->txtEdtWheel, ui->txtEdtZoom,
        ui->txtEdtTMLeft, ui->txtEdtTMTop, ui->txtEdtTMRight, ui->txtEdtTMBottom
    };
    for ( int i=0 ; i<(int)(sizeof(edits)/sizeof(QBorderStyleEdit*)) ; i++ )
    {
        edits[i]->setColor(s_clrNormalBorderColor, s_clrNormalBgColor);
        edits[i]->update();
    }
}

void QTouchSettingDialog::setModifyEditColor(QBorderStyleEdit* pEdit)
{
    pEdit->setColor(s_clrModifyBorderColor, s_clrModifyBgColor);
    pEdit->update();
}

void QTouchSettingDialog::setModifyAllEditColors()
{
    QBorderStyleEdit* edits[] = {
        ui->txtEdtTap, ui->txtEdtLongTap, ui->txtEdtWheel, ui->txtEdtZoom,
        ui->txtEdtTMLeft, ui->txtEdtTMTop, ui->txtEdtTMRight, ui->txtEdtTMBottom
    };
    for ( int i=0 ; i<(int)(sizeof(edits)/sizeof(QBorderStyleEdit*)) ; i++ )
    {
        edits[i]->setColor(s_clrModifyBorderColor, s_clrModifyBgColor);
        edits[i]->update();
    }
}

static unsigned char mm_2hex2word( const char ** pstr )
{
    const char * str = *pstr;
    unsigned char u8Ret = 0;

    if ( str == NULL )
        return 0;

    while ( str[0] == ' ' || str[0] == '\t' )
        str++;

    if ( str[0] >= '0' && str[0] <= '9' )
        u8Ret = *str++ - '0';
    else if ( str[0] >= 'A' && str[0] <= 'F' )
        u8Ret = *str++ - 'A' + 10;
    else if ( str[0] >= 'a' && str[0] <= 'f' )
        u8Ret = *str++ - 'a' + 10;
    else
    {
        *pstr = NULL;
        return u8Ret;
    }

    if ( str[0] >= '0' && str[0] <= '9' )
        u8Ret = (u8Ret << 4) | (*str++ - '0');
    else if ( str[0] >= 'A' && str[0] <= 'F' )
        u8Ret = (u8Ret << 4) | (*str++ - 'A' + 10);
    else if ( str[0] >= 'a' && str[0] <= 'f' )
        u8Ret = (u8Ret << 4) | (*str++ - 'a' + 10);
    else
    {
        *pstr = NULL;
        return u8Ret;
    }

    *pstr = str;
    return u8Ret;
}

void QTouchSettingDialog::TPDP_OnRSP(T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short /*ticktime*/, const char */*partid*/, int /*id*/, bool /*bFinal*/, const char *szCmd)
{
    char szTemp[64];

    if ( strstr(szCmd, cstrFactorialScreenMargin) == szCmd )
    {
        const char* buf = szCmd + sizeof(cstrFactorialScreenMargin) - 1;

        char margin[4];
        for ( int ni = 0; ni < 4; ni++ )
        {
            if ( buf[0] == 0 || buf == NULL )
                break;

            margin[ni] = (char)mm_2hex2word(&buf);
        }

        snprintf( szTemp, 64, "%.1f", margin[0] / 10.0f );
        ui->txtEdtTMLeft->setText( szTemp );
        snprintf( szTemp, 64, "%.1f", margin[1] / 10.0f );
        ui->txtEdtTMTop->setText( szTemp );
        snprintf( szTemp, 64, "%.1f", margin[2] / 10.0f );
        ui->txtEdtTMRight->setText( szTemp );
        snprintf( szTemp, 64, "%.1f", margin[3] / 10.0f );
        ui->txtEdtTMBottom->setText( szTemp );
    }
    else if ( strstr(szCmd, cstrTimeA) == szCmd )
    {
        int nTime = atoi(szCmd + sizeof(cstrTimeA) - 1);
        if( nTime < NV_DEF_TIME_A_RANGE_START )
            nTime = NV_DEF_TIME_A_RANGE_START;
        if( nTime > NV_DEF_TIME_A_RANGE_END )
            nTime = NV_DEF_TIME_A_RANGE_END;
        ui->txtEdtTap->setText( QString::number(nTime) );
        ui->horzSliderTap->setValue( nTime/100 );
    }
    else if ( strstr(szCmd, cstrTimeL) == szCmd )
    {
        int nTime = atoi(szCmd + sizeof(cstrTimeL) - 1);
        if( nTime < NV_DEF_TIME_L_RANGE_START )
            nTime = NV_DEF_TIME_L_RANGE_START;
        if( nTime > NV_DEF_TIME_L_RANGE_END )
            nTime = NV_DEF_TIME_L_RANGE_END;

        ui->txtEdtLongTap->setText( QString::number(nTime) );
        ui->horzSliderLongTap->setValue( nTime/100 );
    }
    else if ( strstr(szCmd, cstrWheelSensitivity) == szCmd )
    {
        int nSens = atoi(szCmd + sizeof(cstrWheelSensitivity) - 1);
        if( !(nSens >= NV_DEF_WHEEL_SENSITIVITY_RANGE_START && nSens <= NV_DEF_WHEEL_SENSITIVITY_RANGE_END) )
        {
            nSens = NV_DEF_WHEEL_SENSITIVITY;
        }

        ui->txtEdtWheel->setText( QString::number(nSens) );
        ui->horzSliderWheel->setValue( nSens );
    }
    else if ( strstr(szCmd, cstrZoomSensitivity) == szCmd )
    {
        int nSens = atoi(szCmd + sizeof(cstrZoomSensitivity) - 1);
        if( !(nSens >= NV_DEF_ZOOM_SENSITIVITY_RANGE_START && nSens <= NV_DEF_ZOOM_SENSITIVITY_RANGE_END) )
        {
            nSens = NV_DEF_ZOOM_SENSITIVITY;
        }

        ui->txtEdtZoom->setText( QString::number(nSens) );
        ui->horzSliderZoom->setValue( nSens );
    }
    else if ( strstr(szCmd, cstrInputMode) == szCmd )
    {
        int nSetInputMode = -1;
        int nCurInputMode = -1;
        int nMonOrientation = 0;
        nSetInputMode = (int)strtol(szCmd + sizeof(cstrInputMode) - 1, NULL, 16);
        char* pCur = (char*)strchr( szCmd, ',' );
        if ( pCur )
        {
            nCurInputMode = (int)strtol( pCur+1, NULL, 16 );

            pCur = (char*)strchr( pCur, ',' );
            if ( pCur )
            {
                nMonOrientation = (int)strtol(pCur+1, NULL, 16);
            }
        }

        qDebug( "monitor orientation: %d", nMonOrientation );

        bool bAutoSelect = (unsigned char)nSetInputMode == 0xff ? true : false;
        ui->gbAutoSelect->setChecked( bAutoSelect );

        switch ( (unsigned char)nCurInputMode )
        {
        case 0x00:  // Mouse
            ui->btnMouse->setChecked(true);
            ui->btnMultitouch->setChecked(false);
            break;
        case 0x02:  // Multi-touch
            ui->btnMouse->setChecked(false);
            ui->btnMultitouch->setChecked(true);
            break;
        }
        if (!ui->btnDigitizerOnly->isChecked())
        {
            ui->btnMouse->setEnabled(true);
            ui->btnMultitouch->setEnabled(true);
        }
    }
    else if ( strstr(szCmd, cstrUsbConfigMode) == szCmd )
    {
        int nUsbCfgMode = (int)strtol(szCmd + sizeof(cstrUsbConfigMode) - 1, NULL, 16);
        ui->btnFull->setEnabled(true);
        ui->btnDigitizerOnly->setEnabled(true);

        QWidget* controls[] = {
            ui->gbAutoSelect, ui->btnMouse, ui->btnMultitouch, ui->btnGestureProfile,
            ui->lblInputModeTitle,
            ui->lblTimeSettingTitle, ui->lblTap, ui->txtEdtTap, ui->horzSliderTap,
            ui->lblLongTap, ui->txtEdtLongTap, ui->horzSliderLongTap,
            ui->lblWheelZoomTitle, ui->lblWheel, ui->lblZoom,
            ui->txtEdtWheel, ui->txtEdtZoom, ui->horzSliderWheel, ui->horzSliderZoom };

        switch ( (unsigned char)nUsbCfgMode )
        {
        case 0x04:      // digitizer only
            ui->btnFull->setChecked(false);
            ui->btnDigitizerOnly->setChecked(true);
            for ( int i=0 ; i<(int)(sizeof(controls)/sizeof(QWidget*)) ; i++ )
            {
                controls[i]->setEnabled(false);
            }
            ui->widgetAreaSetting->enableControlsWithoutSingleClick(false);
            break;
        case 0x07:      // full
            ui->btnFull->setChecked(true);
            ui->btnDigitizerOnly->setChecked(false);
            for ( int i=0 ; i<(int)(sizeof(controls)/sizeof(QWidget*)) ; i++ )
            {
                controls[i]->setEnabled(true);
            }
            ui->widgetAreaSetting->enableControlsWithoutSingleClick(true);
            break;
        }
    }
}

bool QTouchSettingDialog::onKeyPress(QKeyEvent *evt)
{
    if (evt->key() == Qt::Key_Escape)
    {
        LOG_I( "From Keyboard(ESC)" );
        on_btnClose_clicked();
        return true;
    }
    return false;
}

bool QTouchSettingDialog::onKeyRelease(QKeyEvent *evt)
{
    QKeyEvent* keyEvt = (QKeyEvent*)evt;
    if ( (keyEvt->key() == Qt::Key_Enter) ||
         (keyEvt->key() == Qt::Key_Return) )
    {
        QWidget* pWidget = focusWidget();
        if (pWidget->objectName().indexOf("txtEdt") >= 0)
        {
            pWidget->clearFocus();
            return true;
        }
    }
    return false;
}

bool QTouchSettingDialog::onRButtonDblClicked()
{
#ifdef SUPPORT_RBUTTON_SHORTCUT
    LOG_I( "From Mouse Shortcut(RBUTTON DOUBLE CLICK)" );

    on_btnSave_clicked();
#endif
    return true;
}

void QTouchSettingDialog::on_btnClose_clicked()
{
    LOG_B( "Exit" );

    ui->btnClose->setEnabled(false);
    close();
}

void QTouchSettingDialog::on_btnReset_clicked()
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

void QTouchSettingDialog::on_btnRefresh_clicked()
{
    LOG_B( "Refresh" );
    requestSensorData( cmdRefresh, false );
}

void QTouchSettingDialog::on_btnSave_clicked()
{
    LOG_B( "Save" );

    setEnabled(false);
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
    resetEditColors();

    setEnabled( true );
    ui->btnSave->setFocus();
    close();
}

void QTouchSettingDialog::on_btnGestureProfile_clicked()
{
    setViewMode(false);
    QGestureProfileDialog gestureProfileDlg(this);
    connect( &gestureProfileDlg, &QGestureProfileDialog::modifiedProfile, this, &QTouchSettingDialog::onModifiedProfile, Qt::QueuedConnection );
    gestureProfileDlg.exec();
    setViewMode(true);
}

void QTouchSettingDialog::sendEditValue( QBorderStyleEdit* txtEdit, float step, float fMin, float fMax, const QString& strCmd )
{
    float fValue = txtEdit->toPlainText().toFloat();
    fValue += step;

    if (fValue < fMin) fValue = fMin;
    if (fValue > fMax) fValue = fMax;

    char szValue[256];

    setModifyEditColor(txtEdit);
    //m_bIsModified = true;

    snprintf( szValue, 256, "%3.1f", fValue );
    txtEdit->setText( szValue );

    if ( (txtEdit == ui->txtEdtTMLeft)
         || (txtEdit == ui->txtEdtTMTop)
         || (txtEdit == ui->txtEdtTMRight)
         || (txtEdit == ui->txtEdtTMBottom) )
    {
        float l1, t1, r1, b1;
        l1 = ui->txtEdtTMLeft->toPlainText().toFloat() * 10;
        t1 = ui->txtEdtTMTop->toPlainText().toFloat() * 10;
        r1 = ui->txtEdtTMRight->toPlainText().toFloat() * 10;
        b1 = ui->txtEdtTMBottom->toPlainText().toFloat() * 10;

        unsigned char l, t, r, b;
        l = (unsigned char)(l1);
        t = (unsigned char)(t1);
        r = (unsigned char)(r1);
        b = (unsigned char)(b1);

        snprintf( szValue, 256, "%02x%02x%02x%02x", l, t, r, b );
    }
    else
    {
        snprintf( szValue, 256, "%3.1f", fValue );
    }

    QString strFullCmd = strCmd + szValue;
    QT3kDevice::instance()->sendCommand( strFullCmd, true );
}

void QTouchSettingDialog::on_btnTMLeftDec_clicked()
{
    LOG_B( "Touch Margin Left \"-\"" );
    sendEditValue( ui->txtEdtTMLeft, -0.1f, -10.f, +10.f, cstrFactorialScreenMargin );
}

void QTouchSettingDialog::on_btnTMLeftInc_clicked()
{
    LOG_B( "Touch Margin Left \"+\"" );
    sendEditValue( ui->txtEdtTMLeft, 0.1f, -10.f, +10.f, cstrFactorialScreenMargin );
}

void QTouchSettingDialog::on_btnTMTopDec_clicked()
{
    LOG_B( "Touch Margin Top \"-\"" );
    sendEditValue( ui->txtEdtTMTop, -0.1f, -10.f, +10.f, cstrFactorialScreenMargin );
}

void QTouchSettingDialog::on_btnTMTopInc_clicked()
{
    LOG_B( "Touch Margin Top \"+\"" );
    sendEditValue( ui->txtEdtTMTop, 0.1f, -10.f, +10.f, cstrFactorialScreenMargin );
}

void QTouchSettingDialog::on_btnTMRightDec_clicked()
{
    LOG_B( "Touch Margin Right \"-\"" );
    sendEditValue( ui->txtEdtTMRight, -0.1f, -10.f, +10.f, cstrFactorialScreenMargin );
}

void QTouchSettingDialog::on_btnTMRightInc_clicked()
{
    LOG_B( "Touch Margin Right \"+\"" );
    sendEditValue( ui->txtEdtTMRight, 0.1f, -10.f, +10.f, cstrFactorialScreenMargin );
}

void QTouchSettingDialog::on_btnTMBottomDec_clicked()
{
    LOG_B( "Touch Margin Bottom \"-\"" );
    sendEditValue( ui->txtEdtTMBottom, -0.1f, -10.f, +10.f, cstrFactorialScreenMargin );
}

void QTouchSettingDialog::on_btnTMBottomInc_clicked()
{
    LOG_B( "Touch Margin Bottom \"+\"" );
    sendEditValue( ui->txtEdtTMBottom, 0.1f, -10.f, +10.f, cstrFactorialScreenMargin );
}

void QTouchSettingDialog::on_gbAutoSelect_clicked()
{
    LOG_B( "Input Mode: Auto Select" );
    unsigned int nInputMode = 0xff;
    if (!ui->gbAutoSelect->isChecked())
    {
        if (ui->btnMouse->isChecked())
            nInputMode = 0x00;             // mouse
        else
            nInputMode = 0x02;             // multitouch
    }

    ui->btnMouse->setEnabled(true);
    ui->btnMultitouch->setEnabled(true);

    char szCmd[50];
    snprintf( szCmd, 50, "%s0x%02x", cstrInputMode, nInputMode );
    QT3kDevice::instance()->sendCommand( szCmd, true );
}

void QTouchSettingDialog::on_btnMouse_clicked()
{
    LOG_B( "Input Mode: Mouse" );
    unsigned int nInputMode = 0x00;
    char szCmd[50];
    snprintf( szCmd, 50, "%s0x%02x", cstrInputMode, nInputMode );
    QT3kDevice::instance()->sendCommand( szCmd, true );
}

void QTouchSettingDialog::on_btnMultitouch_clicked()
{
    LOG_B( "Input Mode: Multi-touch" );
    unsigned int nInputMode = 0x02;
    char szCmd[50];
    snprintf( szCmd, 50, "%s0x%02x", cstrInputMode, nInputMode );
    QT3kDevice::instance()->sendCommand( szCmd, true );
}

void QTouchSettingDialog::on_btnFull_clicked()
{
    LOG_B( "USB Config Mode: Full" );
    unsigned int nUsbCfgMode = 0x07;
    char szCmd[50];
    snprintf( szCmd, 50, "%s0x%02x", cstrUsbConfigMode, nUsbCfgMode );
    QT3kDevice::instance()->sendCommand( szCmd, true );

    snprintf( szCmd, 50, "%s?", cstrInputMode );
    QT3kDevice::instance()->sendCommand( szCmd, true );
}

void QTouchSettingDialog::on_btnDigitizerOnly_clicked()
{
    LOG_B( "USB Config Mode: Digitizer only" );
    unsigned int nUsbCfgMode = 0x04;
    char szCmd[50];
    snprintf( szCmd, 50, "%s0x%02x", cstrUsbConfigMode, nUsbCfgMode );
    QT3kDevice::instance()->sendCommand( szCmd, true );

    snprintf( szCmd, 50, "%s?", cstrInputMode );
    QT3kDevice::instance()->sendCommand( szCmd, true );
}

void QTouchSettingDialog::on_horzSliderTap_valueChanged(int value)
{
    value *= 100;
    if (value != ui->txtEdtTap->toPlainText().toInt())
    {
        LOG_I( "Slider Tap Time" );
        QString strCmd = QString(cstrTimeA) + QString::number(value);
        QT3kDevice::instance()->sendCommand( strCmd );
    }
}

void QTouchSettingDialog::on_horzSliderLongTap_valueChanged(int value)
{
    value *= 100;
    if (value != ui->txtEdtLongTap->toPlainText().toInt())
    {
        LOG_I( "Slider Long Tap Time" );
        QString strCmd = QString(cstrTimeL) + QString::number(value);
        QT3kDevice::instance()->sendCommand( strCmd );
    }
}

void QTouchSettingDialog::on_horzSliderWheel_valueChanged(int value)
{
    if (value != ui->txtEdtWheel->toPlainText().toInt())
    {
        LOG_I( "Slider Wheel Sensitivity" );
        QString strCmd = QString(cstrWheelSensitivity) + QString::number(value);
        QT3kDevice::instance()->sendCommand( strCmd );
    }
}

void QTouchSettingDialog::on_horzSliderZoom_valueChanged(int value)
{
    if (value != ui->txtEdtZoom->toPlainText().toInt())
    {
        LOG_I( "Slider Zoom Sensitivity" );
        QString strCmd = QString(cstrZoomSensitivity) + QString::number(value);
        QT3kDevice::instance()->sendCommand( strCmd );
    }
}
