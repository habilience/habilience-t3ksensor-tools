#include "stdInclude.h"

#include "QSensorSettingWidget.h"
#include "ui_QSensorSettingWidget.h"

#include <QShowEvent>

#include "Common/nv.h"

#include "QAssistanceWidget.h"
#include "QDetectionGraphView.h"
#include "T3kCfgWnd.h"

typedef enum
{
        buzzeridClick         = 0,
        buzzeridCalibration   = 1,
        buzzeridKeytone       = 2,
        buzzeridError         = 3,
        buzzeridSensorAttach  = 4,
        buzzeridPenPairing    = 5,
        buzzeridUsbAttach     = 6,
} BuzzerID;

QSensorSettingWidget::QSensorSettingWidget(T3kHandle*& pHandle, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QSensorSettingWidget), m_pT3kHandle(pHandle)
{
    ui->setupUi(this);
    setFont( parent->font() );

    ui->TitleTouchScreen->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_TOUCHED.png");
    ui->TitleBuzzer->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_BUZZER.png" );
    ui->TitleShortcutkey->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_SHORTCUT.png" );
    ui->BtnDiagnostics->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_SETTING.png" );

    ui->LBPenPairing->setVisible(false);
    ui->BtnChkPenPairing->setVisible(false);
    ui->BtnSoundPenPairing->setVisible(false);

    m_strCaptionON = tr("ON");
    m_strCaptionOFF = tr("OFF");

    ui->LBKeytone->setVisible( false );
    ui->BtnChkKeytone->setVisible( false );
    ui->BtnSoundKeytone->setVisible( false );

    ui->LBUSBAttach->setVisible( false );
    ui->BtnChkUSBAttach->setVisible( false );
    ui->BtnSoundUSB->setVisible( false );

    ui->LBSensorAttach->setVisible( false );
    ui->BtnChkSensor->setVisible( false );
    ui->BtnSoundSensor->setVisible( false );

    ui->BtnSoundError->setVisible( false );

    ui->LBCalibration->setVisible( false );
    ui->BtnChkCalibration->setVisible( false );
    ui->BtnSoundCalibration->setVisible( false );

    // usbconfigmode
    ui->CBLCalibrationKey->setEnabled(false);
    ui->CBLCalibrationKeyPresses->setEnabled(false);
    ui->CBLTouchEnableKey->setEnabled(false);
    ui->CBLTouchKeyPresses->setEnabled(false);

    Init();
}

QSensorSettingWidget::~QSensorSettingWidget()
{
    m_RequestSensorData.Stop();
    delete ui;
}

void QSensorSettingWidget::Init()
{
    ui->CBLCalibrationKey->addItem( "Not Used" );
    ui->CBLCalibrationKey->addItem( "Scroll Lock" );
    ui->CBLCalibrationKey->addItem( "Num Lock" );
    ui->CBLCalibrationKey->addItem( "Caps Lock" );

    ui->CBLTouchEnableKey->addItem( "Not Used" );
    ui->CBLTouchEnableKey->addItem( "Scroll Lock" );
    ui->CBLTouchEnableKey->addItem( "Num Lock" );
    ui->CBLTouchEnableKey->addItem( "Caps Lock" );

    QString strNum;
    for( int i = NV_DEF_CALIBRATION_KEY_NO_RANGE_START/2 ; i<=NV_DEF_CALIBRATION_KEY_NO_RANGE_END/2 ; i++ )
    {
        strNum = QString("%1").arg(i*2);
        ui->CBLCalibrationKeyPresses->addItem( strNum );
        ui->CBLTouchKeyPresses->addItem( strNum );
    }

    OnChangeLanguage();
}

void QSensorSettingWidget::OnChangeLanguage()
{
    if( !winId() ) return;

    QLangRes& Res = QLangManager::GetPtr()->GetResource();

    ui->TitleTouchScreen->setText( Res.GetResString(QString::fromUtf8("SETTING"), QString::fromUtf8("TITLE_CAPTION_TOUCH_ENABLE")) );
    ui->TitleBuzzer->setText( Res.GetResString(QString::fromUtf8("SETTING"), QString::fromUtf8("TITLE_CAPTION_BUZZER")) );
    ui->TitleShortcutkey->setText( Res.GetResString(QString::fromUtf8("SETTING"), QString::fromUtf8("TITLE_SHORTCUT_KEY")) );

    int nSel = ui->CBLCalibrationKey->currentIndex();
    ui->CBLCalibrationKey->removeItem(  0 );
    ui->CBLCalibrationKey->insertItem( 0, Res.GetResString(QString::fromUtf8("SETTING"), QString::fromUtf8("TEXT_NOT_USED")) );
    ui->CBLCalibrationKey->setCurrentIndex( nSel );

    nSel = ui->CBLTouchEnableKey->currentIndex();
    ui->CBLTouchEnableKey->removeItem( 0 );
    ui->CBLTouchEnableKey->insertItem( 0, Res.GetResString(QString::fromUtf8("SETTING"), QString::fromUtf8("TEXT_NOT_USED")) );
    ui->CBLTouchEnableKey->setCurrentIndex( nSel );

    ui->LBTouchScreen->setText( Res.GetResString(QString::fromUtf8("SETTING"), QString::fromUtf8("TEXT_TOUCH_ENABLE")) );
    ui->LBSingleClk->setText( Res.GetResString(QString::fromUtf8("SETTING"), QString::fromUtf8("TEXT_CLICK")) );
    ui->LBCalibration->setText( Res.GetResString(QString::fromUtf8("SETTING"), QString::fromUtf8("TEXT_CALIBRATION")) );
    ui->LBError->setText( Res.GetResString(QString::fromUtf8("SETTING"), QString::fromUtf8("TEXT_ERROR")) );
    ui->LBKeytone->setText( Res.GetResString(QString::fromUtf8("SETTING"), QString::fromUtf8("TEXT_KEYTONE")) );
    ui->LBSensorAttach->setText( Res.GetResString(QString::fromUtf8("SETTING"), QString::fromUtf8("TEXT_SENSOR_ATTACH")) );
    ui->LBPenPairing->setText( Res.GetResString(QString::fromUtf8("SETTING"), QString::fromUtf8("TEXT_PEN_PAIRING")) );
    ui->LBUSBAttach->setText( Res.GetResString(QString::fromUtf8("SETTING"), QString::fromUtf8("TEXT_USB_ATTACH")) );

    ui->LBCalibrationKey->setText( Res.GetResString(QString::fromUtf8("SETTING"), QString::fromUtf8("TEXT_CALIBRATION_KEY")) );
    ui->LBCalibrationKeyPresses->setText( Res.GetResString(QString::fromUtf8("SETTING"), QString::fromUtf8("TEXT_CALIBRATION_NUMBER_OF_PRESS")) );
    ui->LBTouchEnableKey->setText( Res.GetResString(QString::fromUtf8("SETTING"), QString::fromUtf8("TEXT_TOUCH_ENABLE_KEY")) );
    ui->LBTouchKeyPresses->setText( Res.GetResString(QString::fromUtf8("SETTING"), QString::fromUtf8("TEXT_TOUCH_ENABLE_NUMBER_OF_PRESS")) );

    m_strCaptionON = Res.GetResString(QString::fromUtf8("SETTING"), QString::fromUtf8("TEXT_ON"));
    m_strCaptionOFF = Res.GetResString(QString::fromUtf8("SETTING"), QString::fromUtf8("TEXT_OFF"));
    ui->BtnChkTouch->setText( ui->BtnChkTouch->isChecked() ? m_strCaptionON : m_strCaptionOFF );
    ui->BtnChkSingleClk->setText( ui->BtnChkSingleClk->isChecked() ? m_strCaptionON : m_strCaptionOFF );
    ui->BtnChkCalibration->setText( ui->BtnChkCalibration->isChecked() ? m_strCaptionON : m_strCaptionOFF );
    ui->BtnChkError->setText( ui->BtnChkError->isChecked() ? m_strCaptionON : m_strCaptionOFF );
    ui->BtnChkKeytone->setText( ui->BtnChkKeytone->isChecked() ? m_strCaptionON : m_strCaptionOFF );
    ui->BtnChkSensor->setText( ui->BtnChkSensor->isChecked() ? m_strCaptionON : m_strCaptionOFF );
    ui->BtnChkUSBAttach->setText( ui->BtnChkUSBAttach->isChecked() ? m_strCaptionON : m_strCaptionOFF );
    ui->BtnChkPenPairing->setText( ui->BtnChkPenPairing->isChecked() ? m_strCaptionON : m_strCaptionOFF );

    QString strPlayToolTip = Res.GetResString( QString::fromUtf8("SETTING"), QString::fromUtf8("BTN_TOOLTIP_PLAY"));
    ui->BtnSoundSingle->setText( strPlayToolTip );
    ui->BtnSoundCalibration->setText( strPlayToolTip );
    ui->BtnSoundError->setText( strPlayToolTip );
    ui->BtnSoundKeytone->setText( strPlayToolTip );
    ui->BtnSoundSensor->setText( strPlayToolTip );
    ui->BtnSoundUSB->setText( strPlayToolTip );
    ui->BtnSoundPenPairing->setText( strPlayToolTip );

    ui->BtnDiagnostics->setText( Res.GetResString(QString::fromUtf8("SETTING"), QString::fromUtf8("BTN_CAMERA_DIAGNOSTICS")) );
}

void QSensorSettingWidget::SetDefault()
{
    RequestGeneralSetting( true );
}

void QSensorSettingWidget::Refresh()
{
    RequestGeneralSetting( false );
}


void QSensorSettingWidget::showEvent(QShowEvent *evt)
{
    if( evt->type() == QEvent::Show )
    {
        RequestGeneralSetting( false );
        setFocusPolicy( Qt::StrongFocus );
    }

    QWidget::showEvent(evt);
}

void QSensorSettingWidget::hideEvent(QHideEvent *evt)
{
    if( evt->type() == QEvent::Hide )
    {
        QAssistanceWidget* pDlg = findChild<QAssistanceWidget*>();
        if( pDlg && pDlg->isVisible() )
            pDlg->ForcedClose();
        QWarningWidget* pWarning = findChild<QWarningWidget*>();
        if( pWarning && pWarning->isVisible() )
            pWarning->close();


        setFocusPolicy( Qt::NoFocus );

        m_RequestSensorData.Stop();
    }

    QWidget::hideEvent(evt);
}

void QSensorSettingWidget::keyPressEvent(QKeyEvent *evt)
{
    if( evt->type() == QEvent::KeyPress )
    {
        ByPassKeyPressEvent( evt );
        return;
    }

    QWidget::keyPressEvent(evt);
}

void QSensorSettingWidget::OnBuzzerPlay(unsigned int nIndex)
{
    if( !m_pT3kHandle || nIndex >= MaxBuzzer )
            return;

    m_pT3kHandle->SendCommand( (const char*)QString("%1%2,3").arg(cstrBuzzerPlay).arg(nIndex).toUtf8().data(), true );
}

void QSensorSettingWidget::OnBnClickedCheckBuzzer( QToolButton* pBtn, unsigned int nIndex )
{
    static const BuzzerID buzzerId[] = { buzzeridError, buzzeridClick, buzzeridKeytone, buzzeridCalibration, buzzeridSensorAttach, buzzeridUsbAttach, buzzeridPenPairing };

    if( !m_pT3kHandle || nIndex >= MaxBuzzer )
            return;

    pBtn->setText( pBtn->isChecked() ? m_strCaptionON : m_strCaptionOFF );

    m_pT3kHandle->SendCommand( (const char*)QString("%1%2:%3").arg(cstrBuzzer).arg(buzzerId[nIndex]).arg(pBtn->isChecked() ? 1 : 0).toUtf8().data(), true );
}

void QSensorSettingWidget::RequestGeneralSetting( bool bDefault )
{
    if( !m_pT3kHandle ) return;

    m_RequestSensorData.Stop();

    char cQ = bDefault ? '*' : '?';

    QString str( cQ );
    m_RequestSensorData.AddItem( cstrCalibrationKey, str );
    m_RequestSensorData.AddItem( cstrCalibrationNo, str );
    m_RequestSensorData.AddItem( cstrTouchEnable, str );
    m_RequestSensorData.AddItem( cstrTouchDisableKey, str );
    m_RequestSensorData.AddItem( cstrTouchDisableNo, str );
    m_RequestSensorData.AddItem( cstrBuzzer, str );

    // calibration
    m_pT3kHandle->SendCommand( (const char*)QString( "%1%2" ).arg(cstrCalibrationKey).arg(cQ).toUtf8().data(), true );
    m_pT3kHandle->SendCommand( (const char*)QString( "%1%2" ).arg(cstrCalibrationNo).arg(cQ).toUtf8().data(), true );

    // touch e/d
    m_pT3kHandle->SendCommand( (const char*)QString( "%1%2" ).arg(cstrTouchEnable).arg(cQ).toUtf8().data(), true );
    m_pT3kHandle->SendCommand( (const char*)QString( "%1%2" ).arg(cstrTouchDisableKey).arg(cQ).toUtf8().data(), true );
    m_pT3kHandle->SendCommand( (const char*)QString( "%1%2" ).arg(cstrTouchDisableNo).arg(cQ).toUtf8().data(), true );

    // buzzer
    m_pT3kHandle->SendCommand( (const char*)QString( "%1%2" ).arg(cstrBuzzer).arg(cQ).toUtf8().data(), true );

    m_RequestSensorData.AddItem( cstrUsbConfigMode, "?" );
    m_pT3kHandle->SendCommand( (const char*)QString("%1?").arg(cstrUsbConfigMode).toUtf8().data(), true );

    m_RequestSensorData.Start( m_pT3kHandle );
}

void QSensorSettingWidget::OnRSP(ResponsePart /*Part*/, ushort /*nTickTime*/, const char */*sPartId*/, long /*lId*/, bool /*bFinal*/, const char *sCmd)
{
    if( !isVisible() ) return;

    if( strstr(sCmd, cstrUsbConfigMode) == sCmd )
    {
        m_RequestSensorData.RemoveItem( cstrUsbConfigMode );

        int nMode = strtol(sCmd + sizeof(cstrUsbConfigMode) - 1, NULL, 16);
        switch( nMode )
        {
        case 0x04: // digitizer
            ui->CBLCalibrationKey->setEnabled(false);
            ui->CBLCalibrationKeyPresses->setEnabled(false);
            ui->CBLTouchEnableKey->setEnabled(false);
            ui->CBLTouchKeyPresses->setEnabled(false);
            break;
        case 0x07: // full
            ui->CBLCalibrationKey->setEnabled(true);
            ui->CBLCalibrationKeyPresses->setEnabled(true);
            ui->CBLTouchEnableKey->setEnabled(true);
            ui->CBLTouchKeyPresses->setEnabled(true);
            break;
        default:
            break;
        }
    }
    else if ( strstr(sCmd, cstrCalibrationKey) == sCmd )
    {
        m_RequestSensorData.RemoveItem( cstrCalibrationKey );

        int nKey;
        nKey = atoi(sCmd + sizeof(cstrCalibrationKey) - 1);
        if( !(nKey >= NV_DEF_CALIBRATION_KEY_RANGE_START && nKey < NV_DEF_CALIBRATION_KEY_RANGE_END) )
            nKey = -1;

        ui->CBLCalibrationKey->setCurrentIndex( nKey+1 );
        ui->CBLCalibrationKeyPresses->setEnabled( ui->CBLCalibrationKey->currentIndex() == 0 ? false : true );
    }
    else if ( strstr(sCmd, cstrCalibrationNo) == sCmd )
    {
        m_RequestSensorData.RemoveItem( cstrCalibrationNo );

        int nNumberOfKey;
        nNumberOfKey = atoi(sCmd + sizeof(cstrCalibrationNo) - 1);
        if( !(nNumberOfKey >= NV_DEF_CALIBRATION_KEY_NO_RANGE_START && nNumberOfKey <= NV_DEF_CALIBRATION_KEY_NO_RANGE_END) )
        {
            ui->CBLCalibrationKeyPresses->setCurrentIndex( -1 );
        }
        ui->CBLCalibrationKeyPresses->setCurrentIndex( nNumberOfKey/2-1 );
    }
    ///////////////////////////////////////////////////////////
    // touch enable/disable
    else if ( strstr(sCmd, cstrTouchEnable) == sCmd )
    {
        m_RequestSensorData.RemoveItem( cstrTouchEnable );

        int nOnOff = atoi(sCmd + sizeof(cstrTouchEnable) - 1);
        if( nOnOff == 1 )
        {
            ui->BtnChkTouch->setChecked( true );
            ui->BtnChkTouch->setText( m_strCaptionON );
        }
        else
        {
            ui->BtnChkTouch->setChecked( false );
            ui->BtnChkTouch->setText( m_strCaptionOFF );
        }
    }
    else if ( strstr(sCmd, cstrTouchDisableKey) == sCmd )
    {
        m_RequestSensorData.RemoveItem( cstrTouchDisableKey );

        int nKey = atoi(sCmd + sizeof(cstrTouchDisableKey) - 1);
        if( !(nKey >= NV_DEF_TOUCH_DISABLE_KEY_RANGE_START && nKey < NV_DEF_TOUCH_DISABLE_KEY_RANGE_END) )
            nKey = -1;

        ui->CBLTouchEnableKey->setCurrentIndex( nKey+1 );
        ui->CBLTouchKeyPresses->setEnabled( ui->CBLTouchEnableKey->currentIndex() == 0 ? false : true );
    }
    else if ( strstr(sCmd, cstrTouchDisableNo) == sCmd )
    {
        m_RequestSensorData.RemoveItem( cstrTouchDisableNo );

        int nNumberOfKey = atoi(sCmd + sizeof(cstrTouchDisableNo) - 1);
        if( !(nNumberOfKey >= NV_DEF_TOUCH_DISABLE_KEY_NO_RANGE_START && nNumberOfKey <= NV_DEF_TOUCH_DISABLE_KEY_NO_RANGE_END) )
        {
            ui->CBLTouchKeyPresses->setCurrentIndex( -1 );
        }
        ui->CBLTouchKeyPresses->setCurrentIndex( nNumberOfKey/2-1 );
    }

    ///////////////////////////////////////////////////////////
    // buzzer
    else if ( strstr(sCmd, cstrBuzzer) == sCmd )
    {
        m_RequestSensorData.RemoveItem( cstrBuzzer );

        ParseBuzzerSetting( sCmd );
    }
}

void QSensorSettingWidget::ParseBuzzerSetting( const char* szCmd )
{
    QString strBuzzerSetting( szCmd );

    int nE = strBuzzerSetting.indexOf( '=' );
    if( nE < 0 ) return;

    strBuzzerSetting.remove( 0, nE+1 );
    QString strId, strV;
    do
    {
        uchar cBuzzerId;
        uchar cOnOff;

        int nS = strBuzzerSetting.indexOf( ':' );
        if( nS < 0 ) break;
        strId = strBuzzerSetting.left( nS );
        strId.trimmed();

        strBuzzerSetting.remove( 0, nS+1 );

        int nC = strBuzzerSetting.indexOf( ' ' );
        if( nC >= 0 )
        {
            strV = strBuzzerSetting.left( nC );
            strBuzzerSetting.remove( 0, nC+1 );
        }
        else
        {
            strV = strBuzzerSetting;
            strBuzzerSetting.clear();
        }

        cBuzzerId = (uchar)strtol( strId.toUtf8().data(), NULL, 10 );

        strV.trimmed();
        cOnOff = (uchar)strtol( strV.toUtf8().data(), NULL, 10 );

        BuzzerID ebuzzerId = (BuzzerID)cBuzzerId;

        switch( ebuzzerId )
        {
        case buzzeridClick:
            ui->BtnChkSingleClk->setChecked( cOnOff == 1 ? true : false );
            ui->BtnChkSingleClk->setText( ui->BtnChkSingleClk->isChecked() ? m_strCaptionON : m_strCaptionOFF );
            break;
        case buzzeridCalibration:
            ui->BtnChkCalibration->setChecked( cOnOff == 1 ? true : false );
            ui->BtnChkCalibration->setText( ui->BtnChkCalibration->isChecked() ? m_strCaptionON : m_strCaptionOFF );
            break;
        case buzzeridKeytone:
            ui->BtnChkKeytone->setChecked( cOnOff == 1 ? true : false );
            ui->BtnChkKeytone->setText( ui->BtnChkKeytone->isChecked() ? m_strCaptionON : m_strCaptionOFF );
            break;
        case buzzeridError:
            ui->BtnChkError->setChecked( cOnOff == 1 ? true : false );
            ui->BtnChkError->setText( ui->BtnChkError->isChecked() ? m_strCaptionON : m_strCaptionOFF );
            break;
        case buzzeridSensorAttach:
            ui->BtnChkSensor->setChecked( cOnOff == 1 ? true : false );
            ui->BtnChkSensor->setText( ui->BtnChkSensor->isChecked() ? m_strCaptionON : m_strCaptionOFF );
            break;
        case buzzeridPenPairing:
            ui->BtnChkPenPairing->setChecked( cOnOff == 1 ? true : false );
            ui->BtnChkPenPairing->setText( ui->BtnChkPenPairing->isChecked() ? m_strCaptionON : m_strCaptionOFF );
            break;
        case buzzeridUsbAttach:
            ui->BtnChkUSBAttach->setChecked( cOnOff == 1 ? true : false );
            ui->BtnChkUSBAttach->setText( ui->BtnChkUSBAttach->isChecked() ? m_strCaptionON : m_strCaptionOFF );
            break;
        default:
            qDebug( "Unknown buzzer id: %d\r\n", ebuzzerId );
            break;
        }
    } while( strBuzzerSetting.size() );
}

void QSensorSettingWidget::on_CBLCalibrationKey_activated(int index)
{
    if( !m_pT3kHandle ) return;

    int nKey = index;
    if( nKey < 0 ) return;

    if( nKey == 0 )
		nKey = 3+1;

    m_pT3kHandle->SendCommand( (const char*)QString("%1%2").arg(cstrCalibrationKey).arg(nKey-1).toUtf8().data(), true );
}

void QSensorSettingWidget::on_CBLCalibrationKeyPresses_activated(int index)
{
    if( !m_pT3kHandle )
        return;

    int nNumberOfPress = index;
    if( nNumberOfPress < 0 )
        return;

    if( nNumberOfPress < 0 )
        nNumberOfPress = NV_DEF_CALIBRATION_KEY_NO;
    else
        nNumberOfPress = (nNumberOfPress + 1) * 2;

    m_pT3kHandle->SendCommand( (const char*)QString("%1%2").arg(cstrCalibrationNo).arg(nNumberOfPress).toUtf8().data(), true );
}

void QSensorSettingWidget::on_CBLTouchEnableKey_activated(int index)
{
    if( !m_pT3kHandle ) return;

    int nKey = index;
    if( nKey < 0 )
        return;

    if( nKey == 0 )
        nKey = 3+1;

    m_pT3kHandle->SendCommand( (const char*)QString("%1%2").arg(cstrTouchDisableKey).arg(nKey-1).toUtf8().data(), true );
}

void QSensorSettingWidget::on_CBLTouchKeyPresses_activated(int index)
{
    if( !m_pT3kHandle ) return;

    int nNumberOfPress = index;
    if( nNumberOfPress < 0 )
        return;

    if( nNumberOfPress < 0 )
        nNumberOfPress = NV_DEF_TOUCH_DISABLE_KEY_NO;
    else
        nNumberOfPress = (nNumberOfPress + 1) * 2;

    m_pT3kHandle->SendCommand( (const char*)QString("%1%2").arg(cstrTouchDisableNo).arg(nNumberOfPress).toUtf8().data(), true );
}

void QSensorSettingWidget::on_BtnChkTouch_clicked()
{
    if( !m_pT3kHandle ) return;

    if( ui->BtnChkTouch->isChecked() )
        m_pT3kHandle->SendCommand( (const char*)QString("%1%2").arg(cstrTouchEnable).arg(1).toUtf8().data(), true );
    else
    {
        QDiableTouchWidget* pDiableTouchWnd = new QDiableTouchWidget(m_pT3kHandle,this);
        pDiableTouchWnd->setAttribute( Qt::WA_DeleteOnClose );
        pDiableTouchWnd->setFont( font() );

        if( pDiableTouchWnd->exec() != QDialog::Accepted )
        {
            ui->BtnChkTouch->setChecked( true );
            return;
        }

        m_pT3kHandle->SendCommand( (const char*)QString("%1%2").arg(cstrTouchEnable).arg(0).toUtf8().data(), true );
    }
}

void QSensorSettingWidget::on_BtnDiagnostics_clicked()
{
    QAssistanceWidget* SensorDlg = new QAssistanceWidget( m_pT3kHandle, this );
    SensorDlg->setFont( font() );
    SensorDlg->setAttribute( Qt::WA_DeleteOnClose );
    SensorDlg->exec();
}

void QSensorSettingWidget::on_BtnSoundError_clicked()
{
    OnBuzzerPlay( 0 );
}

void QSensorSettingWidget::on_BtnSoundSingle_clicked()
{
    OnBuzzerPlay( 1 );
}

void QSensorSettingWidget::on_BtnSoundKeytone_clicked()
{
    OnBuzzerPlay( 2 );
}

void QSensorSettingWidget::on_BtnSoundCalibration_clicked()
{
    OnBuzzerPlay( 3 );
}

void QSensorSettingWidget::on_BtnSoundSensor_clicked()
{
    OnBuzzerPlay( 4 );
}

void QSensorSettingWidget::on_BtnSoundUSB_clicked()
{
    OnBuzzerPlay( 5 );
}

void QSensorSettingWidget::on_BtnSoundPenPairing_clicked()
{
    OnBuzzerPlay( 6 );
}

void QSensorSettingWidget::on_BtnChkError_clicked()
{
    OnBnClickedCheckBuzzer( ui->BtnChkError, 0 );
}

void QSensorSettingWidget::on_BtnChkSingleClk_clicked()
{
    OnBnClickedCheckBuzzer( ui->BtnChkSingleClk, 1 );
}

void QSensorSettingWidget::on_BtnChkKeytone_clicked()
{
    OnBnClickedCheckBuzzer( ui->BtnChkKeytone, 2 );
}

void QSensorSettingWidget::on_BtnChkCalibration_clicked()
{
    OnBnClickedCheckBuzzer( ui->BtnChkCalibration, 3 );
}

void QSensorSettingWidget::on_BtnChkSensor_clicked()
{
    OnBnClickedCheckBuzzer( ui->BtnChkSensor, 4 );
}

void QSensorSettingWidget::on_BtnChkUSBAttach_clicked()
{
    OnBnClickedCheckBuzzer( ui->BtnChkUSBAttach, 5 );
}

void QSensorSettingWidget::on_BtnChkPenPairing_clicked()
{
    OnBnClickedCheckBuzzer( ui->BtnChkPenPairing, 6 );
}
