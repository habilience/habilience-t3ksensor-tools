//#include "stdInclude.h"

#include "../Common/T3k_ver.h"

#include "QMainMenuWidget.h"
#include "ui_QMainMenuWidget.h"

#include <QDateTime>
#include <QCoreApplication>
#include <QResizeEvent>
#include <QMessageBox>

#include "T3kCfgWnd.h"

#include "QUnderlineLabel.h"
#include "QLangManager.h"
#include "QSoftkey.h"
#include "QT3kUserData.h"

#include "CfgCustomCmdDef.h"

extern bool g_bIsScreenShotMode;

#define QICON_WIDTH             40
#define QICON_HEIGHT            40

QMainMenuWidget::QMainMenuWidget(T30xHandle*& pHandle, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QMainMenuWidget), m_pT3kHandle(pHandle)
{
    ui->setupUi(this);
    setFont( parent->font() );

#if defined(Q_OS_MAC)
    QFont ft( parent->font() );
    ft.setPointSize( ft.pointSize()-1 );
    ui->EditFirmWareVer->setFont( ft );
    ui->MainMenuLayout->setSpacing( 10 );
#elif defined(Q_OS_X11)
    QFont font( parent->font() );
    font.setPointSize(10);
    ui->EditFirmWareVer->setFont( font );
#endif

    m_fMMVersion = 0.0f;
    m_bMMNoCam = 0;
    m_bDigitizerMode = false;

    ui->ProgramInfoLabel->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_INFO.png" );
    ui->FirmwareInfoLabel->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_INFO.png" );

    QPixmap* pIconArray = new QPixmap( ":/T3kCfgRes/Resources/PNG_ICON_MENU_STRIP.png" );
    QList<QIcon> listIcon;
    int nCount = pIconArray->width()/40;
    for( int i=0; i<nCount; i++ )
    {
        listIcon.push_back( QIcon(pIconArray->copy(QRect( i*QICON_WIDTH, 0, QICON_WIDTH, QICON_HEIGHT ) )) );
    }
    delete pIconArray;

    ui->BtnMainMouse->setIcon( listIcon.at(1).pixmap( QSize(QICON_WIDTH,QICON_HEIGHT), QIcon::Active ) );
    ui->BtnMainCali->setIcon( listIcon.at(2).pixmap( QSize(QICON_WIDTH,QICON_HEIGHT), QIcon::Active ) );
    ui->BtnMainSensor->setIcon( listIcon.at(3).pixmap( QSize(QICON_WIDTH,QICON_HEIGHT), QIcon::Active ) );
    ui->BtnMainSoftkey->setIcon( listIcon.at(4).pixmap( QSize(QICON_WIDTH,QICON_HEIGHT), QIcon::Active ) );
    ui->BtnMainGeneral->setIcon( listIcon.at(5).pixmap( QSize(QICON_WIDTH,QICON_HEIGHT), QIcon::Active ) );

    ui->BtnMainSoftkey->setVisible( false );

    QString strAppVer = QCoreApplication::applicationVersion();

    QString strProgInfo = QT3kUserData::GetInstance()->GetProgramInfo();

    if( strProgInfo.isEmpty() )
    {
        strProgInfo = QString::fromLocal8Bit("    T3kCfg");
    }

    if ( g_bIsScreenShotMode )
    {
        ui->EditProgVer->setText( strProgInfo + " Ver " + "x.xx" );
    }
    else
    {
        ui->EditProgVer->setText( strProgInfo + " Ver " + strAppVer );
    }

    // usbconfigmode
    ui->BtnMainSoftkey->setVisible(false);

    OnChangeLanguage();
}

QMainMenuWidget::~QMainMenuWidget()
{
    m_RequestSensorData.Stop();

    delete ui;
}

void QMainMenuWidget::OnChangeLanguage()
{
    if( !winId() )
        return;

    QLangRes& Res = QLangManager::GetPtr()->GetResource();
    ui->BtnMainMouse->setText( Res.GetResString( QString::fromUtf8("START PAGE"), QString::fromUtf8("BTN_CAPTION_MOUSE") ) );
    ui->BtnMainMouse->setDescription( Res.GetResString( QString::fromUtf8("START PAGE"), QString::fromUtf8("BTN_CAPTION_MOUSE_SUB1") ) + "\n" +
                                      Res.GetResString( QString::fromUtf8("START PAGE"), QString::fromUtf8("BTN_CAPTION_MOUSE_SUB2") ) );

    ui->BtnMainCali->setText( Res.GetResString( QString::fromUtf8("START PAGE"), QString::fromUtf8("BTN_CAPTION_CALIBRATION") ) );
    ui->BtnMainCali->setDescription( Res.GetResString( QString::fromUtf8("START PAGE"), QString::fromUtf8("BTN_CAPTION_CALIBRATION_SUB1") ) + "\n" +
                                     Res.GetResString( QString::fromUtf8("START PAGE"), QString::fromUtf8("BTN_CAPTION_CALIBRATION_SUB2") ) );

    ui->BtnMainSensor->setText( Res.GetResString( QString::fromUtf8("START PAGE"), QString::fromUtf8("BTN_CAPTION_SENSOR_SETTING") ) );
    ui->BtnMainSensor->setDescription( Res.GetResString( QString::fromUtf8("START PAGE"), QString::fromUtf8("BTN_CAPTION_SENSOR_SETTING_SUB1") ) + "\n" +
                                       Res.GetResString( QString::fromUtf8("START PAGE"), QString::fromUtf8("BTN_CAPTION_SENSOR_SETTING_SUB2") ) );

    ui->BtnMainGeneral->setText( Res.GetResString( QString::fromUtf8("START PAGE"), QString::fromUtf8("BTN_CAPTION_GENERAL_SETTING") ) );
    ui->BtnMainGeneral->setDescription( Res.GetResString( QString::fromUtf8("START PAGE"), QString::fromUtf8("BTN_CAPTION_GENERAL_SETTING_SUB1") ) + "\n" +
                                        Res.GetResString( QString::fromUtf8("START PAGE"), QString::fromUtf8("BTN_CAPTION_GENERAL_SETTING_SUB2") ) );

    ui->BtnMainSoftkey->setText( Res.GetResString( QString::fromUtf8("START PAGE"), QString::fromUtf8("BTN_CAPTION_SOFTKEY_SETTING") ) );
//    ui->BtnMainSoftkey->setDescription( Res.GetResString( QString::fromUtf8("START PAGE"), QString::fromUtf8("BTN_CAPTION_SOFTKEY_SETTING_SUB1") ) + "\n" +
//                                       Res.GetResString( QString::fromUtf8("START PAGE"), QString::fromUtf8("BTN_CAPTION_SOFTKEY_SETTING_SUB2") ) );

    ui->ProgramInfoLabel->setText(Res.GetResString( QString::fromUtf8("MAIN"), QString::fromUtf8("TITLE_CAPTION_INFORMATION") ) );
    ui->FirmwareInfoLabel->setText( Res.GetResString( QString::fromUtf8("MAIN"), QString::fromUtf8("TITLE_CAPTION_FIRMWARE") ) );

    if( Res.IsR2L() )
    {
        ui->EditProgVer->setAlignment( Qt::AlignVCenter | Qt::AlignRight );
        ui->EditFirmWareVer->setAlignment( Qt::AlignVCenter | Qt::AlignRight );

        ui->ExtraMenuLayout->setAlignment( ui->BtnMainSoftkey, Qt::AlignRight );
    }
    else
    {
        ui->EditProgVer->setAlignment( Qt::AlignVCenter | Qt::AlignLeft );
        ui->EditFirmWareVer->setAlignment( Qt::AlignVCenter | Qt::AlignLeft );

        ui->ExtraMenuLayout->setAlignment( ui->BtnMainSoftkey, Qt::AlignLeft );
    }
}

void QMainMenuWidget::RequestInformation()
{
    if ( !m_pT3kHandle->IsOpen() )
        return;

    ushort nModel = QT3kUserData::GetInstance()->GetModel();
    m_RequestSensorData.Stop();

    m_RequestSensorData.AddItem( cstrFirmwareVersion, "?" );
    m_RequestSensorData.AddItem( cstrFirmwareVersion, "?", QRequestHIDManager::CM1 );
    m_RequestSensorData.AddItem( cstrAdminSettingTime, "?", QRequestHIDManager::CM1 );
    m_RequestSensorData.AddItem( cstrFirmwareVersion, "?", QRequestHIDManager::CM2 );
    m_RequestSensorData.AddItem( cstrAdminSettingTime, "?", QRequestHIDManager::CM2 );

    if( nModel != 0x0000 && nModel != 0x3000 && nModel != 0x3100 )
    {
        m_RequestSensorData.AddItem( cstrFirmwareVersion, "?", QRequestHIDManager::CM1_1 );
        m_RequestSensorData.AddItem( cstrAdminSettingTime, "?", QRequestHIDManager::CM1_1 );
        m_RequestSensorData.AddItem( cstrFirmwareVersion, "?", QRequestHIDManager::CM2_1 );
        m_RequestSensorData.AddItem( cstrAdminSettingTime, "?", QRequestHIDManager::CM2_1 );
    }

    m_pT3kHandle->SendCommand( (const char*)QString("%1?").arg(cstrFirmwareVersion).toUtf8().data(), true );
    m_pT3kHandle->SendCommand( (const char*)QString("%1%2?").arg(cstrCam1).arg(cstrFirmwareVersion).toUtf8().data(), true );
    m_pT3kHandle->SendCommand( (const char*)QString("%1%2?").arg(cstrCam1).arg(cstrAdminSettingTime).toUtf8().data(), true );
    m_pT3kHandle->SendCommand( (const char*)QString("%1%2?").arg(cstrCam2).arg(cstrFirmwareVersion).toUtf8().data(), true );
    m_pT3kHandle->SendCommand( (const char*)QString("%1%2?").arg(cstrCam2).arg(cstrAdminSettingTime).toUtf8().data(), true );

    if( nModel != 0x0000 && nModel != 0x3000 && nModel != 0x3100 )
    {
        m_pT3kHandle->SendCommand( (const char*)QString("%1sub/%2?").arg(cstrCam1).arg(cstrFirmwareVersion).toUtf8().data(), true );
        m_pT3kHandle->SendCommand( (const char*)QString("%1sub/%2?").arg(cstrCam1).arg(cstrAdminSettingTime).toUtf8().data(), true );
        m_pT3kHandle->SendCommand( (const char*)QString("%1sub/%2?").arg(cstrCam2).arg(cstrFirmwareVersion).toUtf8().data(), true );
        m_pT3kHandle->SendCommand( (const char*)QString("%1sub/%2?").arg(cstrCam2).arg(cstrAdminSettingTime).toUtf8().data(), true );
    }

    m_pT3kHandle->SendCommand( (const char*)QString("%1?").arg(cstrUsbConfigMode).toUtf8().data(), false );

    if( !m_bDigitizerMode )
    {
        m_RequestSensorData.AddItem( cstrSoftkey, "?" );

        m_pT3kHandle->SendCommand( (const char*)QString("%1?").arg(cstrSoftkey).toUtf8().data(), true );
    }

    m_RequestSensorData.Start( m_pT3kHandle );
}

void QMainMenuWidget::on_BtnMainMouse_clicked()
{
    emit ShowMenuEvent( (int)MenuMouseSetting );
}

void QMainMenuWidget::on_BtnMainCali_clicked()
{
    emit ShowMenuEvent( (int)MenuCalibrationSetting );
}

void QMainMenuWidget::on_BtnMainSensor_clicked()
{
    emit ShowMenuEvent( (int)MenuSensorSetting );
}

void QMainMenuWidget::on_BtnMainGeneral_clicked()
{
    emit ShowMenuEvent( (int)MenuGeneralSetting );
}

void QMainMenuWidget::on_BtnMainSoftkey_clicked()
{
    emit ShowMenuEvent( (int)MenuSoftkeySetting );
}

void QMainMenuWidget::UpdateInformation()
{
    bool bVerDiff = false;
    QString strFirmware;
    strFirmware =           "  Main module\r\n";
    strFirmware +=          QString("    - MM Ver %1%2\r\n").arg(m_strMMVersion).arg(m_strMMSN);
    strFirmware +=          "  Camera module\r\n";
    for( QMap<int,CMVerInfo>::iterator iter = m_mapCMVerInfo.begin(); iter != m_mapCMVerInfo.end(); ++iter )
    {
        CMVerInfo& info = iter.value();
        strFirmware +=      QString("    - CM%1%2 Ver %3%4\r\n").arg((((iter.key()-1)%2)+1)).arg(((iter.key()-1)/2) == 0 ? "" : "_1")
                            .arg(info.strCMVersion).arg(info.strCMSN);
        strFirmware +=      QString("    Alignment date: %1\r\n").arg(info.strSetDate);

        if( !bVerDiff )
        {
            QString strMM( m_strMMVersion.left( m_strMMVersion.indexOf(' ') ) );
            if( strMM.right(1).compare("a") >= 0 && strMM.right(1).compare("z") <= 0 )
                strMM = strMM.left( strMM.length()-1 );

            QString strCM( info.strCMVersion.left( info.strCMVersion.indexOf(' ') ) );
            if( strCM.right(1).compare("a") >= 0 && strCM.right(1).compare("z") <= 0 )
                strCM = strCM.left( strCM.length()-1 );

            if( strMM != strCM )
                bVerDiff = true;
        }
    }

    if( ui->EditFirmWareVer )
        ui->EditFirmWareVer->setText( QString::fromLocal8Bit(strFirmware.toStdString().c_str()) );

    if( !g_bIsScreenShotMode && bVerDiff )
        ui->EditFirmWareVer->setTextColor( Qt::red );
    else
        ui->EditFirmWareVer->setTextColor( QColor(100,100,100) );

    QT3kUserData* pInst = QT3kUserData::GetInstance();
    pInst->SetFirmwareVersion( m_fMMVersion );
}

void QMainMenuWidget::OnRSP(ResponsePart Part, short /*lTickTime*/, const char */*sPartId*/, long /*lId*/, bool /*bFinal*/, const char *sCmd)
{
    if( !isVisible() ) return;

    bool bUpdateInformation = false;
    const char* szCmd = sCmd;

    if( strstr(sCmd, cstrUsbConfigMode) == sCmd )
    {
        m_RequestSensorData.RemoveItem( cstrUsbConfigMode );

        int nMode = strtol(sCmd + sizeof(cstrUsbConfigMode) - 1, NULL, 16);
        switch( nMode )
        {
        case 0x04: // digitizer
            ui->BtnMainSoftkey->setVisible(false);
            m_bDigitizerMode = true;
            break;
        case 0x07: // full
            ui->BtnMainSoftkey->setVisible(true);
            m_bDigitizerMode = false;
            break;
        default:
            break;
        }
    }
    else if( strstr(szCmd, cstrSoftkey) == szCmd )
    {
        m_RequestSensorData.RemoveItem( cstrSoftkey );

        QString strSoftKey( szCmd );

        int nE = strSoftKey.indexOf( '=' );
        if( nE >= 0 )
        {
            strSoftKey.remove( 0, nE+1 );

            if( strSoftKey.size() && strSoftKey.at( 0 ) != '0' )
            {
                QSoftkeyArray arySK;
                arySK.Load( strSoftKey.toStdString().c_str(), NULL );
                int nSKCount = arySK.GetSize();
                ui->BtnMainSoftkey->setVisible( nSKCount ? true : false );
            }
            else
                ui->BtnMainSoftkey->setVisible( false );
        }
    }
    else
    {
        switch( Part )
        {
        case CM1:
        case CM2:
        case CM1_1:
        case CM2_1:
            if ( strstr(szCmd, cstrFirmwareVersion) == szCmd )
            {
                m_RequestSensorData.RemoveItem( cstrFirmwareVersion, (QRequestHIDManager::eRequestPart)Part );

                char szVer[255];
                if ( g_bIsScreenShotMode )
                    strncpy( szVer, "x.xx", 255 );
                else
                    strncpy( szVer, (szCmd + sizeof(cstrFirmwareVersion) - 1), 255 );

                CMVerInfo info = m_mapCMVerInfo.value( Part );
                info.strCMVersion = QString("%1").arg(szVer);
                info.bNoCam = -1;

                if( g_bIsScreenShotMode )
                {
                   info.fCMVersion = 0;
                   info.nModel = 0;
                }
                else
                {
                    info.fCMVersion = atof( szVer ) + .005;
                    info.nModel = atoi( strchr( szVer, ' ' ) + 2 );
                }

                m_mapCMVerInfo.insert( Part, info );

                bUpdateInformation = true;
            }
            else if ( strstr(szCmd, cstrAdminSettingTime) == szCmd )
            {
                m_RequestSensorData.RemoveItem( cstrAdminSettingTime, (QRequestHIDManager::eRequestPart)Part );

                long lDate = strtol(szCmd + sizeof(cstrAdminSettingTime) - 1, NULL, 0);
                QDateTime dtDate;
                dtDate.setTime_t( lDate );
                CMVerInfo info = m_mapCMVerInfo.value( Part );
                info.strSetDate = dtDate.toString("yyyy MM dd hh:mm");
                m_mapCMVerInfo.insert( Part, info );

                bUpdateInformation = true;
            }
            else if ( strstr(szCmd, cstrAdminSerial) == szCmd )
            {
                m_RequestSensorData.RemoveItem( cstrAdminSerial, (QRequestHIDManager::eRequestPart)Part );

                char szVer[255];
                strncpy( szVer, (szCmd + sizeof(cstrAdminSerial) - 1), 255 );
                QString strSN( QString("%1").arg(szVer) );
                strSN.trimmed();
                //strSN.Trim( _T(" ,") );
                strSN.trimmed();
                CMVerInfo info = m_mapCMVerInfo.value( Part );
                if( !strSN.size() )
                    info.strCMSN = QString( "(S/N:%1)").arg(strSN);
                else
                    info.strCMSN.clear();

                bUpdateInformation = true;
            }
            break;

        case MM:
            if ( strstr(szCmd, cstrFirmwareVersion) == szCmd )
            {
                m_RequestSensorData.RemoveItem( cstrFirmwareVersion );

                char szVer[255];
                if ( g_bIsScreenShotMode )
                    strncpy( szVer, "x.xx", 255 );
                else
                    strncpy( szVer, (szCmd + sizeof(cstrFirmwareVersion) - 1), 255 );

                m_strMMVersion = QString(("%1")).arg(szVer);
                bUpdateInformation = true;

                if ( g_bIsScreenShotMode )
                {
                    strncpy( szVer, (szCmd + sizeof(cstrFirmwareVersion) - 1), 255 );

                    char szActualVer[255];
                    strncpy( szActualVer, (szCmd + sizeof(cstrFirmwareVersion) - 1), 255 );
                    m_fMMVersion = atof( szActualVer ) + .005;
                }
                else
                    m_fMMVersion = atof( szVer ) + .005;

                m_bMMNoCam = -1;

                float fFirmwareVersion = (float)atof(szCmd + sizeof(cstrFirmwareVersion) - 1);
                if( fFirmwareVersion < MM_MIN_SUPPORT_FIRMWARE_VERSION )
                {
                    QString strFirmwareVersion = QString("%1").arg( fFirmwareVersion, 0, 'f', 1 );
                    connect( this, SIGNAL(ShowErrorMsgBox(QString)), parent(), SLOT(OnShowErrorMsgBox(QString)), Qt::QueuedConnection );
                    emit ShowErrorMsgBox(strFirmwareVersion);
                }
            }
            else if ( strstr(szCmd, cstrAdminSerial) == szCmd )
            {
                m_RequestSensorData.RemoveItem( cstrAdminSerial );

                char szVer[255];
                const char* sss = szCmd + sizeof(cstrAdminSerial) - 1;
                if( (uchar)sss[0] == 0xff )
                {
                    m_strMMSN.clear();
                }
                else
                {
                    strncpy( szVer, (szCmd + sizeof(cstrAdminSerial) - 1), 255 );
                    //m_strMMSN.Format(_T("(S/N:%6s)"),  szVer);
                    QString strSN(QString("%1").arg(szVer));
                    //strSN.Trim( _T(" ,") );
                    if( !strSN.size() )
                        m_strMMSN = QString( "(S/N:%1)").arg(strSN);
                    else
                        m_strMMSN.clear();
                }
                bUpdateInformation = true;
            }
            break;
        default:
            break;
        }

        if( bUpdateInformation )
        {
            UpdateInformation();
        }
    }
}

void QMainMenuWidget::OnRSE(ResponsePart Part, short /*lTickTime*/, const char */*sPartId*/, long /*lId*/, bool /*bFinal*/, const char *sCmd)
{
    if( !isVisible() ) return;

    if( strstr( sCmd, cstrNoCam ) == sCmd )
    {
        if( Part == CM1 || Part == CM2 )
        {
            m_RequestSensorData.RemoveItem( cstrFirmwareVersion, (QRequestHIDManager::eRequestPart)(Part+2));
            m_RequestSensorData.RemoveItem( cstrAdminSettingTime, (QRequestHIDManager::eRequestPart)(Part+2));
//            CMVerInfo info = m_mapCMVerInfo.value( Part+2 );
//            m_mapCMVerInfo.insert( Part+2, info );
        }

        m_RequestSensorData.RemoveItem( cstrFirmwareVersion, (QRequestHIDManager::eRequestPart)Part );
        m_RequestSensorData.RemoveItem( cstrAdminSettingTime, (QRequestHIDManager::eRequestPart)Part );

        if( Part != CM1_1 && Part != CM2_1 )
        {
            CMVerInfo info = m_mapCMVerInfo.value( Part );
            m_mapCMVerInfo.insert( Part, info );
        }

        UpdateInformation();
    }
}

void QMainMenuWidget::showEvent(QShowEvent *evt)
{
    if( evt->type() == QEvent::Show )
    {
        m_mapCMVerInfo.clear();

        RequestInformation();
    }

    QWidget::showEvent(evt);
}

void QMainMenuWidget::hideEvent(QHideEvent *evt)
{
    m_RequestSensorData.Stop();

    ui->EditFirmWareVer->clear();
    ui->BtnMainSoftkey->hide();

    QWidget::hideEvent(evt);
}

void QMainMenuWidget::closeEvent(QCloseEvent *evt)
{
    m_RequestSensorData.Stop();

    QWidget::closeEvent(evt);
}
