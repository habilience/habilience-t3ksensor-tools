#include "QGeneralSettingWidget.h"

#include <QShowEvent>
#include <QSettings>
#include <QMessageBox>

#include "stdInclude.h"
#include "T3kCfgWnd.h"

#include "QT3kUserData.h"
#include "QCustomDefaultSensor.h"

#include "Common/nv.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

QGeneralSettingWidget::QGeneralSettingWidget(QT3kDevice*& pHandle, QWidget *parent) :
    QWidget(parent),
    m_pT3kHandle(pHandle)
{
    setupUi(this);
    setFont( qApp->font() );

    m_nInputModeV = 0x0000;
    m_nInputMode = 0;
    m_nTimerAutoInputMode = 0;

    m_nChkUsbCfgMode = -1;

    m_nDisplayOrientation = -1;

    ChkInputModeAutoSelect->setChecked( false );

    TitleLanguage->SetIconImage( ":/T3kCfgRes/resources/PNG_ICON_LANGUAGE.png");
    TitleInputMode->SetIconImage( ":/T3kCfgRes/resources/PNG_ICON_INPUT_MODE.png");
    TitleDisplayOrientation->SetIconImage( ":/T3kCfgRes/resources/PNG_ICON_MULTI_MONITOR.png");

    int nSelectLanguage = QLangManager::instance()->getActiveLanguage();

    int nSelIdx = -1;
    int nPos = QLangManager::instance()->getFirstAvailableLanguage();
    while( nPos >= 0 )
    {
        QString strLangName = QLangManager::instance()->getLanguageName( nPos );
        CBLLanguage->addItem( strLangName );
        int nIdx = CBLLanguage->count()-1;
        CBLLanguage->setItemData( nIdx, nPos );
        if( nPos == nSelectLanguage )
            nSelIdx = nIdx;

        nPos = QLangManager::instance()->getNextAvailableLanguage();
    }

    CBLLanguage->setCurrentIndex( nSelIdx );

    // usbconfigmode
//    TitleDisplayOrientation->setVisible( false );
//    GBOrientation->setVisible( false );
//    RBtnLandscape->setVisible( false );
//    RBtnPortrait->setVisible( false );
//    RBtnLandscapeF->setVisible( false );
//    RBtnPortraitF->setVisible( false );

    onChangeLanguage();

    QRect rcL( CBLLanguage->x(), CBLLanguage->y(), CBLLanguage->width()-1, CBLLanguage->height() );
    rcL.setTopLeft( mapFromParent( rcL.topLeft() ) );
    LBLanguageMsg->setGeometry( rcL.left()-rcL.width()/2, rcL.top()+rcL.height()/2, rcL.width()*2, rcL.height() );

    CBLLanguage->setVisible( false );

    TitleTrayIcon->SetIconImage( ":/T3kCfgRes/resources/PNG_ICON_TRAYICON.png" );

    QString strPath = QCoreApplication::applicationDirPath();
    if( strPath.at( strPath.size()-1 ) == '/' )
        strPath.remove( strPath.size()-1, 1 );

    QSettings RegOption( "Habilience", "T3kCfg" );
    RegOption.beginGroup( "Options" );
    if( RegOption.value( "Exec Path", "" ).toString().compare( strPath ) )
    {
        RegOption.setValue( "TrayIcon", false );
        RegOption.setValue( "Exec Path", strPath );
    }

    if( RegOption.value( "TrayIcon", false ).toBool() )
        chkTrayIcon->setChecked( true );
    else
        chkTrayIcon->setChecked( false );
    RegOption.endGroup();

    if( QLangManager::instance()->getAvailableLanguageCount() <= 1 )
    {
        CBLLanguage->hide();
        LBLanguageMsg->show();
        LanguageLayout->removeWidget( CBLLanguage );
        LanguageLayout->addWidget( LBLanguageMsg );
    }
    else
    {
        CBLLanguage->show();
        LBLanguageMsg->hide();
    }
}

QGeneralSettingWidget::~QGeneralSettingWidget()
{
    m_RequestSensorData.Stop();
}

void QGeneralSettingWidget::onChangeLanguage()
{
    if( !winId() ) return;

    QLangRes& Res = QLangManager::instance()->getResource();

    TitleLanguage->setText( Res.getResString(QString::fromUtf8("SETTING"), QString::fromUtf8("TITLE_LANGUAGE")) );
    TitleInputMode->setText( Res.getResString(QString::fromUtf8("SETTING"), QString::fromUtf8("TITLE_CAPTION_TOUCH_INPUT_MODE")) );

    LBLanguage->setText( Res.getResString(QString::fromUtf8("SETTING"), QString::fromUtf8("TEXT_LANGUAGE")) );
    LBLanguageMsg->setText( Res.getResString(QString::fromUtf8("SETTING"), QString::fromUtf8("TEXT_LANGUAGE_NOT_FOUND")) );

    LBInputMode->setText( Res.getResString(QString::fromUtf8("SETTING"), QString::fromUtf8("TEXT_INPUT_MODE")) );
    RBMouse->setText( Res.getResString(QString::fromUtf8("SETTING"), QString::fromUtf8("TEXT_INPUT_MODE_MOUSE")) );
    RBMultiTouchWin7->setText( Res.getResString(QString::fromUtf8("SETTING"), QString::fromUtf8("TEXT_INPUT_MODE_MULTI_TOUCH")) );

    ChkInputModeAutoSelect->setText( Res.getResString(QString::fromUtf8("SETTING"), QString::fromUtf8("TEXT_INPUT_MODE_AUTO_SELECT")) );

    TitleTrayIcon->setText( Res.getResString( QString::fromUtf8("SETTING"), QString::fromUtf8("TITLE_TRAY_ICON") ) );
    chkTrayIcon->setText( Res.getResString( QString::fromUtf8("SETTING"), QString::fromUtf8("TEXT_CHECK_TRAYICON") ) );

    TitleDisplayOrientation->setText( Res.getResString( QString::fromUtf8("SETTING"), QString::fromUtf8("TITLE_DISPLAY_ORIENTATION") ) );
#ifdef Q_OS_WIN
    GBOrientation->setTitle( Res.getResString( QString::fromUtf8("SETTING"), QString::fromUtf8("TEXT_DISPLAY_ORIENTATION") ) );
    RBtnLandscape->setText( Res.getResString( QString::fromUtf8("SETTING"), QString::fromUtf8("TEXT_DISPLAY_ORIENTATION_1") ) );
    RBtnPortrait->setText( Res.getResString( QString::fromUtf8("SETTING"), QString::fromUtf8("TEXT_DISPLAY_ORIENTATION_2") ) );
    RBtnLandscapeF->setText( Res.getResString( QString::fromUtf8("SETTING"), QString::fromUtf8("TEXT_DISPLAY_ORIENTATION_3") ) );
    RBtnPortraitF->setText( Res.getResString( QString::fromUtf8("SETTING"), QString::fromUtf8("TEXT_DISPLAY_ORIENTATION_4") ) );
#elif defined(Q_OS_LINUX)
    GBOrientation->setTitle( Res.getResString( QString::fromUtf8("SETTING"), QString::fromUtf8("TEXT_DISPLAY_X11_ORIENTATION") ) );
    RBtnLandscape->setText( Res.getResString( QString::fromUtf8("SETTING"), QString::fromUtf8("TEXT_DISPLAY_X11_ORIENTATION_1") ) );
    RBtnPortrait->setText( Res.getResString( QString::fromUtf8("SETTING"), QString::fromUtf8("TEXT_DISPLAY_X11_ORIENTATION_2") ) );
    RBtnLandscapeF->setText( Res.getResString( QString::fromUtf8("SETTING"), QString::fromUtf8("TEXT_DISPLAY_X11_ORIENTATION_3") ) );
    RBtnPortraitF->setText( Res.getResString( QString::fromUtf8("SETTING"), QString::fromUtf8("TEXT_DISPLAY_X11_ORIENTATION_4") ) );
#elif defined(Q_OS_MAC)
    GBOrientation->setTitle( Res.getResString( QString::fromUtf8("SETTING"), QString::fromUtf8("TEXT_DISPLAY_MAC_ORIENTATION") ) );
    RBtnLandscape->setText( Res.getResString( QString::fromUtf8("SETTING"), QString::fromUtf8("TEXT_DISPLAY_MAC_ORIENTATION_1") ) );
    RBtnPortrait->setText( Res.getResString( QString::fromUtf8("SETTING"), QString::fromUtf8("TEXT_DISPLAY_MAC_ORIENTATION_2") ) );
    RBtnLandscapeF->setText( Res.getResString( QString::fromUtf8("SETTING"), QString::fromUtf8("TEXT_DISPLAY_MAC_ORIENTATION_3") ) );
    RBtnPortraitF->setText( Res.getResString( QString::fromUtf8("SETTING"), QString::fromUtf8("TEXT_DISPLAY_MAC_ORIENTATION_4") ) );
#endif

    QFontMetrics ft( ChkInputModeAutoSelect->font() );
    int nBlankW = ft.width( " " );
    int nLen = (ft.width( ChkInputModeAutoSelect->text() ) + 50) / nBlankW;
    QString str;
    for( int i=0; i<nLen; i++ )
        str += " ";
    GBInputMode->setTitle( str );
}

void QGeneralSettingWidget::SetDefault()
{
    RequestGeneralSetting( true );
}

void QGeneralSettingWidget::Refresh()
{
    RequestGeneralSetting( false );
}

void QGeneralSettingWidget::RequestGeneralSetting( bool bDefault )
{
    if( !m_pT3kHandle )
        return;

    m_RequestSensorData.Stop();

    char cQ = bDefault ? '*' : '?';

    QString str( cQ );
    m_RequestSensorData.AddItem( cstrInputMode, str );
    bool bSupportDOCmd = QT3kUserData::GetInstance()->GetFirmwareVersion() >= MM_MIN_SUPPORT_DISPLAYORICMD_VERSION;
    if( bSupportDOCmd )
        m_RequestSensorData.AddItem( cstrDisplayOrientation, "?" );

    QCustomDefaultSensor* pInstance = QCustomDefaultSensor::Instance();
    if( pInstance->IsLoaded() )
        m_pT3kHandle->sendCommand( QString("%1%2").arg(cstrInputMode).arg(pInstance->GetDefaultData(cstrInputMode, str)), true );
    else
        m_pT3kHandle->sendCommand( QString("%1%2").arg(cstrInputMode).arg(cQ), true );

    if( bSupportDOCmd )
        m_pT3kHandle->sendCommand( QString("%1%2").arg(cstrDisplayOrientation).arg(cQ), true );

    m_RequestSensorData.Start( m_pT3kHandle );

    if( bDefault )
    {
        int nDefineIndex = QLangManager::instance()->getDefaultLanguage();
        for( int i=0; i<CBLLanguage->count(); i++ )
        {
            bool bOK = false;
            int nItemIdx = CBLLanguage->itemData( i ).toInt( &bOK );
            if( bOK && nItemIdx == nDefineIndex )
            {
                CBLLanguage->setCurrentIndex( i );
                on_CBLLanguage_activated( i );
                break;
            }
        }

        QSettings RegOption( "Habilience", "T3kCfg" );
        RegOption.beginGroup( "Options" );
        RegOption.setValue( "TrayIcon", false );
        RegOption.endGroup();

        chkTrayIcon->setChecked( false );

        if( !bSupportDOCmd )
            m_pT3kHandle->sendCommand( QString("%1*").arg(cstrCalibration), true );
    }

    m_RequestSensorData.AddItem( cstrUsbConfigMode, "?" );
    m_nChkUsbCfgMode = m_pT3kHandle->sendCommand( QString("%1?").arg(cstrUsbConfigMode), true );
}

void QGeneralSettingWidget::TPDP_OnRSP(T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short /*ticktime*/, const char */*partid*/, int /*id*/, bool /*bFinal*/, const char *cmd)
{
    if( !isVisible() ) return;

    if( strstr(cmd, cstrUsbConfigMode) == cmd )
    {
        m_RequestSensorData.RemoveItem( cstrUsbConfigMode );
        m_nChkUsbCfgMode = -1;

        int nMode = strtol(cmd + sizeof(cstrUsbConfigMode) - 1, NULL, 16);
        switch( nMode )
        {
        case 0x04: // digitizer
            GBInputMode->setEnabled(false);
            if( m_nDisplayOrientation != -1 )
            {
                TitleDisplayOrientation->setVisible( false );
                GBOrientation->setVisible( false );
                RBtnLandscape->setVisible( false );
                RBtnPortrait->setVisible( false );
                RBtnLandscapeF->setVisible( false );
                RBtnPortraitF->setVisible( false );
            }
            break;
        case 0x07: // full
            GBInputMode->setEnabled(true);
            if( m_nDisplayOrientation != -1 )
            {
                TitleDisplayOrientation->setVisible( true );
                GBOrientation->setVisible( true );
                RBtnLandscape->setVisible( true );
                RBtnPortrait->setVisible( true );
                RBtnLandscapeF->setVisible( true );
                RBtnPortraitF->setVisible( true );
            }
            break;
        default:
            break;
        }
    }
    else if ( strstr(cmd, cstrInputMode) == cmd )
    {
        m_RequestSensorData.RemoveItem( cstrInputMode );

        int nInputMode = -1;
        nInputMode = strtol(cmd + sizeof(cstrInputMode) - 1, NULL, 16);
        int nInputModeMulti = -1;
        char* pInputModeData = NULL;
        pInputModeData = (char*)strchr( cmd, ',' );
        if( pInputModeData )
        {
            nInputModeMulti = strtol( pInputModeData+1, NULL, 16 );
            if( QT3kUserData::GetInstance()->GetFirmwareVersion() < MM_MIN_SUPPORT_DISPLAYORICMD_VERSION )
            {
                if( (pInputModeData = (char*)strchr( pInputModeData+1, ',' )) )
                {
                    m_nDisplayOrientation = strtol( pInputModeData+1, NULL, 10 );

                    if( !TitleDisplayOrientation->isVisible() )
                    {
                        TitleDisplayOrientation->setVisible( true );
                        GBOrientation->setVisible( true );
                        RBtnLandscape->setVisible( true );
                        RBtnPortrait->setVisible( true );
                        RBtnLandscapeF->setVisible( true );
                        RBtnPortraitF->setVisible( true );
                    }

                    ChangeRadioButtonOrientation( m_nDisplayOrientation );
                }
                else
                {
                    if( TitleDisplayOrientation->isVisible() )
                    {
                        TitleDisplayOrientation->setVisible( false );
                        GBOrientation->setVisible( false );
                        RBtnLandscape->setVisible( false );
                        RBtnPortrait->setVisible( false );
                        RBtnLandscapeF->setVisible( false );
                        RBtnPortraitF->setVisible( false );
                    }

                    m_nDisplayOrientation = -1;
                }
            }
        }
        m_nInputModeV = (nInputMode << 8) | nInputModeMulti;

        if( nInputMode == 0xFF )
        {
            nInputMode = nInputModeMulti;
            ChkInputModeAutoSelect->setChecked( true );
        }
        else
        {
            if( ChkInputModeAutoSelect->isChecked() )
                ChkInputModeAutoSelect->setChecked( false );
        }

        m_nInputMode = nInputMode;

        switch( m_nInputMode )
        {
        case 0:
            RBMouse->setChecked( true );
            break;
        case 2:
            RBMultiTouchWin7->setChecked( true );
            break;
        default:
            break;
        }
    }
    else if( strstr( cmd, cstrDisplayOrientation ) == cmd )
    {
        m_RequestSensorData.RemoveItem( cstrDisplayOrientation );

        m_nDisplayOrientation = strtol(cmd + sizeof(cstrDisplayOrientation) - 1, NULL, 10);

        ChangeRadioButtonOrientation( m_nDisplayOrientation );
    }
}

void QGeneralSettingWidget::TPDP_OnRSE(T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short /*ticktime*/, const char */*partid*/, int id, bool /*bFinal*/, const char */*cmd*/)
{
    if( m_nChkUsbCfgMode == id )
    {
        m_RequestSensorData.RemoveItem( cstrUsbConfigMode );
        m_nChkUsbCfgMode = -1;
    }
}

void QGeneralSettingWidget::showEvent(QShowEvent *evt)
{
    if( evt->type() == QEvent::Show )
    {
        RequestGeneralSetting( false );
        setFocusPolicy( Qt::StrongFocus );
    }

    return QWidget::showEvent(evt);
}

void QGeneralSettingWidget::hideEvent(QHideEvent *evt)
{
    if( evt->type() == QEvent::Hide )
    {
        setFocusPolicy( Qt::NoFocus );
        m_RequestSensorData.Stop();
    }
    QWidget::hideEvent(evt);
}

void QGeneralSettingWidget::keyPressEvent(QKeyEvent *evt)
{
    if( evt->type() == QEvent::KeyPress )
    {
        ByPassKeyPressEvent( evt );
        return;
    }

    QWidget::keyPressEvent(evt);
}

void QGeneralSettingWidget::timerEvent(QTimerEvent *evt)
{
    if( evt->type() == QEvent::Timer )
    {
        if( evt->timerId() == m_nTimerAutoInputMode )
        {
            killTimer( m_nTimerAutoInputMode );
            m_nTimerAutoInputMode = 0;

            QString str;
            if( ChkInputModeAutoSelect->isChecked() )
            {
                m_pT3kHandle->sendCommand( str.sprintf( "%s0xFF", cstrInputMode ), true );
            }
            else
            {
                m_pT3kHandle->sendCommand( str.sprintf( "%s0x%02x", cstrInputMode, m_nInputMode ), true );
            }
        }
    }

    QWidget::timerEvent(evt);
}

void QGeneralSettingWidget::on_CBLLanguage_activated(int index)
{
    int nSelIdx = index;
    if( nSelIdx < 0 ) return;

    int nLangIdx = CBLLanguage->itemData( nSelIdx ).toInt();

    QLangManager::instance()->setLanguage( nLangIdx );
}

void QGeneralSettingWidget::on_chkTrayIcon_toggled(bool checked)
{
    emit RegisterTrayIcon( checked );

    QSettings RegisterValue( tr("Habilience"), tr("T3kCfg") );
    RegisterValue.beginGroup( tr("Options") );
    RegisterValue.setValue( tr("TrayIcon"), checked );
    RegisterValue.endGroup();
}

void QGeneralSettingWidget::on_ChkInputModeAutoSelect_clicked(bool /*clicked*/)
{
    if( m_nTimerAutoInputMode )
        killTimer( m_nTimerAutoInputMode );

    m_nTimerAutoInputMode = startTimer( 500 );
}

void QGeneralSettingWidget::on_RBMouse_clicked()
{
    if( ChkInputModeAutoSelect->isChecked() )
        ChkInputModeAutoSelect->setChecked( false );

    m_pT3kHandle->sendCommand( QString("%10x00").arg(cstrInputMode), true );

    if( (QT3kUserData::GetInstance()->getFirmwareVersionStr() <= "2.8a") && chkTrayIcon->isChecked() )
    {
        m_pT3kHandle->sendCommand( QString("%1?").arg(cstrMouseProfile), true );
    }
}

void QGeneralSettingWidget::on_RBMultiTouchWin7_clicked()
{
#if defined(Q_OS_WIN)
    OSVERSIONINFOW Info;
    Info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOW);
    if( GetVersionEx( &Info ) )
    {
        if( Info.dwMajorVersion <= 6 && Info.dwMinorVersion <= 1 )
        {
            bool bMsg = false;
            if( Info.dwMajorVersion == 6 && Info.dwMinorVersion == 1 )
            {
                typedef bool (WINAPI *fnGetProductInfo)(DWORD, DWORD, DWORD, DWORD, PDWORD);
                DWORD dwType = 0;
                fnGetProductInfo GetProductInfo = (fnGetProductInfo)GetProcAddress( GetModuleHandle(L"kernel32.dll"), "GetProductInfo" );
                GetProductInfo( Info.dwMajorVersion, Info.dwMinorVersion, 0, 0, &dwType );
                if( dwType == 0x00000002 || // PRODUCT_HOME_BASIC
                    dwType == 0x00000043 || // PRODUCT_HOME_BASIC_E
                    dwType == 0x00000005 || // PRODUCT_HOME_BASIC_N
                    dwType == 0x0000000B || // PRODUCT_STARTER
                    dwType == 0x00000042 || // PRODUCT_STARTER_E
                    dwType == 0x0000002F )  // RPODUCT_STARTER_N
                {
                    bMsg = true;
                }
            }
            else
            {
                bMsg = true;
            }

            if( bMsg )
            {
                QLangRes& Res = QLangManager::instance()->getResource();
                QString strMessage = Res.getResString( QString::fromUtf8("SETTING"), QString::fromUtf8("TEXT_INPUT_MODE_WARNING_MSG") );
                QString strMsgTitle = Res.getResString( QString::fromUtf8("WARNING SENSOR DIAGNOSIS"), QString::fromUtf8("TITLE_CAPTION") );

                QMessageBox msgBox( this );
                msgBox.setWindowTitle( strMsgTitle );
                msgBox.setText( strMessage );
                msgBox.setStandardButtons( QMessageBox::Yes | QMessageBox::No );
                msgBox.setIcon( QMessageBox::Warning );
                msgBox.setButtonText( QMessageBox::Yes, Res.getResString( QString::fromUtf8("MESSAGEBOX"), QString::fromUtf8("BTN_CAPTION_YES") ) );
                msgBox.setButtonText( QMessageBox::No, Res.getResString( QString::fromUtf8("MESSAGEBOX"), QString::fromUtf8("BTN_CAPTION_NO") ) );
                msgBox.setFont( font() );

                if( msgBox.exec() != QMessageBox::Yes )
                {
                    RBMultiTouchWin7->setChecked( false );
                    RBMouse->setChecked( true );
                    return;
                }
            }
        }
    }
#endif
    if( ChkInputModeAutoSelect->isChecked() )
        ChkInputModeAutoSelect->setChecked( false );

    QString str;
    m_pT3kHandle->sendCommand( str.sprintf( "%s0x%02x", cstrInputMode, 0x02 ), true );

    if( (QT3kUserData::GetInstance()->getFirmwareVersionStr() <= "2.8a") && chkTrayIcon->isChecked() )
    {
        m_pT3kHandle->sendCommand( QString("%1?").arg(cstrMouseProfile), true );
    }
}

void QGeneralSettingWidget::ChangeRadioButtonOrientation(int nOrientation)
{
    switch( nOrientation )
    {
    case 0:
        RBtnLandscape->setChecked( true );
        break;
    case 1:
        RBtnPortrait->setChecked( true );
        break;
    case 2:
        RBtnLandscapeF->setChecked( true );
        break;
    case 3:
        RBtnPortraitF->setChecked( true );
        break;
    default:
        Q_ASSERT( false );
        break;
    }
}

const uint g_nCalibration[4] = { 0, 1, 2, 3 };
void QGeneralSettingWidget::on_RBtnLandscape_clicked()
{
    if( QT3kUserData::GetInstance()->GetFirmwareVersion() >= MM_MIN_SUPPORT_DISPLAYORICMD_VERSION )
    {
        m_pT3kHandle->sendCommand( QString("%1%2").arg(cstrDisplayOrientation).arg(0), true );
        return;
    }
    QString strCmd;
    if( m_pT3kHandle->sendCommand( strCmd.sprintf( "%s0x%02x,0x%02x,0x%02x", cstrInputMode, m_nInputModeV >> 8, m_nInputModeV & 0x00FF, 0x000 ), true ) )
    {
        int nCalVIdx = g_nCalibration[(m_nDisplayOrientation)%4];
        m_pT3kHandle->sendCommand( QString("%1%2").arg(cstrCalibration).arg(nCalVIdx), true );
    }
}

void QGeneralSettingWidget::on_RBtnPortrait_clicked()
{
    if( QT3kUserData::GetInstance()->GetFirmwareVersion() >= MM_MIN_SUPPORT_DISPLAYORICMD_VERSION )
    {
        m_pT3kHandle->sendCommand( QString("%1%2").arg(cstrDisplayOrientation).arg(1), true );
        return;
    }
    QString strCmd;
    if( m_pT3kHandle->sendCommand( strCmd.sprintf( "%s0x%02x,0x%02x,0x%02x", cstrInputMode, m_nInputModeV >> 8, m_nInputModeV & 0x00FF, 0x0001 ), true ) )
    {
        int nCalVIdx = g_nCalibration[(m_nDisplayOrientation+3)%4];
        m_pT3kHandle->sendCommand( QString("%1%2").arg(cstrCalibration).arg(nCalVIdx), true );
    }
}

void QGeneralSettingWidget::on_RBtnLandscapeF_clicked()
{
    if( QT3kUserData::GetInstance()->GetFirmwareVersion() >= MM_MIN_SUPPORT_DISPLAYORICMD_VERSION )
    {
        m_pT3kHandle->sendCommand( QString("%1%2").arg(cstrDisplayOrientation).arg(2), true );
        return;
    }
    QString strCmd;
    if( m_pT3kHandle->sendCommand( strCmd.sprintf( "%s0x%02x,0x%02x,0x%02x", cstrInputMode, m_nInputModeV >> 8, m_nInputModeV & 0x00FF, 0x0002 ), true ) )
    {
        int nCalVIdx = g_nCalibration[(m_nDisplayOrientation+2)%4];
        m_pT3kHandle->sendCommand( QString("%1%2").arg(cstrCalibration).arg(nCalVIdx), true );
    }
}

void QGeneralSettingWidget::on_RBtnPortraitF_clicked()
{
    if( QT3kUserData::GetInstance()->GetFirmwareVersion() >= MM_MIN_SUPPORT_DISPLAYORICMD_VERSION )
    {
        m_pT3kHandle->sendCommand( QString("%1%2").arg(cstrDisplayOrientation).arg(3), true );
        return;
    }
    QString strCmd;
    if( m_pT3kHandle->sendCommand( strCmd.sprintf( "%s0x%02x,0x%02x,0x%02x", cstrInputMode, m_nInputModeV >> 8, m_nInputModeV & 0x00FF, 0x0003 ), true ) )
    {
        int nCalVIdx = g_nCalibration[(m_nDisplayOrientation+1)%4];
        m_pT3kHandle->sendCommand( QString("%1%2").arg(cstrCalibration).arg(nCalVIdx), true );
    }
}
