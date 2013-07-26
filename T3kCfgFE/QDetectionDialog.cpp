#include "QDetectionDialog.h"
#include "ui_QDetectionDialog.h"

#include "dialog.h"
#include <QPainter>
#include <QCloseEvent>
#include "QShowMessageBox.h"
#include "QLogSystem.h"
#include "QT3kDevice.h"
#include "AppData.h"

#include "../common/QUtils.h"
#include "../common/T3kConstStr.h"

#include "QSensorInitDataCfg.h"

#include "QMyApplication.h"

QDetectionDialog::QDetectionDialog(Dialog *parent) :
    QDialog(parent), m_pMainDlg(parent),
    ui(new Ui::QDetectionDialog)
{
    ui->setupUi(this);
    QT3kDevice* pDevice = QT3kDevice::instance();

    Qt::WindowFlags flags = windowFlags();
    Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
    flags &= ~helpFlag;
    if (!pDevice->isVirtualDevice())
    {
#if defined(Q_OS_WIN)
        flags |= Qt::MSWindowsFixedSizeDialogHint;
#endif
        flags |= Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint;
    }

    setWindowFlags(flags);
    setAttribute(Qt::WA_DeleteOnClose);

    m_detectionMode = DetectionModeNone;
    m_bEnterAutoRangeSetting = false;
    m_nCamTouchCount[0] = m_nCamTouchCount[1] = 0;
    m_nCamTouchMax[0] = m_nCamTouchMax[1] = 0;
    m_bTouchOK = false;
    m_bCamTouch = false;
    m_dwTickTouch = 0;

    LOG_I( "Enter [Detection]" );
    onChangeLanguage();

    m_pMainDlg->setInstantMode( T3K_HID_MODE_COMMAND|T3K_HID_MODE_VIEW );
    ui->cmdAsyncMngr->setT3kDevice(QT3kDevice::instance());

    if (g_AppData.bIsSubCameraExist)
    {
        ui->btnMain->setVisible(true);
        ui->btnSub->setVisible(true);
    }
    else
    {
        ui->btnMain->setVisible(false);
        ui->btnSub->setVisible(false);
    }

    setDetectionMode(DetectionModeMain);
    // TODO:
    //ui->widgetDetection1->setDisplayCrackInfo(true);
    //ui->widgetDetection2->setDisplayCrackInfo(true);

    requestSensorData( cmdLoadFactoryDefault, false );

    installEventFilter(this);
}

QDetectionDialog::~QDetectionDialog()
{
    m_pMainDlg->onCloseMenu();
    delete ui;
}

#define MAIN_TAG    "MAIN"
#define RES_TAG     "DETECTION"
void QDetectionDialog::onChangeLanguage()
{
    QLangRes& res = QLangManager::getResource();

    static bool s_bIsR2L = false;
    bool bIsR2L = res.isR2L();
    if ( bIsR2L )
        setLayoutDirection( Qt::RightToLeft );
    else
        setLayoutDirection( Qt::LeftToRight );

    ui->btnMain->setText( res.getResString(RES_TAG, "BTN_CAPTION_MAIN_CAMERA") );
    ui->btnSub->setText( res.getResString(RES_TAG, "BTN_CAPTION_SUB_CAMERA") );

    ui->btnAutoRangeSetting->setText( res.getResString(RES_TAG, "BTN_CAPTION_AUTO_RANGE_SETTING") );

    ui->chkAutoDetectionLine->setText( res.getResString(RES_TAG, "BTN_CAPTION_AUTO_CORRECTION_DETECTION_LINE") );
    ui->chkSimpleDetection->setText( res.getResString(RES_TAG, "BTN_CAPTION_SIMPLE_DETECTION") );
    ui->chkInvertDetection->setText( res.getResString(RES_TAG, "BTN_CAPTION_INVERT_DETECTION") );

    ui->btnReset->setText( res.getResString( MAIN_TAG, "BTN_CAPTION_RESET") );
    ui->btnRefresh->setText( res.getResString( MAIN_TAG, "BTN_CAPTION_REFRESH") );
    ui->btnSave->setText( res.getResString( MAIN_TAG, "BTN_CAPTION_SAVE") );
    ui->btnClose->setText( res.getResString( MAIN_TAG, "BTN_CAPTION_CLOSE") );

    if (bIsR2L != s_bIsR2L)
    {
        // TODO: !!!!
    }

    s_bIsR2L = bIsR2L;
}

int QDetectionDialog::getIndexFromPart(ResponsePart Part)
{
    int nIndex = -1;
    switch( Part )
    {
    case CM1:
        nIndex = IDX_CM1;
        break;
    case CM2:
        nIndex = IDX_CM2;
        break;
    case CM1_1:
        nIndex = IDX_CM1_1;
        break;
    case CM2_1:
        nIndex = IDX_CM2_1;
        break;
    default:
        nIndex = IDX_MM;
        break;
    }
    return nIndex;
}

bool QDetectionDialog::canClose()
{
    if (false)  // TODO:
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

void QDetectionDialog::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QRect rcBody(0, 0, width()-1, height()-1);
    p.fillRect( rcBody, Qt::white );

    if (g_AppData.bIsSafeMode)
    {
        m_pMainDlg->drawSafeMode(rcBody, p);
    }
}

void QDetectionDialog::closeEvent(QCloseEvent *evt)
{
    if (!canClose())
        evt->ignore();
}

bool QDetectionDialog::eventFilter(QObject *obj, QEvent *evt)
{
    if (evt->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvt = (QKeyEvent*)evt;
        if (keyEvt->key() == Qt::Key_Escape)
        {
            if (m_bEnterAutoRangeSetting)
            {
                leaveAutoRangeSetting();
                return true;
            }
        }
    }
    return QDialog::eventFilter(obj, evt);
}

void QDetectionDialog::reject()
{
    LOG_I( "From Keyboard(ESC)" );
    ui->btnClose->setEnabled(false);
    close();
}

void QDetectionDialog::accept()
{
    close();
}

bool QDetectionDialog::requestSensorData( RequestCmd cmd, bool bWait )
{
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

    return ui->cmdAsyncMngr->getLastResult();
}

void QDetectionDialog::sensorReset()
{
    QString strSensorCmd;

    resetDataWithInitData( cstrAutoTuning, false );
    resetDataWithInitData( cstrSimpleDetection, false );
    resetDataWithInitData( cstrInvertDetection, false );

    strSensorCmd = sCam1 + cstrDetectionThreshold;
    resetDataWithInitData( strSensorCmd );
    strSensorCmd = sCam1 + cstrDetectionRange + "**";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = sCam1 + cstrSensorGain;
    resetDataWithInitData( strSensorCmd );

    strSensorCmd = sCam2 + cstrDetectionThreshold;
    resetDataWithInitData( strSensorCmd );
    strSensorCmd = sCam2 + cstrDetectionRange + "**";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = sCam2 + cstrSensorGain;
    resetDataWithInitData( strSensorCmd );

    if ( g_AppData.bIsSubCameraExist )
    {
        strSensorCmd = sCam1_1 + cstrDetectionThreshold;
        resetDataWithInitData( strSensorCmd );
        strSensorCmd = sCam1_1 + cstrDetectionRange;
        resetDataWithInitData( strSensorCmd );
        strSensorCmd = sCam1_1 + cstrSensorGain;
        resetDataWithInitData( strSensorCmd );

        strSensorCmd = sCam2_1 + cstrDetectionThreshold;
        resetDataWithInitData( strSensorCmd );
        strSensorCmd = sCam2_1 + cstrDetectionRange;
        resetDataWithInitData( strSensorCmd );
        strSensorCmd = sCam2_1 + cstrSensorGain;
        resetDataWithInitData( strSensorCmd );
    }
}

void QDetectionDialog::sensorLoadFactoryDefault()
{
    // TODO:
    //ui->widgetDetection1->clear();
    //ui->widgetDetection2->clear();
    QString strSensorCmd;

    strSensorCmd = QString(cstrAutoTuning) + "?";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = QString(cstrSimpleDetection) + "?";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = QString(cstrInvertDetection) + "?";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );

    strSensorCmd = sCam1 + cstrDetectionThreshold + "*";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = sCam1 + cstrDetectionRange + "*";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = sCam1 + cstrSensorGain + "*";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );

    strSensorCmd = sCam2 + cstrDetectionThreshold + "*";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = sCam2 + cstrDetectionRange + "*";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = sCam2 + cstrSensorGain + "*";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );

    if ( g_AppData.bIsSubCameraExist )
    {
        strSensorCmd = sCam1_1 + cstrDetectionThreshold + "*";
        ui->cmdAsyncMngr->insertCommand( strSensorCmd );
        strSensorCmd = sCam1_1 + cstrDetectionRange + "*";
        ui->cmdAsyncMngr->insertCommand( strSensorCmd );
        strSensorCmd = sCam1_1 + cstrSensorGain + "*";
        ui->cmdAsyncMngr->insertCommand( strSensorCmd );

        strSensorCmd = sCam2_1 + cstrDetectionThreshold + "*";
        ui->cmdAsyncMngr->insertCommand( strSensorCmd );
        strSensorCmd = sCam2_1 + cstrDetectionRange + "*";
        ui->cmdAsyncMngr->insertCommand( strSensorCmd );
        strSensorCmd = sCam2_1 + cstrSensorGain + "*";
        ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    }
}

void QDetectionDialog::sensorRefresh()
{
    // TODO:
    //ui->widgetDetection1->clear();
    //ui->widgetDetection2->clear();

    QString strSensorCmd;

    strSensorCmd = QString(cstrAutoTuning) + "?";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = QString(cstrSimpleDetection) + "?";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = QString(cstrInvertDetection) + "?";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );

    switch ( m_detectionMode )
    {
    default:
        break;
    case DetectionModeMain:
        strSensorCmd = sCam1 + cstrDetectionThreshold + "?";
        ui->cmdAsyncMngr->insertCommand( strSensorCmd );
        strSensorCmd = sCam1 + cstrDetectionRange + "?";
        ui->cmdAsyncMngr->insertCommand( strSensorCmd );
        strSensorCmd = sCam1 + cstrSensorGain + "?";
        ui->cmdAsyncMngr->insertCommand( strSensorCmd );

        strSensorCmd = sCam2 + cstrDetectionThreshold + "?";
        ui->cmdAsyncMngr->insertCommand( strSensorCmd );
        strSensorCmd = sCam2 + cstrDetectionRange + "?";
        ui->cmdAsyncMngr->insertCommand( strSensorCmd );
        strSensorCmd = sCam2 + cstrSensorGain + "?";
        ui->cmdAsyncMngr->insertCommand( strSensorCmd );
        break;
    case DetectionModeSub:
        strSensorCmd = sCam1_1 + cstrDetectionThreshold + "?";
        ui->cmdAsyncMngr->insertCommand( strSensorCmd );
        strSensorCmd = sCam1_1 + cstrDetectionRange + "?";
        ui->cmdAsyncMngr->insertCommand( strSensorCmd );
        strSensorCmd = sCam1_1 + cstrSensorGain + "?";
        ui->cmdAsyncMngr->insertCommand( strSensorCmd );

        strSensorCmd = sCam2_1 + cstrDetectionThreshold + "?";
        ui->cmdAsyncMngr->insertCommand( strSensorCmd );
        strSensorCmd = sCam2_1 + cstrDetectionRange + "?";
        ui->cmdAsyncMngr->insertCommand( strSensorCmd );
        strSensorCmd = sCam2_1 + cstrSensorGain + "?";
        ui->cmdAsyncMngr->insertCommand( strSensorCmd );
        break;
    }
}

void QDetectionDialog::sensorWriteToFactoryDefault()
{
    QString strSensorCmd;

    strSensorCmd = sCam1 + cstrDetectionThreshold + "!";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = sCam1 + cstrDetectionRange + "!";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = sCam1 + cstrSensorGain + "!";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );

    strSensorCmd = sCam2 + cstrDetectionThreshold + "!";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = sCam2 + cstrDetectionRange + "!";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    strSensorCmd = sCam2 + cstrSensorGain + "!";
    ui->cmdAsyncMngr->insertCommand( strSensorCmd );

    if ( g_AppData.bIsSubCameraExist )
    {
        strSensorCmd = sCam1_1 + cstrDetectionThreshold + "!";
        ui->cmdAsyncMngr->insertCommand( strSensorCmd );
        strSensorCmd = sCam1_1 + cstrDetectionRange + "!";
        ui->cmdAsyncMngr->insertCommand( strSensorCmd );
        strSensorCmd = sCam1_1 + cstrSensorGain + "!";
        ui->cmdAsyncMngr->insertCommand( strSensorCmd );

        strSensorCmd = sCam2_1 + cstrDetectionThreshold + "!";
        ui->cmdAsyncMngr->insertCommand( strSensorCmd );
        strSensorCmd = sCam2_1 + cstrDetectionRange + "!";
        ui->cmdAsyncMngr->insertCommand( strSensorCmd );
        strSensorCmd = sCam2_1 + cstrSensorGain + "!";
        ui->cmdAsyncMngr->insertCommand( strSensorCmd );
    }
}

void QDetectionDialog::resetDataWithInitData( const QString& strCmd, bool bWithFactoryDefault/*=true*/)
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

void QDetectionDialog::enableAllControls( bool bEnable )
{
    QWidget* controls[] = {
        ui->btnMain, ui->btnSub,
        ui->btnReset, ui->btnRefresh, ui->btnSave, ui->btnClose,
        ui->btnAutoRangeSetting,
        ui->chkAutoDetectionLine, ui->chkInvertDetection, ui->chkSimpleDetection
    };

    for ( int i=0 ; i<(int)(sizeof(controls)/sizeof(QWidget*)) ; i++ )
    {
        controls[i]->setEnabled(bEnable);
    }

    // TODO:
    //ui->widgetDetection1->enableAllControls(bEnable);
    //ui->widgetDetection2->enableAllControls(bEnable);
}

void QDetectionDialog::playBuzzer( BuzzerType type )
{
    int nCat = 0, nType = 0;

    switch( type )
    {
    case BuzzerEnterCalibration:
        nCat = 3, nType = 1;
        break;
    case BuzzerCancelCalibration:
        nCat = 3, nType = 3;
        break;
    case BuzzerCalibrationSucces:
        nCat = 3, nType = 2;
        break;
    case BuzzerClick:
        nCat = 1, nType = 1;
        break;
    case BuzzerNextPoint:
        nCat = 5, nType = 1;
        break;
    }

    QString strCmd;
    strCmd = QString(cstrBuzzerPlay) + QString::number(nCat) + "," + QString::number(nType);
    QT3kDevice::instance()->sendCommand(strCmd, true);
}

void QDetectionDialog::enterAutoRangeSetting()
{
    QLayout* layout = this->layout();
    layout->setContentsMargins(60, 60, 60, 60);
    update();

    QLangRes& res = QLangManager::getResource();
    ui->btnAutoRangeSetting->setText( res.getResString(RES_TAG, "BTN_CAPTION_CANCEL_AUTO_RANGE") );

    m_pMainDlg->setInstantMode( T3K_HID_MODE_COMMAND|T3K_HID_MODE_VIEW );

    if ( ui->cmdAsyncMngr->isStarted() )
    {
        ui->cmdAsyncMngr->stop();
        ui->cmdAsyncMngr->resetCommands();
    }

    QString strCmd;
    strCmd = sCam1 + cstrDetectionRange + QString::number(0) + "," + QString::number(0xffff);
    ui->cmdAsyncMngr->insertCommand( strCmd );
    strCmd = sCam2 + cstrDetectionRange + QString::number(0) + "," + QString::number(0xffff);
    ui->cmdAsyncMngr->insertCommand( strCmd );

    if ( g_AppData.bIsSubCameraExist )
    {
        strCmd = sCam1_1 + cstrDetectionRange + QString::number(0) + "," + QString::number(0xffff);
        ui->cmdAsyncMngr->insertCommand( strCmd );
        strCmd = sCam2_1 + cstrDetectionRange + QString::number(0) + "," + QString::number(0xffff);
        ui->cmdAsyncMngr->insertCommand( strCmd );
    }

    ui->cmdAsyncMngr->start( (unsigned int)-1 );

    ui->btnClose->setEnabled(false);
    ui->btnSave->setEnabled(false);
    ui->btnRefresh->setEnabled(false);
    ui->btnReset->setEnabled(false);
    ui->chkAutoDetectionLine->setEnabled(false);
    ui->chkInvertDetection->setEnabled(false);
    ui->chkSimpleDetection->setEnabled(false);
    ui->btnMain->setEnabled(false);
    ui->btnSub->setEnabled(false);

    m_lCam1Left = 0xFFFF;
    m_lCam1Right = 0x0000;
    m_lCam2Left = 0xFFFF;
    m_lCam2Right = 0x0000;

    m_bTouchOK = false;
    m_bEnterAutoRangeSetting = true;
    m_bCamTouch = false;
    m_nAutoRangeStep = 0;
    showArrow();

    playBuzzer( BuzzerEnterCalibration );

    // TODO:
    //ui->widgetDetection1->setDisplayCrackInfo(false);
    //ui->widgetDetection2->setDisplayCrackInfo(false);
}

void QDetectionDialog::leaveAutoRangeSetting()
{
    QLayout* layout = this->layout();
    layout->setContentsMargins(9, 9, 9, 9);
    update();

    ui->btnClose->setEnabled(true);
    ui->btnSave->setEnabled(true);
    ui->btnRefresh->setEnabled(true);
    ui->btnReset->setEnabled(true);
    ui->chkAutoDetectionLine->setEnabled(true);
    ui->chkInvertDetection->setEnabled(true);
    ui->chkSimpleDetection->setEnabled(true);
    ui->btnMain->setEnabled(true);
    ui->btnSub->setEnabled(true);

    QLangRes& res = QLangManager::getResource();
    ui->btnAutoRangeSetting->setText( res.getResString(RES_TAG, "BTN_CAPTION_AUTO_RANGE_SETTING") );

    m_pMainDlg->setInstantMode( T3K_HID_MODE_COMMAND|T3K_HID_MODE_VIEW );

    m_bTouchOK = false;
    m_bCamTouch = false;
    m_bEnterAutoRangeSetting = false;
    m_nAutoRangeStep = 0;
    hideArrow();

    QT3kDevice* pDevice = QT3kDevice::instance();
    if (pDevice->isVirtualDevice())
    {
        FeatureCursorPos feature;
        feature.ReportID = REPORTID_FEATURE_CURSOR_POS;
        feature.IsScreenCoordinate = false;
        feature.Show = false;
        feature.TouchOK = false;
        feature.X = 0;
        feature.Y = 0;
        feature.Progress = 0;

        pDevice->setFeature( &feature, sizeof(FeatureCursorPos) );
    }

    playBuzzer( BuzzerCancelCalibration );

    if (ui->cmdAsyncMngr->isStarted())
    {
        ui->cmdAsyncMngr->stop();
        ui->cmdAsyncMngr->resetCommands();
    }

    QString strCmd;
    strCmd = sCam1 + cstrDetectionRange + "*";
    ui->cmdAsyncMngr->insertCommand(strCmd);
    strCmd = sCam2 + cstrDetectionRange + "*";
    ui->cmdAsyncMngr->insertCommand(strCmd);

    if ( g_AppData.bIsSubCameraExist )
    {
        strCmd = sCam1_1 + cstrDetectionRange;
        resetDataWithInitData( strCmd );
        strCmd = sCam2_1 + cstrDetectionRange;
        resetDataWithInitData( strCmd );
    }

    ui->cmdAsyncMngr->start( (unsigned int)-1 );

    // TODO:
    //ui->widgetDetection1->setDisplayCrackInfo(true);
    //ui->widgetDetection2->setDisplayCrackInfo(true);
}

void QDetectionDialog::setDetectionMode( DetectionMode mode )
{
    if (mode == DetectionModeNone)
        return;

    QLangRes& res = QLangManager::getResource();
    QString strCamera = res.getResString(MAIN_TAG, "TEXT_CAMERA");

    m_detectionMode = mode;

    if (ui->cmdAsyncMngr->isStarted())
    {
        ui->cmdAsyncMngr->stop();
        ui->cmdAsyncMngr->resetCommands();
    }

    if ( mode == DetectionModeMain )
    {
        ui->btnMain->setChecked(true);
        ui->btnSub->setChecked(false);

        ui->cmdAsyncMngr->insertCommand( sCam1 + "mode=detection" );
        ui->cmdAsyncMngr->insertCommand( sCam2 + "mode=detection" );
    }
    else
    {
        ui->btnMain->setChecked(false);
        ui->btnSub->setChecked(true);

        ui->cmdAsyncMngr->insertCommand( sCam1_1 + "mode=detection" );
        ui->cmdAsyncMngr->insertCommand( sCam2_1 + "mode=detection" );
    }

    QEventLoop loop;
    loop.connect( ui->cmdAsyncMngr, SIGNAL(asyncFinished(bool,int)), SLOT(quit()));

    ui->cmdAsyncMngr->start( 2000 );

    loop.exec();

    int nReason;
    bool bResult = ui->cmdAsyncMngr->getLastResult(&nReason);
    if (!bResult)
    {
        LOG_I( "QDetectionDialog::setDetectionMode - error: %d", nReason );
        qDebug( "QDetectionDialog::setDetectionMode - error: %d", nReason );
    }

    // TODO:
    if ( mode == DetectionModeMain )
    {
//        ui->widgetDetection1->setTitle( strCamera + " 1" );
//        ui->widgetDetection2->setTitle( strCamera + " 2" );
//        ui->widgetDetection1->setCameraIndex( IDX_CM1 );
//        ui->widgetDetection2->setCameraIndex( IDX_CM2 );
    }
    else
    {
//        ui->widgetDetection1->setTitle( strCamera + " 1-1" );
//        ui->widgetDetection2->setTitle( strCamera + " 2-1" );
//        ui->widgetDetection1->setCameraIndex( IDX_CM1_1 );
//        ui->widgetDetection2->setCameraIndex( IDX_CM2_1 );
    }
}

void QDetectionDialog::onFinishAutoRange()
{
    analysisTouchObj();

    m_bCamTouch = false;
    m_bEnterAutoRangeSetting = false;

    hideArrow();

    QT3kDevice* pDevice = QT3kDevice::instance();
    if (pDevice->isVirtualDevice())
    {
        FeatureCursorPos feature;
        feature.ReportID = REPORTID_FEATURE_CURSOR_POS;
        feature.IsScreenCoordinate = false;
        feature.Show = false;
        feature.TouchOK = false;
        feature.X = 0;
        feature.Y = 0;
        feature.Progress = 0;

        pDevice->setFeature( &feature, sizeof(FeatureCursorPos) );
    }

    m_nAutoRangeStep = 0;

    QLayout* layout = this->layout();
    layout->setContentsMargins(9, 9, 9, 9);
    update();

    ui->btnClose->setEnabled(true);
    ui->btnSave->setEnabled(true);
    ui->btnRefresh->setEnabled(true);
    ui->btnReset->setEnabled(true);
    ui->chkAutoDetectionLine->setEnabled(true);
    ui->chkInvertDetection->setEnabled(true);
    ui->chkSimpleDetection->setEnabled(true);
    ui->btnMain->setEnabled(true);
    ui->btnSub->setEnabled(true);

    m_pMainDlg->setInstantMode(T3K_HID_MODE_COMMAND|T3K_HID_MODE_VIEW);

    QLangRes& res = QLangManager::getResource();
    ui->btnAutoRangeSetting->setText( res.getResString(RES_TAG, "BTN_CAPTION_AUTO_RANGE_SETTING") );

    if (m_lCam1Left < m_lCam1Right && m_lCam2Left < m_lCam2Right)       // success
    {
        playBuzzer( BuzzerCalibrationSucces );

        m_pMainDlg->setInstantMode(T3K_HID_MODE_COMMAND);
        // TODO:
        // autorange complete dialog show!!!
        m_pMainDlg->setInstantMode(T3K_HID_MODE_COMMAND|T3K_HID_MODE_VIEW);

        if (ui->cmdAsyncMngr->isStarted())
        {
            ui->cmdAsyncMngr->stop();
            ui->cmdAsyncMngr->resetCommands();
        }

        QString strCmd;
        strCmd = sCam1 + cstrDetectionRange + QString::number(m_lCam1Left) + "," + QString::number(m_lCam1Right);
        ui->cmdAsyncMngr->insertCommand(strCmd);
        strCmd = sCam2 + cstrDetectionRange + QString::number(m_lCam2Left) + "," + QString::number(m_lCam2Right);
        ui->cmdAsyncMngr->insertCommand(strCmd);

        if ( g_AppData.bIsSubCameraExist )
        {
            strCmd = sCam1_1 + cstrDetectionRange;
            resetDataWithInitData( strCmd );
            strCmd = sCam2_1 + cstrDetectionRange;
            resetDataWithInitData( strCmd );
        }

        ui->cmdAsyncMngr->start( (unsigned int)-1 );

        // TODO:
        //ui->widgetDetection1->setModifiedRange();
        //ui->widgetDetection2->setModifiedRange();
    }
    else    // fail
    {
        playBuzzer( BuzzerCancelCalibration );
    }

    // TODO:
    //ui->widgetDetection1->setDisplayCrackInfo(true);
    //ui->widgetDetection2->setDisplayCrackInfo(true);
}

void QDetectionDialog::TPDP_OnDTC(T3K_DEVICE_INFO /*devInfo*/, ResponsePart Part, unsigned short /*ticktime*/, const char */*partid*/, unsigned char */*layerid*/, unsigned long *start_pos, unsigned long *end_pos, int cnt)
{
    int nCameraIndex = getIndexFromPart(Part);
    int nCIdx = (nCameraIndex == IDX_CM1) || (nCameraIndex == IDX_CM1_1) ? 0 : 1;

    m_nCamTouchCount[nCIdx] = cnt;

    if (!m_bEnterAutoRangeSetting)
        return;

    bool bTouch = false;
    if (m_nCamTouchCount[0] > 0 && m_nCamTouchCount[1] > 0)
    {
        bTouch = true;
    }
    else if (m_nCamTouchCount[0] == 0 && m_nCamTouchCount[1] == 0)
    {
        bTouch = false;
    }
    else
    {
        return;
    }

    if (bTouch)
    {
        if (m_bTouchOK)
            return;

        if (!m_bCamTouch)   // 1st touch
        {
            m_bCamTouch = true;
            m_dwTickTouch = 0;
        }

        if (m_bCamTouch)
        {
            if (end_pos[cnt-1] - start_pos[0] < 0xffff/4)
            {
                if (m_dwTickTouch < 200)
                {
                    m_ptCamTouchObj[nCIdx][m_nAutoRangeStep].setX( start_pos[0] );
                    m_ptCamTouchObj[nCIdx][m_nAutoRangeStep].setY( end_pos[cnt-1] );
                    m_dwTickTouch ++;
                }
                else
                {
                    m_ptCamTouchObj[nCIdx][m_nAutoRangeStep].setX( start_pos[0] < (unsigned long)m_ptCamTouchObj[nCIdx][m_nAutoRangeStep].x() ? start_pos[0] : m_ptCamTouchObj[nCIdx][m_nAutoRangeStep].x() );
                    m_ptCamTouchObj[nCIdx][m_nAutoRangeStep].setY( end_pos[cnt-1] > (unsigned long)m_ptCamTouchObj[nCIdx][m_nAutoRangeStep].y() ? end_pos[cnt-1] : m_ptCamTouchObj[nCIdx][m_nAutoRangeStep].y() );
                    m_dwTickTouch ++;
                }
            }

            if (m_dwTickTouch >= 400)
            {
                if (m_nAutoRangeStep < 3)
                {
                    m_bTouchOK = true;
                    m_nAutoRangeStep ++;
                    showArrow();
                    playBuzzer( BuzzerNextPoint );
                }
                else
                {
                    onFinishAutoRange();
                }
            }
        }
    }
    else
    {
        if (m_bTouchOK)
            m_bTouchOK = false;
        m_bCamTouch = false;
    }
}

void QDetectionDialog::TPDP_OnRSP(T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short /*ticktime*/, const char */*partid*/, int /*id*/, bool /*bFinal*/, const char *cmd)
{
    if ( strstr(cmd, cstrAutoTuning) == cmd )
    {
        const char* pC = cmd + sizeof(cstrAutoTuning) - 1;
        int nI = atoi(pC);
        ui->chkAutoDetectionLine->setChecked( nI==1 ? true : false );

        // TODO:
        //ui->widgetDetection1->setCheckAutoOffset( nI==1 ? true : false );
        //ui->widgetDetection2->setCheckAutoOffset( nI==1 ? true : false );
    }
    else if ( strstr(cmd, cstrSimpleDetection) == cmd )
    {
        const char* pC = cmd + sizeof(cstrSimpleDetection) - 1;
        int nI = atoi(pC);
        ui->chkSimpleDetection->setChecked( nI==1 ? true : false );
    }
    else if ( strstr(cmd, cstrInvertDetection) == cmd )
    {
        const char* pC = cmd + sizeof(cstrInvertDetection) - 1;
        int nI = atoi(pC);
        ui->chkInvertDetection->setChecked( nI==1 ? true : false );
    }
}


void QDetectionDialog::showArrow()
{

}

void QDetectionDialog::hideArrow()
{

}

void QDetectionDialog::analysisTouchObj()
{
    const int nMaxW = 100;
    // left !!
    m_lCam1Left = 0xFFFF;
    for ( int i=0 ; i<4 ; i++ )
    {
        long lW = qAbs(m_ptCamTouchObj[0][i].x() - m_ptCamTouchObj[0][i].y()) / 2;
        if ( lW > nMaxW ) lW = nMaxW;
        long lLeft = m_ptCamTouchObj[0][i].y() - lW;
        if ( lLeft < m_lCam1Left )
        {
            m_lCam1Left = lLeft;
        }
    }
    m_lCam2Left = 0xFFFF;
    for ( int i=0 ; i<4 ; i++ )
    {
        long lW = qAbs(m_ptCamTouchObj[1][i].x() - m_ptCamTouchObj[1][i].y()) / 2;
        if ( lW > nMaxW ) lW = nMaxW;
        long lLeft = m_ptCamTouchObj[1][i].y() - lW;
        if ( lLeft < m_lCam2Left )
        {
            m_lCam2Left = lLeft;
        }
    }

    m_lCam1Right = 0x0000;
    for ( int i=0 ; i<4 ; i++ )
    {
        long lW = qAbs(m_ptCamTouchObj[0][i].x() - m_ptCamTouchObj[0][i].y()) / 2;
        if ( lW > nMaxW ) lW = nMaxW;
        long lRight = m_ptCamTouchObj[0][i].x() + lW;
        if ( lRight > m_lCam1Right )
        {
            m_lCam1Right = lRight;
        }
    }
    m_lCam2Right = 0x0000;
    for ( int i=0 ; i<4 ; i++ )
    {
        long lW = qAbs(m_ptCamTouchObj[1][i].x() - m_ptCamTouchObj[1][i].y()) / 2;
        if ( lW > nMaxW ) lW = nMaxW;
        long lRight = m_ptCamTouchObj[1][i].x() + lW;
        if ( lRight > m_lCam2Right )
        {
            m_lCam2Right = lRight;
        }
    }
}

void QDetectionDialog::on_btnMain_clicked()
{
    LOG_B( "Main" );
    setDetectionMode(DetectionModeMain);
}

void QDetectionDialog::on_btnSub_clicked()
{
    LOG_B( "Sub" );
    setDetectionMode(DetectionModeSub);
}

void QDetectionDialog::on_btnReset_clicked()
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

void QDetectionDialog::on_btnAutoRangeSetting_clicked()
{
    LOG_B( "Auto Range Setting" );
    if ( !m_bEnterAutoRangeSetting )
    {
        LOG_I( "Enter Auto Range Setting" );
        if ( m_detectionMode == DetectionModeSub )
        {
            setDetectionMode(DetectionModeMain);
            if ( !requestSensorData( cmdLoadFactoryDefault, true ) )
            {
                LOG_I( "Error RequestSensorData timeout!" );
                return;
            }
        }
        enterAutoRangeSetting();
    }
    else
    {
        LOG_I( "Exit Auto Range Setting" );
        leaveAutoRangeSetting();
    }
}

void QDetectionDialog::on_btnRefresh_clicked()
{
    LOG_B( "Refresh" );
    requestSensorData( cmdRefresh, false );
}

void QDetectionDialog::on_btnSave_clicked()
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

    // TODO:
//    ui->widgetDetection1->setModified(false);
//    ui->widgetDetection2->setModified(false);

    setEnabled( true );
    ui->btnSave->setFocus();
    close();
}

void QDetectionDialog::on_btnClose_clicked()
{
    LOG_B( "Exit" );
    ui->btnClose->setEnabled(false);
    close();
}

void QDetectionDialog::on_chkAutoDetectionLine_clicked()
{
    LOG_B( "Auto Correction: Detection Line" );

    QString strCmd;
    strCmd = QString(cstrAutoTuning) + QString::number( ui->chkAutoDetectionLine->isChecked() ? 1 : 0 );
    QT3kDevice::instance()->sendCommand( strCmd, true );
}

void QDetectionDialog::on_chkSimpleDetection_clicked()
{
    LOG_B( "Simple Detection" );

    QLangRes& res = QLangManager::getResource();
    QString strTitle = res.getResString( MAIN_TAG, "TEXT_WARNING_MESSAGE_TITLE" );

    int nRet;
    if (ui->chkSimpleDetection->isChecked())
    {
        nRet = showMessageBox( this,
            res.getResString(RES_TAG, "TEXT_WARNING_ENABLE_SIMPLE_DETECTION_PROMPT"),
            strTitle,
            QMessageBox::Information, QMessageBox::Yes|QMessageBox::No, QMessageBox::No );
        if (nRet != QMessageBox::Yes)
        {
            ui->chkSimpleDetection->setChecked(false);
            return;
        }
    }
    else
    {
        nRet = showMessageBox( this,
            res.getResString(RES_TAG, "TEXT_WARNING_DISABLE_SIMPLE_DETECTION_PROMPT"),
            strTitle,
            QMessageBox::Information, QMessageBox::Yes|QMessageBox::No, QMessageBox::No );
        if (nRet != QMessageBox::Yes)
        {
            ui->chkSimpleDetection->setChecked(true);
            return;
        }
    }

    QString strCmd;
    strCmd = QString(cstrSimpleDetection) + QString::number( ui->chkSimpleDetection->isChecked() ? 1 : 0 );
    QT3kDevice::instance()->sendCommand( strCmd, true );
}

void QDetectionDialog::on_chkInvertDetection_clicked()
{
    LOG_B( "Invert Detection" );

    QLangRes& res = QLangManager::getResource();
    QString strTitle = res.getResString( MAIN_TAG, "TEXT_WARNING_MESSAGE_TITLE" );

    int nRet;
    if (ui->chkInvertDetection->isChecked())
    {
        nRet = showMessageBox( this,
            res.getResString(RES_TAG, "TEXT_WARNING_ENABLE_INVERT_DETECTION_PROMPT"),
            strTitle,
            QMessageBox::Information, QMessageBox::Yes|QMessageBox::No, QMessageBox::No );
        if (nRet != QMessageBox::Yes)
        {
            ui->chkInvertDetection->setChecked(false);
            return;
        }
    }
    else
    {
        nRet = showMessageBox( this,
            res.getResString(RES_TAG, "TEXT_WARNING_DISABLE_INVERT_DETECTION_PROMPT"),
            strTitle,
            QMessageBox::Information, QMessageBox::Yes|QMessageBox::No, QMessageBox::No );
        if (nRet != QMessageBox::Yes)
        {
            ui->chkInvertDetection->setChecked(true);
            return;
        }
    }

    QString strCmd;
    strCmd = QString(cstrInvertDetection) + QString::number( ui->chkInvertDetection->isChecked() ? 1 : 0 );
    QT3kDevice::instance()->sendCommand( strCmd, true );
}
