#include "T3kCfgWnd.h"
#include "ui_T3kCfgWnd.h"

#include "QMainMenuWidget.h"
#include "QMouseSettingWidget.h"
#include "QCalibrationSettingWidget.h"
#include "QSensorSettingWidget.h"
#include "QGeneralSettingWidget.h"
#include "QSoftKeySettingWidget.h"
#include "QMenuStripWidget.h"

#include "QProfileLabel.h"

#include "QSelectSensorWidget.h"
#include "ui/QLicenseWidget.h"
#include "QT3kUserData.h"

#include "QWidgetCloseEventManager.h"

#include "Common/nv.h"
#include "../T3k_ver.h"
#include "QUtils.h"
#include "QGUIUtils.h"

#include "../common/T3kSMDef.h"

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
#include <QDir>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#include "T3kPacketDef.h"

#include "t3kcomdef.h"

extern bool g_bIsScreenShotMode;

#define CUSTOM_CONFIG_FILENAME  "config.ini"

T3kCfgWnd::T3kCfgWnd(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::T3kCfgWnd)
{
    ui->setupUi(this);

    QT3kUserData::GetInstance()->setTopParent( this );

    Qt::WindowFlags flags = windowFlags();
    Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
    flags &= ~helpFlag;
#if defined(Q_OS_WIN)
    flags |= Qt::MSWindowsFixedSizeDialogHint;
#endif
    setWindowFlags(flags);

#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
    genAdjustButtonWidgetForWinAndX11( this );
#endif

    QFont ft( getSystemFont(NULL) );
    ft.setPointSizeF( ft.pointSizeF()+1.0 );
    setFont( ft );

    setFixedSize( width(), height() );

    m_strCompanyUrl = "http://www.habilience.com";
    ui->BtnMainLink->SetAlignmentText( QFlatTextButton::FBA_CENTER );

    ui->BtnLogo->ChangeIcon( ":/T3kCfgRes/resources/PNG_HABILIENCE_LOGO.png" );
    m_strTitle = QString::fromLocal8Bit("T3k Series Configurator");
    setWindowTitle( m_strTitle + " Ver " + QCoreApplication::applicationVersion() );

    m_pT3kHandle = NULL;
    m_nProfileIndex = -1;

    m_pTrayIcon = NULL;
    m_pTrayMenu = NULL;
    m_bRegisterTrayIcon = false;

    m_bFirmwareDownloading = false;
    m_bIsConnect = false;

    m_nTimerExit = 0;
    m_nTimerObserver = 0;
    m_nTimerChkTrayDoubleClk = 0;

    m_nTimerChkHIDCom = 0;

    m_bSoftkey = false;

    m_nCurInputMode = -1;
    m_nProfileIndexData = -1;

    m_nSendCmdID = -1;

    m_bInit = false;

    m_bRunOtherTool = false;
    m_bPrevShowed = false;

    m_pT3kHandle = QT3kDevice::instance();

    QT3kUserData::GetInstance()->setT3kHandle( m_pT3kHandle );

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

    m_pMainWidget = new QMainMenuWidget( m_pT3kHandle );
    m_pMouseSettingWidget = new QMouseSettingWidget( m_pT3kHandle );
    m_pCaliSettingWidget = new QCalibrationSettingWidget( m_pT3kHandle );
    m_pSensorSettingWidget = new QSensorSettingWidget( m_pT3kHandle );
    m_pGeneralSettingWidget = new QGeneralSettingWidget( m_pT3kHandle );
    m_pSoftkeySettingWidget = new QSoftKeySettingWidget( m_pT3kHandle );
    m_pMenuWidget = NULL;

    //m_pMainWidget->setGeometry( 0, 50, 620, 330 );
    m_pMainWidget->hide();
    m_pMainWidget->setAttribute( Qt::WA_DeleteOnClose );
    m_pMainWidget->setFont( font() );
    connect( m_pMainWidget, SIGNAL(ShowMenuEvent(int)), this, SLOT(onShowMenuEvent(int)) );

    //m_pMouseSettingWidget->setGeometry( 0, 50, 620, 340 );
    m_pMouseSettingWidget->hide();
    m_pMouseSettingWidget->setAttribute( Qt::WA_DeleteOnClose );
    connect( m_pMouseSettingWidget, SIGNAL(ByPassKeyPressEvent(QKeyEvent*)), this ,SLOT(onByPassKeyPressEvent(QKeyEvent*)), Qt::DirectConnection );

    //m_pCaliSettingWidget->setGeometry( 0, 50, 620, 340 );
    m_pCaliSettingWidget->hide();
    m_pCaliSettingWidget->setAttribute( Qt::WA_DeleteOnClose );
    connect( m_pCaliSettingWidget, SIGNAL(ByPassKeyPressEvent(QKeyEvent*)), this ,SLOT(onByPassKeyPressEvent(QKeyEvent*)), Qt::DirectConnection );

    //m_pSensorSettingWidget->setGeometry( 0, 50, 620, 340 );
    m_pSensorSettingWidget->hide();
    m_pSensorSettingWidget->setAttribute( Qt::WA_DeleteOnClose );
    connect( m_pSensorSettingWidget, SIGNAL(ByPassKeyPressEvent(QKeyEvent*)), this ,SLOT(onByPassKeyPressEvent(QKeyEvent*)), Qt::DirectConnection );

    //m_pGeneralSettingWidget->setGeometry( 0, 50, 620, 340 );
    m_pGeneralSettingWidget->hide();
    m_pGeneralSettingWidget->setAttribute( Qt::WA_DeleteOnClose );
    connect( m_pGeneralSettingWidget, SIGNAL(ByPassKeyPressEvent(QKeyEvent*)), this ,SLOT(onByPassKeyPressEvent(QKeyEvent*)), Qt::DirectConnection );
    connect( m_pGeneralSettingWidget, SIGNAL(RegisterTrayIcon(bool)), this, SLOT(onRegisterTrayIcon(bool)) );

    //m_pSoftkeySettingWidget->setGeometry( 0, 50, 620, 340 );
    m_pSoftkeySettingWidget->hide();
    m_pSoftkeySettingWidget->setAttribute( Qt::WA_DeleteOnClose );
    connect( m_pSoftkeySettingWidget, SIGNAL(ByPassKeyPressEvent(QKeyEvent*)), this ,SLOT(onByPassKeyPressEvent(QKeyEvent*)), Qt::DirectConnection );

    connect( m_pGeneralSettingWidget, SIGNAL(enableMacOSXGesture(bool)), m_pMouseSettingWidget, SIGNAL(enableMacOSXGesture(bool)) );

    m_pMenuWidget = new QMenuStripWidget( m_pT3kHandle, this );
    m_pMenuWidget->setGeometry( 0, 0, 620, 50 );
    m_pMenuWidget->hide();
    m_pMenuWidget->setAttribute( Qt::WA_DeleteOnClose );
    connect( m_pMenuWidget, SIGNAL(ShowMenuEvent(int)), this, SLOT(onShowMenuEvent(int)) );

    QTcpSocket* pSocket = QT3kUserData::GetInstance()->GetRemoteSocket();
    connect( pSocket, SIGNAL(connected()), this, SLOT(onConnectedRemote()) );
    connect( pSocket, SIGNAL(disconnected()), this, SLOT(onDisconnectedRemote()) );

    ui->SWMenu->insertWidget( 0, m_pMainWidget );
    ui->SWMenu->insertWidget( 1, m_pMouseSettingWidget );
    ui->SWMenu->insertWidget( 2, m_pCaliSettingWidget );
    ui->SWMenu->insertWidget( 3, m_pSensorSettingWidget );
    ui->SWMenu->insertWidget( 4, m_pSoftkeySettingWidget );
    ui->SWMenu->insertWidget( 5, m_pGeneralSettingWidget );

    ui->SWMenu->setCurrentIndex( -1 );

    connect( ui->SWMenu, &QSlidingStackedWidget::animationFinished, this, &T3kCfgWnd::OnFinishAnimationMenu );

    connect( this, &T3kCfgWnd::connectedDevice, this, &T3kCfgWnd::onOpenT3kDevice );
    connect( this, &T3kCfgWnd::connectedDevice, m_pGeneralSettingWidget, &QGeneralSettingWidget::onConnectedDevice, Qt::QueuedConnection );
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
        if( m_pT3kHandle->isOpen() )
            m_pT3kHandle->close();
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
    m_bInit = true;
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

    onChangeLanguage();

    // Open T3k
    if( !OpenT30xHandle() )
    {
        if( !m_nTimerExit )
            m_nTimerExit = startTimer( 10 );
        return;
    }

    m_nSendCmdID = -1;
    m_pT3kHandle->sendCommand( QString("%1?").arg(cstrInputMode), true, 5000 );
    m_pT3kHandle->sendCommand( QString("%1?").arg(cstrCalibrationScreenMargin), true, 5000 );

    if( !m_nTimerChkHIDCom )
        m_nTimerChkHIDCom = startTimer( 3000 );
}

#ifndef MAX_PATH
#define MAX_PATH 255
#endif
bool T3kCfgWnd::OpenT30xHandle()
{
    if( !m_pT3kHandle ) return false;

    // windnsoul
    QT3kDeviceEventHandler::instance()->setSocket( QT3kUserData::GetInstance()->GetRemoteSocket() );

    int nIdx = 0;
    int nTotalSensorCount = 0;
    for ( int d = 0 ; d<COUNT_OF_DEVICE_LIST ; d++)
    {
        int nCnt = QT3kDevice::getDeviceCount( DEVICE_LIST[d].nVID, DEVICE_LIST[d].nPID, DEVICE_LIST[d].nMI );
        if( nCnt > 0 && (d != COUNT_OF_DEVICE_LIST-1 || nTotalSensorCount == 0) )
            nIdx = d;
        nTotalSensorCount += nCnt;
    }

    if( !nTotalSensorCount )
        return true;

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

        bRet = m_pT3kHandle->open( nVID, nPID, 1, nIdx );
        if( bRet )
        {
            emit connectedDevice( m_pT3kHandle->getDeviceInfo() );
            QT3kUserData::GetInstance()->SetModel( nPID );
        }

        return bRet;
    }

    bRet = QT3kDevice::instance()->open( DEVICE_LIST[nIdx].nVID, DEVICE_LIST[nIdx].nPID, DEVICE_LIST[nIdx].nMI, 0 );

    if( bRet )
    {
        emit connectedDevice( m_pT3kHandle->getDeviceInfo() );
        QT3kUserData::GetInstance()->SetModel( DEVICE_LIST[nIdx].nPID );
    }

    return bRet;
}

void T3kCfgWnd::LoadCompany()
{
    QString strConfigFolderPath( QCoreApplication::applicationDirPath() + "/config/" );
    if( !QDir(strConfigFolderPath).exists() ) return;

    if( QFile::exists( QString(strConfigFolderPath + "logo.png") ) )
    {
        ui->BtnLogo->ChangeIcon( strConfigFolderPath + "logo.png" );
    }
    else if( QFile::exists( QString(strConfigFolderPath + "logo.bmp") ) )
    {
        ui->BtnLogo->ChangeIcon( strConfigFolderPath + "logo.bmp" );
    }

    if( QFile::exists( strConfigFolderPath + "icon.png" ) )
    {
        QApplication::setWindowIcon( QIcon( strConfigFolderPath + "icon.png" ) );
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

        if( !strCopyright.isEmpty() )
        {
            ui->BtnMainLink->setText( QString("(c) %1").arg(strCopyright) );
        }
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
}

void T3kCfgWnd::onChangeLanguage()
{
    if( !winId() ) return;

    QLangRes& Res = QLangManager::instance()->getResource();

    ui->BtnMainDefault->setText( Res.getResString( QString::fromUtf8("MAIN"), QString::fromUtf8("BTN_CAPTION_DEFAULT") ) );
    if( IsRegisterTrayIcon() )
    {
        ui->BtnMainExit->setText( Res.getResString( QString::fromUtf8("COMMON"), QString::fromUtf8("TEXT_CLOSE") ) );
        m_pOpenQAction->setText( Res.getResString( QString::fromUtf8("TRAYICON"), QString::fromUtf8("TEXT_MENU_OPEN") ) );
        m_pExitQAction->setText( Res.getResString( QString::fromUtf8("TRAYICON"), QString::fromUtf8("TEXT_MENU_EXIT") ) );
    }
    else
        ui->BtnMainExit->setText( Res.getResString( QString::fromUtf8("MAIN"), QString::fromUtf8("BTN_CAPTION_EXIT") ) );
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

        bool bSimpleMouseProfile = false;
        QString strVer = QT3kUserData::GetInstance()->getFirmwareVersionStr();
        int nExtraVer = strVer.mid( strVer.indexOf( '.' )+2, 1 ).toInt(0, 16);
        if( (nExtraVer >= 0x0A && nExtraVer <= 0x0F) )
        {
            if( strVer < MM_MIN_SIMPLE_MOUSE_PROFILE2 )
                bSimpleMouseProfile = true;
        }
        else
        {
            if( strVer < MM_MIN_SIMPLE_MOUSE_PROFILE1 )
                bSimpleMouseProfile = true;
        }
        if( bSimpleMouseProfile )
        {
            m_pTrayMenu->addActions( m_listProfilesQAction );
            m_pTrayMenu->addSeparator();
        }
        m_pTrayMenu->addAction( m_pOpenQAction );
        m_pTrayMenu->addAction( m_pExitQAction );
        m_pTrayMenu->setFont( font() );

        connect( m_pTrayMenu, SIGNAL(triggered(QAction*)), this, SLOT(OnTrayChangeProfile(QAction*)) );

        m_pTrayIcon = new QMySystemTrayIcon( this );
        m_pTrayIcon->setContextMenu( m_pTrayMenu );

        connect(m_pTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                this, SLOT(OnTrayIconActions(QSystemTrayIcon::ActivationReason)));

        QIcon TIcon;
        TIcon.addPixmap(QPixmap(QString::fromUtf8(":/T3kCfgRes/resources/T3kCfg.png")), QIcon::Normal, QIcon::Off);
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
        TIcon.addPixmap(QPixmap(":/T3kCfgRes/resources/TRAY_DISCONNECT.png"), QIcon::Normal, QIcon::Off);
        m_pTrayIcon->setIcon( TIcon );
        strToolTip = QString("%1\r\nDisconnected").arg(m_strTitle);
    }
    else
    {
        TIcon.addPixmap(QPixmap(QString(":/T3kCfgRes/resources/TRAY_PF%1.png").arg(nProfileIndex+1).toStdString().c_str()), QIcon::Normal, QIcon::Off);
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
        if( !m_pT3kHandle->isOpen() )
        {
            pMenuAction->setEnabled( false );
        }
        else
        {
            if( pMenuAction->isChecked() )
                pMenuAction->setChecked( false );
        }
    }

    if( m_pT3kHandle->isOpen() )
        m_listProfilesQAction.at(m_nProfileIndex)->setChecked( true );
}

void T3kCfgWnd::CreateAction()
{
    QProfileLabel ProfileLabels;

    bool bCheck = false;
    QString strVer = QT3kUserData::GetInstance()->getFirmwareVersionStr();
    if( strVer.isEmpty() ) strVer = qApp->applicationVersion();
    qDebug() << "===================" << strVer;
    int nExtraVer = strVer.mid( strVer.indexOf( '.' )+2, 1 ).toInt(0, 16);
    if( (nExtraVer >= 0x0A && nExtraVer <= 0x0F) )
    {
        if( strVer < MM_MIN_SIMPLE_MOUSE_PROFILE2 )
            bCheck = true;
    }
    else
    {
        if( strVer < MM_MIN_SIMPLE_MOUSE_PROFILE1 )
            bCheck = true;
    }
    int nCnt = bCheck ? 5 : 2;
    for( int i=0; i<nCnt; i++ )
    {
        QAction* pAction = new QAction( ProfileLabels.GetLabel(i), this );
        if( bCheck ) pAction->setCheckable( true );
        m_listProfilesQAction.push_back( pAction );
    }

    QLangRes& Res = QLangManager::instance()->getResource();
    m_pOpenQAction = new QAction( Res.getResString( QString::fromUtf8("TRAYICON"), QString::fromUtf8("TEXT_MENU_OPEN") ), this );
    connect(m_pOpenQAction, SIGNAL(triggered()), this, SLOT(OnTrayOpenT3kCfg()));

    m_pExitQAction = new QAction( Res.getResString( QString::fromUtf8("TRAYICON"), QString::fromUtf8("TEXT_MENU_EXIT") ), this );
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

        m_nSendCmdID = m_pT3kHandle->sendCommand( strCmd, true );
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
#elif defined(Q_OS_MAC)
    QString strExec( qApp->applicationDirPath() );
    strExec.replace( "/Contents/MacOS", "" );
    qDebug() << strExec;
    QStringList args;
    args << "-e";
    args << "tell application \"" + strExec + "\"";
    args << "-e";
    args << "activate";
    args << "-e";
    args << "end tell";
    QProcess::startDetached( "osascript", args );
#else
    raise();
    activateWindow();
#endif
}

void T3kCfgWnd::showEvent(QShowEvent *)
{
    if( !m_bInit )
        Init();

    if( !m_bFirmwareDownloading )
    {
        if( !m_pT3kHandle->isOpen() )
            HideContentsMenu();
        else
            ShowContentsMenu();
    }

    QSettings RegOption( "Habilience", "T3kCfg" );
    RegOption.beginGroup( "Options" );

    if( RegOption.value( "TrayIcon", false ).toBool() )
        onRegisterTrayIcon( true );
    RegOption.endGroup();
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
            str = QLangManager::instance()->getResource().getResString( QString::fromUtf8("MAIN"), QString::fromUtf8("TEXT_FIRMWARE_DOWNLOAD") );
        if( !m_bIsConnect )
            str = QLangManager::instance()->getResource().getResString( QString::fromUtf8("MAIN"), QString::fromUtf8("TEXT_DEVICE_NOT_CONNECTED") );

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
            ShowMainMenu( 0 );
            return;
        }
    }

    QDialog::keyPressEvent(evt);
}

void T3kCfgWnd::FlatMenuToLeft()
{
    if( ui->SWMenu->currentIndex() == 0 ) return;

    ShowMainMenu( false );
    m_pMenuWidget->hide();

    if( ui->SWMenu->currentIndex() == 5 )
        ui->SWMenu->slideInIdx( m_bSoftkey ? 4 : 3 );
    else
        ui->SWMenu->slideInPrev();
}

void T3kCfgWnd::FlatMenuToRight()
{
    if( ui->SWMenu->currentIndex() == ui->SWMenu->count()-1 ) return;

    ShowMainMenu( false );
    m_pMenuWidget->hide();

    if( ui->SWMenu->currentIndex() == 3 )
        ui->SWMenu->slideInIdx( m_bSoftkey ? 4 : 5 );
    else
        ui->SWMenu->slideInNext();
}

void T3kCfgWnd::onShowMenuEvent( int nMenu )
{
    ShowMainMenu( false );
    m_pMenuWidget->hide();

    ui->SWMenu->slideInIdx( nMenu );

    m_pMenuWidget->SetMenuButton( nMenu );
}

void T3kCfgWnd::OnFinishAnimationMenu()
{
    if( ui->SWMenu->currentIndex() == 0 )
    {
        ShowMainMenu( true );
    }
    else
        m_pMenuWidget->show();
}

void T3kCfgWnd::ShowContentsMenu()
{
    m_pMainWidget->show();
    m_pMainWidget->RequestInformation();
}

void T3kCfgWnd::HideContentsMenu()
{
    ShowMainMenu( true );
    m_pMenuWidget->hide();
    ui->SWMenu->setCurrentIndex( 0 );
    m_pMenuWidget->SetMenuButton( 0 );

    m_pMainWidget->hide();
}

void T3kCfgWnd::onOpenT3kDevice(T3K_DEVICE_INFO /*info*/)
{
    if( !m_pT3kHandle->getReportCommand() )
        m_pT3kHandle->setReportCommand( true );

    m_bIsConnect = true;

    ShowContentsMenu();
    update();

    EnableTrayProfile( true );
}

void T3kCfgWnd::TPDP_OnDisconnected(T3K_DEVICE_INFO /*devInfo*/)
{
    if( m_pT3kHandle )
        m_pT3kHandle->close( /*true*/ );

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

    if( !m_nTimerChkHIDCom )
        m_nTimerChkHIDCom = startTimer( 1000 );
}

void T3kCfgWnd::TPDP_OnDownloadingFirmware(T3K_DEVICE_INFO /*devInfo*/, bool bIsDownload)
{
    m_bFirmwareDownloading = bIsDownload;
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
    EnableTrayProfile( !bIsDownload );
    qDebug( "Firmware download: %d\r\n", bIsDownload );

    update();

    if( !m_bFirmwareDownloading )
    {
        if( !m_pT3kHandle->getReportCommand() )
            m_pT3kHandle->setReportCommand( true );

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

        uchar cV[4][2] = { 0, };
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

void T3kCfgWnd::TPDP_OnRSP(T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short /*ticktime*/, const char */*partid*/, int id, bool /*bFinal*/, const char *cmd)
{
    if( !winId() ) return;
    if( m_nSendCmdID >= 0 && m_nSendCmdID != id ) return;
    m_nSendCmdID = -1;
    if( (ui->SWMenu->currentIndex() == 0 ||
            ui->SWMenu->currentIndex() == 5) && strstr(cmd, cstrInputMode) == cmd )
    {
        char* pInputMode = NULL;
        pInputMode = (char*)strchr( cmd, ',' );

        int nMode1 = -1;

        if( !pInputMode )
            m_nCurInputMode = strtol(cmd + sizeof(cstrInputMode) - 1, NULL, 16);
        else
        {
            nMode1 = strtol(cmd + sizeof(cstrInputMode) - 1, NULL, 16);
            if( nMode1 == 0xFF )
                m_nCurInputMode = strtol( pInputMode+1, NULL, 16 );
            else
                m_nCurInputMode = nMode1;
        }

        m_nSendCmdID = m_pT3kHandle->sendCommand( QString("%1?").arg(cstrMouseProfile), true );
    }
    else if ( strstr(cmd,  cstrMouseProfile) == cmd )
    {
        if( (m_nCurInputMode < 0 ) ) return;
        //if( !IsRegisterTrayIcon() || !m_pTrayIcon->isSystemTrayAvailable() ) return;

        int nProfile = atoi(cmd + sizeof(cstrMouseProfile) - 1);

        char* pProfile = (char*)strchr( cmd, ',' );
        if( pProfile )
        {
            int nMouseIdx = strtol(cmd + sizeof(cstrMouseProfile) - 1, NULL, 16);
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
                m_nSendCmdID = m_pT3kHandle->sendCommand( strCmd, true );
        }
#endif
    }
    else if( strstr(cmd, cstrSoftkey) == cmd )
    {
        QString strSoftKey( cmd );

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
        if( (bRet = ParseProfileforIsMacMargin( cmd, m_nProfileIndex )) != -1 )
        {
            QT3kUserData::GetInstance()->SetMacMargin( bRet == 1 ? true : false );
        }
    }
}

bool T3kCfgWnd::onRegisterTrayIcon(bool bRegister)
{
    if( bRegister )
    {
        ui->BtnMainExit->setText( QLangManager::instance()->getResource().getResString( QString::fromUtf8("COMMON"), QString::fromUtf8("TEXT_CLOSE") ) );
        if( !IsRegisterTrayIcon() )
        {
            if( CreateTrayIcon() )
            {
                if( m_pT3kHandle->isOpen() )
                {
                    m_nSendCmdID = m_pT3kHandle->sendCommand( QString("%1?").arg(cstrInputMode), true );
                }
                else
                    SetTrayIconIamge( -1 );
            }

            return true;
        }
        else
        {
            m_pTrayIcon->show();

            m_nSendCmdID = m_pT3kHandle->sendCommand( QString("%1?").arg(cstrInputMode), true );
        }
    }
    else
    {
        ui->BtnMainExit->setText( QLangManager::instance()->getResource().getResString( QString::fromUtf8("MAIN"), QString::fromUtf8("BTN_CAPTION_EXIT") ) );
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
    QLangRes& Res = QLangManager::instance()->getResource();
    QString strMessage = Res.getResString( QString::fromUtf8("TOUCH SETTING"), QString::fromUtf8("MSGBOX_TEXT_DEFAULT") );
    QString strMsgTitle = Res.getResString( QString::fromUtf8("TOUCH SETTING"), QString::fromUtf8("MSGBOX_TITLE_DEFAULT") );

    QMessageBox msgBox( this );
    msgBox.setWindowTitle( strMsgTitle );
    msgBox.setText( strMessage );
    msgBox.setStandardButtons( QMessageBox::Yes | QMessageBox::No );
    msgBox.setIcon( QMessageBox::Question );
    msgBox.setButtonText( QMessageBox::Yes, Res.getResString( QString::fromUtf8("MESSAGEBOX"), QString::fromUtf8("BTN_CAPTION_YES") ) );
    msgBox.setButtonText( QMessageBox::No, Res.getResString( QString::fromUtf8("MESSAGEBOX"), QString::fromUtf8("BTN_CAPTION_NO") ) );
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
            QSharedMemory CheckDuplicateRuns( T3K_SM_NAME );
            if( CheckDuplicateRuns.isAttached() || CheckDuplicateRuns.attach( QSharedMemory::ReadWrite ) )
            {
                CheckDuplicateRuns.lock();
                T3K_SHAREDMEMORY* stSM = (T3K_SHAREDMEMORY*)CheckDuplicateRuns.constData();

                if( stSM->szRunningFE == 1 )
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
                #ifdef Q_OS_WIN
                    if( FindWindow( T3KFE_DIALOG_CLASSNAME, NULL ) )
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
                #endif
                    {
                        if( m_bPrevShowed )
                        {
                            show();
                        #ifdef Q_OS_WIN
                            SetForegroundWindow( (HWND)winId() );
                            SetWindowPos( (HWND)winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE );
                            SetWindowPos( (HWND)winId(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE );
                        #else
                            raise();
                            activateWindow();
                        #endif
                            m_bPrevShowed = false;
                        }
                        m_bRunOtherTool = false;
                    }
                }

                if( stSM->szNotifyCFG == 1 )
                    OnTrayOpenT3kCfg();
                else if( stSM->szNotifyCFG == 2 )
                {
                    qApp->exit( 0 );
                }
                stSM->szNotifyCFG = 0;
                CheckDuplicateRuns.unlock();
            }
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
            if( m_pT3kHandle && !m_pT3kHandle->isOpen() )
            {
                if( OpenT30xHandle() )
                {
                    m_nSendCmdID = -1;
                    m_pT3kHandle->sendCommand( QString("%1?").arg(cstrInputMode), true, 5000 );
                    m_pT3kHandle->sendCommand( QString("%1?").arg(cstrCalibrationScreenMargin), true, 5000 );
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

    QDialog::timerEvent(evt);
}

void T3kCfgWnd::on_BtnLogo_clicked()
{
    QLicenseWidget wig( ":/T3kCfgRes/resources/License.html", this );
    wig.activateWindow();
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
    QLangRes& Res = QLangManager::instance()->getResource();
    QString strErrorText = Res.getResString( QString::fromUtf8("MAIN"), QString::fromUtf8("MSGBOX_TEXT_INVALID_FIRMWARE_VERSION") );
    QString strMsgTitle = Res.getResString( QString::fromUtf8("MAIN"), QString::fromUtf8("MSGBOX_TITLE_INVALID_FIRMWARE_VERSION") );
    QString strError( strErrorText.arg(strFirmwareVersion).arg(MM_MIN_SUPPORT_FIRMWARE_VERSION).arg(MM_NEXT_FIRMWARE_VERSION-0.1) );

    QMessageBox msgBox( this );
    msgBox.setWindowTitle( strMsgTitle );
    msgBox.setInformativeText( strError );
    msgBox.setStandardButtons( QMessageBox::Ok );
    msgBox.setIcon( QMessageBox::Critical );
    msgBox.setButtonText( QMessageBox::Ok, Res.getResString( QString::fromUtf8("MESSAGEBOX"), QString::fromUtf8("BTN_CAPTION_OK") ) );
    msgBox.setFont( font() );
    if( msgBox.exec() == QMessageBox::Ok )
        qApp->exit( 0 );
}

void T3kCfgWnd::onConnectedRemote()
{

}

void T3kCfgWnd::onDisconnectedRemote()
{
   // windnsoul
    m_pT3kHandle->onReceiveRawDataFlag( false );
}
