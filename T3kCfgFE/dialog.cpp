#include "dialog.h"
#include "ui_dialog.h"

#include "QT3kDevice.h"
#include <QPaintEvent>
#include <QCloseEvent>
#include <QKeyEvent>
#include <QPainter>
#include "QUtils.h"
#include "QInitDataIni.h"

#include "T3k_ver.h"
#include "AppData.h"
#include <QSettings>
#include <QDesktopWidget>
#include "QLogSystem.h"
#include "QSensorInitDataCfg.h"
#include "QSelectSensorDataDialog.h"
#include "T3kConstStr.h"

#include "QBentCfgParam.h"

#include "t3kcomdef.h"

#include "QSelectDeviceDialog.h"
#include "QShowMessageBox.h"

#include "QEnterFileNameDialog.h"
#include "QDataProgressDialog.h"

#include "QSideviewDialog.h"
#include "QDetectionDialog.h"
#include "QBentAdjustmentDialog.h"
#include "QTouchSettingDialog.h"
#include "QRemoteTouchMarkDialog.h"

#include "ui/QLicenseWidget.h"

#include <QDir>

#include "conf.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    m_EventRedirect(this),
    ui(new Ui::Dialog)
{
    m_bFirmwareDownload = false;
    m_bIsConnected = false;
    m_bInvalidFirmwareVersion = false;
    m_bEnterTurnOffCheck = false;
    m_nDeviceCount = 0;
    m_TimerReconnect = 0;
    m_TimerRefreshInfo = 0;
    m_TimerCheckDevice = 0;

    m_pDlgSelectDevice = NULL;

    m_pDlgSideview = NULL;
    m_pDlgDetection = NULL;
    m_pDlgBentAdjustment = NULL;
    m_pDlgTouchSetting = NULL;
    m_pDlgRemoteTouchMark = NULL;

    m_oldMenu = MenuNone;

    memset( m_SensorAppInfo, 0, sizeof(SensorAppInfo) * IDX_MAX );
    memset( m_TempSensorAppInfo, 0, sizeof(SensorAppInfo) * IDX_MAX );

    ui->setupUi(this);

    Qt::WindowFlags flags = windowFlags();
    Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
    flags &= ~helpFlag;
#if defined(Q_OS_WIN)
    flags |= Qt::MSWindowsFixedSizeDialogHint;
#endif
    flags |= Qt::WindowMinMaxButtonsHint;
    setWindowFlags(flags);
    setFixedSize(this->size());

    ui->btnSelectSensor->setAlignment(QStyleButton::AlignCenter);
    ui->btnSelectSensor->setCaptionFontHeight(15);

    ui->btnReset->setAlignment(QStyleButton::AlignCenter);
    ui->btnReset->setCaptionFontHeight(19);
    ui->btnExit->setAlignment(QStyleButton::AlignCenter);
    ui->btnExit->setCaptionFontHeight(19);

    ui->btnTouchMark->setAlignment(QStyleButton::AlignCenter);
    ui->btnTouchMark->setCaptionFontHeight(15);

    const int nMenuButtonMargin = 8;

    ui->btnSideview->setAlignment(QStyleButton::AlignLeft);
    ui->btnSideview->setCaptionFontHeight(19);
    ui->btnSideview->setMargin( nMenuButtonMargin, 0, 0, 0 );

    ui->btnDetection->setAlignment(QStyleButton::AlignLeft);
    ui->btnDetection->setCaptionFontHeight(19);
    ui->btnDetection->setMargin( nMenuButtonMargin, 0, 0, 0 );

    ui->btnBentAdjustment->setAlignment(QStyleButton::AlignLeft);
    ui->btnBentAdjustment->setCaptionFontHeight(19);
    ui->btnBentAdjustment->setMargin( nMenuButtonMargin, 0, 0, 0 );

    ui->btnTouchSetting->setAlignment(QStyleButton::AlignLeft);
    ui->btnTouchSetting->setCaptionFontHeight(19);
    ui->btnTouchSetting->setMargin( nMenuButtonMargin, 0, 0, 0 );

    ui->lblSoftlogicInfo->setVisible(false);
    ui->btnTouchMark->setVisible(false);
    if (ui->chkSafeMode->isVisible())
    {
        ui->chkSafeMode->setVisible(false);
        ui->layoutMid->removeWidget(ui->chkSafeMode);
        ui->layoutMid->removeItem(ui->spacerBody);
    }

    ui->cmdAsyncMngr->setVisible(false);

    onCreate();

    m_fntErrorTitle = getSystemFont(this);
    m_fntErrorTitle.setPixelSize( 14 );
    m_fntErrorDetail = m_fntErrorTitle;

    m_fntErrorTitle.setBold(true);
    m_fntErrorDetail.setBold(false);

    m_EventRedirect.installEventListener(this);
    installEventFilter(&m_EventRedirect);
    ui->btnBentAdjustment->installEventFilter(&m_EventRedirect);
    ui->btnDetection->installEventFilter(&m_EventRedirect);
    ui->btnExit->installEventFilter(&m_EventRedirect);
    ui->btnReset->installEventFilter(&m_EventRedirect);
    ui->btnSelectSensor->installEventFilter(&m_EventRedirect);
    ui->btnSideview->installEventFilter(&m_EventRedirect);
    ui->btnTouchMark->installEventFilter(&m_EventRedirect);
    ui->btnTouchSetting->installEventFilter(&m_EventRedirect);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::onCreate()
{
    if ( !QInitDataIni::instance()->load() )
    {
        QInitDataIni::instance()->save();
    }
}

void Dialog::onDestroy()
{
    if (m_TimerReconnect)
    {
        killTimer(m_TimerReconnect);
        m_TimerReconnect = 0;
    }

    if (m_TimerRefreshInfo)
    {
        killTimer(m_TimerRefreshInfo);
        m_TimerRefreshInfo = 0;
    }

    if (m_TimerCheckDevice)
    {
        killTimer(m_TimerCheckDevice);
        m_TimerCheckDevice = 0;
    }

    QT3kDevice* pDevice = QT3kDevice::instance();
    if (pDevice->isOpen())
        pDevice->close();

    // save windows position
    QSettings winPosSettings( "Habilience", "T3kCfgFE" );
    winPosSettings.beginGroup("Window Position");
    winPosSettings.setValue( "x", QVariant::fromValue(x()) );
    winPosSettings.setValue( "y", QVariant::fromValue(y()) );
    winPosSettings.endGroup();
}

void Dialog::setInstantMode( int nInstantMode )
{
    if (g_AppData.bIsSafeMode)
        nInstantMode |= T3K_HID_MODE_MOUSE_DISABLE;

    QT3kDevice::instance()->setInstantMode(nInstantMode, 5000);
}

bool Dialog::openDevice()
{
    QT3kDevice* pDevice = QT3kDevice::instance();

    if (m_TimerReconnect)
    {
        killTimer(m_TimerReconnect);
        m_TimerReconnect = 0;
    }

    if (pDevice->isOpen())
        return true;

    checkDeviceStatus();

    bool bRet = false;
    if (m_nDeviceCount > 1)
    {
        Q_ASSERT( m_pDlgSelectDevice );
        if (m_pDlgSelectDevice->isVisible())
        {

        }
        else
        {
            if (m_pDlgSelectDevice->exec() == QDialog::Accepted)
            {
                QSelectDeviceDialog::DEVICE_ID deviceId = m_pDlgSelectDevice->getDeviceId();
                bRet = QT3kDevice::instance()->open( deviceId.nVID, deviceId.nPID, deviceId.nMI, deviceId.nDeviceIndex );
            }
        }
    }
    else
    {
        for (int d=0 ; d<COUNT_OF_DEVICE_LIST ; d++)
        {
            int nDevCnt = QT3kDevice::getDeviceCount( DEVICE_LIST[d].nVID, DEVICE_LIST[d].nPID, DEVICE_LIST[d].nMI );
            if (nDevCnt > 0)
            {
                bRet = QT3kDevice::instance()->open( DEVICE_LIST[d].nVID, DEVICE_LIST[d].nPID, DEVICE_LIST[d].nMI, 0 );
                break;
            }
        }
    }

    if (bRet)
    {

    }
    else
    {
        if (!m_TimerReconnect)
            m_TimerReconnect = startTimer(2000);
    }

    ui->btnSelectSensor->setVisible( m_nDeviceCount > 1 ? true : false );

    if ( bRet )
        onDeviceConnected();

    return bRet;
}

void Dialog::checkDeviceStatus()
{
    int nDeviceCount = 0;
    for (int d=0 ; d<COUNT_OF_DEVICE_LIST ; d++)
    {
        int nCnt = QT3kDevice::getDeviceCount( DEVICE_LIST[d].nVID, DEVICE_LIST[d].nPID, DEVICE_LIST[d].nMI );
        nDeviceCount += nCnt;
    }

    if (m_nDeviceCount != 0 && m_nDeviceCount != nDeviceCount)
    {
        qDebug( "device status changed - %d/%d", nDeviceCount, m_nDeviceCount );
        onDeviceDisconnected();
    }
    m_nDeviceCount = nDeviceCount;
}

void Dialog::onChangeLanguage()
{
    if (!isVisible())
        return;

#define MAIN_TAG "MAIN"

    QLangRes& res = QLangManager::getResource();
    static bool s_bIsR2L = false;
    bool bIsR2L = res.isR2L();
    if (bIsR2L)
        setLayoutDirection(Qt::RightToLeft);
    else
        setLayoutDirection(Qt::LeftToRight);

    QString strTitle = res.getResString(MAIN_TAG, "TITLE");
    if (!g_AppData.bScreenShotMode)
    {
        QString strVer = T3000_VERSION;
        setWindowTitle(strTitle + " ver " + strVer);
    }
    else
    {
        setWindowTitle(strTitle);
    }

    ui->btnSelectSensor->setText( res.getResString(MAIN_TAG, "BTN_CAPTION_SELECT_SENSOR") );
    ui->chkSafeMode->setText( res.getResString(MAIN_TAG, "BTN_CAPTION_CHECK_SAFE_MODE") );
    ui->btnTouchMark->setText( res.getResString(MAIN_TAG, "BTN_CAPTION_TOUCH_MARK") );
    ui->btnExit->setText( res.getResString(MAIN_TAG, "BTN_CAPTION_EXIT") );
    ui->btnSideview->setText( res.getResString(MAIN_TAG, "BTN_CAPTION_SIDE_VIEW") );
    ui->btnDetection->setText( res.getResString(MAIN_TAG, "BTN_CAPTION_DETECTION") );
    ui->btnBentAdjustment->setText( res.getResString(MAIN_TAG, "BTN_CAPTION_BENT_ADJUSTMENT") );
    ui->btnTouchSetting->setText( res.getResString(MAIN_TAG, "BTN_CAPTION_TOUCH_SETTING") );

    ui->btnReset->setText( res.getResString(MAIN_TAG, "BTN_CAPTION_RESET") );

    ui->btnSideview->setAdditionalText( trim(res.getResString(MAIN_TAG, "TEXT_SIDE_VIEW_ADDITIONAL_TEXT"), "\""), 1 );
    ui->btnDetection->setAdditionalText( trim(res.getResString(MAIN_TAG, "TEXT_DETECTION_ADDITIONAL_TEXT"), "\""), 1 );
    ui->btnBentAdjustment->setAdditionalText( trim(res.getResString(MAIN_TAG, "TEXT_BENT_ADJUSTMENT_ADDITIONAL_TEXT"), "\""), 1 );
    ui->btnTouchSetting->setAdditionalText( trim(res.getResString(MAIN_TAG, "TEXT_TOUCH_SETTING_ADDITIONAL_TEXT"), "\""), 1 );

    ui->btnSideview->update();
    ui->btnDetection->update();
    ui->btnBentAdjustment->update();
    ui->btnTouchSetting->update();

    if (s_bIsR2L != bIsR2L)
    {
        // TODO: adjust ui
    }

    s_bIsR2L = bIsR2L;
}

void Dialog::onInitDialog()
{
    qDebug( "Dialg::onInitDialog" );
    if (!m_pDlgSelectDevice)
        m_pDlgSelectDevice = new QSelectDeviceDialog(this);

    QString strExecPath = QCoreApplication::applicationDirPath();
    strExecPath = rstrip(strExecPath, "/\\");
    strExecPath += QDir::separator();

    QString strLangPath = strExecPath;
    strLangPath += "Languages";
    QString strLangZipPath = strExecPath;
    strLangZipPath += "T3kCfgFE.rsc";

    if ( !QLangManager::instance()->setZipResource( strLangZipPath ) )
    {
        QLangManager::instance()->setRootPath( strLangPath );
    }
    if (!QLangManager::instance()->setLanguage(QInitDataIni::instance()->getActiveLanguageIndex()))
    {
        onChangeLanguage();
    }

    if (m_bIsConnected)
    {
        ui->btnSideview->setVisible(true);
        ui->btnDetection->setVisible(true);
        ui->btnBentAdjustment->setVisible(true);
        ui->btnTouchSetting->setVisible(true);

        ui->btnReset->setEnabled(true);
        ui->btnTouchMark->setEnabled(true);
        if (ui->chkSafeMode->isVisible())
        {
            ui->chkSafeMode->setVisible(false);
            ui->layoutMid->removeWidget(ui->chkSafeMode);
            ui->layoutMid->removeItem(ui->spacerBody);
        }
    }
    else
    {
        ui->btnSideview->setVisible(false);
        ui->btnDetection->setVisible(false);
        ui->btnBentAdjustment->setVisible(false);
        ui->btnTouchSetting->setVisible(false);

        ui->btnReset->setEnabled(false);
        ui->btnTouchMark->setEnabled(false);
        if (!ui->chkSafeMode->isVisible())
        {
            ui->layoutMid->addItem(ui->spacerBody);
            ui->layoutMid->addWidget(ui->chkSafeMode);
            ui->chkSafeMode->setVisible(true);
        }
    }

    // restore window position
    QSettings winPosSettings( "Habilience", "T3kCfgFE" );
    winPosSettings.beginGroup("Window Position");
    int nX = winPosSettings.value("x", -1).toInt();
    int nY = winPosSettings.value("y", -1).toInt();
    winPosSettings.endGroup();

    QDesktopWidget desktopWidget;
    bool bInScreen = false;
    QRect rcMove( nX, nY, width(), height() );
    for ( int i=0; i<desktopWidget.screenCount(); i++ )
    {
        if ( desktopWidget.screenGeometry(i).contains( rcMove ) )
        {
            move( nX, nY );
            bInScreen = true;
            break;
        }
    }
    if (!bInScreen)
    {
        const QRect rcPrimaryMon = desktopWidget.screenGeometry( desktopWidget.primaryScreen() );
        move( rcPrimaryMon.left()+(rcPrimaryMon.width()-width())/2, rcPrimaryMon.top()+(rcPrimaryMon.height()-height())/2 );
    }

    QString strFileName = "";
    QSensorInitDataCfg::Enumerator* pEnumerator = QSensorInitDataCfg::instance()->getEnumerator();
    pEnumerator->enumCfgData();
    if (pEnumerator->getCfgDataCount() > 1)
    {
        QSelectSensorDataDialog selectDataDialog(this);
        selectDataDialog.setSaveMode(false);
        if (selectDataDialog.exec() == QDialog::Accepted)
        {
            strFileName = selectDataDialog.getFileName();
        }
    }
    else if (pEnumerator->getCfgDataCount() == 1)
    {
        QString strCfgFileName;
        QDateTime tmDate;
        if (pEnumerator->getCfgDataAt(0, strCfgFileName, tmDate))
        {
            strFileName = strCfgFileName;
        }
    }

    if (!strFileName.isEmpty())
    {
        QSensorInitDataCfg::instance()->load(strFileName);
        QSettings settings( "Habilience", "T3kCfgFE" );
        settings.beginGroup("SENSOR_INIT_DATA");
        settings.setValue( "FILENAME", strFileName );
        settings.endGroup();
    }

    updateResetButton();

    ui->cmdAsyncMngr->setT3kDevice(QT3kDevice::instance());
    openDevice();
}

void Dialog::updateResetButton()
{
    if ( QSensorInitDataCfg::instance()->isLoaded() )
    {
        ui->btnReset->setColor( QColor(248, 156, 160), QColor(100, 100, 100), QColor(240, 68, 77), QColor(0, 0, 0) );
        ui->btnReset->setAdditionalText( QSensorInitDataCfg::instance()->getFileName(), 2 );
    }
    else
    {
        ui->btnReset->setColor( QColor(180, 180, 180), QColor(100, 100, 100), QColor(43, 140, 238), QColor(0, 0, 0) );
        ui->btnReset->setAdditionalText( "", 2 );
    }
}

void Dialog::drawSafeMode(QRect rcBody, QPainter& p)
{
    rcBody.adjust(1, 1, -1, -1);

    QFont fntSafeMode = font();
    fntSafeMode.setPixelSize(10);

    p.save();
    p.setFont( fntSafeMode );

    QPen penSafeMode;
    penSafeMode.setColor( QColor(255, 0, 0) );
    penSafeMode.setCapStyle(Qt::FlatCap);
    penSafeMode.setWidth(3);
    penSafeMode.setStyle(Qt::DashLine);

    p.setPen( penSafeMode );
    p.setBrush(Qt::NoBrush);

    p.drawRect( rcBody );

    rcBody.adjust( 3, 3, -3, -3 );

    QString strSafeMode = ui->chkSafeMode->text();//"Safe Mode";

    QFontMetrics fm(fntSafeMode);
    QRect rcSafeMode = fm.boundingRect( strSafeMode );
    //rcSafeMode.adjust( -1, -1, 1, 1 );

    p.setPen( QColor(255, 0, 0) );
    p.setBackgroundMode(Qt::TransparentMode);
    int flags = Qt::AlignHCenter|Qt::AlignVCenter|Qt::TextSingleLine;
    p.drawText( rcBody.left(), rcBody.top(), rcSafeMode.width(), rcSafeMode.height(), flags, strSafeMode );
    p.drawText( rcBody.right()-rcSafeMode.width(), rcBody.top(), rcSafeMode.width(), rcSafeMode.height(), flags, strSafeMode );
    p.drawText( rcBody.left(), rcBody.bottom()-rcSafeMode.height(), rcSafeMode.width(), rcSafeMode.height(), flags, strSafeMode );
    p.drawText( rcBody.right()-rcSafeMode.width(), rcBody.bottom()-rcSafeMode.height(), rcSafeMode.width(), rcSafeMode.height(), flags, strSafeMode );

    p.drawText( rcBody.center().x() - rcSafeMode.width()/2, rcBody.top(), rcSafeMode.width(), rcSafeMode.height(), flags, strSafeMode );
    p.drawText( rcBody.center().x() - rcSafeMode.width()/2, rcBody.bottom()-rcSafeMode.height(), rcSafeMode.width(), rcSafeMode.height(), flags, strSafeMode );

    p.restore();
}

void Dialog::showEvent(QShowEvent *evt)
{
    QDialog::showEvent(evt);
    qDebug( "Dialgo::showEvent" );
    if (evt->type() == QEvent::Show)
    {
        onInitDialog();
    }
}

void Dialog::paintEvent(QPaintEvent */*evt*/)
{
    QRect rcBody(0, 0, width()-1, height()-1);
    QPainter p(this);

    p.fillRect(rcBody, QColor(225, 225, 225));

    QLangRes& res = QLangManager::getResource();

    QString strWarningTitle = "";
    QString strWarningDetail = "";

    if (m_bIsConnected)
    {
        if (m_bFirmwareDownload)
        {
            strWarningTitle = res.getResString( MAIN_TAG, "TEXT_WARNING_FIRMWARE_DOWNLOAD" );
        }
        else
        {
            if (m_bInvalidFirmwareVersion)
            {
                strWarningTitle = res.getResString( MAIN_TAG, "TEXT_ERROR_INVALID_FIRMWARE" );
                strWarningTitle.replace( "%s", "%1" );
                strWarningTitle = strWarningTitle.arg(m_strFirmwareVersion);

                QString strFormat = res.getResString( MAIN_TAG, "TEXT_ERROR_SUPPORT_FIRMWARE_VERSION2" );
                strFormat.replace( "%s", "%1" ).replace( "%1.2f", "%2" );
                if ( (MM_NEXT_FIRMWARE_VERSION-0.1f)-MM_MIN_FIRMWARE_VERSION > 0.0f )
                {
                    char szSupportVer[64];
                    snprintf( szSupportVer, 64, "%1.1fx ~ %1.1fx", MM_MIN_FIRMWARE_VERSION, MM_NEXT_FIRMWARE_VERSION-0.1f );
                    strWarningDetail = strFormat.arg(m_strFirmwareVersion).arg(szSupportVer);
                }
                else
                {
                    char szSupportVer[64];
                    snprintf( szSupportVer, 64, "%1.2f", MM_MIN_FIRMWARE_VERSION );
                    strWarningDetail = strFormat.arg(m_strFirmwareVersion).arg(szSupportVer);
                }
            }
        }
    }
    else
    {
        strWarningTitle = res.getResString( MAIN_TAG, "TEXT_WARNING_DEVICE_NOT_CONNECT" );
    }

    if (!strWarningTitle.isEmpty())
    {
        p.save();
        p.setRenderHint(QPainter::Antialiasing);

        p.setPen( QColor(255, 0, 0) );
        p.setFont(m_fntErrorTitle);

        int nTitleOffsetY = 0;
        int nTitleOffsetX = 0;

        int nBtm = ui->line->frameGeometry().top();
        QRect rcText = rcBody;
        rcText.setBottom(nBtm);
        if (!strWarningDetail.isEmpty())
        {
            QFontMetrics fntMetrics(m_fntErrorTitle);
            QSize sizeTitle = fntMetrics.size( Qt::AlignCenter|Qt::AlignVCenter, strWarningTitle );

            nTitleOffsetY = sizeTitle.height() / 3;
            rcText.adjust( 0, -nTitleOffsetY, 0, -nTitleOffsetY );

            nTitleOffsetY = sizeTitle.height() - nTitleOffsetY;
            nTitleOffsetX = (rcBody.width()-sizeTitle.width()) / 2 + 4;
        }

        p.drawText( rcText, Qt::AlignCenter|Qt::AlignVCenter, strWarningTitle );

        if (!strWarningDetail.isEmpty())
        {
            rcText = rcBody;
            p.setFont(m_fntErrorDetail);

            rcText.adjust( nTitleOffsetX, nTitleOffsetY, nTitleOffsetX, nTitleOffsetY );

            p.drawText( rcText, Qt::AlignLeft|Qt::AlignVCenter, strWarningDetail );
        }

        p.restore();
    }

    if (g_AppData.bIsSafeMode)
    {
        QRect rcBody(0, 0, width()-1, height()-1);
        drawSafeMode( rcBody, p );
    }
}

void Dialog::closeEvent(QCloseEvent *evt)
{
    if (!closeAllSubMenuDialogs())
        evt->ignore();

    if (evt->type() == QEvent::Close)
    {
        onDestroy();
    }
}

void Dialog::timerEvent(QTimerEvent *evt)
{
    if (evt->type() == QEvent::Timer)
    {
        if (evt->timerId() == m_TimerReconnect)
        {
            openDevice();
        }
        else if (evt->timerId() == m_TimerRefreshInfo)
        {
            refreshInfo();
        }
        else if (evt->timerId() == m_TimerCheckDevice)
        {
            checkDeviceStatus();
        }
    }
}

bool Dialog::eventFilter(QObject *target, QEvent *evt)
{
    if (evt->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvent = (QKeyEvent*)evt;

        if (keyEvent->key() == Qt::Key_Control)
        {
            QLangRes& res = QLangManager::getResource();

            QString strBtnTitle = ui->btnReset->text();
            QString strResetText = res.getResString(MAIN_TAG, "BTN_CAPTION_RESET");
            QString strSaveResetText = res.getResString(MAIN_TAG, "BTN_CAPTION_SAVE_RESET");

            if (strBtnTitle == strResetText)
            {
                ui->btnReset->setText( strSaveResetText );
                ui->btnReset->setAdditionalText( "", 2 );
            }
            else
            {
                ui->btnReset->setText( strResetText );
                updateResetButton();
            }
        }
    }
    return QDialog::eventFilter(target, evt);
}

void Dialog::onDeviceConnected()
{
    LOG_I( "Connected..." );
    qDebug( "Connected..." );
    m_bIsConnected = true;

    m_bEnterTurnOffCheck = false;

    ui->btnSideview->setVisible(true);
    ui->btnDetection->setVisible(true);
    ui->btnBentAdjustment->setVisible(true);
    ui->btnTouchSetting->setVisible(true);

    ui->btnReset->setEnabled(true);
    ui->btnTouchMark->setEnabled(true);
    if (ui->chkSafeMode->isVisible())
    {
        ui->chkSafeMode->setVisible(false);
        ui->layoutMid->removeWidget(ui->chkSafeMode);
        ui->layoutMid->removeItem(ui->spacerBody);
    }

    setInstantMode(T3K_HID_MODE_COMMAND);
    QT3kDevice* pDevice = QT3kDevice::instance();

    refreshInfo();
    if (m_TimerRefreshInfo)
        killTimer( m_TimerRefreshInfo );
    m_TimerRefreshInfo = startTimer( 5000 );

    if (pDevice->isVirtualDevice())
    {
        if (!ui->btnTouchMark->isVisible())
        {
            ui->btnTouchMark->setVisible(true);
            ui->btnReset->setCaptionFontHeight(15);
        }
        else
        {
            ui->btnTouchMark->setVisible(false);
            ui->btnReset->setCaptionFontHeight(19);
        }
    }

    if (!m_TimerCheckDevice)
        m_TimerCheckDevice = startTimer(1000);

}

void Dialog::onDeviceDisconnected()
{
    LOG_I( "Disconnected..." );
    qDebug( "Disconnected..." );

    QT3kDevice* pDevice = QT3kDevice::instance();
    pDevice->close();

    if (m_TimerRefreshInfo)
    {
        killTimer(m_TimerRefreshInfo);
        m_TimerRefreshInfo = 0;
    }

    if (!isVisible())
        return;

    m_bIsConnected = false;

    closeAllSubMenuDialogs();
    if (m_pDlgRemoteTouchMark)
    {
        delete m_pDlgRemoteTouchMark;
    }

    ui->btnSideview->setVisible(false);
    ui->btnDetection->setVisible(false);
    ui->btnBentAdjustment->setVisible(false);
    ui->btnTouchSetting->setVisible(false);

    if (!ui->chkSafeMode->isVisible())
    {
        ui->layoutMid->addItem(ui->spacerBody);
        ui->layoutMid->addWidget(ui->chkSafeMode);
        ui->chkSafeMode->setVisible(true);
    }
    ui->btnReset->setEnabled(false);
    ui->btnTouchMark->setEnabled(false);

    ui->btnSelectSensor->setVisible(false);
    ui->lblSoftlogicInfo->setVisible(false);

    memset(m_SensorAppInfo, 0, sizeof(SensorAppInfo)*IDX_MAX);
    ui->txtEdtDisplayFirmware->setHtml("");
    m_strFirmwareVersion = "";
    m_strModelName = "";

    if (isShownMessageBox())
    {
        QMessageBox* pMsgBox = getShownMessageBox();
        if (pMsgBox)
        {
            pMsgBox->close();
        }
    }

    if (!m_TimerReconnect)
        m_TimerReconnect = startTimer(500);
    if (m_TimerCheckDevice)
    {
        killTimer(m_TimerCheckDevice);
        m_TimerCheckDevice = 0;
    }

    update();
}

void Dialog::refreshInfo()
{
    QT3kDevice* pDevice = QT3kDevice::instance();

    if ( !pDevice->isOpen() )
        return;

    QCmdAsyncManagerCtrl& cmd = *ui->cmdAsyncMngr;
    if ( cmd.isStarted() )
    {
        cmd.stop();
        cmd.resetCommands();
    }

    if (m_bFirmwareDownload)
        return;

    if ( (m_pDlgSideview && m_pDlgSideview->isVisible()) ||
         (m_pDlgDetection && m_pDlgDetection->isVisible()) ||
         (m_pDlgBentAdjustment && m_pDlgBentAdjustment->isVisible()) ||
         (m_pDlgTouchSetting && m_pDlgTouchSetting->isVisible()) )
    {
        return;
    }

    memset( m_TempSensorAppInfo, 0, sizeof(SensorAppInfo) * IDX_MAX );

    char szCmd[256];

    snprintf( szCmd, 256, "%s%s?", cstrCam1, cstrFirmwareVersion );
    cmd.insertCommand( szCmd );

    snprintf( szCmd, 256, "%s%s?", cstrCam2, cstrFirmwareVersion );
    cmd.insertCommand( szCmd );

    snprintf( szCmd, 256, "%ssub/%s?", cstrCam1, cstrFirmwareVersion );
    cmd.insertCommand( szCmd );

    snprintf( szCmd, 256, "%ssub/%s?", cstrCam2, cstrFirmwareVersion );
    cmd.insertCommand( szCmd );

    snprintf( szCmd, 256, "%s?", cstrFirmwareVersion );
    cmd.insertCommand( szCmd );

    snprintf( szCmd, 256, "%s?", cstrTouchEnable );
    cmd.insertCommand( szCmd );

    snprintf( szCmd, 256, "%s?", cstrFactorialSoftlogic );
    cmd.insertCommand( szCmd );

    cmd.start( (unsigned int)-1 );
}

void Dialog::TPDP_OnDisconnected( T3K_DEVICE_INFO /*devInfo*/ )
{
    onDeviceDisconnected();
}

void Dialog::TPDP_OnDownloadingFirmware( T3K_DEVICE_INFO /*devInfo*/, bool bIsDownload )
{
    if ( m_bFirmwareDownload != bIsDownload )
    {
        m_bFirmwareDownload = bIsDownload;
        update();
    }

    if (bIsDownload)
    {
        closeAllSubMenuDialogs();
        if (m_pDlgRemoteTouchMark)
        {
            delete m_pDlgRemoteTouchMark;
        }

        QCmdAsyncManagerCtrl& cmd = *ui->cmdAsyncMngr;
        if ( cmd.isStarted() )
        {
            cmd.stop();
            cmd.resetCommands();
        }

        if (m_TimerRefreshInfo)
        {
            killTimer(m_TimerRefreshInfo);
            m_TimerRefreshInfo = 0;
        }

        ui->btnSideview->setVisible(false);
        ui->btnDetection->setVisible(false);
        ui->btnBentAdjustment->setVisible(false);
        ui->btnTouchSetting->setVisible(false);

        if (!ui->chkSafeMode->isVisible())
        {
            ui->layoutMid->addItem(ui->spacerBody);
            ui->layoutMid->addWidget(ui->chkSafeMode);
            ui->chkSafeMode->setVisible(true);
        }

        ui->btnReset->setEnabled(false);
        ui->btnTouchMark->setEnabled(false);

        memset(m_SensorAppInfo, 0, sizeof(SensorAppInfo)*IDX_MAX);
        ui->txtEdtDisplayFirmware->setHtml("");
        m_strFirmwareVersion = "";
        m_strModelName = "";

        ui->btnSelectSensor->setVisible(false);
        ui->lblSoftlogicInfo->setVisible(false);
    }
    else
    {
        ui->btnSideview->setVisible(true);
        ui->btnDetection->setVisible(true);
        ui->btnBentAdjustment->setVisible(true);
        ui->btnTouchSetting->setVisible(true);

        if (ui->chkSafeMode->isVisible())
        {
            ui->chkSafeMode->setVisible(false);
            ui->layoutMid->removeWidget(ui->chkSafeMode);
            ui->layoutMid->removeItem(ui->spacerBody);
        }

        ui->btnReset->setEnabled(true);
        ui->btnTouchMark->setEnabled(true);

        ui->btnSelectSensor->setVisible( m_nDeviceCount > 1 ? true : false );

        refreshInfo();

        if (!m_TimerRefreshInfo)
        {
            m_TimerRefreshInfo = startTimer(5000);
        }
    }
}

void Dialog::versionStringToSensorInfo(SensorAppInfo* info, const QString& strVer)
{
    QString str = strVer;
    QString strMajor = extractLeft( str, '.' );
    QString strMinor = extractLeft( str, ' ' );
    QString strModelName = trim( str );

    info->nVersionMajor = strMajor.toInt(0, 16);
    info->nVersionMinor = strMinor.toInt(0, 16);
    info->bIsValid = true;
    snprintf( info->szModel, 256, "%s", (const char*)strModelName.toLatin1() );
    if ( (info->nVersionMinor & 0xf) != 0 )
        snprintf( info->szVersion, 256, "%x.%02x", info->nVersionMajor, info->nVersionMinor );
    else
        snprintf( info->szVersion, 256, "%x.%x", info->nVersionMajor, info->nVersionMinor );
}

void Dialog::updateVersionInformation()
{
    if (memcmp(m_SensorAppInfo, m_TempSensorAppInfo, sizeof(SensorAppInfo)*IDX_MAX) == 0)
        return;

    memcpy(m_SensorAppInfo, m_TempSensorAppInfo, sizeof(SensorAppInfo)*IDX_MAX);

    m_strModelName = m_SensorAppInfo[IDX_MM].szModel;
    m_strFirmwareVersion = m_SensorAppInfo[IDX_MM].szVersion;

    g_AppData.bIsSubCameraExist = false;
    if (m_SensorAppInfo[IDX_CM1_1].bIsValid || m_SensorAppInfo[IDX_CM2_1].bIsValid)
    {
        g_AppData.bIsSubCameraExist = true;
    }
    g_AppData.nCameraCount = 0;
    if (m_SensorAppInfo[IDX_CM1].bIsValid) g_AppData.nCameraCount++;
    if (m_SensorAppInfo[IDX_CM2].bIsValid) g_AppData.nCameraCount++;
    if (m_SensorAppInfo[IDX_CM1_1].bIsValid) g_AppData.nCameraCount++;
    if (m_SensorAppInfo[IDX_CM2_1].bIsValid) g_AppData.nCameraCount++;

    for ( int i=0 ; i<IDX_MAX ; i++ )
        g_AppData.cameraConnectionInfo[i] = m_SensorAppInfo[i].bIsValid;

    g_AppData.strFirmwareVersion = m_SensorAppInfo[IDX_MM].szVersion;
    g_AppData.strModelName = m_SensorAppInfo[IDX_MM].szModel;

    float fFirmwareVersion = atof((const char*)m_strFirmwareVersion.toLatin1());
    qDebug( "updateVersionInformation: %f", fFirmwareVersion );

    float fMinDesireFW = (float)MM_MIN_FIRMWARE_VERSION;
    float fMaxDesireFW = (float)MM_NEXT_FIRMWARE_VERSION;

    bool bInvalidFirmwareVersion = false;
    if ( (fFirmwareVersion < fMinDesireFW) || (fFirmwareVersion >= fMaxDesireFW) )
    {
        bInvalidFirmwareVersion = true;
    }

    if (m_bInvalidFirmwareVersion != bInvalidFirmwareVersion)
    {
        m_bInvalidFirmwareVersion = bInvalidFirmwareVersion;
        update();

        if (m_bInvalidFirmwareVersion)
        {
            ui->btnSideview->setVisible(false);
            ui->btnDetection->setVisible(false);
            ui->btnBentAdjustment->setVisible(false);
            ui->btnTouchSetting->setVisible(false);

            ui->btnReset->setEnabled(false);
            ui->btnTouchMark->setEnabled(false);
        }
        else
        {
            ui->btnSideview->setVisible(true);
            ui->btnDetection->setVisible(true);
            ui->btnBentAdjustment->setVisible(true);
            ui->btnTouchSetting->setVisible(true);

            ui->btnReset->setEnabled(true);
            ui->btnTouchMark->setEnabled(true);
        }
    }

    QString strVersionInfoHTML;
    QString strHead = "<html><body><b>"
            "<font color=\"#303030\">"
            "<table border=\"0\" width=\"100%\">"
                "<tr>"
                    "<td colspan=\"3\" bgcolor=\"#999999\">"
                        "<font color=\"white\">Firmware Version</font>"
                    "</td>"
                "</tr>";
    QString strTail = "</table></font></b></body></html>";
    QString strDummyVersion = "x.xx";
    QString strDummyModel = "T3xxx";

    strVersionInfoHTML = strHead;

    QString strVersion, strModel;
    strVersion = g_AppData.bScreenShotMode ? strDummyVersion : m_SensorAppInfo[IDX_MM].szVersion;
    strModel = g_AppData.bScreenShotMode ? strDummyModel : m_SensorAppInfo[IDX_MM].szModel;
    strVersionInfoHTML += "<tr>"
                            "<td width=\"40%\">MM</td>"
                            "<td width=\"25%\">"+strVersion+"</td>"
                            "<td width=\"35%\">"+strModel+"</td>"
                          "</tr>";

    if (!g_AppData.bIsSubCameraExist)
    {
        if (m_SensorAppInfo[IDX_CM1].bIsValid)
        {
            bool bInvalidVersion = false;
            if (strcmp(m_SensorAppInfo[IDX_MM].szVersion, m_SensorAppInfo[IDX_CM1].szVersion) != 0)
            {
                bInvalidVersion = true;
            }
            strVersion = g_AppData.bScreenShotMode ? strDummyVersion : m_SensorAppInfo[IDX_CM1].szVersion;
            if (bInvalidVersion) strVersion = "<font color=#880015>"+strVersion+"</font>";
            strModel = g_AppData.bScreenShotMode ? strDummyModel : m_SensorAppInfo[IDX_CM1].szModel;
            strVersionInfoHTML += "<tr>"
                                    "<td>CM1</td>"
                                    "<td>"+strVersion+"</td>"
                                    "<td>"+strModel+"</td>"
                                  "</tr>";
        }
        else
        {
            strVersionInfoHTML += "<tr>"
                                    "<td>CM1</td>"
                                    "<td colspan=\"2\"><font color=#880015>disconnect</font></td>"
                                  "</tr>";
        }
        if (m_SensorAppInfo[IDX_CM2].bIsValid)
        {
            bool bInvalidVersion = false;
            if (strcmp(m_SensorAppInfo[IDX_MM].szVersion, m_SensorAppInfo[IDX_CM2].szVersion) != 0)
            {
                bInvalidVersion = true;
            }
            strVersion = g_AppData.bScreenShotMode ? strDummyVersion : m_SensorAppInfo[IDX_CM2].szVersion;
            if (bInvalidVersion) strVersion = "<font color=#880015>"+strVersion+"</font>";
            strModel = g_AppData.bScreenShotMode ? strDummyModel : m_SensorAppInfo[IDX_CM2].szModel;
            strVersionInfoHTML += "<tr>"
                                    "<td>CM2</td>"
                                    "<td>"+strVersion+"</td>"
                                    "<td>"+strModel+"</td>"
                                  "</tr>";
        }
        else
        {
            strVersionInfoHTML += "<tr>"
                                    "<td>CM2</td>"
                                    "<td colspan=\"2\"><font color=#880015>disconnect</font></td>"
                                  "</tr>";
        }
    }
    else
    {
        if (m_SensorAppInfo[IDX_CM1].bIsValid)
        {
            bool bInvalidVersion = false;
            if (strcmp(m_SensorAppInfo[IDX_MM].szVersion, m_SensorAppInfo[IDX_CM1].szVersion) != 0)
            {
                bInvalidVersion = true;
            }
            strVersion = g_AppData.bScreenShotMode ? strDummyVersion : m_SensorAppInfo[IDX_CM1].szVersion;
            if (bInvalidVersion) strVersion = "<font color=#880015>"+strVersion+"</font>";
            strModel = g_AppData.bScreenShotMode ? strDummyModel : m_SensorAppInfo[IDX_CM1].szModel;
            strVersionInfoHTML += "<tr>"
                                    "<td>CM1</td>"
                                    "<td>"+strVersion+"</td>"
                                    "<td>"+strModel+"</td>"
                                  "</tr>";
        }
        else
        {
            strVersionInfoHTML += "<tr>"
                                    "<td>CM1</td>"
                                    "<td colspan=\"2\"><font color=#880015>disconnect</font></td>"
                                  "</tr>";
        }
        if (m_SensorAppInfo[IDX_CM1_1].bIsValid)
        {
            bool bInvalidVersion = false;
            if (strcmp(m_SensorAppInfo[IDX_MM].szVersion, m_SensorAppInfo[IDX_CM1_1].szVersion) != 0)
            {
                bInvalidVersion = true;
            }
            strVersion = g_AppData.bScreenShotMode ? strDummyVersion : m_SensorAppInfo[IDX_CM1_1].szVersion;
            if (bInvalidVersion) strVersion = "<font color=#880015>"+strVersion+"</font>";
            strModel = g_AppData.bScreenShotMode ? strDummyModel : m_SensorAppInfo[IDX_CM1_1].szModel;
            strVersionInfoHTML += "<tr>"
                                    "<td>CM1-1</td>"
                                    "<td>"+strVersion+"</td>"
                                    "<td>"+strModel+"</td>"
                                  "</tr>";
        }
        else
        {
            strVersionInfoHTML += "<tr>"
                                    "<td>CM1-1</td>"
                                    "<td colspan=\"2\"><font color=#880015>disconnect</font></td>"
                                  "</tr>";
        }
        if (m_SensorAppInfo[IDX_CM2].bIsValid)
        {
            bool bInvalidVersion = false;
            if (strcmp(m_SensorAppInfo[IDX_MM].szVersion, m_SensorAppInfo[IDX_CM2].szVersion) != 0)
            {
                bInvalidVersion = true;
            }
            strVersion = g_AppData.bScreenShotMode ? strDummyVersion : m_SensorAppInfo[IDX_CM2].szVersion;
            if (bInvalidVersion) strVersion = "<font color=#880015>"+strVersion+"</font>";
            strModel = g_AppData.bScreenShotMode ? strDummyModel : m_SensorAppInfo[IDX_CM2].szModel;
            strVersionInfoHTML += "<tr>"
                                    "<td>CM2</td>"
                                    "<td>"+strVersion+"</td>"
                                    "<td>"+strModel+"</td>"
                                  "</tr>";
        }
        else
        {
            strVersionInfoHTML += "<tr>"
                                    "<td>CM2</td>"
                                    "<td colspan=\"2\"><font color=#880015>disconnect</font></td>"
                                  "</tr>";
        }
        if (m_SensorAppInfo[IDX_CM2_1].bIsValid)
        {
            bool bInvalidVersion = false;
            if (strcmp(m_SensorAppInfo[IDX_MM].szVersion, m_SensorAppInfo[IDX_CM2_1].szVersion) != 0)
            {
                bInvalidVersion = true;
            }
            strVersion = g_AppData.bScreenShotMode ? strDummyVersion : m_SensorAppInfo[IDX_CM2_1].szVersion;
            if (bInvalidVersion) strVersion = "<font color=#880015>"+strVersion+"</font>";
            strModel = g_AppData.bScreenShotMode ? strDummyModel : m_SensorAppInfo[IDX_CM2_1].szModel;
            strVersionInfoHTML += "<tr>"
                                    "<td>CM2-1</td>"
                                    "<td>"+strVersion+"</td>"
                                    "<td>"+strModel+"</td>"
                                  "</tr>";
        }
        else
        {
            strVersionInfoHTML += "<tr>"
                                    "<td>CM2-1</td>"
                                    "<td colspan=\"2\"><font color=#880015>disconnect</font></td>"
                                  "</tr>";
        }
    }

    strVersionInfoHTML += strTail;

    ui->txtEdtDisplayFirmware->setHtml(strVersionInfoHTML);
}

void Dialog::TPDP_OnRSP( T3K_DEVICE_INFO /*devInfo*/, ResponsePart Part, unsigned short /*ticktime*/,
                 const char* /*partid*/, int /*id*/, bool /*bFinal*/, const char * szCmd )
{
    if ( !isVisible() ) return;

    QLangRes& res = QLangManager::getResource();

    //qDebug( "[%s] %s", partid, szCmd );

    switch ( Part )
    {
    case CM1:
        if ( strstr( szCmd, "cam" ) == szCmd )
            goto MM_LOG;
        LOG_R( "<- cam1/%s", szCmd );
        break;
    case CM1_1:
        if ( strstr( szCmd, "cam" ) == szCmd )
            goto MM_LOG;
        LOG_R( "<- cam1/sub/%s", szCmd );
        break;
    case CM2:
        if ( strstr( szCmd, "cam" ) == szCmd )
            goto MM_LOG;
        LOG_R( "<- cam2/%s", szCmd );
        break;
    case CM2_1:
        if ( strstr( szCmd, "cam" ) == szCmd )
            goto MM_LOG;
        LOG_R( "<- cam2/sub/%s", szCmd );
        break;
    case MM:
MM_LOG:
        LOG_R( "<- %s", szCmd );
        break;
    }

    if ( strstr(szCmd, "no cam") == szCmd )
    {
        switch ( Part )
        {
        default:
            break;
        case CM1:
            m_TempSensorAppInfo[IDX_CM1].bIsValid = false;
            break;
        case CM2:
            m_TempSensorAppInfo[IDX_CM2].bIsValid = false;
            break;
        case CM1_1:
            m_TempSensorAppInfo[IDX_CM1_1].bIsValid = false;
            break;
        case CM2_1:
            m_TempSensorAppInfo[IDX_CM2_1].bIsValid = false;
            break;
        }
    }

    if ( strstr(szCmd, cstrFirmwareVersion) == szCmd )
    {
        char szVer[255];
        switch ( Part )
        {
        case CM1:
            strncpy( szVer, szCmd + sizeof(cstrFirmwareVersion) - 1, 255 );
            versionStringToSensorInfo(&m_TempSensorAppInfo[IDX_CM1], szVer);
            break;
        case CM2:
            strncpy( szVer, szCmd + sizeof(cstrFirmwareVersion) - 1, 255 );
            versionStringToSensorInfo(&m_TempSensorAppInfo[IDX_CM2], szVer);
            break;
        case CM1_1:
            strncpy( szVer, szCmd + sizeof(cstrFirmwareVersion) - 1, 255 );
            versionStringToSensorInfo(&m_TempSensorAppInfo[IDX_CM1_1], szVer);
            break;
        case CM2_1:
            strncpy( szVer, szCmd + sizeof(cstrFirmwareVersion) - 1, 255 );
            versionStringToSensorInfo(&m_TempSensorAppInfo[IDX_CM2_1], szVer);
            break;
        case MM:
            strncpy( szVer, szCmd + sizeof(cstrFirmwareVersion) - 1, 255 );
            versionStringToSensorInfo(&m_TempSensorAppInfo[IDX_MM], szVer);

            updateVersionInformation();
            break;
        }
    }

    else if ( strstr(szCmd, cstrFactorialSoftlogic) == szCmd )
    {
        const char* pI = szCmd + sizeof(cstrFactorialSoftlogic) - 1;

        bool bSoftlogicDetected = false;
        for ( int i=0 ; i<(int)strlen(pI) ; i++ )
        {
            if ( pI[i] != '0' )
            {
                bSoftlogicDetected = true;
                break;
            }
        }

        if (bSoftlogicDetected && !ui->lblSoftlogicInfo->isVisible())
            ui->lblSoftlogicInfo->setVisible(true);
        if (!bSoftlogicDetected && ui->lblSoftlogicInfo->isVisible())
            ui->lblSoftlogicInfo->setVisible(false);

    }
    else if ( strstr(szCmd, cstrSoftlogic) == szCmd )
    {
        const char* pI = szCmd + sizeof(cstrSoftlogic) - 1;

        bool bSoftlogicDetected = false;
        for ( int i=0 ; i<(int)strlen(pI) ; i++ )
        {
            if ( pI[i] != '0' )
            {
                bSoftlogicDetected = true;
                break;
            }
        }

        if (bSoftlogicDetected && !ui->lblSoftlogicInfo->isVisible())
            ui->lblSoftlogicInfo->setVisible(true);
        if (!bSoftlogicDetected && ui->lblSoftlogicInfo->isVisible())
            ui->lblSoftlogicInfo->setVisible(false);
    }
    else if ( strstr(szCmd, cstrTouchEnable) == szCmd )
    {
        const char* pI = szCmd + sizeof(cstrTouchEnable) - 1;
        bool bEnabled = atoi( pI ) == 1 ? true : false;

        if ( !bEnabled && !m_bEnterTurnOffCheck )
        {
            m_bEnterTurnOffCheck = true;

            int nRet = showMessageBox( this,
                res.getResString(MAIN_TAG, "TEXT_WARNING_SENSOR_TURNED_OFF"),
                res.getResString(MAIN_TAG, "TEXT_WARNING_MESSAGE_TITLE"),
                QMessageBox::Warning, QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes );
            if ( nRet == QMessageBox::No ) return;
            if ( nRet == QMessageBox::Yes )
            {
                QT3kDevice* pDevice = QT3kDevice::instance();
                QString strCmd = cstrTouchEnable;
                strCmd += '*';
                pDevice->sendCommand( strCmd, true );
            }
            m_bEnterTurnOffCheck = false;
        }
    }
}

void Dialog::onCloseTouchMarkDialog()
{
    if (m_pDlgRemoteTouchMark)
    {
        m_pDlgRemoteTouchMark = NULL;
    }
}

void Dialog::onCloseMenu()
{
    if (m_pDlgSideview)
    {
        m_pDlgSideview = NULL;
        if (ui->btnSideview->isChecked())
            ui->btnSideview->setChecked(false);
    }
    if (m_pDlgDetection)
    {
        m_pDlgDetection = NULL;
        if (ui->btnDetection->isChecked())
            ui->btnDetection->setChecked(false);
    }
    if (m_pDlgBentAdjustment)
    {
        m_pDlgBentAdjustment = NULL;
        if (ui->btnBentAdjustment->isChecked())
            ui->btnBentAdjustment->setChecked(false);
    }
    if (m_pDlgTouchSetting)
    {
        m_pDlgTouchSetting = NULL;
        if (ui->btnTouchSetting->isChecked())
            ui->btnTouchSetting->setChecked(false);
    }

    m_oldMenu = MenuNone;
    ui->btnReset->setEnabled(true);
}

bool Dialog::closeAllSubMenuDialogs()
{
    bool bForceCloseDialog = m_bFirmwareDownload || !m_bIsConnected;
    if (m_pDlgSideview)
    {
        if (bForceCloseDialog || m_pDlgSideview->canClose())
        {
            m_pDlgSideview->onClose();
            delete m_pDlgSideview;
        }
        else
            return false;
    }
    if (m_pDlgDetection)
    {
        if (bForceCloseDialog || m_pDlgDetection->canClose())
        {
            m_pDlgDetection->onClose();
            delete m_pDlgDetection;
        }
        else
            return false;
    }
    if (m_pDlgBentAdjustment)
    {
        if (bForceCloseDialog || m_pDlgBentAdjustment->canClose())
        {
            m_pDlgBentAdjustment->onClose();
            delete m_pDlgBentAdjustment;
        }
        else
            return false;
    }
    if (m_pDlgTouchSetting)
    {
        if (bForceCloseDialog || m_pDlgTouchSetting->canClose())
        {
            m_pDlgTouchSetting->onClose();
            delete m_pDlgTouchSetting;
        }
        else
            return false;
    }
    // NULL -> onCloseMenu()

    return true;
}

void Dialog::switchMenu( SelectMenu menu )
{
    if (!closeAllSubMenuDialogs())
    {
        ui->btnSideview->setChecked(false);
        ui->btnDetection->setChecked(false);
        ui->btnBentAdjustment->setChecked(false);
        ui->btnTouchSetting->setChecked(false);
        switch (m_oldMenu)
        {
        case MenuNone:
            break;
        case MenuSideview:
            ui->btnSideview->setChecked(true);
            break;
        case MenuDetection:
            ui->btnDetection->setChecked(true);
            break;
        case MenuBentAdjustment:
            ui->btnBentAdjustment->setChecked(true);
            break;
        case MenuTouchSetting:
            ui->btnTouchSetting->setChecked(true);
            break;
        }
        return;
    }

    QDialog* pCurrentDlg = NULL;
    switch (menu)
    {
    case MenuNone:
        return;
    case MenuSideview:
        if (ui->btnSideview->isChecked())
        {
            if (!m_pDlgSideview)
                m_pDlgSideview = new QSideviewDialog(this);
            pCurrentDlg = m_pDlgSideview;
        }
        break;
    case MenuDetection:
        if (ui->btnDetection->isChecked())
        {
            if (!m_pDlgDetection)
                m_pDlgDetection = new QDetectionDialog(this);
            pCurrentDlg = m_pDlgDetection;
        }
        break;
    case MenuBentAdjustment:
        if (ui->btnBentAdjustment->isChecked())
        {
            if (!m_pDlgBentAdjustment)
                m_pDlgBentAdjustment = new QBentAdjustmentDialog(this);
            pCurrentDlg = m_pDlgBentAdjustment;
        }
        break;
    case MenuTouchSetting:
        if (ui->btnTouchSetting->isChecked())
        {
            if (!m_pDlgTouchSetting)
                m_pDlgTouchSetting = new QTouchSettingDialog(this);
            pCurrentDlg = m_pDlgTouchSetting;
        }
        break;
    }

    if (pCurrentDlg)
    {
        QRect rcScreen = QApplication::desktop()->screenGeometry(QApplication::desktop()->primaryScreen());
        if (g_AppData.bScreenShotMode || QT3kDevice::instance()->isVirtualDevice())
        {
            rcScreen.setWidth(1024);
            rcScreen.setHeight(768);

            pCurrentDlg->move(rcScreen.x(), rcScreen.y());
            pCurrentDlg->resize(rcScreen.width(), rcScreen.height());
            pCurrentDlg->show();
        }
        else
        {
//            pCurrentDlg->move(rcScreen.x(), rcScreen.y());
//            pCurrentDlg->resize(rcScreen.width(), rcScreen.height());
            pCurrentDlg->showFullScreen();
        }

        m_oldMenu = menu;
        ui->btnReset->setEnabled(false);
    }
    else
    {
        m_oldMenu = MenuNone;
    }
}

void Dialog::onHandleMessage(const QString &/*msg*/)
{
    raise();
    activateWindow();
}

void Dialog::on_btnSelectSensor_clicked()
{
    LOG_B( "Select Sensor..." );

    onDeviceDisconnected();
    openDevice();
}

void Dialog::on_btnSideview_clicked()
{
    LOG_B( "Sideview" );

    switchMenu(MenuSideview);
}

void Dialog::on_btnDetection_clicked()
{
    LOG_B( "Detection" );

    switchMenu(MenuDetection);
}

void Dialog::on_btnBentAdjustment_clicked()
{
    LOG_B( "Bent Adjustment" );

    switchMenu(MenuBentAdjustment);
}

void Dialog::on_btnTouchSetting_clicked()
{
    LOG_B( "Touch Setting" );

    switchMenu(MenuTouchSetting);
}

void Dialog::on_btnReset_clicked()
{
    LOG_B( "Reset" );

    QT3kDevice* pDevice = QT3kDevice::instance();
    if (!pDevice->isOpen())
        return;

    ui->btnReset->setEnabled(false);

    QString strBtnTitle = ui->btnReset->text();
    QLangRes& res = QLangManager::getResource();
    QString strResetText = res.getResString( MAIN_TAG, "BTN_CAPTION_RESET" );
    if (strBtnTitle == strResetText )
    {
        loadSensorDefaultSettings();
    }
    else
    {
        saveSensorDefaultSettings();
        ui->btnReset->setText( strResetText );
    }

    ui->btnReset->setEnabled(true);
    updateResetButton();

    refreshInfo();
}

void Dialog::saveSensorDefaultSettings()
{
    QString strFileName;
    QSensorInitDataCfg::Enumerator* pEnumerator = QSensorInitDataCfg::instance()->getEnumerator();
    pEnumerator->enumCfgData();
    if ( pEnumerator->getCfgDataCount() > 0 )
    {
        QSelectSensorDataDialog selectSensorDataDlg(this);
        selectSensorDataDlg.setSaveMode( true );
        if ( selectSensorDataDlg.exec() != QDialog::Accepted )
            return;

        strFileName = selectSensorDataDlg.getFileName();
    }
    else
    {
        QEnterFileNameDialog enterFileNameDlg(this);
        if ( enterFileNameDlg.exec() != QDialog::Accepted )
            return;

        strFileName = enterFileNameDlg.getFileName();
    }

    if ( strFileName.isEmpty() ) return;

    QCmdAsyncManagerCtrl& cmd = *ui->cmdAsyncMngr;

    if ( cmd.isStarted() )
    {
        cmd.stop();
        cmd.resetCommands();
    }

    if (m_TimerRefreshInfo)
    {
        killTimer(m_TimerRefreshInfo);
        m_TimerRefreshInfo = 0;
    }

    QDataProgressDialog dataProgressDlg( QT3kDevice::instance(), this, true );

    QString strCmd;

    /* sideview section */
    dataProgressDlg.insertCommand( "# sideview section" );
    strCmd = sCam1 + cstrAmbientLight + "*";
    dataProgressDlg.insertCommand( strCmd );
    strCmd = sCam2 + cstrAmbientLight + "*";
    dataProgressDlg.insertCommand( strCmd );

    if (g_AppData.bIsSubCameraExist)
    {
        strCmd = sCam1_1 + cstrAmbientLight + "*";
        dataProgressDlg.insertCommand( strCmd );
        strCmd = sCam2_1 + cstrAmbientLight + "*";
        dataProgressDlg.insertCommand( strCmd );
    }

    /* detection section */
    dataProgressDlg.insertCommand( "# detection section" );

    strCmd = QString(cstrAutoTuning) + "?";
    dataProgressDlg.insertCommand( strCmd );
    strCmd = QString(cstrSimpleDetection) + "?";
    dataProgressDlg.insertCommand( strCmd );
    strCmd = QString(cstrInvertDetection) + "?";
    dataProgressDlg.insertCommand( strCmd );

    strCmd = sCam1 + cstrDetectionThreshold + "*";
    dataProgressDlg.insertCommand( strCmd );
    strCmd = sCam1 + cstrSensorGain + "*";
    dataProgressDlg.insertCommand( strCmd );

    strCmd = sCam2 + cstrDetectionThreshold + "*";
    dataProgressDlg.insertCommand( strCmd );
    strCmd = sCam2 + cstrSensorGain + "*";
    dataProgressDlg.insertCommand( strCmd );

    if ( g_AppData.bIsSubCameraExist )
    {
        strCmd = sCam1_1 + cstrDetectionThreshold + "*";
        dataProgressDlg.insertCommand( strCmd );
        strCmd = sCam1_1 + cstrDetectionRange + "*";
        dataProgressDlg.insertCommand( strCmd );
        strCmd = sCam1_1 + cstrSensorGain + "*";
        dataProgressDlg.insertCommand( strCmd );

        strCmd = sCam2_1 + cstrDetectionThreshold + "*";
        dataProgressDlg.insertCommand( strCmd );
        strCmd = sCam2_1 + cstrDetectionRange + "*";
        dataProgressDlg.insertCommand( strCmd );
        strCmd = sCam2_1 + cstrSensorGain + "*";
        dataProgressDlg.insertCommand( strCmd );
    }

    /* bentadjustment section */
    dataProgressDlg.insertCommand( "# bentadjustment section" );

    // only margin!!
    QBentCfgParam* param = QBentCfgParam::instance();
    char szTemp[256];
    snprintf( szTemp, 256, "{bent margin}=%d,%d,%d,%d,%d",
              (int)(param->marginLeft() * 100 + .5f),
              (int)(param->marginTop() * 100 + .5f),
              (int)(param->marginRight() * 100 + .5f),
              (int)(param->marginBottom() * 100 + .5f),
              param->direction() );
    dataProgressDlg.insertCommand( szTemp );

    /* sensor setting section */
    dataProgressDlg.insertCommand( "# sensor setting section" );
    strCmd = QString(cstrAreaC) + "*";
    dataProgressDlg.insertCommand( strCmd );
    strCmd = QString(cstrAreaD) + "*";
    dataProgressDlg.insertCommand( strCmd );
    strCmd = QString(cstrAreaM) + "*";
    dataProgressDlg.insertCommand( strCmd );
    strCmd = QString(cstrAreaP) + "*";
    dataProgressDlg.insertCommand( strCmd );
    strCmd = QString(cstrFactorialScreenMargin) + "?";
    dataProgressDlg.insertCommand( strCmd );
    strCmd = QString(cstrTimeA) + "*";
    dataProgressDlg.insertCommand( strCmd );
    strCmd = QString(cstrTimeL) + "*";
    dataProgressDlg.insertCommand( strCmd );
    strCmd = QString(cstrWheelSensitivity) + "*";
    dataProgressDlg.insertCommand( strCmd );
    strCmd = QString(cstrZoomSensitivity) + "*";
    dataProgressDlg.insertCommand( strCmd );
    strCmd = QString(cstrInputMode) + "*";
    dataProgressDlg.insertCommand( strCmd );
    strCmd = QString(cstrUsbConfigMode) + "*";
    dataProgressDlg.insertCommand( strCmd );

    /* gesture profile section */
    dataProgressDlg.insertCommand( "# gesture profile section" );
    strCmd = QString(cstrMouseProfile) + "?";
    dataProgressDlg.insertCommand( strCmd );
    strCmd = QString(cstrMouseProfile1) + "?";
    dataProgressDlg.insertCommand( strCmd );
    strCmd = QString(cstrMouseProfile2) + "?";
    dataProgressDlg.insertCommand( strCmd );
    strCmd = QString(cstrMouseProfile3) + "?";
    dataProgressDlg.insertCommand( strCmd );
    strCmd = QString(cstrMouseProfile4) + "?";
    dataProgressDlg.insertCommand( strCmd );
    strCmd = QString(cstrMouseProfile5) + "?";
    dataProgressDlg.insertCommand( strCmd );

    if ( dataProgressDlg.exec() != QDialog::Accepted )
    {
        QLangRes& res = QLangManager::getResource();
        int nRet = showMessageBox( this,
            res.getResString( MAIN_TAG, "TEXT_ERROR_LOADSAVE_SENSOR_DEFAULT" ),
            res.getResString( MAIN_TAG, "TEXT_ERROR_MESSAGE_TITLE" ),
            QMessageBox::Critical, QMessageBox::Ok, QMessageBox::Ok );
        if (nRet != QMessageBox::Ok)
            return;
        QSensorInitDataCfg::instance()->load( QSensorInitDataCfg::instance()->getPrevLoadedFileName() );
        return;
    }

    QSensorInitDataCfg::instance()->save( strFileName );

    QSettings settings( "Habilience", "T3kCfgFE" );
    settings.beginGroup("SENSOR_INIT_DATA");
    settings.setValue( "FILENAME", strFileName );
    settings.endGroup();
}

bool Dialog::isExistSensorDefaultValue( const QString& strSensorCmd )
{
    const QString strNoSensorDefaultCmd[] =
    {
        cstrAutoTuning,
        cstrSimpleDetection,
        cstrInvertDetection,
        cstrFactorialScreenMargin,
        cstrMouseProfile,
        cstrMouseProfile1,
        cstrMouseProfile2,
        cstrMouseProfile3,
        cstrMouseProfile4,
        cstrMouseProfile5
    };

    for ( int i=0 ; i<(int)(sizeof(strNoSensorDefaultCmd)/sizeof(QString)) ; i++ )
    {
        if (strSensorCmd.indexOf(strNoSensorDefaultCmd[i]) >= 0)
        {
            return false;
        }
    }

    return true;
}

void Dialog::loadSensorDefaultSettings( bool bQuestion/*=true*/ )
{
    QLangRes& res = QLangManager::getResource();
    if ( bQuestion )
    {
        int nRet = showMessageBox( this,
            res.getResString(MAIN_TAG, "TEXT_WARNING_RESET_SENSOR_DEFAULT"),
            res.getResString(MAIN_TAG, "TEXT_WARNING_MESSAGE_TITLE"),
            QMessageBox::Warning, QMessageBox::Ok|QMessageBox::Cancel, QMessageBox::Cancel );
        if ( nRet != QMessageBox::Ok ) return;
    }

    QCmdAsyncManagerCtrl& cmd = *ui->cmdAsyncMngr;

    if ( cmd.isStarted() )
    {
        cmd.stop();
        cmd.resetCommands();
    }

    QDataProgressDialog dataProgressDlg( QT3kDevice::instance(), this, false );

    const QString strMMCmd[] =
    {
        cstrTimeA,
        cstrTimeL,
        cstrAreaC,
        cstrAreaD,
        cstrAreaM,
        cstrAreaP,
        cstrWheelSensitivity,
        cstrZoomSensitivity,
        cstrMouseProfile,
        cstrMouseProfile1,
        cstrMouseProfile2,
        cstrMouseProfile3,
        cstrMouseProfile4,
        cstrMouseProfile5,
        cstrCalibrationKey,
        cstrCalibrationNo,
        cstrTouchDisableKey,
        cstrTouchDisableNo,
        cstrTouchEnable,
        cstrBuzzer,
        cstrInputMode,
        cstrUsbConfigMode,
        cstrCalibrationScreenMargin
    };
    const QString strCMCmd[] =
    {
        cstrAmbientLight,
        cstrDetectionThreshold,
        cstrDetectionRange,
        cstrDetectionLine,
        cstrDetectionCoef1,
        cstrDetectionCoef2,
        cstrDetectionCoef3
    };

    QString strCmd;

    for ( int i=0 ; i<(int)(sizeof(strCMCmd)/sizeof(QString)) ; i++ )
    {
        strCmd = sCam1 + strCMCmd[i] + "*";
        dataProgressDlg.insertCommand( strCmd );
        strCmd = sCam2 + strCMCmd[i] + "*";
        dataProgressDlg.insertCommand( strCmd );
    }

    for ( int i=0 ; i<(int)(sizeof(strMMCmd)/sizeof(QString)) ; i++ )
    {
        strCmd = strMMCmd[i] + "*";
        dataProgressDlg.insertCommand( strCmd );
    }

    strCmd = QString(cstrCalibration) + "**";
    dataProgressDlg.insertCommand( strCmd );

    // Load Sensor Configuration Data
    ///////////////////////////////////////////////////////////////
    if ( QSensorInitDataCfg::instance()->isLoaded() )
    {
        QString strCmd;
        QString strSensorCmd, strValue;
        for ( int i=0 ; i<QSensorInitDataCfg::instance()->getItemCount() ; i++ )
        {
            if ( QSensorInitDataCfg::instance()->getItemAt(i, strSensorCmd, strValue) )
            {
                if ( strSensorCmd.at(0).toLatin1() == '{' ) continue;
                if ( strSensorCmd.at(0).toLatin1() == '#' ) continue;
                if ( strSensorCmd.indexOf( "cam1/detection_range" ) >= 0 )
                    continue;
                if ( strSensorCmd.indexOf( "cam2/detection_range" ) >= 0 )
                    continue;

                strCmd = strSensorCmd + "=" + strValue;
                dataProgressDlg.insertCommand( strCmd );
                if ( isExistSensorDefaultValue( strCmd ) )
                {
                    // write to default
                    strCmd = strSensorCmd + "=!";
                    dataProgressDlg.insertCommand( strCmd );
                }
            }
        }

        /* bentadjustment section */
        // only margin!!
        if ( QSensorInitDataCfg::instance()->getValue( "{bent margin}", strValue ) )
        {
            if ( !strValue.isEmpty() )
            {
                do
                {
                    int nMarginLeft, nMarginUp, nMarginRight, nMarginDown;
                    int nDirection;
                    int nCP;
                    nCP = strValue.indexOf(',');
                    if ( nCP <= 0 ) break;
                    nMarginLeft = trim( strValue.left(nCP) ).toInt(0, 10);
                    strValue.remove(0, nCP+1);
                    nCP = strValue.indexOf(',');
                    if ( nCP <= 0 ) break;
                    nMarginUp = trim( strValue.left(nCP) ).toInt(0, 10);
                    strValue.remove(0, nCP+1);
                    nCP = strValue.indexOf(',');
                    if ( nCP <= 0 ) break;
                    nMarginRight = trim( strValue.left(nCP) ).toInt(0, 10);
                    strValue.remove(0, nCP+1);
                    nCP = strValue.indexOf(',');
                    if ( nCP <= 0 ) break;
                    nMarginDown = trim( strValue.left(nCP) ).toInt(0, 10);
                    strValue.remove(0, nCP+1);
                    if ( strValue.isEmpty() ) break;
                    nDirection = trim( strValue.left(nCP) ).toInt(0, 10);

                    QBentCfgParam* param = QBentCfgParam::instance();
                    param->setMargin(nMarginLeft/100.f, nMarginUp/100.f, nMarginRight/100.f, nMarginDown/100.f);
                    param->setDirection( nDirection );
                } while ( false );
            }
        }

    }

    if ( dataProgressDlg.exec() != QDialog::Accepted )
    {
        showMessageBox( this,
            res.getResString(MAIN_TAG, "TEXT_ERROR_LOADSAVE_SENSOR_DEFAULT"),
            res.getResString(MAIN_TAG, "TEXT_ERROR_MESSAGE_TITLE"),
            QMessageBox::Warning, QMessageBox::Ok, QMessageBox::Ok );
    }
}

void Dialog::on_btnTouchMark_clicked()
{
    LOG_B( "Touch Mark Window..." );

    if (m_pDlgRemoteTouchMark)
    {
        delete m_pDlgRemoteTouchMark;
    }
    else
    {
        m_pDlgRemoteTouchMark = new QRemoteTouchMarkDialog(this);

        // restore window position
        QSettings winPosSettings( "Habilience", "T3kCfgFE" );
        winPosSettings.beginGroup("TouchMarkWindow");
        int nX = winPosSettings.value("x", -1).toInt();
        int nY = winPosSettings.value("y", -1).toInt();
        int nW = winPosSettings.value("w", -1).toInt();
        int nH = winPosSettings.value("h", -1).toInt();
        if (nW < 0) nW = 640;
        if (nH < 0) nH = 480;
        winPosSettings.endGroup();

        QDesktopWidget desktopWidget;
        bool bInScreen = false;
        QRect rcMove( nX, nY, nW, nH );
        for ( int i=0; i<desktopWidget.screenCount(); i++ )
        {
            if ( desktopWidget.screenGeometry(i).contains( rcMove ) )
            {
                m_pDlgRemoteTouchMark->move( rcMove.topLeft() );
                m_pDlgRemoteTouchMark->resize( rcMove.size() );
                bInScreen = true;
                break;
            }
        }
        if (!bInScreen)
        {
            const QRect rcPrimaryMon = desktopWidget.screenGeometry( desktopWidget.primaryScreen() );
            m_pDlgRemoteTouchMark->move( rcPrimaryMon.left() + (rcPrimaryMon.width()-rcMove.width())/2, rcPrimaryMon.top() + (rcPrimaryMon.height()-rcMove.height())/2 );
            m_pDlgRemoteTouchMark->resize( rcMove.size() );

        }
        m_pDlgRemoteTouchMark->show();
    }
}

void Dialog::on_btnExit_clicked()
{
    LOG_B( "Exit" );

    close();
}

void Dialog::on_chkSafeMode_clicked(bool checked)
{
    g_AppData.bIsSafeMode = checked;
    LOG_B( "Safe Mode: Checked(%s)", g_AppData.bIsSafeMode ? "true" : "false" );

    ui->chkSafeMode->setFocus();

    update();
}

void Dialog::on_btnLicense_clicked()
{
    QLicenseWidget wig( ":/T3kCfgFERes/resources/License.html", this );
    wig.activateWindow();
    wig.exec();
}

bool Dialog::isShortcutWidget(QWidget* widget)
{
    QWidget* shortcutWidgets[] = {
        ui->btnSideview,
        ui->btnDetection,
        ui->btnBentAdjustment,
        ui->btnTouchSetting,
        ui->btnReset
    };
    int nCountOfWidgets = sizeof(shortcutWidgets) / sizeof(QWidget*);

    for ( int i=0 ; i<nCountOfWidgets ; i++ )
    {
        if (shortcutWidgets[i] == widget)
            return true;
    }

    return false;
}

void Dialog::focusChangeToNextShortcutWidget(bool bDirection)
{
    QWidget* shortcutWidgets[] = {
        ui->btnSideview,
        ui->btnDetection,
        ui->btnBentAdjustment,
        ui->btnTouchSetting,
        ui->btnReset
    };
    int nCountOfWidgets = sizeof(shortcutWidgets) / sizeof(QWidget*);

    QWidget* focus = focusWidget();
    if (bDirection)
    {
        bool bIsShortcutWidget = false;
        for ( int i=0 ; i<nCountOfWidgets ; i++ )
        {
            if (shortcutWidgets[i] == focus)
            {
                if (i != nCountOfWidgets-1)
                    shortcutWidgets[i+1]->setFocus();
                bIsShortcutWidget = true;
            }
        }

        if (!bIsShortcutWidget)
            shortcutWidgets[0]->setFocus();
    }
    else
    {
        bool bIsShortcutWidget = false;
        for ( int i=nCountOfWidgets-1 ; i>=0 ; i-- )
        {
            if (shortcutWidgets[i] == focus)
            {
                if (i != 0)
                    shortcutWidgets[i-1]->setFocus();
                bIsShortcutWidget = true;
            }
        }

        if (!bIsShortcutWidget)
            shortcutWidgets[nCountOfWidgets-1]->setFocus();
    }
}

bool Dialog::onKeyPress(QKeyEvent *evt)
{
    if (evt->key() == Qt::Key_Control)
    {
        QLangRes& res = QLangManager::getResource();

        QString strBtnTitle = ui->btnReset->text();
        QString strResetText = res.getResString(MAIN_TAG, "BTN_CAPTION_RESET");
        QString strSaveResetText = res.getResString(MAIN_TAG, "BTN_CAPTION_SAVE_RESET");

        if (strBtnTitle == strResetText)
        {
            ui->btnReset->setText( strSaveResetText );
            ui->btnReset->setAdditionalText( "", 2 );
        }
        else
        {
            ui->btnReset->setText( strResetText );
            updateResetButton();
        }
        return true;
    }
    else if (evt->key() == Qt::Key_Escape)
    {
        return true;
    }
    return false;
}

bool Dialog::onMouseWheel(QWheelEvent *evt)
{
#ifdef SUPPORT_RBUTTON_SHORTCUT
    int nDelta = evt->delta();

    qDebug( "wheel" );

    focusChangeToNextShortcutWidget( nDelta < 0 );
#else
    evt=evt;
#endif
    return true;
}

void Dialog::onRButtonClicked()
{
#ifdef SUPPORT_RBUTTON_SHORTCUT
    QWidget* focus = focusWidget();
    if (isShortcutWidget(focus))
    {
        LOG_I( "From Mouse Shortcut(RBUTTON CLICK)" );
        QPushButton* btnWidget = (QPushButton*)focus;
        btnWidget->click();
    }
#endif
}
