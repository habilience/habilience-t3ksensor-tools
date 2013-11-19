#include "stdInclude.h"

#include "QCalibrationSettingWidget.h"
#include "ui_QCalibrationSettingWidget.h"

#include "Common/nv.h"

#include "T3kCfgWnd.h"
#include "QT3kUserData.h"
#include "QLangManager.h"
#include "QConfigData.h"
#include "QWarningWidget.h"
#include "QAdvancedSettingWidget.h"

#include <QShowEvent>
#include <QMessageBox>

#define USER_AREA_RANGE_START   0x0000                  //NV_DEF_..._AREA_RANGE_START
#define USER_AREA_RANGE_END		0x7fff                  //NV_DEF_..._AREA_RANGE_END
#define	USER_AREA_RANGE_MAX		USER_AREA_RANGE_END		//NV_DEF_..._AREA_RANGE_MAX


QCalibrationSettingWidget::QCalibrationSettingWidget(T3kHandle*& pHandle, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QCalibrationSettingWidget), m_pT3kHandle(pHandle)
{
    ui->setupUi(this);
    setFont( qApp->font() );
    ui->BtnTouchSetting->setFont( font() );
    ui->BtnAdvanced->setFont( font() );

    m_fScreenMargin = 0.f;

    m_fMMVersion    = 0.0f;
    m_nUsbConfigMode = 0;

    m_pTouchSettingWnd = NULL;
    m_pAdvancedWidget = NULL;

    m_pwndCalibration = new QCalibrationWidget( m_pT3kHandle );

    ui->TitleCalibration->SetIconImage( ":/T3kCfgRes/resources/PNG_ICON_CALIBRATION.png" );
    ui->TitleAreaSetting->SetIconImage( ":/T3kCfgRes/resources/PNG_ICON_AREA_SETTING.png" );
    ui->BtnTouchSetting->SetIconImage( ":/T3kCfgRes/resources/PNG_ICON_SETTING.png" );

    // usbconfigmode
    ui->BtnDoubleClkDec->setEnabled(false);
    ui->BtnDoubleClkInc->setEnabled(false);
    ui->EditDoubleClk->setEnabled(false);
    ui->BtnPalmDec->setEnabled(false);
    ui->BtnPalmInc->setEnabled(false);
    ui->EditPalm->setEnabled(false);
    ui->BtnTwoTouchDec->setEnabled(false);
    ui->BtnTwoTouchInc->setEnabled(false);
    ui->EditTwoTouch->setEnabled(false);
    ui->BtnTouchSetting->setEnabled(false);

    ui->BtnAdvanced->setVisible( false );

    onChangeLanguage();
}

QCalibrationSettingWidget::~QCalibrationSettingWidget()
{
    if( m_pwndCalibration )
    {
        delete m_pwndCalibration;
        m_pwndCalibration = NULL;
    }

    if( m_pTouchSettingWnd )
    {
        delete m_pTouchSettingWnd;
        m_pTouchSettingWnd = NULL;
    }
    delete ui;
}

void QCalibrationSettingWidget::onChangeLanguage()
{
    if( !winId() ) return;

    QLangRes& Res = QLangManager::instance()->getResource();
    ui->TitleCalibration->setText( Res.getResString( QString::fromUtf8("CALIBRATION SETTING"), QString::fromUtf8("TITLE_CAPTION_CALIBRATION")) );
    ui->LBScreenMargin->setText( Res.getResString(QString::fromUtf8("CALIBRATION SETTING"), QString::fromUtf8("TEXT_SCREEN_MARGIN")) );
    ui->LBMarginPersent->setText( Res.getResString(QString::fromUtf8("CALIBRATION SETTING"), QString::fromUtf8("TEXT_SCREEN_PERCENT_OF_SCREEN")) );
    ui->LBCalibrate->setText( Res.getResString(QString::fromUtf8("CALIBRATION SETTING"), QString::fromUtf8("TEXT_CALIBRATE")) );
    ui->BtnCalibration->setText( Res.getResString(QString::fromUtf8("CALIBRATION SETTING"), QString::fromUtf8("BTN_CAPTION_CALIBRATE")) );

    ui->TitleAreaSetting->setText( Res.getResString(QString::fromUtf8("CALIBRATION SETTING"), QString::fromUtf8("TITLE_AREA_SETTING")) );
    ui->LBSingleClk->setText( Res.getResString(QString::fromUtf8("CALIBRATION SETTING"), QString::fromUtf8("TEXT_SINGLE_CLICK")) );
    ui->LBDoubleClk->setText( Res.getResString(QString::fromUtf8("CALIBRATION SETTING"), QString::fromUtf8("TEXT_DOUBLE_CLICK")) );
    ui->LBTwoTouch->setText( Res.getResString(QString::fromUtf8("CALIBRATION SETTING"), QString::fromUtf8("TEXT_TWO_TOUCH")) );
    ui->LBPalm->setText( Res.getResString(QString::fromUtf8("CALIBRATION SETTING"), QString::fromUtf8("TEXT_PALM")) );

    ui->BtnTouchSetting->setText( Res.getResString(QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("BTN_CAPTION_TOUCH_SETTING")) );
    ui->BtnAdvanced->setText( Res.getResString(QString::fromUtf8("CALIBRATION SETTING"), QString::fromUtf8("BTN_ADVANCED")) );

    ui->BtnTouchSetting->setMinimumWidth( Res.isR2L() ? 90 : 0 ); // incorrect "auto calc text width" to arabic
}

void QCalibrationSettingWidget::RequestSensorData( bool bDefault )
{
    if( !m_pT3kHandle ) return;

    m_RequestSensorData.Stop();

    char cQ = bDefault ? '*' : '?';

    QString str( cQ );
    m_RequestSensorData.AddItem( cstrFirmwareVersion, "?" );
    m_RequestSensorData.AddItem( cstrCalibrationScreenMargin, str );
    //m_RequestSensorData.AddItem( cstrCalibrationMode, str );
    m_RequestSensorData.AddItem( cstrAreaC, str );
    m_RequestSensorData.AddItem( cstrAreaD, str );
    m_RequestSensorData.AddItem( cstrAreaM, str );
    m_RequestSensorData.AddItem( cstrAreaP, str );

    // calibration
    QString strTemp;
    m_pT3kHandle->SendCommand( (const char*)strTemp.sprintf( "%s?", cstrFirmwareVersion ).toUtf8(), true );
    m_pT3kHandle->SendCommand( (const char*)strTemp.sprintf( "%s%c", cstrCalibrationScreenMargin, cQ ).toUtf8(), true );
    //m_pT3kHandle->SendCommand( (const char*)strTemp.sprintf( "%s%c", cstrCalibrationMode, cQ ).toUtf8(), true );
    m_pT3kHandle->SendCommand( (const char*)strTemp.sprintf( "%s%c", cstrAreaC, cQ ).toUtf8(), true );
    m_pT3kHandle->SendCommand( (const char*)strTemp.sprintf( "%s%c", cstrAreaD, cQ ).toUtf8(), true );
    m_pT3kHandle->SendCommand( (const char*)strTemp.sprintf( "%s%c", cstrAreaM, cQ ).toUtf8(), true );
    m_pT3kHandle->SendCommand( (const char*)strTemp.sprintf( "%s%c", cstrAreaP, cQ ).toUtf8(), true );
    if( cQ == '*' )
    {
        m_RequestSensorData.AddItem( cstrCalibration, str );

        m_pT3kHandle->SendCommand( (const char*)strTemp.sprintf( "%s%c", cstrCalibration, cQ ).toUtf8(), true );
    }

    m_RequestSensorData.AddItem( cstrUsbConfigMode, "?" );
    m_pT3kHandle->SendCommand( (const char*)QString("%1?").arg(cstrUsbConfigMode).toUtf8().data(), true );


    m_RequestSensorData.Start( m_pT3kHandle );
}

void QCalibrationSettingWidget::ShowCalibrationWindow( bool bShow, int nScreenMargin/*=-1*/, int nMacMargin/*=0*/ )
{
    if( m_pwndCalibration->winId() )
    {
        float fScreenMargin = 0.0f;
        if( nScreenMargin < 0 )
        {
            QString strV( ui->EditMargin->text() );

            if( strV.size() )
                fScreenMargin = m_fScreenMargin;
            else
                fScreenMargin = (float)strtod( strV.toUtf8().data(), NULL );
        }
        else
            fScreenMargin = (float)nScreenMargin/10.0f;

        m_pwndCalibration->ShowWindow( bShow, m_nUsbConfigMode, fScreenMargin, nMacMargin, m_fMMVersion );
    }
}

void QCalibrationSettingWidget::showEvent(QShowEvent *evt)
{
    RequestSensorData( false );
    setFocusPolicy( Qt::StrongFocus );

    QString str = QConfigData::instance()->getData( "ADVANCED", "USER_ADJUSTMENT", "" ).toString();
    ui->BtnAdvanced->setVisible( str.isEmpty() ? false : str.toInt() == 1 ? true : false );

    QWidget::showEvent(evt);
}

void QCalibrationSettingWidget::hideEvent(QHideEvent *evt)
{
    setFocusPolicy( Qt::NoFocus );

    if( m_pTouchSettingWnd && m_pTouchSettingWnd->isVisible() )
        m_pTouchSettingWnd->close();

    if( m_pwndCalibration && m_pwndCalibration->isVisible() )
        m_pwndCalibration->ShowWindow( false );

    if( m_pAdvancedWidget && m_pAdvancedWidget->isVisible() )
        m_pAdvancedWidget->close();

    m_RequestSensorData.Stop();

    QWidget::hideEvent(evt);
}

void QCalibrationSettingWidget::keyPressEvent(QKeyEvent *evt)
{
    if( evt->type() == QEvent::KeyPress )
    {
        ByPassKeyPressEvent( evt );
        return;
    }

    QWidget::keyPressEvent(evt);
}

void QCalibrationSettingWidget::OnRSP(ResponsePart Part, ushort /*nTickTime*/, const char */*sPartId*/, long /*lId*/, bool /*bFinal*/, const char *sCmd)
{
    if( !winId() ) return;

    if( strstr(sCmd, cstrUsbConfigMode) == sCmd )
    {
        m_RequestSensorData.RemoveItem( cstrUsbConfigMode );

        m_nUsbConfigMode = strtol(sCmd + sizeof(cstrUsbConfigMode) - 1, NULL, 16);
        switch( m_nUsbConfigMode )
        {
        case 0x04: // digitizer
            ui->BtnDoubleClkDec->setEnabled(false);
            ui->BtnDoubleClkInc->setEnabled(false);
            ui->EditDoubleClk->setEnabled(false);
            ui->BtnPalmDec->setEnabled(false);
            ui->BtnPalmInc->setEnabled(false);
            ui->EditPalm->setEnabled(false);
            ui->BtnTwoTouchDec->setEnabled(false);
            ui->BtnTwoTouchInc->setEnabled(false);
            ui->EditTwoTouch->setEnabled(false);
            ui->BtnTouchSetting->setEnabled(false);
            break;
        case 0x07: // full
            ui->BtnDoubleClkDec->setEnabled(true);
            ui->BtnDoubleClkInc->setEnabled(true);
            ui->EditDoubleClk->setEnabled(true);
            ui->BtnPalmDec->setEnabled(true);
            ui->BtnPalmInc->setEnabled(true);
            ui->EditPalm->setEnabled(true);
            ui->BtnTwoTouchDec->setEnabled(true);
            ui->BtnTwoTouchInc->setEnabled(true);
            ui->EditTwoTouch->setEnabled(true);
            ui->BtnTouchSetting->setEnabled(true);
            break;
        default:
            break;
        }
    }
    else if ( strstr(sCmd, cstrCalibrationScreenMargin) == sCmd )
    {
        m_RequestSensorData.RemoveItem( cstrCalibrationScreenMargin );

        float fScreenMargin;
        fScreenMargin = (float)atof(sCmd + sizeof(cstrCalibrationScreenMargin) - 1);
        if( fScreenMargin < NV_DEF_CALIBRATION_SCRNMARGIN_START )
            fScreenMargin = NV_DEF_CALIBRATION_SCRNMARGIN_START;

        if( fScreenMargin > NV_DEF_CALIBRATION_SCRNMARGIN_END )
            fScreenMargin = NV_DEF_CALIBRATION_SCRNMARGIN_END;

        m_fScreenMargin = fScreenMargin;
        QString strV;
        strV.sprintf( "%.1f", fScreenMargin );
        ui->EditMargin->setText( strV );
    }
    else if ( strstr(sCmd, cstrAreaC) == sCmd )
    {
        m_RequestSensorData.RemoveItem( cstrAreaC );

        int nArea = atoi(sCmd + sizeof(cstrAreaC) - 1);
        if( nArea < USER_AREA_RANGE_START )
            nArea = USER_AREA_RANGE_START;
        if( nArea > USER_AREA_RANGE_END )
            nArea = USER_AREA_RANGE_END;

        float fArea = (float)nArea * 100.f / USER_AREA_RANGE_MAX;
        fArea = RoundP1(fArea);

        QString strV;
        strV.sprintf( "%.1f", fArea );
        ui->EditSingleClk->setText( strV );
    }
    else if ( strstr(sCmd, cstrAreaD) == sCmd )
    {
        m_RequestSensorData.RemoveItem( cstrAreaD );

        int nArea = atoi(sCmd + sizeof(cstrAreaD) - 1);
        if( nArea < USER_AREA_RANGE_START )
            nArea = USER_AREA_RANGE_START;
        if( nArea > USER_AREA_RANGE_END )
            nArea = USER_AREA_RANGE_END;

        float fArea = (float)nArea * 100.f / USER_AREA_RANGE_MAX;
        fArea = RoundP1(fArea);

        QString strV;
        strV.sprintf( "%.1f", fArea );
        ui->EditDoubleClk->setText( strV );
    }
    else if ( strstr(sCmd, cstrAreaM) == sCmd )
    {
        m_RequestSensorData.RemoveItem( cstrAreaM );

        int nArea = atoi(sCmd + sizeof(cstrAreaM) - 1);
        if( nArea < USER_AREA_RANGE_START )
            nArea = USER_AREA_RANGE_START;
        if( nArea > USER_AREA_RANGE_END )
            nArea = USER_AREA_RANGE_END;

        float fArea = (float)nArea * 100.f / USER_AREA_RANGE_MAX;
        fArea = RoundP1(fArea);

        QString strV;
        strV.sprintf( "%.1f", fArea );
        ui->EditTwoTouch->setText( strV );
    }
    else if ( strstr(sCmd, cstrAreaP) == sCmd )
    {
        m_RequestSensorData.RemoveItem( cstrAreaP );

        int nArea = atoi(sCmd + sizeof(cstrAreaP) - 1);
        if( nArea < USER_AREA_RANGE_START )
            nArea = USER_AREA_RANGE_START;
        if( nArea > USER_AREA_RANGE_END )
            nArea = USER_AREA_RANGE_END;

        float fArea = (float)nArea * 100.f / USER_AREA_RANGE_MAX;
        fArea = RoundP1(fArea);

        QString strV;
        strV.sprintf( "%.1f", fArea );
        ui->EditPalm->setText( strV );
    }
    else if( strstr( sCmd, cstrCalibrationMode ) == sCmd &&
             (QT3kUserData::GetInstance()->GetFirmwareVersion() < MM_MIN_SUPPORT_STT_VERSION) )
    {
        m_RequestSensorData.RemoveItem( cstrCalibrationMode );

        int nMode = atoi(sCmd + sizeof(cstrCalibrationMode) - 1);
        if ( (nMode == MODE_CALIBRATION_NONE) || (nMode == MODE_CALIBRATION_SELF) )
        {
            if ( nMode == MODE_CALIBRATION_SELF )
            {
                char* pArg = NULL;
                pArg = (char*)strchr( sCmd, ',' );
                int nScreenMargin = 0;
                int nMacMargin = -1;
                if( pArg )
                {
                    nScreenMargin = atoi( pArg+1 );
                    pArg = (char*)strchr( pArg+1, ',' );
                    if( pArg )
                        nMacMargin = atoi( pArg+1 );
                }
                else
                {
                    nScreenMargin = -1;
                }
#ifdef Q_OS_WIN
                ShowCalibrationWindow( true, nScreenMargin, nMacMargin );
#else
                ShowCalibrationWindow( true, nScreenMargin, false );
#endif
                // Calibration mode!
                ui->BtnCalibration->setEnabled( false );
            }
            else
            {
                ui->BtnCalibration->setEnabled( true );
                ShowCalibrationWindow( false );
            }
        }
    }
    else if ( strstr(sCmd, cstrCalibration) == sCmd )
    {
        m_RequestSensorData.RemoveItem( cstrCalibration );
    }
    else if( Part == MM && strstr( sCmd, cstrFirmwareVersion ) == sCmd )
    {
        m_RequestSensorData.RemoveItem( cstrFirmwareVersion );

        QString strMMVersion = sCmd + sizeof(cstrFirmwareVersion) - 1;
        strMMVersion.trimmed();
        m_fMMVersion = 0.0f;
        m_fMMVersion = strMMVersion.left( strMMVersion.indexOf( ' ' ) ).toFloat();
    }
}

void QCalibrationSettingWidget::OnSTT(ResponsePart /*Part*/, ushort /*nTickTime*/, const char */*sPartId*/, const char *pStatus)
{
    if( !winId() ) return;

    if( strstr( pStatus, cstrCalibrationMode ) == pStatus )
    {
        m_RequestSensorData.RemoveItem( cstrCalibrationMode );

        int nMode = atoi(pStatus + sizeof(cstrCalibrationMode) - 1);
        if ( (nMode == MODE_CALIBRATION_NONE) || (nMode == MODE_CALIBRATION_SELF) )
        {
            if ( nMode == MODE_CALIBRATION_SELF )
            {
                char* pArg = NULL;
                pArg = (char*)strchr( pStatus, ',' );
                int nScreenMargin = 0;
                int nMacMargin = -1;
                if( pArg )
                {
                    nScreenMargin = atoi( pArg+1 );
                    pArg = (char*)strchr( pArg+1, ',' );
                    if( pArg )
                        nMacMargin = atoi( pArg+1 );
                }
                else
                {
                    nScreenMargin = -1;
                }
#ifdef Q_OS_WIN
                ShowCalibrationWindow( true, nScreenMargin, nMacMargin );
#else
                ShowCalibrationWindow( true, nScreenMargin, false );
#endif
                // Calibration mode!
                ui->BtnCalibration->setEnabled( false );
            }
            else
            {
                ui->BtnCalibration->setEnabled( true );
                ShowCalibrationWindow( false );
            }
        }
    }
}

void QCalibrationSettingWidget::SetDefault()
{
    RequestSensorData( true );
}

void QCalibrationSettingWidget::Refresh()
{
    RequestSensorData( false );
}

void QCalibrationSettingWidget::ChildClose()
{
    if( m_pwndCalibration && m_pwndCalibration->isVisible() )
        m_pwndCalibration->ShowWindow( false );

    if( m_pTouchSettingWnd && m_pTouchSettingWnd->isVisible() )
        m_pTouchSettingWnd->close();
}

void QCalibrationSettingWidget::on_BtnCalibration_clicked()
{
    //ShowCalibrationWindow( true ); return;
    if( !m_pT3kHandle ) return;

    QT3kUserData* pUD = QT3kUserData::GetInstance();
    float fVer = pUD->GetFirmwareVersion();
    if( fVer >= MM_MIN_SUPPORT_MACMARGIN_N_INVERTWHEEL_VERSION )
    {
        bool bShow = false;
#ifdef Q_OS_MAC
        bShow = !pUD->IsMacMargin();
#else
        bShow = pUD->IsMacMargin();
#endif
        if( bShow )
        {
            QLangRes& Res = QLangManager::instance()->getResource();
#ifdef Q_OS_MAC
            QString strMessage = Res.getResString( QString::fromUtf8("CALIBRATION SETTING"), QString::fromUtf8("TEXT_CALIBRATION_WARNING") );
            QString strMsgTitle = Res.getResString( QString::fromUtf8("WARNING SENSOR DIAGNOSIS"), QString::fromUtf8("TITLE_CAPTION") );
#else
            QString strMessage = Res.getResString( QString::fromUtf8("CALIBRATION SETTING"), QString::fromUtf8("TEXT_CALIBRATION_WARNING2") );
            QString strMsgTitle = Res.getResString( QString::fromUtf8("WARNING SENSOR DIAGNOSIS"), QString::fromUtf8("TITLE_CAPTION") );
#endif

            QMessageBox msgBox( this );
            msgBox.setWindowTitle( strMsgTitle );
            msgBox.setText( strMessage );
            msgBox.setStandardButtons( QMessageBox::Ok );
            msgBox.setIcon( QMessageBox::Warning );
            msgBox.setButtonText( QMessageBox::Ok, Res.getResString( QString::fromUtf8("MESSAGEBOX"), QString::fromUtf8("BTN_CAPTION_OK") ) );
            msgBox.setFont( font() );

            msgBox.exec();
        }
    }

    m_pT3kHandle->SendCommand( (const char*)QString("%1%2").arg(cstrCalibrationMode).arg(MODE_CALIBRATION_SELF).toUtf8(), true );
}

void QCalibrationSettingWidget::on_BtnMarginDec_clicked()
{
    if( !m_pT3kHandle ) return;

    QString strV( ui->EditMargin->text() );
    float fV = (float)strtod( strV.toUtf8().data(), NULL );
    fV -= 1.0f;

    if( fV < NV_DEF_CALIBRATION_SCRNMARGIN_START || fV > NV_DEF_CALIBRATION_SCRNMARGIN_END )
        fV = NV_DEF_CALIBRATION_SCRNMARGIN_START;

    strV.sprintf( "%.1f", fV );
    ui->EditMargin->setText( strV );

    m_pT3kHandle->SendCommand( (const char*)strV.sprintf( "%s%3.1f", cstrCalibrationScreenMargin, fV ).toUtf8(), true );
}

void QCalibrationSettingWidget::on_BtnMarginInc_clicked()
{
    if( !m_pT3kHandle ) return;

    QString strV( ui->EditMargin->text() );
    float fV = (float)strtod( strV.toUtf8().data(), NULL );
    fV += 1.0f;

    if( fV < NV_DEF_CALIBRATION_SCRNMARGIN_START || fV > NV_DEF_CALIBRATION_SCRNMARGIN_END )
        fV = NV_DEF_CALIBRATION_SCRNMARGIN_END;

    strV.sprintf( "%.1f", fV );
    ui->EditMargin->setText( strV );

    m_pT3kHandle->SendCommand( (const char*)strV.sprintf( "%s%3.1f", cstrCalibrationScreenMargin, fV ).toUtf8(), true );
}

void QCalibrationSettingWidget::on_BtnSingleClkDec_clicked()
{
    OnRangeLeftRight( true,
                      ui->EditSingleClk,
                      USER_AREA_RANGE_START,
                      USER_AREA_RANGE_END,
                      USER_AREA_RANGE_MAX,
                      cstrAreaC );
}

void QCalibrationSettingWidget::on_BtnSingleClkInc_clicked()
{
    OnRangeLeftRight( false,
                      ui->EditSingleClk,
                      USER_AREA_RANGE_START,
                      USER_AREA_RANGE_END,
                      USER_AREA_RANGE_MAX,
                      cstrAreaC );
}

void QCalibrationSettingWidget::on_BtnDoubleClkDec_clicked()
{
    OnRangeLeftRight( true,
                      ui->EditDoubleClk,
                      USER_AREA_RANGE_START,
                      USER_AREA_RANGE_END,
                      USER_AREA_RANGE_MAX,
                      cstrAreaD );
}

void QCalibrationSettingWidget::on_BtnDoubleClkInc_clicked()
{
    OnRangeLeftRight( false,
                      ui->EditDoubleClk,
                      USER_AREA_RANGE_START,
                      USER_AREA_RANGE_END,
                      USER_AREA_RANGE_MAX,
                      cstrAreaD );
}

void QCalibrationSettingWidget::on_BtnTwoTouchDec_clicked()
{
    OnRangeLeftRight( true,
                      ui->EditTwoTouch,
                      USER_AREA_RANGE_START,
                      USER_AREA_RANGE_END,
                      USER_AREA_RANGE_MAX,
                      cstrAreaM );
}

void QCalibrationSettingWidget::on_BtnTwoTouchInc_clicked()
{
    OnRangeLeftRight( false,
                      ui->EditTwoTouch,
                      USER_AREA_RANGE_START,
                      USER_AREA_RANGE_END,
                      USER_AREA_RANGE_MAX,
                      cstrAreaM );
}

void QCalibrationSettingWidget::on_BtnPalmDec_clicked()
{
    OnRangeLeftRight( true,
                      ui->EditPalm,
                      USER_AREA_RANGE_START,
                      USER_AREA_RANGE_END,
                      USER_AREA_RANGE_MAX,
                      cstrAreaP );
}

void QCalibrationSettingWidget::on_BtnPalmInc_clicked()
{
    OnRangeLeftRight( false,
                      ui->EditPalm,
                      USER_AREA_RANGE_START,
                      USER_AREA_RANGE_END,
                      USER_AREA_RANGE_MAX,
                      cstrAreaP );
}

float QCalibrationSettingWidget::ChangeRangeValue( bool bDecrease, float fCurrentValue )
{
    float fChangeValue = 0.1f;
    if( fCurrentValue < 1.f )
        fChangeValue = 0.1f;
    else if( fCurrentValue < 3.f )
        fChangeValue = 0.2f;
    else if( fCurrentValue < 5.f )
        fChangeValue = 0.4f;
    else if( fCurrentValue < 20.f )
        fChangeValue = 0.6f;
    else if( fCurrentValue < 25.f )
        fChangeValue = 1.f;
    else
        fChangeValue = 2.f;

    return bDecrease ? fChangeValue*(-1) : fChangeValue;
}

void QCalibrationSettingWidget::OnRangeLeftRight( bool bDecrease, QLineEdit* pEdit, int nRangeS, int nRangeE, int nRangeMax, const char* szCmdH )
{
    if( !m_pT3kHandle ) return;

    QString strV( pEdit->text() );
    float fV = (float)strtod( strV.toUtf8().data(), NULL );
    fV += ChangeRangeValue( bDecrease, fV );

    int nV = (int)(fV * nRangeMax / 100.f);

    if( nV < nRangeS )
    {
        nV = nRangeS;
        fV = (float)nV * 100.f / nRangeMax;
        fV = RoundP1(fV);
    }

    if( nV > nRangeE )
    {
        nV = nRangeE;
        fV = (float)nV * 100.f / nRangeMax;
        fV = RoundP1(fV);
    }

    strV.sprintf( "%.1f", fV );
    pEdit->setText( strV );

    QByteArray btrTemp;
    btrTemp = strV.sprintf( "%s%d", szCmdH, nV ).toUtf8();
    const char* pszCmd = btrTemp.data();
    m_pT3kHandle->SendCommand( pszCmd, true );
}

void QCalibrationSettingWidget::on_BtnTouchSetting_clicked()
{
    if( !m_pTouchSettingWnd )
    {
        m_pTouchSettingWnd = new QTouchSettingWidget( m_pT3kHandle, QT3kUserData::GetInstance()->getTopParent() );
        m_pTouchSettingWnd->setFont( font() );
    }

    m_pTouchSettingWnd->exec();
}

void QCalibrationSettingWidget::on_BtnAdvanced_clicked()
{
    QWarningWidget warning( QT3kUserData::GetInstance()->getTopParent() );
    if( warning.exec() == QDialog::Rejected )
        return;

    if( !m_pAdvancedWidget )
        m_pAdvancedWidget = new QAdvancedSettingWidget( QT3kUserData::GetInstance()->getTopParent() );

    m_pAdvancedWidget->exec();
}
