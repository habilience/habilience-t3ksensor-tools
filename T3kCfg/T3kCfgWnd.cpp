#include "T3kCfgWnd.h"
#include "ui_T3kCfgWnd.h"

#include "QMainMenuWidget.h"
#include "QMouseSettingWidget.h"
#include "QCalibrationSettingWidget.h"
#include "QSensorSettingWidget.h"
#include "QGeneralSettingWidget.h"
#include "QSoftKeySettingWidget.h"
#include "QMenuStripWidget.h"

#include "QRDisableScreenWidget.h"

#include "QProfileLabel.h"

#include "QSelectSensorWidget.h"
#include "QLicenseWidget.h"
#include "QT3kUserData.h"

#include "QWidgetCloseEventManager.h"

#include "Common/nv.h"
#include "../Common/T3k_ver.h"

#include <QMenu>
#include <QSettings>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QUrl>
#include <QPainter>
#include <QMessageBox>
#include <QFileInfo>
#include <QSharedMemory>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QGraphicsOpacityEffect>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#include "T3kPacketDef.h"

extern bool g_bIsScreenShotMode;

#define CUSTOM_CONFIG_FILENAME  "Config.ini"

T3kCfgWnd::T3kCfgWnd(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::T3kCfgWnd)
{
    ui->setupUi(this);

    setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);

#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
    genAdjustButtonWidgetForWinAndX11( this );
#endif

    QFont ft( qApp->font() );
    ft.setPointSizeF( ft.pointSizeF()+1.0 );
    setFont( ft );

    setFixedSize( width(), height() );

#ifdef HITACHI_VER
    ui->BtnLogo->ChangeIcon( ":/T3kCfgRes/Resources/PNG_HABILIENCE_LOGO.png" );
#else
    ui->BtnLogo->ChangeIcon( ":/T3kCfgRes/Resources/PNG_HABILIENCE_LOGO.png" );
#endif

    m_strTitle = QString::fromLocal8Bit("T3k Series Configurator");
    m_strCompanyUrl = "http://www.habilience.com";

    m_pT3kHandle = NULL;
    m_nProfileIndex = -1;
    m_nCurrentMenu = 0;

    m_bAnimation = false;
    m_pCurrObj = NULL;
    m_pNextObj = NULL;

    m_pTrayIcon = NULL;
    m_pTrayMenu = NULL;
    m_bRegisterTrayIcon = false;

    m_bFirmwareDownloading = false;
    m_bIsConnect = false;

    m_nTimerExit = 0;
    m_nTimerObserver = 0;
    m_nTimerChkTrayDoubleClk = 0;

    m_nTimerChkHIDCom = 0;

    m_eFlatMenuStatus = MenuHome;

    m_bSoftkey = false;

    m_nCurInputMode = -1;
    m_nProfileIndexData = -1;

    m_nSendCmdID = -1;

#ifdef Q_OS_WIN
    m_bRunOtherTool = false;
    m_bPrevShowed = false;
#endif

    setAnimated( true );
    setDockOptions( AnimatedDocks );

    m_pT3kHandle = new T3kHandle();

    QSettings regT3kCfg( "Habilience", "T3kCfg" );

    QString strT3kCfgVer = regT3kCfg.value( "T3kCfg Version" ).toString();
    QString strAppVer( QCoreApplication::applicationVersion() );
    if( strAppVer != strT3kCfgVer )
    {
        regT3kCfg.setValue( "T3kCfg Version", strAppVer );

        regT3kCfg.beginGroup( "Language" );
        regT3kCfg.setValue( "Default", 0 );
        regT3kCfg.setValue( "IsDefined", false );
        regT3kCfg.endGroup();

        regT3kCfg.beginGroup( "Options" );
        regT3kCfg.setValue( "TrayIcon", false );
        regT3kCfg.setValue( "Exec Path", "" );
        regT3kCfg.endGroup();

        regT3kCfg.beginGroup( "ProfileNames" );
        regT3kCfg.setValue( "Profile1", QProfileName1 );
        regT3kCfg.setValue( "Profile2", QProfileName2 );
        regT3kCfg.setValue( "Profile3", QProfileName3 );
        regT3kCfg.setValue( "Profile4", QProfileName4 );
        regT3kCfg.setValue( "Profile5", QProfileName5 );
        regT3kCfg.endGroup();

        regT3kCfg.beginGroup( "Window Position" );
        QDesktopWidget DeskWidget;
        const QRect rcPrimaryMon = DeskWidget.screenGeometry( DeskWidget.primaryScreen() );
        regT3kCfg.setValue( "x", (rcPrimaryMon.width()-width())/2 );
        regT3kCfg.setValue( "y", (rcPrimaryMon.height()-height())/2 );
        regT3kCfg.endGroup();
    }

    LoadCompany();

    m_pMainWidget = new QMainMenuWidget( m_pT3kHandle, this );
    m_pMouseSettingWidget = new QMouseSettingWidget( m_pT3kHandle, this );
    m_pCaliSettingWidget = new QCalibrationSettingWidget( m_pT3kHandle, this );
    m_pSensorSettingWidget = new QSensorSettingWidget( m_pT3kHandle, this );
    m_pGeneralSettingWidget = new QGeneralSettingWidget( m_pT3kHandle, this );
    m_pSoftkeySettingWidget = new QSoftKeySettingWidget( m_pT3kHandle, this );
    m_pMenuWidget = NULL;

    m_pMainWidget->setGeometry( 0, 50, 620, 330 );
    m_pMainWidget->hide();
    m_pMainWidget->setAttribute( Qt::WA_DeleteOnClose );
    m_pMainWidget->setFont( font() );
    connect( m_pMainWidget, SIGNAL(ShowMenuEvent(int)), this, SLOT(onShowMenuEvent(int)) );

    m_pMouseSettingWidget->setGeometry( 0, 50, 620, 340 );
    m_pMouseSettingWidget->hide();
    m_pMouseSettingWidget->setAttribute( Qt::WA_DeleteOnClose );
    connect( m_pMouseSettingWidget, SIGNAL(ByPassKeyPressEvent(QKeyEvent*)), this ,SLOT(onByPassKeyPressEvent(QKeyEvent*)), Qt::DirectConnection );

    m_pCaliSettingWidget->setGeometry( 0, 50, 620, 340 );
    m_pCaliSettingWidget->hide();
    m_pCaliSettingWidget->setAttribute( Qt::WA_DeleteOnClose );
    connect( m_pCaliSettingWidget, SIGNAL(ByPassKeyPressEvent(QKeyEvent*)), this ,SLOT(onByPassKeyPressEvent(QKeyEvent*)), Qt::DirectConnection );

    m_pSensorSettingWidget->setGeometry( 0, 50, 620, 340 );
    m_pSensorSettingWidget->hide();
    m_pSensorSettingWidget->setAttribute( Qt::WA_DeleteOnClose );
    connect( m_pSensorSettingWidget, SIGNAL(ByPassKeyPressEvent(QKeyEvent*)), this ,SLOT(onByPassKeyPressEvent(QKeyEvent*)), Qt::DirectConnection );

    m_pGeneralSettingWidget->setGeometry( 0, 50, 620, 340 );
    m_pGeneralSettingWidget->hide();
    m_pGeneralSettingWidget->setAttribute( Qt::WA_DeleteOnClose );
    connect( m_pGeneralSettingWidget, SIGNAL(ByPassKeyPressEvent(QKeyEvent*)), this ,SLOT(onByPassKeyPressEvent(QKeyEvent*)), Qt::DirectConnection );
    connect( m_pGeneralSettingWidget, SIGNAL(RegisterTrayIcon(bool)), this, SLOT(onRegisterTrayIcon(bool)) );

    m_pSoftkeySettingWidget->setGeometry( 0, 50, 620, 340 );
    m_pSoftkeySettingWidget->hide();
    m_pSoftkeySettingWidget->setAttribute( Qt::WA_DeleteOnClose );
    connect( m_pSoftkeySettingWidget, SIGNAL(ByPassKeyPressEvent(QKeyEvent*)), this ,SLOT(onByPassKeyPressEvent(QKeyEvent*)), Qt::DirectConnection );

    m_pMenuWidget = new QMenuStripWidget( m_pT3kHandle, this );
    m_pMenuWidget->hide();
    m_pMenuWidget->setAttribute( Qt::WA_DeleteOnClose );
    connect( m_pMenuWidget, SIGNAL(ShowMenuEvent(int)), this, SLOT(onShowMenuEvent(int)) );

    QTcpSocket* pSocket = QT3kUserData::GetInstance()->GetRemoteSocket();
    connect( pSocket, SIGNAL(connected()), this, SLOT(onConnectedRemote()) );
    connect( pSocket, SIGNAL(disconnected()), this, SLOT(onDisconnectedRemote()) );

    Init();
}

T3kCfgWnd::~T3kCfgWnd()
{
#ifdef Q_OS_WIN
    if( m_nTimerObserver )
    {
        killTimer( m_nTimerObserver );
        m_nTimerObserver = 0;
    }
#endif

    if( m_nTimerChkHIDCom )
    {
        killTimer( m_nTimerChkHIDCom );
        m_nTimerChkHIDCom = 0;
    }

    if( m_nTimerChkTrayDoubleClk )
    {
        killTimer( m_nTimerChkTrayDoubleClk );
        m_nTimerChkTrayDoubleClk = 0;
    }

    if( m_pT3kHandle )
    {
        m_pT3kHandle->Close();
        m_pT3kHandle = NULL;
    }

    if( m_listProfilesQAction.size() )
    {
        QAction* pAction = NULL;
        foreach( pAction, m_listProfilesQAction )
        {
            if( pAction )
                delete pAction;
        }
        m_listProfilesQAction.clear();
    }

    if( m_pTrayMenu )
    {
        delete m_pTrayMenu;
        m_pTrayMenu = NULL;
    }

    if( m_pTrayIcon )
    {
        delete m_pTrayIcon;
        m_pTrayIcon = NULL;
    }

    delete ui;
}

void T3kCfgWnd::Init()
{
    setWindowIcon( QIcon(":/T3kCfgRes/Resources/T3kCfg.png") );

    QSettings WindowPosition( "Habilience", "T3kCfg" );

    // Window History Pos
    WindowPosition.beginGroup( "Window Position" );
    int nX = WindowPosition.value( "x" ).toInt();
    int nY = WindowPosition.value( "y" ).toInt();
    QDesktopWidget DeskWidget;
    bool bRet = false;
    QRect rcMove( nX, nY, width(), height() );
    for( int i=0; i<DeskWidget.screenCount(); i++ )
    {
        if( DeskWidget.screenGeometry( i ).contains( rcMove ) )
        {
            move( nX, nY );
            bRet = true;
            break;
        }
    }
    if( !bRet )
    {
        QDesktopWidget DeskWidget;
        const QRect rcPrimaryMon = DeskWidget.screenGeometry( DeskWidget.primaryScreen() );
        move( (rcPrimaryMon.width()-width())/2, (rcPrimaryMon.height()-height())/2 );
    }
    WindowPosition.endGroup();

    ui->BtnMainDefault->hide();

    if( !m_nTimerObserver )
        m_nTimerObserver = startTimer( 100 );

    OnChangeLanguage();

    m_bShow = false;
    // Open T3k
    if( !OpenT30xHandle() )
    {
        if( !m_nTimerExit )
            m_nTimerExit = startTimer( 10 );
        return;
    }

    m_nSendCmdID = -1;
    m_pT3kHandle->SendCommand( (const char*)QString("%1?").arg(cstrInputMode).toUtf8().data(), true, 5000 );
    m_pT3kHandle->SendCommand( (const char*)QString("%1?").arg(cstrCalibrationScreenMargin).toUtf8().data(), true, 5000 );

    m_bShow = true;

    if( !m_nTimerChkHIDCom )
        m_nTimerChkHIDCom = startTimer( 3000 );
}

#ifndef MAX_PATH
#define MAX_PATH 255
#endif
bool T3kCfgWnd::OpenT30xHandle()
{
    if( !m_pT3kHandle ) return false;

    m_pT3kHandle->SetNotify( TPDPEventMultiCaster::GetPtr() );

    TPDPEventMultiCaster::GetPtr()->SetSocket( QT3kUserData::GetInstance()->GetRemoteSocket() );

    int nTotalSensorCount = 0;
    char pszPath[MAX_PATH];
    memset( pszPath, 0, sizeof(char)*MAX_PATH );
    T3kHandle::TouchSensorModel eModel = T3kHandle::TSM_NONE;
    do
    {
#ifdef HITACHI_VER
        nTotalSensorCount = m_pT3kHandle->GetDeviceCount( HITACHI_VID, HITACHI_PID );
        if( nTotalSensorCount != 0 ) eModel = T3kHandle::TSM_SPEC;
        break;
#endif
        int nOldT3000DetectCnt = m_pT3kHandle->GetDeviceCount( 0xFFFF, 0x0000 );
        nTotalSensorCount += nOldT3000DetectCnt;
        if( nTotalSensorCount > 1 ) break;
        if( nOldT3000DetectCnt != 0 ) eModel = T3kHandle::TSM_OLDT3000;

        int nT3000DetectCnt = m_pT3kHandle->GetDeviceCount( 0x2200, 0x3000 );
        nTotalSensorCount += nT3000DetectCnt;
        if( nTotalSensorCount > 1 ) break;
        if( nT3000DetectCnt != 0 ) eModel = T3kHandle::TSM_T3000;

        int nT3100DetectCnt = m_pT3kHandle->GetDeviceCount( 0x2200, 0x3100 );
        nTotalSensorCount += nT3100DetectCnt;
        if( nTotalSensorCount > 1 ) break;
        if( nT3100DetectCnt != 0 ) eModel = T3kHandle::TSM_T3100;

        int nT3200DetectCnt = m_pT3kHandle->GetDeviceCount( 0x2200, 0x3200 );
        nTotalSensorCount += nT3200DetectCnt;
        if( nTotalSensorCount > 1 ) break;
        if( nT3200DetectCnt != 0 ) eModel = T3kHandle::TSM_T3200;

        int nT3500DetectCnt = m_pT3kHandle->GetDeviceCount( 0x2200, 0x3500 );
        nTotalSensorCount += nT3500DetectCnt;
        if( nTotalSensorCount > 1 ) break;
        if( nT3500DetectCnt != 0 ) eModel = T3kHandle::TSM_T3500;

        int nT3900DetectCnt = m_pT3kHandle->GetDeviceCount( 0x2200, 0x3900 );
        nTotalSensorCount += nT3900DetectCnt;
        if( nTotalSensorCount > 1 ) break;
        if( nT3900DetectCnt != 0 ) eModel = T3kHandle::TSM_T3900;
    }
    while( false );

    if( !nTotalSensorCount )
    {
        return true;
    }

    bool bRet = false;
    if( nTotalSensorCount > 1 )
    {
        QSelectSensorWidget* pDlg = new QSelectSensorWidget( this );
        pDlg->setAttribute( Qt::WA_DeleteOnClose );

        if( pDlg->exec() == QDialog::Rejected )
            return true;

        QT3kUserData* pUD = QT3kUserData::GetInstance();
        ushort nVID = pUD->GetSelectedVID();
        ushort nPID = pUD->GetSelectedPID();
        ushort nIdx = pUD->GetSelectedIdx();

        bRet = m_pT3kHandle->OpenWithVIDPID( nVID, nPID, 1, nIdx );
        if( bRet )
            QT3kUserData::GetInstance()->SetModel( nPID );

        return bRet;
    }

    unsigned short nVID = 0x0000;
    unsigned short nPID = 0x0000;
    switch( eModel )
    {
    case T3kHandle::TSM_OLDT3000:
        nVID = 0xFFFF;  nPID = 0x0000;
        break;
    case T3kHandle::TSM_T3000:
        nVID = 0x2200;  nPID = 0x3000;
        break;
    case T3kHandle::TSM_T3100:
        nVID = 0x2200;  nPID = 0x3100;
        break;
    case T3kHandle::TSM_T3200:
        nVID = 0x2200;  nPID = 0x3200;
        break;
    case T3kHandle::TSM_T3500:
        nVID = 0x2200;  nPID = 0x3500;
        break;
    case T3kHandle::TSM_T3900:
        nVID = 0x2200;  nPID = 0x3900;
        break;
#ifdef HITACHI_VER
    case T3kHandle::TSM_SPEC:
        nVID = HITACHI_VID;  nPID = HITACHI_PID;
        break;
#endif
    case T3kHandle::TSM_NONE:
    default:
        return false;
        break;
    }

    bRet = m_pT3kHandle->OpenWithVIDPID( nVID, nPID, 1 );
    if( bRet )
        QT3kUserData::GetInstance()->SetModel( nPID );

    return bRet;
}

void T3kCfgWnd::LoadCompany()
{
    QString strConfigFolderPath( QCoreApplication::applicationDirPath() + "/config/" );

#ifdef HITACHI_VER
    if( QFile::exists( QString(strConfigFolderPath + "Logo.png") ) ||
        QFile::exists( QString(strConfigFolderPath + "Logo.bmp") ) ||
        QFile::exists( strConfigFolderPath + CUSTOM_CONFIG_FILENAME ) )
        return;
#endif

    if( QFile::exists( QString(strConfigFolderPath + "logo.png") ) )
    {
        ui->BtnLogo->ChangeIcon( strConfigFolderPath + "logo.png" );
    }
    else if( QFile::exists( QString(strConfigFolderPath + "logo.bmp") ) )
    {
        ui->BtnLogo->ChangeIcon( strConfigFolderPath + "logo.bmp" );
    }

    QString strConfig( strConfigFolderPath + CUSTOM_CONFIG_FILENAME );

    bool bConfig = QFile::exists( strConfig );
    if( bConfig )
    {
        QString strProgramVer;
        QString strCopyright;

        QSettings ConfigIni( strConfig, QSettings::IniFormat );
        ConfigIni.setIniCodec( "UTF-8" );
        ConfigIni.beginGroup( QString::fromUtf8("INFO") );

        QVariant var;
        QString str;
        var = ConfigIni.value( QString::fromUtf8("URL") );
        if( var.isValid() )
        {
            str = GetStringFromVariant( var );
            if( !str.isEmpty() )
            {
                m_strCompanyUrl = str;
            }
        }
        var = ConfigIni.value( QString::fromUtf8("COMPANY") );
        if( var.isValid() )
        {
            strCopyright = GetStringFromVariant( var );
        }
        var = ConfigIni.value( QString::fromUtf8("TITLE") );
        if( var.isValid() )
        {
            str = GetStringFromVariant( var );
            if( !str.isEmpty() )
                m_strTitle = str;
        }
        var = ConfigIni.value( QString::fromUtf8("PROGRAM_INFO") );
        if( var.isValid() )
        {
            str = GetStringFromVariant( var );
            if( !str.isEmpty() )
                m_strProgInfo = str;
        }
        var = ConfigIni.value( QString::fromUtf8("PROGRAM_VER") );
        if( var.isValid() )
            strProgramVer = GetStringFromVariant( var );

        ConfigIni.endGroup();

        if( !strProgramVer.isEmpty() )
        {
            QCoreApplication::setApplicationVersion( strProgramVer );
        }

        if( strCopyright.isEmpty() )
        {
            ui->BtnMainLink->setText( QString::fromLocal8Bit("Copyright (c) 2013 Habilience") );
        }
        else
        {
            ui->BtnMainLink->setText( QString("Copyright (c) 2013 %1").arg(strCopyright) );
        }
    }
    else
    {
        ui->BtnMainLink->setText( QString::fromLocal8Bit("Copyright (c) 2013 Habilience") );
    }

    if ( g_bIsScreenShotMode )
    {
        setWindowTitle( m_strTitle );
    }
    else
    {
        setWindowTitle( m_strTitle + " Ver " + QCoreApplication::applicationVersion() );
    }


    QT3kUserData* pInst = QT3kUserData::GetInstance();
    pInst->SetProgramTitle( m_strTitle );
    pInst->SetProgramInfo( m_strProgInfo );

    QFontMetricsF ftMetrics( ui->BtnMainLink->font() );
    ui->BtnMainLink->setGeometry( ui->BtnMainLink->x(), ui->BtnMainLink->y(),
                                  10 + ftMetrics.width( ui->BtnMainLink->text() ),
                                  ui->BtnMainLink->height() );

    ui->line->setGeometry( ui->BtnMainLink->x() + ui->BtnMainLink->width() + 5, ui->line->y(), width() - (ui->BtnMainLink->x() + ui->BtnMainLink->width() + 5), ui->line->height() );
}

void T3kCfgWnd::OnChangeLanguage()
{
    if( !winId() ) return;

    QLangRes& Res = QLangManager::GetPtr()->GetResource();

    ui->BtnMainDefault->setText( Res.GetResString( QString::fromUtf8("MAIN"), QString::fromUtf8("BTN_CAPTION_DEFAULT") ) );
    if( IsRegisterTrayIcon() )
    {
        ui->BtnMainExit->setText( Res.GetResString( QString::fromUtf8("MAIN"), QString::fromUtf8("BTN_CAPTION_CLOSE") ) );
        m_pOpenQAction->setText( Res.GetResString( QString::fromUtf8("TRAYICON"), QString::fromUtf8("TEXT_MENU_OPEN") ) );
        m_pExitQAction->setText( Res.GetResString( QString::fromUtf8("TRAYICON"), QString::fromUtf8("TEXT_MENU_EXIT") ) );
    }
    else
        ui->BtnMainExit->setText( Res.GetResString( QString::fromUtf8("MAIN"), QString::fromUtf8("BTN_CAPTION_EXIT") ) );
}

QString T3kCfgWnd::GetStringFromVariant( QVariant& var )
{
    QString str;
    switch( var.type() )
    {
    case QVariant::StringList:
        {
            QStringList strList( var.toStringList() );
            str = strList.at(0);
            for( int i=1; i<strList.count(); i++ )
                str += ", " + strList[i];
        }
        break;
    case QVariant::String:
        str = var.toString();
        break;
    default:
        break;
    }

    return str;
}

bool T3kCfgWnd::CreateTrayIcon()
{
    if( QSystemTrayIcon::isSystemTrayAvailable() && !m_bRegisterTrayIcon )
    {
        CreateAction();
        m_pTrayMenu = new QMenu( this );
        m_pTrayMenu->addActions( m_listProfilesQAction );
        m_pTrayMenu->addSeparator();
        m_pTrayMenu->addAction( m_pOpenQAction );
        m_pTrayMenu->addAction( m_pExitQAction );
        m_pTrayMenu->setFont( font() );

        connect( m_pTrayMenu, SIGNAL(triggered(QAction*)), this, SLOT(OnTrayChangeProfile(QAction*)) );

        m_pTrayIcon = new QMySystemTrayIcon( this );
        m_pTrayIcon->setContextMenu( m_pTrayMenu );

        connect(m_pTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                this, SLOT(OnTrayIconActions(QSystemTrayIcon::ActivationReason)));

        QIcon TIcon;
        TIcon.addPixmap(QPixmap(QString::fromUtf8(":/T3kCfgRes/Resources/T3kCfg.png")), QIcon::Normal, QIcon::Off);
        m_pTrayIcon->setIcon( TIcon );
        m_pTrayIcon->show();
        m_bRegisterTrayIcon = true;
        return true;
    }

    return false;
}

void T3kCfgWnd::SetTrayIconIamge( int nProfileIndex )
{
    if( !m_bRegisterTrayIcon ) return;
    QString strToolTip;
    QIcon TIcon;
    if( nProfileIndex < 0 )
    {
        TIcon.addPixmap(QPixmap(":/T3kCfgRes/Resources/TRAY_DISCONNECT.png"), QIcon::Normal, QIcon::Off);
        m_pTrayIcon->setIcon( TIcon );
        strToolTip = QString("%1\r\nDisconnected").arg(m_strTitle);
    }
    else
    {
        TIcon.addPixmap(QPixmap(QString(":/T3kCfgRes/Resources/TRAY_PF%1.png").arg(nProfileIndex+1).toStdString().c_str()), QIcon::Normal, QIcon::Off);
        m_pTrayIcon->setIcon( TIcon );

        QString strAppVer = QCoreApplication::applicationVersion();

        strToolTip = QString("%1 (%2)\r\nProfile : %3").arg(m_strTitle).arg(strAppVer).arg( m_listProfilesQAction.at( nProfileIndex )->text());
    }
    m_pTrayIcon->setToolTip( QString::fromLocal8Bit(strToolTip.toStdString().c_str()) );
}

void T3kCfgWnd::EnableTrayProfile(bool bEnalble)
{
    QAction* pAction = NULL;
    foreach( pAction, m_listProfilesQAction )
    {
        pAction->setEnabled( bEnalble );
    }
}

void T3kCfgWnd::OnTrayIconActions(QSystemTrayIcon::ActivationReason TrayReason)
{
    switch( TrayReason )
    {
    case QSystemTrayIcon::DoubleClick:
#ifndef Q_OS_MAC
        killTimer( m_nTimerChkTrayDoubleClk );
        m_nTimerChkTrayDoubleClk = 0;
        OnTrayOpenT3kCfg();
#endif
        break;

    case QSystemTrayIcon::Unknown:
        break;

    case QSystemTrayIcon::MiddleClick:
        break;

    case QSystemTrayIcon::Trigger:
#ifndef Q_OS_MAC
        if( !m_nTimerChkTrayDoubleClk )
        {
            m_ptTrayMenu = cursor().pos();
            m_nTimerChkTrayDoubleClk = startTimer( QApplication::doubleClickInterval()+100 );
        }
        else
            m_nTimerChkTrayDoubleClk = 0;
#endif
        break;

    case QSystemTrayIcon::Context:
        ShowTrayMenu();
        m_pTrayMenu->activateWindow();
        break;
    }
}

void T3kCfgWnd::ShowTrayMenu()
{
    QAction* pMenuAction = NULL;
    foreach( pMenuAction, m_listProfilesQAction )
    {
        if( !m_pT3kHandle->IsOpen() )
        {
            pMenuAction->setEnabled( false );
        }
        else
        {
            if( pMenuAction->isChecked() )
                pMenuAction->setChecked( false );
        }
    }

    if( m_pT3kHandle->IsOpen() )
        m_listProfilesQAction.at(m_nProfileIndex)->setChecked( true );
}

void T3kCfgWnd::CreateAction()
{
    QProfileLabel ProfileLabels;
    for( int i=0; i<5; i++ )
    {
        QAction* pAction = new QAction( ProfileLabels.GetLabel(i), this );
        pAction->setCheckable( true );
        m_listProfilesQAction.push_back( pAction );
    }

    QLangRes& Res = QLangManager::GetPtr()->GetResource();
    m_pOpenQAction = new QAction( Res.GetResString( QString::fromUtf8("TRAYICON"), QString::fromUtf8("TEXT_MENU_OPEN") ), this );
    connect(m_pOpenQAction, SIGNAL(triggered()), this, SLOT(OnTrayOpenT3kCfg()));

    m_pExitQAction = new QAction( Res.GetResString( QString::fromUtf8("TRAYICON"), QString::fromUtf8("TEXT_MENU_EXIT") ), this );
    connect(m_pExitQAction, SIGNAL(triggered()), this, SLOT(CoercionExit()));
}

void T3kCfgWnd::OnTrayChangeProfile( QAction* pAction )
{
    if( pAction == m_pExitQAction )
    {
        CoercionExit();
    }
    else if ( pAction == m_pOpenQAction )
    {
        OnTrayOpenT3kCfg();
    }
    else
    {
        int nProfileIndex = 0;
        QAction* pMenuAction = NULL;
        foreach( pMenuAction, m_listProfilesQAction )
        {
            if( pAction == pMenuAction )
                break;
            nProfileIndex++;
        }

        qDebug( "%d", nProfileIndex );

        int nMouseIdx = m_nProfileIndexData >> 16;
        int nMultiIdx = m_nProfileIndexData & 0x00FF;

        QString strCmd;
        switch( m_nCurInputMode )
        {
        case 0xFF:
        case -1:
            break;
        case 0x02:
            strCmd = QString("%1%2,%3").arg(cstrMouseProfile).arg(nMouseIdx).arg(nProfileIndex+1);
            break;
        case 0x00:
        default:
            strCmd = QString("%1%2,%3").arg(cstrMouseProfile).arg(nProfileIndex+1).arg(nMultiIdx);
            break;
        }

        m_nSendCmdID = m_pT3kHandle->SendCommand( (const char*)strCmd.toUtf8().data(), true );
    }
}

void T3kCfgWnd::OnTrayOpenT3kCfg()
{
#ifdef Q_OS_WIN
    if( m_bRunOtherTool ) return;
#endif
    if( isHidden() )
    {
        onShowMenuEvent( 0 );
        show();
    }

    if( isMinimized() )
        showNormal();

#ifdef Q_OS_WIN
    SetForegroundWindow( (HWND)winId() );
    SetWindowPos( (HWND)winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE );
    SetWindowPos( (HWND)winId(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE );
#else
    raise();
    activateWindow();
#endif
}

void T3kCfgWnd::showEvent(QShowEvent *)
{
    if( m_bShow )
    {
        if( !m_pT3kHandle->IsOpen() )
            HideContentsMenu();
        else
            ShowContentsMenu();
        m_bShow = false;
    }
}

void T3kCfgWnd::closeEvent(QCloseEvent */*evt*/)
{
    QSettings RegOption( "Habilience", "T3kCfg" );
    RegOption.beginGroup( "Options" );
    if( RegOption.value( "TrayIcon", false ).toBool() )
        hide();
    else
        CoercionExit();

    RegOption.endGroup();
}

void T3kCfgWnd::paintEvent(QPaintEvent */*evt*/)
{
    bool bDraw = ( m_bFirmwareDownloading || !m_bIsConnect );
    if( bDraw )
    {
        QRect rcClient( 0, 0, width()-1, height()-1 );
        QString str;

        if( m_bFirmwareDownloading )
            str = QLangManager::GetPtr()->GetResource().GetResString( QString::fromUtf8("MAIN"), QString::fromUtf8("TEXT_FIRMWARE_DOWNLOAD") );
        if( !m_bIsConnect )
            str = QLangManager::GetPtr()->GetResource().GetResString( QString::fromUtf8("MAIN"), QString::fromUtf8("TEXT_DEVICE_NOT_CONNECTED") );

        QPainter dc;
        dc.begin( this );

        dc.setPen( Qt::red );
        QFont ft( font() );
        ft.setPixelSize( 16 );
        dc.setFont( ft );
        dc.drawText( rcClient, str, QTextOption( Qt::AlignVCenter | Qt::AlignHCenter ) );

        dc.end();
    }
}

void T3kCfgWnd::ExitApplication()
{
    emit close();
}

void T3kCfgWnd::CoercionExit()
{
    QSettings WindowPosition( tr("Habilience"), tr("T3kCfg") );
    WindowPosition.beginGroup( tr("Window Position") );
    WindowPosition.setValue( "x", x() );
    WindowPosition.setValue( "y", y() );
    WindowPosition.endGroup();

    qApp->exit( 0 );
}

void T3kCfgWnd::on_BtnMainExit_clicked()
{
    ExitApplication();
}

void T3kCfgWnd::ShowMainMenu(bool bShow)
{
    if( bShow )
    {
        ui->BtnMainLink->show();
        ui->BtnMainDefault->hide();
    }
    else
    {
        ui->BtnMainLink->hide();
        ui->BtnMainDefault->show();
    }
}

void T3kCfgWnd::keyPressEvent(QKeyEvent *evt)
{
#ifdef Q_OS_MAC
    if( evt->modifiers() & Qt::ControlModifier )
#else
    if( evt->modifiers() & Qt::AltModifier )
#endif
    {
#ifdef Q_OS_MAC
        if( evt->key() == Qt::Key_BracketLeft )
#else
        if( evt->key() == Qt::Key_Left )
#endif
        {
            FlatMenuToLeft();
            return;
        }
#ifdef Q_OS_MAC
        if( evt->key() == Qt::Key_BracketRight )
#else
        if( evt->key() == Qt::Key_Right )
#endif
        {
            FlatMenuToRight();
            return;
        }

        if( evt->key() == Qt::Key_Up )
        {

            return;
        }
    }

    QMainWindow::keyPressEvent(evt);
}

void T3kCfgWnd::FlatMenuToLeft()
{
    if( m_eFlatMenuStatus == MenuHome ) return;

    switch( m_eFlatMenuStatus )
    {
    case MenuHome:
    case MenuMouseSetting:
    case MenuRemote:
        break;
    case MenuCalibrationSetting:
        onShowMenuEvent( (int)MenuMouseSetting );
        break;
    case MenuSensorSetting:
        onShowMenuEvent( (int)MenuCalibrationSetting );
        break;
    case MenuSoftkeySetting:
        onShowMenuEvent( (int)MenuSensorSetting );
        break;
    case MenuGeneralSetting:
        onShowMenuEvent( (int)(m_bSoftkey ? MenuSoftkeySetting : MenuSensorSetting) );
        break;
    default:
        Q_ASSERT( false );
        break;
    }
}

void T3kCfgWnd::FlatMenuToRight()
{
    if( m_eFlatMenuStatus == MenuHome ) return;

    switch( m_eFlatMenuStatus )
    {
    case MenuMouseSetting:
        onShowMenuEvent( (int)MenuCalibrationSetting );
        break;
    case MenuCalibrationSetting:
        onShowMenuEvent( (int)MenuSensorSetting );
    case MenuSensorSetting:
        onShowMenuEvent( (int)(m_bSoftkey ? MenuSoftkeySetting : MenuGeneralSetting) );
        break;
    case MenuSoftkeySetting:
        onShowMenuEvent( (int)MenuGeneralSetting );
        break;
    case MenuHome:
    case MenuGeneralSetting:
    case MenuRemote:
        break;
    default:
        Q_ASSERT( false );
        break;
    }
}

void T3kCfgWnd::onShowMenuEvent( int nMenu )
{
    if( m_bAnimation ) return;
    int nDir = m_nCurrentMenu - nMenu;
    if( !nDir ) return;

    m_pCurrObj = NULL;
    m_pNextObj = NULL;

    switch( m_nCurrentMenu )
    {
    case MenuHome:
        m_pCurrObj = m_pMainWidget; break;
    case MenuMouseSetting:
        m_pCurrObj = m_pMouseSettingWidget; break;
    case MenuCalibrationSetting:
        m_pCurrObj = m_pCaliSettingWidget; break;
    case MenuSensorSetting:
        m_pCurrObj = m_pSensorSettingWidget; break;
    case MenuGeneralSetting:
        m_pCurrObj = m_pGeneralSettingWidget; break;
    case MenuSoftkeySetting:
        m_pCurrObj = m_pSoftkeySettingWidget; break;
    case MenuRemote:
        break;
    default:
        break;
    }

    switch( nMenu )
    {
    case MenuHome:
        if( m_pMenuWidget )
            m_pMenuWidget->hide();
        m_pNextObj = m_pMainWidget;
        m_eFlatMenuStatus = MenuHome;
        break;

    case MenuMouseSetting:
        if( m_pMenuWidget )
            m_pMenuWidget->show();
        m_pNextObj = m_pMouseSettingWidget;
        m_eFlatMenuStatus = MenuMouseSetting;
        break;

    case MenuCalibrationSetting:
        if( m_pMenuWidget )
            m_pMenuWidget->show();
        m_pNextObj = m_pCaliSettingWidget;
        m_eFlatMenuStatus = MenuCalibrationSetting;
        break;

    case MenuSensorSetting:
        if( m_pMenuWidget )
            m_pMenuWidget->show();
        m_pNextObj = m_pSensorSettingWidget;
        m_eFlatMenuStatus = MenuSensorSetting;
        break;

    case MenuGeneralSetting:
        if( m_pMenuWidget )
            m_pMenuWidget->show();
        m_pNextObj = m_pGeneralSettingWidget;
        m_eFlatMenuStatus = MenuGeneralSetting;
        break;

    case MenuSoftkeySetting:
        if( m_pMenuWidget )
            m_pMenuWidget->show();
        m_pNextObj = m_pSoftkeySettingWidget;
        m_eFlatMenuStatus = MenuSoftkeySetting;
        break;

    case MenuRemote:
        break;

    default:
        break;
    }

    if( !m_pCurrObj || !m_pNextObj ) return;

    m_pMenuWidget->SetMenuButton( nMenu );
    m_nCurrentMenu = nMenu;

    if( m_eFlatMenuStatus != MenuHome )
        ShowMainMenu( false );

    int n = 1;
    if( nDir > 0 )
        n *= -1;

    if( QApplication::layoutDirection() == Qt::RightToLeft )
        n *= -1;

    QPropertyAnimation *aniCrurent = new QPropertyAnimation(m_pCurrObj, "geometry");
    aniCrurent->setDuration(200);

    aniCrurent->setStartValue(QRect(m_pCurrObj->geometry().left(), m_pCurrObj->geometry().top(),
                                   m_pCurrObj->geometry().width(), m_pCurrObj->geometry().height()));

    aniCrurent->setEndValue(QRect(m_pCurrObj->geometry().left()-m_pCurrObj->geometry().width()*n, m_pCurrObj->geometry().top(),
                                 m_pCurrObj->geometry().width(), m_pCurrObj->geometry().height()));

    QPropertyAnimation *aniNext = new QPropertyAnimation(m_pNextObj, "geometry");
    aniNext->setDuration(200);

    aniNext->setStartValue(QRect(m_pNextObj->geometry().left()+m_pNextObj->geometry().width()*n, m_pNextObj->geometry().top(),
                                   m_pNextObj->geometry().width(), m_pNextObj->geometry().height()));
    aniNext->setEndValue(QRect(m_pNextObj->geometry().left(), m_pNextObj->geometry().top(),
                                 m_pNextObj->geometry().width(), m_pNextObj->geometry().height()));

    m_bAnimation = true;
    QParallelAnimationGroup *group = new QParallelAnimationGroup();

    group->addAnimation(aniCrurent);
    group->addAnimation(aniNext);

    connect( group, SIGNAL(finished()), this, SLOT(OnFinishAnimationMenu()) );
    group->start(QAbstractAnimation::DeleteWhenStopped);
    m_pNextObj->show();
}

void T3kCfgWnd::OnFinishAnimationMenu()
{
    m_pCurrObj->setGeometry( 0, 50, m_pCurrObj->width(), m_pCurrObj->height() );
    m_pCurrObj->hide();
    m_bAnimation = false;

    if( m_eFlatMenuStatus == MenuHome )
        ShowMainMenu( true );
}

void T3kCfgWnd::ShowContentsMenu()
{
    m_pMainWidget->show();
    m_pMainWidget->RequestInformation();
}

void T3kCfgWnd::HideContentsMenu()
{
    onShowMenuEvent( (int)MenuHome );
    m_pMainWidget->hide();
}

void T3kCfgWnd::OnOpenT3kDevice(T3K_HANDLE)
{
    if( !m_pT3kHandle->GetReportCommand() )
        m_pT3kHandle->SetReportCommand( true );

    m_bIsConnect = true;

    ShowContentsMenu();
    update();

    EnableTrayProfile( true );
}

void T3kCfgWnd::OnCloseT3kDevice(T3K_HANDLE)
{
    if( m_pT3kHandle )
    {
        m_pT3kHandle->Close( true );
    }
    if( !isWindow() )
        return;

    m_bIsConnect = false;
    m_bFirmwareDownloading = false;
    HideContentsMenu();

    m_pMouseSettingWidget->hide();
    m_pCaliSettingWidget->hide();
    m_pCaliSettingWidget->ChildClose();
    m_pSensorSettingWidget->hide();
    m_pSoftkeySettingWidget->hide();
    m_pGeneralSettingWidget->hide();

    update();

    SetTrayIconIamge( -1 );
    EnableTrayProfile( false );
}

void T3kCfgWnd::OnFirmwareDownload( bool bDownload )
{
    m_bFirmwareDownloading = bDownload;
    if( m_bFirmwareDownloading )
    {
        HideContentsMenu();
        m_pMouseSettingWidget->hide();
        m_pCaliSettingWidget->hide();
        m_pCaliSettingWidget->ChildClose();
        m_pSensorSettingWidget->hide();
        m_pSoftkeySettingWidget->hide();
        m_pGeneralSettingWidget->hide();
    }
    else
        ShowContentsMenu();
    EnableTrayProfile( !bDownload );
    qDebug( "Firmware download: %d\r\n", bDownload );

    update();

    if( !m_bFirmwareDownloading )
    {
        if( !m_pT3kHandle->GetReportCommand() )
            m_pT3kHandle->SetReportCommand( true );

        if( isVisible() )
            m_pMainWidget->RequestInformation();
    }
}

inline bool Extract2Word( QString& str, uchar& cV )
{
    QString strWord( str.left( 2 ) );
    if( !strWord.size() ) return false;
    str.remove( 0, 2 );
    cV = (uchar)strtol( strWord.toStdString().c_str(), NULL, 16 );
    return true;
}

int ParseProfileforIsMacMargin( const char* sCmd, int nProfileIndex )
{
    bool bParseProfile = false;

    switch( nProfileIndex )
    {
    case 0:
        if ( strstr(sCmd, cstrMouseProfile1) == sCmd )
            bParseProfile = true;
        break;
    case 1:
        if ( strstr(sCmd, cstrMouseProfile2) == sCmd )
            bParseProfile = true;
        break;
    case 2:
        if ( strstr(sCmd, cstrMouseProfile3) == sCmd )
            bParseProfile = true;
        break;
    case 3:
        if ( strstr(sCmd, cstrMouseProfile4) == sCmd )
            bParseProfile = true;
        break;
    case 4:
        if ( strstr(sCmd, cstrMouseProfile5) == sCmd )
            bParseProfile = true;
        break;
    default:
        break;
    }

    if ( bParseProfile )
    {
        QString strProfile( sCmd );

        int nE = strProfile.indexOf( ('=') );
        if( nE < 0 ) return -1;

        strProfile.remove( 0, nE+1 );

        uchar cKey = 0;
        if( !Extract2Word( strProfile, cKey) ) return -1;

        uchar cV[4][2];
        ushort wFlags;

        if( cKey == 0x00 )
        {
            if( !Extract2Word( strProfile, cV[0][0] ) ) return -1;
            if( !Extract2Word( strProfile, cV[0][1] ) ) return -1;
            wFlags = (ushort)cV[0][0] << 8 | cV[0][1];
            return MM_MOUSEPROFILE_MAC_OS_MARGIN & wFlags ? 1 : 0;
        }
    }
    return -1;
}

void T3kCfgWnd::OnRSP( ResponsePart /*Part*/, ushort /*nTickTime*/, const char* /*sPartId*/, long lId, bool /*bFinal*/, const char* sCmd )
{
    if( !winId() ) return;
    if( m_nSendCmdID >= 0 && m_nSendCmdID != lId ) return;
    m_nSendCmdID = -1;
    if( m_nCurrentMenu == 0 && strstr(sCmd, cstrInputMode) == sCmd )
    {
        char* pInputMode = NULL;
        pInputMode = (char*)strchr( sCmd, ',' );

        int nMode1 = -1;

        if( !pInputMode )
            m_nCurInputMode = strtol(sCmd + sizeof(cstrInputMode) - 1, NULL, 16);
        else
        {
            nMode1 = strtol(sCmd + sizeof(cstrInputMode) - 1, NULL, 16);
            if( nMode1 == 0xFF )
                m_nCurInputMode = strtol( pInputMode+1, NULL, 16 );
            else
                m_nCurInputMode = nMode1;
        }

        m_nSendCmdID = m_pT3kHandle->SendCommand( (const char*)QString("%1?").arg(cstrMouseProfile).toUtf8().data(), true );
    }
    else
    if ( strstr(sCmd,  cstrMouseProfile) == sCmd )
    {
        if( (m_nCurInputMode < 0 ) ) return;
        //if( !IsRegisterTrayIcon() || !m_pTrayIcon->isSystemTrayAvailable() ) return;

        int nProfile = atoi(sCmd + sizeof(cstrMouseProfile) - 1);

        char* pProfile = (char*)strchr( sCmd, ',' );
        if( pProfile )
        {
            int nMouseIdx = strtol(sCmd + sizeof(cstrMouseProfile) - 1, NULL, 16);
            int nMultiIdx = strtol( pProfile+1, NULL, 16 );

            m_nProfileIndexData = (nMouseIdx << 16) | nMultiIdx;

            switch( m_nCurInputMode )
            {
            case 0xFF:
            case -1:
                Q_ASSERT(false);
                break;
            case 0x02:
                nProfile = nMultiIdx;
                break;
            case 0x00:
            default:
                nProfile = nMouseIdx;
                break;
            }
        }

        if( nProfile < NV_DEF_MOUSE_PROFILE_RANGE_START || nProfile > NV_DEF_MOUSE_PROFILE_RANGE_END+1 ) nProfile = NV_DEF_MOUSE_PROFILE;
        int nCurProfile = nProfile-1;

        SetTrayIconIamge( nCurProfile );
        m_nProfileIndex = nCurProfile;
#ifdef Q_OS_MAC
        if( m_nCurInputMode >= 0 )
        {
            QString strCmd;
            switch( m_nProfileIndex )
            {
            case 0:
                strCmd = QString("%1?").arg(cstrMouseProfile1);
                break;
            case 1:
                strCmd = QString("%1?").arg(cstrMouseProfile2);
                break;
            case 2:
                strCmd = QString("%1?").arg(cstrMouseProfile3);
                break;
            case 3:
                strCmd = QString("%1?").arg(cstrMouseProfile4);
                break;
            case 4:
                strCmd = QString("%1?").arg(cstrMouseProfile5);
                break;
            }

            if( !strCmd.isEmpty() )
                m_nSendCmdID = m_pT3kHandle->SendCommand( (const char*)strCmd.toUtf8().data(), true );
        }
#endif
    }
    else if( strstr(sCmd, cstrSoftkey) == sCmd )
    {
        QString strSoftKey( sCmd );

        int nE = strSoftKey.indexOf( '=' );
        if( nE >= 0 )
        {
            strSoftKey.remove( 0, nE+1 );
            m_bSoftkey = strSoftKey.size() ? true : false;
        }
    }
    else
    {
        int bRet = -1;
        if( (bRet = ParseProfileforIsMacMargin( sCmd, m_nProfileIndex )) != -1 )
        {
            QT3kUserData::GetInstance()->SetMacMargin( bRet == 1 ? true : false );
        }
    }
}

bool T3kCfgWnd::onRegisterTrayIcon(bool bRegister)
{
    if( bRegister )
    {
        ui->BtnMainExit->setText( QLangManager::GetPtr()->GetResource().GetResString( QString::fromUtf8("MAIN"), QString::fromUtf8("BTN_CAPTION_CLOSE") ) );
        if( !IsRegisterTrayIcon() )
        {
            if( CreateTrayIcon() )
            {
                if( m_pT3kHandle->IsOpen() )
                {
                    m_nSendCmdID = m_pT3kHandle->SendCommand( (const char*)QString("%1?").arg(cstrMouseProfile).toUtf8().data(), true );
                }
                else
                    SetTrayIconIamge( -1 );
            }

            return true;
        }
        else
        {
            m_pTrayIcon->show();

            m_nSendCmdID = m_pT3kHandle->SendCommand( (const char*)QString("%1?").arg(cstrMouseProfile).toUtf8().data(), true );
        }
    }
    else
    {
        ui->BtnMainExit->setText( QLangManager::GetPtr()->GetResource().GetResString( QString::fromUtf8("MAIN"), QString::fromUtf8("BTN_CAPTION_EXIT") ) );
        if( IsRegisterTrayIcon() )
        {
            m_pTrayIcon->hide();
            return true;
        }
    }
    return false;
}

void T3kCfgWnd::onByPassKeyPressEvent(QKeyEvent *evt)
{
    keyPressEvent( evt );
}

void T3kCfgWnd::on_BtnMainLink_clicked()
{
    if( !m_strCompanyUrl.size() ) return;
    QUrl url( m_strCompanyUrl );

    QDesktopServices::openUrl( url );
}

void T3kCfgWnd::on_BtnMainDefault_clicked()
{
    QLangRes& Res = QLangManager::GetPtr()->GetResource();
    QString strMessage = Res.GetResString( QString::fromUtf8("TOUCH SETTING"), QString::fromUtf8("MSGBOX_TEXT_DEFAULT") );
    QString strMsgTitle = Res.GetResString( QString::fromUtf8("TOUCH SETTING"), QString::fromUtf8("MSGBOX_TITLE_DEFAULT") );

    QMessageBox msgBox( this );
    msgBox.setWindowTitle( strMsgTitle );
    msgBox.setText( strMessage );
    msgBox.setStandardButtons( QMessageBox::Yes | QMessageBox::No );
    msgBox.setIcon( QMessageBox::Question );
    msgBox.setButtonText( QMessageBox::Yes, Res.GetResString( QString::fromUtf8("MESSAGEBOX"), QString::fromUtf8("BTN_CAPTION_YES") ) );
    msgBox.setButtonText( QMessageBox::No, Res.GetResString( QString::fromUtf8("MESSAGEBOX"), QString::fromUtf8("BTN_CAPTION_NO") ) );
    msgBox.setFont( font() );

    if( msgBox.exec() != QMessageBox::Yes ) return;

    if( m_pMouseSettingWidget && m_pMouseSettingWidget->isVisible() )
    {
        m_pMouseSettingWidget->SetDefault();
        return;
    }

    if( m_pCaliSettingWidget && m_pCaliSettingWidget->isVisible() )
    {
        m_pCaliSettingWidget->SetDefault();
        return;
    }

    if( m_pSensorSettingWidget && m_pSensorSettingWidget->isVisible() )
    {
        m_pSensorSettingWidget->SetDefault();
        return;
    }

    if( m_pGeneralSettingWidget && m_pGeneralSettingWidget->isVisible() )
    {
        m_pGeneralSettingWidget->SetDefault();
        return;
    }

    if( m_pSoftkeySettingWidget && m_pSoftkeySettingWidget->isVisible() )
    {
        m_pSoftkeySettingWidget->SetDefault();
        return;
    }
}

void T3kCfgWnd::timerEvent(QTimerEvent *evt)
{
    if( evt->type() == QEvent::Timer )
    {
        if( evt->timerId() == m_nTimerObserver )
        {
            QSharedMemory CheckDuplicateRuns( "Habilience T3k Series Configure" );
            if( CheckDuplicateRuns.attach( QSharedMemory::ReadWrite ) )
            {
                CheckDuplicateRuns.lock();
                const void* pData = CheckDuplicateRuns.constData();
                ST_SHAREDMEMORY* stSM = (ST_SHAREDMEMORY*)pData;
                if( stSM->szDuplicateRuns == 1 )
                    OnTrayOpenT3kCfg();
                else if( stSM->szDuplicateRuns == 2 )
                {
                    qApp->exit( 0 );
                }
                stSM->szDuplicateRuns = 0;
                CheckDuplicateRuns.unlock();
            }

#ifdef Q_OS_WIN
            HWND hOtherToolAppWnd = FindWindow( T3KFE_DIALOG_CLASSNAME, NULL );
            if( hOtherToolAppWnd )
            {
                if( !m_bRunOtherTool )
                {
                    if( isVisible() )
                    {
                        hide();
                        onShowMenuEvent( QMENU_HOME );
                        m_bPrevShowed = true;
                    }
                    m_bRunOtherTool = true;


                }
            }
            else
            {
                if( m_bPrevShowed )
                {
                    show();
                    SetForegroundWindow( (HWND)winId() );
                    SetWindowPos( (HWND)winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE );
                    SetWindowPos( (HWND)winId(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE );

                    m_bPrevShowed = false;
                }
                m_bRunOtherTool = false;
            }
#endif
        }

        if( evt->timerId() == m_nTimerChkTrayDoubleClk )
        {
            killTimer( m_nTimerChkTrayDoubleClk );
            m_nTimerChkTrayDoubleClk = 0;
            if( m_pTrayIcon->contextMenu() )
            {
                m_pTrayIcon->contextMenu()->popup( m_ptTrayMenu );
                m_pTrayIcon->contextMenu()->activateWindow();
            }
            m_pTrayIcon->UserActivated(QSystemTrayIcon::Context);
        }

        if( evt->timerId() == m_nTimerChkHIDCom )
        {
            if( m_pT3kHandle && !m_pT3kHandle->IsOpen() )
            {
                if( OpenT30xHandle() )
                {
                    m_nSendCmdID = -1;
                    m_pT3kHandle->SendCommand( (const char*)QString("%1?").arg(cstrInputMode).toUtf8().data(), true, 5000 );
                    m_pT3kHandle->SendCommand( (const char*)QString("%1?").arg(cstrCalibrationScreenMargin).toUtf8().data(), true, 5000 );
                }
            }
        }

        if( evt->timerId() == m_nTimerExit )
        {
            killTimer( m_nTimerExit );
            m_nTimerExit = 0;
            CoercionExit();
        }

        if( evt->timerId() == m_nActiveWndTimer )
        {
            killTimer( m_nActiveWndTimer );
            m_nActiveWndTimer = 0;

            if( !isActiveWindow() )
            {
                if( m_nActiveWndTimer )
                {
                    killTimer( m_nActiveWndTimer );
                    m_nActiveWndTimer = 0;
                }

                m_nActiveWndTimer = startTimer( 1000 );
            }
            else
                ExceptionFirmwareVer( m_strFirmwareVerError );
        }
    }

    QMainWindow::timerEvent(evt);
}

void T3kCfgWnd::on_BtnLogo_clicked()
{
    QLicenseWidget wig( this );
    wig.exec();
}

void T3kCfgWnd::OnShowErrorMsgBox(QString str)
{
    m_strFirmwareVerError = str;
    if( !isActiveWindow() )
    {
        if( m_nActiveWndTimer )
        {
            killTimer( m_nActiveWndTimer );
            m_nActiveWndTimer = 0;
        }

        m_nActiveWndTimer = startTimer( 1000 );
    }
    else
        ExceptionFirmwareVer( m_strFirmwareVerError );
}

void T3kCfgWnd::ExceptionFirmwareVer(QString str)
{
    QString strFirmwareVersion(str);
    QLangRes& Res = QLangManager::GetPtr()->GetResource();
    QString strErrorText = Res.GetResString( QString::fromUtf8("MAIN"), QString::fromUtf8("MSGBOX_TEXT_INVALID_FIRMWARE_VERSION") );
    QString strMsgTitle = Res.GetResString( QString::fromUtf8("MAIN"), QString::fromUtf8("MSGBOX_TITLE_INVALID_FIRMWARE_VERSION") );
    QString strError( strErrorText.arg(strFirmwareVersion).arg(MM_MIN_SUPPORT_FIRMWARE_VERSION).arg(MM_NEXT_FIRMWARE_VERSION-0.1) );

    QMessageBox msgBox( this );
    msgBox.setWindowTitle( strMsgTitle );
    msgBox.setInformativeText( strError );
    msgBox.setStandardButtons( QMessageBox::Ok );
    msgBox.setIcon( QMessageBox::Critical );
    msgBox.setButtonText( QMessageBox::Ok, Res.GetResString( QString::fromUtf8("MESSAGEBOX"), QString::fromUtf8("BTN_CAPTION_OK") ) );
    msgBox.setFont( font() );
    if( msgBox.exec() == QMessageBox::Ok )
        qApp->exit( 0 );
}

void T3kCfgWnd::onConnectedRemote()
{

}

void T3kCfgWnd::onDisconnectedRemote()
{
    m_pT3kHandle->onReceiveRawDataFlag( false );
}
