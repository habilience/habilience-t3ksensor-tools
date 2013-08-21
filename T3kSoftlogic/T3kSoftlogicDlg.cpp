#include "T3kSoftlogicDlg.h"
#include "ui_T3kSoftlogicDlg.h"

#include "../common/T3kConstStr.h"
#include "../common/T3k_ver.h"
//#include "DSelectModel.h"

#include "../common/QIniFormat.h"

#include <QDesktopWidget>
#include <QSettings>
#include <QFile>
#include <QMessageBox>
#include <QFileDialog>
#include <QtEvents>
#include <QMimeData>

#define ID_TIMER_RE_CONNECT			(100)
#define ID_TIMER_CHECK_DEVICE		(300)

#define WND_HIDE                    0
#define WND_SHOW                    1
#define WND_MIN                     2
#define WND_MAX                     3


extern bool g_bScreenShotMode;

T3kSoftlogicDlg::T3kSoftlogicDlg(QWidget *parent, QString strModel) :
    QMainWindow(parent),
    ui(new Ui::T3kSoftlogicDlg), m_strDataFileFromCmdLine(strModel)
{
    ui->setupUi(this);

    ui->centralwidget->setLayout( ui->MainLayout );

    m_pTabPanelWidget = new TabPanelWidget( this );
    m_pTabKeyDesignWidget = new TabKeyDesignWidget( this );
    m_pTabLogicDesignWidget = new TabLogicDesignWidget( this );
    m_pTabCalibrationWidget = new TabCalibrationWidget( this );

    m_pT3kHandle = NULL;
    m_bFirmwareDownload = false;
    m_bIsInvalidFirmware = false;
    m_bIsConnected = false;
    m_nTimerReconnect = 0;

    m_nFirstActiveTab = -1;
    m_nT3kDeviceCount = 0;

    //m_hDeviceNotify = NULL;

    m_pT3kHandle = new T3kHandle();
    m_pT3kHandle->SetNotify( TPDPEventMultiCaster::GetPtr() );

    bool bLoadOK = false;

    if( !m_strDataFileFromCmdLine.isEmpty() )
    {
        if( loadModel( m_strDataFileFromCmdLine ) )
        {
            bLoadOK = true;
        }
    }

    // Load Previous Model-Data
    if( !bLoadOK )
    {
        QSettings settings( "Habilience", "T3kSoftlogic" );
        settings.beginGroup( "RECENT MODEL" );
        QString strRecentModelName = settings.value( "MODEL_NAME" ).toString();
        settings.endGroup();
        if( !strRecentModelName.isEmpty() )
        {
            //CString strModelPathName;
            //strModelPathName.Format( _T("%s\\SoftlogicData\\%s.hsk"), GetExecuteDirectory(), strRecentModelName );
            if( !loadModel( strRecentModelName ) )
            {
                bLoadOK = false;
            }
            else
            {
                bLoadOK = true;
            }
        }
    }

    m_SelectDeviceDlg = new QSelectDeviceWidget( this );

    ui->TabMainMenu->addTab( m_pTabPanelWidget, "Panel" );
    ui->TabMainMenu->addTab( m_pTabKeyDesignWidget, "Key" );
    ui->TabMainMenu->addTab( m_pTabLogicDesignWidget, "Logic" );
    ui->TabMainMenu->addTab( m_pTabCalibrationWidget, "Calibration" );

    installEventFilter( m_pTabCalibrationWidget );

    ui->PreviewLayout->addLayout( &m_StackedPreviewLayout );

    m_StackedPreviewLayout.setObjectName( "StackedPrewviewLayout" );
    m_StackedPreviewLayout.setParent( ui->PreviewLayout );

    m_StackedPreviewLayout.addWidget( ui->PreviewKey );
    m_StackedPreviewLayout.addWidget( ui->PreviewLogic );

    m_StackedPreviewLayout.setCurrentIndex( 0 );

    ui->PreviewKey->setFont( font() );
    ui->PreviewKey->setScreenMode( QKeyDesignWidget::ScreenModePreview );
    ui->PreviewKey->setVisible( true );

    ui->PreviewLogic->setFont( font() );
    ui->PreviewLogic->setScreenMode( QKeyDesignWidget::ScreenModePreview );
//m_pPrevKeyDesignWidget.ModifyStyle( 0, WS_BORDER );

    if( !bLoadOK )
    {
        on_BtnNew_clicked();
    }
    else
    {
        m_pTabPanelWidget->updateUIFromData();
        updatePreviewCanvas();
    }

//    DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
//    GUID HIDGuid = { 0x4D1E55B2, 0xF16F, 0x11CF, { 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 } };

//    ZeroMemory( &NotificationFilter, sizeof(NotificationFilter) );
//    NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
//    NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
//    NotificationFilter.dbcc_classguid = HIDGuid;

//    m_hDeviceNotify = RegisterDeviceNotification(
//        GetSafeHwnd(),              // events recipient
//        &NotificationFilter,        // type of device
//        DEVICE_NOTIFY_WINDOW_HANDLE // type of recipient handle
//        );

    init();

    connect( QApplication::desktop(), &QDesktopWidget::resized, this, &T3kSoftlogicDlg::onResizedScreen );
}

T3kSoftlogicDlg::~T3kSoftlogicDlg()
{
    delete ui;

    if( m_pT3kHandle )
    {
        m_pT3kHandle->Close();
        delete m_pT3kHandle;
        m_pT3kHandle = NULL;
    }

    if( m_SelectDeviceDlg )
    {
        delete m_SelectDeviceDlg;
        m_SelectDeviceDlg = NULL;
    }
}

void T3kSoftlogicDlg::onHandleMessage(const QString &msg)
{
    raise();
    activateWindow();

    QString strCmdLine = msg;
    strCmdLine.replace( '\"', "" );

    if( !checkModified() )
        return;

    if( loadModel( strCmdLine ) )
    {
        m_pTabPanelWidget->updateUIFromData();
        T3kCommonData::instance()->resetCalibrationData();
        updatePreviewCanvas();

        //m_wndTab.ResetNotify();
    }
}

void T3kSoftlogicDlg::init()
{
    QSettings settings( "Habilience", "T3kSoftlogic" );
    settings.beginGroup( "APP_SETTING" );
    bool bCheckInvertDrawing = settings.value( "INVERT_DRAWING", false ).toBool();
    settings.endGroup();
    setInvertDrawing( bCheckInvertDrawing );

    settings.beginGroup( "Windows" );
    QString strPos = settings.value( "Main_Pos" ).toString();
    int nShow =  settings.value( "Main_Show", 1 ).toInt();
    settings.endGroup();
    if ( !strPos.isEmpty() )
    {
        QRect rcWin;
        QString str;
        int nD;
        do
        {
            nD = strPos.indexOf(',');
            if ( nD <= 0 ) break;
            str = strPos.left(nD); strPos = strPos.mid(nD + 1);
            rcWin.setLeft( str.toLong() );
            nD = strPos.indexOf(',');
            if ( nD <= 0 ) break;
            str = strPos.left(nD); strPos = strPos.mid(nD + 1);
            rcWin.setTop( str.toLong() );
            nD = strPos.indexOf(',');
            if ( nD <= 0 ) break;
            str = strPos.left(nD); strPos = strPos.mid(nD + 1);
            rcWin.setRight( str.toLong() );
            rcWin.setBottom( strPos.toLong() );
            rcWin = rcWin.normalized();

//            QRect rcCaption( rcWin );
//            rcCaption.adjust();  :GetSystemMetrics(SM_CXSIZEFRAME), ::GetSystemMetrics(SM_CYSIZEFRAME));
//            rcCaption.bottom = rcCaption.top + ::GetSystemMetrics(SM_CYCAPTION);
//            rcCaption.left += rcCaption.Height();
//            rcCaption.right -= rcCaption.Height() * 4;

            QDesktopWidget DeskWidget;
            bool bRet = false;
            for( int i=0; i<DeskWidget.screenCount(); i++ )
            {
                if( DeskWidget.screenGeometry( i ).contains( rcWin ) )
                {
                    move( rcWin.x(), rcWin.y() );
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
        }
        while ( false );
    }
    else
    {
        QDesktopWidget DeskWidget;
        const QRect rcPrimaryMon = DeskWidget.screenGeometry( DeskWidget.primaryScreen() );
        move( (rcPrimaryMon.width()-width())/2, (rcPrimaryMon.height()-height())/2 );
    }

    if( nShow == WND_MIN )
        nShow = WND_SHOW;

    switch( nShow )
    {
    case WND_HIDE:
        hide();
        break;
    case WND_MIN:
        break;
    case WND_MAX:
        showMaximized();
        break;
    case WND_SHOW:
    default:
        show();
        break;
    }

    if ( g_bScreenShotMode )
    {
        QString strTitle( windowTitle() + " " );
        strTitle += T3000_VERSION;
        setWindowTitle( strTitle );
    }

    openT3kHandle();

    setAcceptDrops( true );

    ui->TabMainMenu->setCurrentIndex( m_nFirstActiveTab );
}

//BOOL T3kSoftlogicDlg::OnDeviceChange( UINT nEventType, DWORD_PTR dwData )
//{
//    DEV_BROADCAST_HDR* pDevHdr = (DEV_BROADCAST_HDR*)dwData;

//    if( nEventType == DBT_DEVICEARRIVAL ||
//        nEventType == DBT_DEVICEREMOVECOMPLETE )
//    {
//        if( pDevHdr && (pDevHdr->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE) )
//        {
//            SetTimer( ID_TIMER_CHECK_DEVICE, 2000, NULL );
//        }
//    }

//    return CDialog::OnDeviceChange( nEventType, dwData );
//}

void T3kSoftlogicDlg::checkT3kDeviceStatus()
{
    int nDevCnt =	T3kHandle::GetDeviceCount( 0xFFFF, 0x0000, 1 ) +
                    T3kHandle::GetDeviceCount( 0x2200, 0x3000, 1 ) +
                    T3kHandle::GetDeviceCount( 0x2200, 0x3100, 1 ) +
                    // later model
                    T3kHandle::GetDeviceCount( 0x2200, 0x3200, 1 ) +
                    T3kHandle::GetDeviceCount( 0x2200, 0x3500, 1 ) +
                    T3kHandle::GetDeviceCount( 0x2200, 0x3900, 1 ) +
                    T3kHandle::GetDeviceCount( 0x2200, 0xFF02, 0 );

    if( m_nT3kDeviceCount != 0 && m_nT3kDeviceCount != nDevCnt )
    {
        m_pT3kHandle->Close();
    }

    m_nT3kDeviceCount = nDevCnt;
}

bool T3kSoftlogicDlg::openT3kHandle()
{
    bool bRet = false;

    if( m_nTimerReconnect )
    {
        killTimer( m_nTimerReconnect );
        m_nTimerReconnect = 0;
    }

    if( m_pT3kHandle->IsOpen() )
        return true;

    checkT3kDeviceStatus();

    if( m_nT3kDeviceCount > 1 )
    {
        if( m_SelectDeviceDlg->exec() == QDialog::Accepted )
        {
            QSelectDeviceWidget::DEVICE_ID deviceId = m_SelectDeviceDlg->getDevId();
            bRet = m_pT3kHandle->OpenWithVIDPID( deviceId.nVID, deviceId.nPID, deviceId.nMI, deviceId.nDeviceIndex );
        }

        m_SelectDeviceDlg->close();
    }
    else
    {
        do
        {
            int nOldT3000DetectCnt = T3kHandle::GetDeviceCount( 0xFFFF, 0x0000, 1 );
            if( nOldT3000DetectCnt > 0 )
            {
                bRet = m_pT3kHandle->OpenWithVIDPID( 0xFFFF, 0x0000, 1, 0 );
                break;
            }
            int nT3000DetectCnt = T3kHandle::GetDeviceCount( 0x2200, 0x3000, 1 );
            if( nT3000DetectCnt > 0 )
            {
                bRet = m_pT3kHandle->OpenWithVIDPID( 0x2200, 0x3000, 1, 0 );
                break;
            }
            int nT3100DetectCnt = T3kHandle::GetDeviceCount( 0x2200, 0x3100, 1 );
            if( nT3100DetectCnt > 0 )
            {
                bRet = m_pT3kHandle->OpenWithVIDPID( 0x2200, 0x3100, 1, 0 );
                break;
            }
            int nT3200DetectCnt = T3kHandle::GetDeviceCount( 0x2200, 0x3200, 1 );
            if( nT3200DetectCnt > 0 )
            {
                bRet = m_pT3kHandle->OpenWithVIDPID( 0x2200, 0x3200, 1, 0 );
                break;
            }
            int nT3500DetectCnt = T3kHandle::GetDeviceCount( 0x2200, 0x3500, 1 );
            if( nT3500DetectCnt > 0 )
            {
                bRet = m_pT3kHandle->OpenWithVIDPID( 0x2200, 0x3500, 1, 0 );
                break;
            }
            int nT3900DetectCnt = T3kHandle::GetDeviceCount( 0x2200, 0x3900, 1 );
            if( nT3900DetectCnt > 0 )
            {
                bRet = m_pT3kHandle->OpenWithVIDPID( 0x2200, 0x3900, 1, 0 );
                break;
            }
            int nT3kVHIDDetectCnt = T3kHandle::GetDeviceCount( 0x2200, 0xFF02, 0 );
            if( nT3kVHIDDetectCnt > 0 )
            {
                bRet = m_pT3kHandle->OpenWithVIDPID( 0x2200, 0xFF02, 0, 0 );
                break;
            }
        } while( false );
    }

    if( bRet )
    {

    }
    else
    {
        if( !m_nTimerReconnect )
            m_nTimerReconnect = startTimer( 2000 );
    }

    return bRet;
}

void T3kSoftlogicDlg::setInvertDrawing( bool bInvert )
{
    ui->PreviewKey->setInvertDrawing( bInvert );
    if ( ui->PreviewKey->isVisible() )
        ui->PreviewKey->update();
//    m_wndPrevLogicDesign.SetInvertDrawing( bInvert );
//    if ( m_wndPrevLogicDesign && m_wndPrevLogicDesign.IsWindowVisible() )
//        m_wndPrevLogicDesign.Invalidate();
//    m_pTabKeyDesignWidget.m_wndDesignCanvas.SetInvertDrawing( bInvert );
//    if ( m_pTabKeyDesignWidget.m_wndDesignCanvas && m_pTabKeyDesignWidget.m_wndDesignCanvas.IsWindowVisible() )
//        m_pTabKeyDesignWidget.m_wndDesignCanvas.Invalidate();
//    m_pTabLogicDesignWidget.m_LogicDesigner.SetInvertDrawing( bInvert );
//    if ( m_pTabLogicDesignWidget.m_LogicDesigner && m_pTabLogicDesignWidget.m_LogicDesigner.IsWindowVisible() )
//        m_pTabLogicDesignWidget.m_LogicDesigner.Invalidate();
//    m_pTabCalibrationWidget.m_wndTestCanvas.SetInvertDrawing( bInvert );
//    if ( m_pTabCalibrationWidget.m_wndTestCanvas && m_pTabCalibrationWidget.m_wndTestCanvas.IsWindowVisible() )
//        m_pTabCalibrationWidget.m_wndTestCanvas.Invalidate();
}

void T3kSoftlogicDlg::OnOpenT3kDevice(T3K_HANDLE)
{
    if( !isVisible() ) return;

    m_bIsConnected = true;

    m_pT3kHandle->SetInstantMode( T3K_HID_MODE_COMMAND, 5000, 0 );

    char szCmd[256];
    sprintf( szCmd, "%s?", cstrFirmwareVersion );
    m_pT3kHandle->SendCommand( szCmd, true );
}

void T3kSoftlogicDlg::OnCloseT3kDevice(T3K_HANDLE)
{
    m_pT3kHandle->Close();
    m_bIsConnected = true;

    if( !m_nTimerReconnect )
        m_nTimerReconnect = startTimer( 500 );

    if( !isVisible() ) return;

}

void T3kSoftlogicDlg::OnFirmwareDownload(bool bDownload)
{
    m_bFirmwareDownload = bDownload;
}

void T3kSoftlogicDlg::OnRSP(ResponsePart Part, ushort, const char *, long, bool, const char *szCmd)
{
    if( !isVisible() ) return;

    if ( strstr(szCmd, cstrFirmwareVersion) == szCmd )
    {
        switch( Part )
        {
        case MM:
            {
                float fFirmwareVersion = (float)atof(szCmd + sizeof(cstrFirmwareVersion) - 1);
                float fMinDesireFW = (float)MM_MIN_FIRMWARE_VERSION;
                float fMaxDesireFW = (float)MM_NEXT_FIRMWARE_VERSION;
                if( (fFirmwareVersion < fMinDesireFW) || (fFirmwareVersion >= fMaxDesireFW) )
                    m_bIsInvalidFirmware = true;
                else
                    m_bIsInvalidFirmware = false;
            }
            break;
        default:
            break;
        }
    }
}

void T3kSoftlogicDlg::enableControls( bool bEnable )
{
    ui->TabMainMenu->tabBar()->setEnabled( bEnable );
    ui->BtnNew->setEnabled( bEnable );
    ui->BtnLoad->setEnabled( bEnable );
    ui->BtnSave->setEnabled( bEnable );
    ui->BtnExit->setEnabled( bEnable );
}

void T3kSoftlogicDlg::updateCalibrationStep( GroupKey* pGroup, CSoftkey* key, int nCalPos, bool bSet )
{
    if( !pGroup && !key )
    {
//        ui->PreviewKey->cancelBlink();
        return;
    }

    if( pGroup )
    {
//        ui->PreviewKey->setBlinkKey( pGroup, nCalPos, bSet );
    }
    if( key )
    {
//        ui->PreviewKey->setBlinkKey( key, nCalPos, bSet );
    }
}

void T3kSoftlogicDlg::updatePreviewCanvas()
{
    switch( m_StackedPreviewLayout.currentIndex() )
    {
    case 0:
    case 1:
    case 3:
        ui->PreviewKey->updateKeys();
        ui->PreviewKey->update();
        break;
    case 2:
        ui->PreviewLogic->update();
    }
}

void T3kSoftlogicDlg::displayPreviewTouchCount( int nTouchCount )
{
    ui->PreviewKey->updateTouchCount( nTouchCount );
}

void T3kSoftlogicDlg::setFocusPanelName()
{
    int nTabIdx = ui->TabMainMenu->currentIndex();
    if( nTabIdx != 0 )
    {
        ui->TabMainMenu->setCurrentIndex( 0 );
    }

    m_pTabPanelWidget->setFocusPaname();
}

void T3kSoftlogicDlg::onCloseCanvasWnd()
{
    updatePreviewCanvas();

    if( ui->TabMainMenu->currentIndex() == 3 )		// download tab
    {
        m_pTabCalibrationWidget->onCloseTestWnd();
    }
}

bool T3kSoftlogicDlg::loadModel( QString lpszPathName )
{
    m_strLoadedModelPathName.clear();

    if( !QFile::exists( lpszPathName ) ) return false;

    QIniFormat ini("hsk");
    if( !ini.open( lpszPathName ) ) return false;

    double x, y;
    int nSP;
    QString strData, strExtra;

    // load panel info.
    strData.clear(), strExtra.clear();

    QString strPanelName( lpszPathName );
    int nR = strPanelName.lastIndexOf( '.' );
    int nL = strPanelName.lastIndexOf( '/' );
    if( nL < 0 )
        nL = strPanelName.lastIndexOf( '\\' );
    if( nR >= 0 && nL >= 0 )
    {
        strPanelName = strPanelName.mid( nL+1, nR-nL-1 );
        T3kCommonData::instance()->getKeys().setPanelName( strPanelName );
    }

    strData = ini.getValue( "panel_dimension" );
    nSP = strData.indexOf( ',' );
    if( nSP >= 0 )
    {
        x = strData.left(nSP).toInt();
        y = strData.right(strData.length() - nSP - 1).toInt();
        T3kCommonData::instance()->getKeys().setPanelDimension( x, y );
    }

    strData = ini.getValue( "screen_dimension" );
    nSP = strData.indexOf( ',' );
    if( nSP >= 0 )
    {
        x = strData.left(nSP).toInt();
        y = strData.right(strData.length() - nSP - 1).toInt();
        T3kCommonData::instance()->getKeys().setScreenDimension( x, y );
    }

    strData = ini.getValue( "screen_offset" );
    nSP = strData.indexOf( ',' );
    if( nSP >= 0 )
    {
        x = strData.left(nSP).toInt();
        y = strData.right(strData.length() - nSP - 1).toInt();
        T3kCommonData::instance()->getKeys().setScreenOffset( x, y );
    }

    // load softkey
    strData.clear(), strExtra.clear();
    strData = ini.getValue( "softkey" );
    strExtra = ini.getValue( "softkey_ext" );
    ini.beginGroup( "softlogic_ext" );
    T3kCommonData::instance()->getKeys().load(strData, strExtra, &ini);
    ini.endGroup();

    strData.clear();
    strData = ini.getValue( "softkey_bind_info" );
    T3kCommonData::instance()->getKeys().loadBindInfo(strData);

    strData.clear();
    strData = ini.getValue( "softkey_gpio_info" );
    T3kCommonData::instance()->getKeys().loadGPIOInfo(strData);

    T3kCommonData::instance()->resetCalibrationData();

    strData.clear(), strExtra.clear();
    QVariant v = ini.getValue( "softlogic" );
    strData = ini.getValue( "softlogic" );
    ini.beginGroup( "softlogic_ext" );
    T3kCommonData::instance()->getLogics().load(strData, &ini);
    ini.endGroup();

    //m_pTabKeyDesignWidget.m_wndDesignCanvas.UpdateKeys();

    m_strLoadedModelPathName = lpszPathName;
    T3kCommonData::instance()->setLoadedModelPathName( m_strLoadedModelPathName );

    return true;
}

bool T3kSoftlogicDlg::saveModel( QString lpszPathName )
{
    QIniFormat ini("hsk");
    if( !ini.open( lpszPathName ) ) return false;

    QString strData, strExtra;

    CSoftkeyArray& keys = T3kCommonData::instance()->getKeys();

    strExtra.clear();
    ini.beginGroup( "softlogic_ext" );
    strData = keys.save(strExtra, &ini);
    ini.endGroup();
    ini.setValue( "softkey", strData ); // "factory soft-key data"
    ini.setValue( "softkey_ext", strExtra ); // "factory soft-key extra data for T3kSoftlogic"

    strData.clear();
    strData = keys.saveBindInfo();
    ini.setValue( "softkey_bind_info", strData ); // "factory soft-key bind information"

    strData.clear();
    strData = keys.saveGPIOInfo();
    ini.setValue( "softkey_gpio_info", strData );  // "factory soft-key gpio information"

    strExtra.clear();
    ini.beginGroup( "softlogic_ext" );
    strData = T3kCommonData::instance()->getLogics().save( &ini );
    ini.endGroup();
    ini.setValue( "softlogic", strData ); // "factory soft-logic data"
    ini.setValue( "panel_name", keys.getPanelName() ); // "panel name"

    double dX, dY;
    keys.getPanelDimension( dX, dY );
    strData = QString("%1,%2").arg(dX, 0, 'f', 3).arg(dY, 0, 'f', 3); // .3f
    ini.setValue( "panel_dimension", strData ); // "panel dimension (mm)"

    keys.getScreenDimension( dX, dY );
    strData = QString("%1,%2").arg(dX, 0, 'f', 3).arg(dY, 0, 'f', 3); // .3f
    ini.setValue( "screen_dimension", strData ); // "screen dimension (mm)"

    keys.getScreenOffset( dX, dY );
    strData = QString("%1,%2").arg(dX, 0, 'f', 3).arg(dY, 0, 'f', 3); // .3f
    ini.setValue( "screen_offset", strData ); // "screen offset: panel left-top <-> screen left-top (mm)"

    m_strLoadedModelPathName = lpszPathName;

    return true;
}

bool T3kSoftlogicDlg::isModified()
{

    if( T3kCommonData::instance()->getKeys().isModified() )
        return true;

    if( m_strLoadedModelPathName.isEmpty() )
        return false;

    if( !QFile::exists(m_strLoadedModelPathName) ) return false;

    QIniFormat ini("hsk");
    if( !ini.open( m_strLoadedModelPathName ) ) return false;

    QString strData;
    strData.clear();
    strData = ini.getValue( "softlogic" );

    ini.beginGroup( "softlogic_ext" );
    bool bRet = T3kCommonData::instance()->getLogics().isModified( strData, &ini );
    ini.endGroup();

    return bRet;
}

bool T3kSoftlogicDlg::checkModified()
{
    if( isModified() )
    {
        QMessageBox msg( QMessageBox::Question, "Warning", "Would you like to save this data?", QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel, this );
        int nRet = msg.exec();
        if( nRet == QMessageBox::Yes )
        {
            on_BtnSave_clicked();
        }
        if( nRet == QMessageBox::Cancel )
            return false;

        return true;
    }

    return true;
}

//void T3kSoftlogicDlg::copyDataFile( QString lpszDataPath, QString lpszFileName )
//{
//    QString strFileName = lpszFileName;
//    int nP = strFileName.lastIndexOf( '\\' );
//    QString strF = strFileName.right( strFileName.length() - nP - 1 );
//    QString strDst = QString("%1\\%2").arg(lpszDataPath).arg(strF);
//}

void T3kSoftlogicDlg::on_BtnNew_clicked()
{
    if( isModified() )
    {
        QMessageBox msg( QMessageBox::Question, "Warning", "Would you like to save this data?", QMessageBox::Yes|QMessageBox::No, this );
        if( msg.exec() == QMessageBox::Yes )
        {
            m_pTabPanelWidget->updateDataFromUI();

            QString strPanelName = T3kCommonData::instance()->getKeys().getPanelName();
            if( strPanelName.isEmpty() )
            {
                QMessageBox msg( QMessageBox::Critical, "Error", "Panel Name is required.", QMessageBox::Ok, this );
                msg.exec();
                setFocusPanelName();
                return;
            }

            QString strModelName( m_strLoadedModelPathName.right( m_strLoadedModelPathName.length() - m_strLoadedModelPathName.lastIndexOf('/') - 1 ) );
            int nIdx = strModelName.indexOf( '.' );
            strModelName = strModelName.left( nIdx );

            if( m_strLoadedModelPathName.isEmpty() || strModelName != strPanelName )
            {
                QString strFileName = strPanelName;
                strFileName += ".hsk";

                QString strDir( QApplication::applicationDirPath() );
                if( !m_strLoadedModelPathName.isEmpty() )
                {
                    int nBP = m_strLoadedModelPathName.lastIndexOf( '/' );
                    strDir = m_strLoadedModelPathName.left( nBP+1 );
                }

                QString strSave = QFileDialog::getSaveFileName( this, "Save", strDir, "Files(*.hsk)", &strFileName );
                if( strSave.isEmpty() )
                    return;

                m_strLoadedModelPathName = strSave;
            }

            saveModel( m_strLoadedModelPathName );

            T3kCommonData::instance()->resetCalibrationData();

            //m_wndTab.ResetNotify();
        }
    }

    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();
    Keys.clear();
    T3kCommonData::instance()->getLogics().clear();

    Keys.resetPanelInfo();

    Keys.setModified( false );

    m_strLoadedModelPathName.clear();

    m_pTabPanelWidget->updateUIFromData();

    T3kCommonData::instance()->resetCalibrationData();

    updatePreviewCanvas();
}

void T3kSoftlogicDlg::on_BtnLoad_clicked()
{
    if( !checkModified() )
        return;

    QString strDir( QApplication::applicationDirPath() );
    if( !m_strLoadedModelPathName.isEmpty() )
    {
        int nBP = m_strLoadedModelPathName.lastIndexOf( '/' );
        strDir = m_strLoadedModelPathName.left( nBP+1 );
    }

    QString strLoad = QFileDialog::getOpenFileName( this, "Open", strDir, "Files(*.hsk)" );

    if( !strLoad.isEmpty() )
    {
        loadModel( strLoad );

        m_pTabPanelWidget->updateUIFromData();
        T3kCommonData::instance()->resetCalibrationData();
        updatePreviewCanvas();

        //m_wndTab.ResetNotify();
    }
}

void T3kSoftlogicDlg::on_BtnSave_clicked()
{
    m_pTabPanelWidget->updateDataFromUI();

    QString strPanelName = T3kCommonData::instance()->getKeys().getPanelName();
    if( strPanelName.isEmpty() )
    {
        QMessageBox msg( QMessageBox::Critical, "Error", "Panel Name is required.", QMessageBox::Ok, this );
        msg.exec();
        setFocusPanelName();
        return;
    }

    QString strModelName( m_strLoadedModelPathName.right( m_strLoadedModelPathName.length() - m_strLoadedModelPathName.lastIndexOf('/') - 1 ) );
    int nIdx = strModelName.indexOf( '.' );
    strModelName = strModelName.left( nIdx );

    if( m_strLoadedModelPathName.isEmpty() || strModelName != strPanelName )
    {
        QString strFileName = strPanelName;
        strFileName += ".hsk";

        QString strDir( QApplication::applicationDirPath() );
        if( !m_strLoadedModelPathName.isEmpty() )
        {
            int nBP = m_strLoadedModelPathName.lastIndexOf( '/' );
            strDir = m_strLoadedModelPathName.left( nBP+1 );
        }

        QString strSave = QFileDialog::getSaveFileName( this, "Save", strDir, "Files(*.hsk)", &strFileName );
        if( strSave.isEmpty() )
            return;

        m_strLoadedModelPathName = strSave;
    }

    saveModel( m_strLoadedModelPathName );
    QString strLoad = m_strLoadedModelPathName;
    loadModel( strLoad );

    m_pTabPanelWidget->updateUIFromData();
    updatePreviewCanvas();

    T3kCommonData::instance()->resetCalibrationData();

    //m_wndTab.ResetNotify();
}

void T3kSoftlogicDlg::on_BtnExit_clicked()
{
    close();
}

void T3kSoftlogicDlg::onResizedScreen(int nScreen)
{

}

void T3kSoftlogicDlg::timerEvent(QTimerEvent *evt)
{
    if( evt->timerId() == m_nTimerReconnect )
    {
        openT3kHandle();
    }
//    else if ( evt->timerId() == ID_TIMER_CHECK_DEVICE )
//    {
//        killTimer( ID_TIMER_CHECK_DEVICE );
//        checkT3kDeviceStatus();
//    }
}

void T3kSoftlogicDlg::keyPressEvent(QKeyEvent *evt)
{
    if( evt->key() == Qt::Key_Escape )
    {
        if( m_pTabCalibrationWidget->isCalibrationMode() )
        {
            return m_pTabCalibrationWidget->keyPressEvent(evt);
        }
    }
}

void T3kSoftlogicDlg::closeEvent(QCloseEvent *)
{
    if( isModified() )
    {
        QMessageBox msg( QMessageBox::Question, "Warning", "Would you like to save this data?", QMessageBox::Yes|QMessageBox::No, this );
        if( msg.exec() == QMessageBox::Yes )
        {
            m_pTabPanelWidget->updateDataFromUI();

            QString strPanelName = T3kCommonData::instance()->getKeys().getPanelName();
            if( strPanelName.isEmpty() )
            {
                QMessageBox msg( QMessageBox::Critical, "Error", "Panel Name is required.", QMessageBox::Ok, this );
                msg.exec();
                setFocusPanelName();
                return;
            }

            QString strModelName( m_strLoadedModelPathName.right( m_strLoadedModelPathName.length() - m_strLoadedModelPathName.lastIndexOf('/') - 1 ) );
            int nIdx = strModelName.indexOf( '.' );
            strModelName = strModelName.left( nIdx );

            if( m_strLoadedModelPathName.isEmpty() || strModelName != strPanelName )
            {
                QString strFileName = strPanelName;
                strFileName += ".hsk";

                QString strDir( QApplication::applicationDirPath() );
                if( !m_strLoadedModelPathName.isEmpty() )
                {
                    int nBP = m_strLoadedModelPathName.lastIndexOf( '/' );
                    strDir = m_strLoadedModelPathName.left( nBP+1 );
                }

                QString strSave = QFileDialog::getSaveFileName( this, "Save", strDir, "Files(*.hsk)", &strFileName );
                if( strSave.isEmpty() )
                    return;

                m_strLoadedModelPathName = strSave;
            }

            saveModel( m_strLoadedModelPathName );

            T3kCommonData::instance()->resetCalibrationData();

            //m_wndTab.ResetNotify();
        }
    }

    //m_wndPrevKeyDesign.DestroyWindow();
    //m_wndPrevLogicDesign.DestroyWindow();

    QRect rc( geometry() );
    QString str = QString("%1,%2,%3,%4").arg(rc.left()).arg(rc.top()).arg(rc.right()).arg(rc.bottom());
    QSettings settings( "Habilience", "T3kSoftlogic" );
    settings.beginGroup( "Windows" );
    settings.setValue( "Main_Pos", str );
    settings.setValue( "Main_Show", isMaximized() ? WND_MAX : isMinimized() ? WND_MIN : isHidden() ? WND_HIDE : WND_SHOW );
    settings.endGroup();

    QString strRecentModelName = m_strLoadedModelPathName;//m_SoftKey.GetPanelName();
    settings.beginGroup( "RECENT MODEL" );
    settings.setValue( "MODEL_NAME", strRecentModelName );
    settings.endGroup();
}

void T3kSoftlogicDlg::dropEvent(QDropEvent *evt)
{
    QString	strFileName;
    if( evt->mimeData()->hasUrls() )
    {
        foreach( const QUrl& url, evt->mimeData()->urls() )
        {
            QString str( url.toLocalFile() );
            if( str.isEmpty() ) continue;

            if( QFileInfo( str ).suffix() == "hsk" )
            {
                strFileName = str;
                break;
            }
        }
    }

    if( strFileName.isEmpty() )
    {
        QMessageBox::about( this, "Warning", QString("Not \"hsk\" file : %s").arg(strFileName) );
        return;
    }

    if( !checkModified() )
        return;

    loadModel( strFileName );

    m_pTabPanelWidget->updateUIFromData();
    T3kCommonData::instance()->resetCalibrationData();
    updatePreviewCanvas();

    //m_wndTab.ResetNotify();
    m_pTabKeyDesignWidget->refresh();
}

void T3kSoftlogicDlg::dragEnterEvent(QDragEnterEvent *evt)
{
    if( evt->mimeData()->hasUrls() )
    {
        foreach( const QUrl& url, evt->mimeData()->urls() )
        {
            QString str( url.toLocalFile() );
            if( str.isEmpty() ) continue;

            if( QFileInfo( str ).suffix() == "hsk" )
            {
                evt->acceptProposedAction();
                return;
            }
        }
    }
}

#define Softlogic_ExtStr    ".hsk"
#define Softlogic_TypStr    "Softlogic.DataFile"
#define Default_Key         "/Default"

static void unregisterShellFileTypes( QString strFileExt, QString strFileTypeName )
{
    QSettings settings( "HKEY_CLASSES_ROOT", QSettings::NativeFormat );

    settings.remove( strFileTypeName );
    settings.remove( strFileExt );
}

static void registerShellFileTypes( QString strFileExt, QString strFileTypeName,
                                   QString strPathExe,
                                   QString strPathIcon, int nIdexIcon )
{
    Q_ASSERT( !strFileExt.contains( ' ' ) );
    Q_ASSERT( !strFileTypeName.contains( ' ' ) );
//    Q_ASSERT( !strPathExe.contains( ' ' ) );
//    Q_ASSERT( !strPathIcon.contains( ' ' ) );

    // first register the type ID of our server
    QSettings settings( "HKEY_CLASSES_ROOT", QSettings::NativeFormat );
    settings.beginGroup( strFileTypeName );
    settings.setValue( Default_Key, strFileTypeName );
    settings.endGroup();

    QString strTemp;
    QString strTempValue;

    strTemp = QString("%1/defaulticon").arg(strFileTypeName);
    strTempValue = QString("%1,%2").arg(strPathIcon).arg(nIdexIcon);

    settings.beginGroup( strTemp );
    settings.setValue( Default_Key, strTempValue );
    settings.endGroup();

    strTemp = QString("%1/shell/open/command").arg(strFileTypeName);
    strTempValue = strPathExe + " \"%1\"";

    settings.beginGroup( strTemp );
    settings.setValue( Default_Key, strTempValue );
    settings.endGroup();

    Q_ASSERT( strFileExt.at(0) == '.' );

    settings.beginGroup( strFileExt );
    strTemp = settings.value( Default_Key ).toString();
    settings.endGroup();

    if( strTemp.isEmpty() || (strTemp.compare( strFileTypeName ) != 0) )
    {
        settings.beginGroup( strFileExt );
        settings.setValue( Default_Key, strFileTypeName );
        settings.endGroup();
    }
}

bool T3kSoftlogicDlg::isAssociateFileExt()
{
    QSettings settings( "HKEY_CLASSES_ROOT", QSettings::NativeFormat );
    settings.beginGroup( Softlogic_TypStr );
    bool bRet = !settings.value( Default_Key ).toString().isEmpty();
    settings.endGroup();
    return bRet;
}

void T3kSoftlogicDlg::doAssociateFileExt()
{
    // register ext
    QString strRegShellFileName( QApplication::applicationFilePath() );
#ifdef Q_OS_WIN
    strRegShellFileName.replace( QChar('/'), QChar('\\') );
#endif
    registerShellFileTypes( Softlogic_ExtStr, Softlogic_TypStr, strRegShellFileName, strRegShellFileName, 1 );
}

void T3kSoftlogicDlg::doRemoveFileExtAssociation()
{
    unregisterShellFileTypes( Softlogic_ExtStr, Softlogic_TypStr );
}

void T3kSoftlogicDlg::doExecute( QString str )
{
    if ( !str.isEmpty() )
    {
        if ( str.compare( "assoc_file_ext" ) == 0 )
            doAssociateFileExt();
        else if ( str.compare( "remove_file_ext" ) == 0 )
            doRemoveFileExtAssociation();
    }
}

void T3kSoftlogicDlg::notifyTab(int nIdx)
{

}

void T3kSoftlogicDlg::on_TabMainMenu_currentChanged(int index)
{
    setUpdatesEnabled( false );

    switch( index )
    {
    case 0:				// panel info
        T3kCommonData::instance()->setKeyDataMode( T3kCommonData::KeyDataModeNormal );
        m_StackedPreviewLayout.setCurrentIndex( 0 );
        break;
    case 1:				// key design
        T3kCommonData::instance()->setKeyDataMode( T3kCommonData::KeyDataModeNormal );
        m_StackedPreviewLayout.setCurrentIndex( 0 );
        break;
    case 2:				// logic design
        T3kCommonData::instance()->setKeyDataMode( T3kCommonData::KeyDataModeNormal );
        m_StackedPreviewLayout.setCurrentIndex( 1 );
        break;
    case 3:				// calibration
        //if( m_SoftKey.IsModified() )
        {
            T3kCommonData::instance()->resetCalibrationData();
        }
        T3kCommonData::instance()->setKeyDataMode( T3kCommonData::KeyDataModeCalibration );
        m_StackedPreviewLayout.setCurrentIndex( 0 );
        break;
    }

    setUpdatesEnabled( true );

    updatePreviewCanvas();
}
