#include "TabCalibrationWidget.h"
#include "ui_TabCalibrationWidget.h"

#include "T3kSoftlogicDlg.h"
#include "T3kConstStr.h"
#include "../T3k_ver.h"

#include <QDesktopWidget>
#include <QSettings>
#include <QtEvents>
#include <QMessageBox>

#ifdef Q_OS_WIN
#include <windows.h>

#if QT_VERSION < QT_VERSION_CHECK(5,3,0)
typedef struct _TOKEN_ELEVATION {
  DWORD TokenIsElevated;
} TOKEN_ELEVATION, *PTOKEN_ELEVATION;
#endif
#endif

#define LBUTTON		(0x01)
#define RBUTTON		(0x02)
#define MBUTTON		(0x04)

extern bool g_bScreenShotMode;

#ifdef Q_OS_WIN
bool isRunAsAdmin()
{
    BOOL fIsRunAsAdmin = FALSE;
    DWORD dwError = ERROR_SUCCESS;
    PSID pAdministratorsGroup = NULL;

    // Allocate and initialize a SID of the administrators group.
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    if (!AllocateAndInitializeSid(
        &NtAuthority, 
        2, 
        SECURITY_BUILTIN_DOMAIN_RID, 
        DOMAIN_ALIAS_RID_ADMINS, 
        0, 0, 0, 0, 0, 0, 
        &pAdministratorsGroup))
    {
        dwError = GetLastError();
        goto Cleanup;
    }

    // Determine whether the SID of administrators group is enabled in 
    // the primary access token of the process.
    if (!CheckTokenMembership(NULL, pAdministratorsGroup, &fIsRunAsAdmin))
    {
        dwError = GetLastError();
        goto Cleanup;
    }

Cleanup:
    // Centralized cleanup for all allocated resources.
    if (pAdministratorsGroup)
    {
        FreeSid(pAdministratorsGroup);
        pAdministratorsGroup = NULL;
    }

    // Throw the error if something failed in the function.
    if (ERROR_SUCCESS != dwError)
    {
        throw dwError;
    }

    return fIsRunAsAdmin;
}

BOOL isProcessElevated()
{
    OSVERSIONINFOW Info;
    Info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOW);
    if( GetVersionEx( &Info ) )
    {
		if( Info.dwMajorVersion < 6 ) // Vista
		{
			return TRUE;
		}
	}

    BOOL fIsElevated = FALSE;
    DWORD dwError = ERROR_SUCCESS;
    HANDLE hToken = NULL;

    // Open the primary access token of the process with TOKEN_QUERY.
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
    {
        dwError = GetLastError();
        goto Cleanup;
    }

    // Retrieve token elevation information.
    TOKEN_ELEVATION elevation;
    DWORD dwSize;
    if (!GetTokenInformation(hToken, TokenElevation, &elevation, 
        sizeof(elevation), &dwSize))
    {
        // When the process is run on operating systems prior to Windows 
        // Vista, GetTokenInformation returns FALSE with the 
        // ERROR_INVALID_PARAMETER error code because TokenElevation is 
        // not supported on those operating systems.
        dwError = GetLastError();
        goto Cleanup;
    }

    fIsElevated = elevation.TokenIsElevated;

Cleanup:
    // Centralized cleanup for all allocated resources.
    if (hToken)
    {
        CloseHandle(hToken);
        hToken = NULL;
    }

    // Throw the error if something failed in the function.
    if (ERROR_SUCCESS != dwError)
    {
        throw dwError;
    }

    return fIsElevated;
}
#endif


TabCalibrationWidget::TabCalibrationWidget(QWidget* parent /*=NULL*/) :
    QWidget(parent),
    ui(new Ui::TabCalibrationWidget)
{
    ui->setupUi(this);

    m_bCheckInvertDrawing = false;
    m_bCalibrationMode = false;
    m_bLoadFromSensor = false;

	m_cMouseButtons = 0x00;

    m_bTouchCheck = false;
	m_nTouchCount = 0;

	m_nSensorGPIOCount = 0;

	m_lAreaC = 0;

    m_bFirmwareDownload = false;

    m_nTimerCheckPoint = 0;
    m_nTimerRecheckPoint = 0;

#ifndef Q_OS_WIN
    ui->BtnHSK->setVisible( false );
#endif


    QSettings settings( "habilience", "T3kSoftlogic" );
    settings.beginGroup( "APP_SETTING" );
    m_bCheckInvertDrawing = settings.value( "INVERT_DRAWING", false ).toBool();
    settings.endGroup();

    ui->ChkBackground->setChecked( m_bCheckInvertDrawing );

#ifdef Q_OS_WIN
    OSVERSIONINFO stOSVerInfo;
    ZeroMemory( &stOSVerInfo, sizeof(OSVERSIONINFO) );
    stOSVerInfo.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
    GetVersionEx( &stOSVerInfo );

    if( stOSVerInfo.dwMajorVersion >= 6 )
    {
        QIcon icon = QApplication::style()->standardIcon(QStyle::SP_VistaShield);
        ui->BtnHSK->setIcon( icon );
    }
#endif

    connect( &m_wndTestCanvas, &QKeyDesignWidget::closeWidget, this, &TabCalibrationWidget::onCloseTestWidget );
}

TabCalibrationWidget::~TabCalibrationWidget()
{
    delete ui;

    if( m_nTimerCheckPoint )
    {
        killTimer( m_nTimerCheckPoint );
        m_nTimerCheckPoint = 0;
    }

    if( m_nTimerRecheckPoint )
    {
        killTimer( m_nTimerRecheckPoint );
        m_nTimerRecheckPoint = 0;
    }
}

void TabCalibrationWidget::onConnectedT3kDevice()
{
    if( !isVisible() ) return;

    ui->LBState->setText( "Connected" );

    ui->BtnCalibration->setEnabled( true );
    ui->BtnTest->setEnabled( true );
    ui->BtnWriteLogic->setEnabled( true );
    ui->BtnEraseAll->setEnabled( true );
    ui->BtnCancel->setEnabled( false );

    QT3kDevice* pT3kHandle = getT3kHandle();

	char szCmd[256];
	sprintf( szCmd, "%s?", cstrFirmwareVersion );
    pT3kHandle->sendCommand( szCmd, true );

	sprintf( szCmd, "%s?", cstrAreaC );
    pT3kHandle->sendCommand( szCmd, true );
}

void TabCalibrationWidget::TPDP_OnDisconnected(T3K_DEVICE_INFO /*devInfo*/)
{
    if( !isVisible() ) return;

    cancelKeyCalibration();

    ui->BtnCalibration->setEnabled( false );
    ui->BtnCancel->setEnabled( false );
    ui->BtnTest->setEnabled( false );
    ui->BtnWriteLogic->setEnabled( false );
    ui->BtnEraseAll->setEnabled( false );

    QT3kDevice* pT3kHandle = getT3kHandle();

    if( pT3kHandle )
    {
        ui->EditModelName->setText( "" );
        ui->EditFWVer->setText( "" );
    }

    ui->LBState->setText( "<font color='red'>The device is not connected.</font>" );

    update();
}

void TabCalibrationWidget::TPDP_OnDownloadingFirmware(T3K_DEVICE_INFO /*devInfo*/, bool bIsDownload)
{
    m_bFirmwareDownload = bIsDownload;

    if( !isVisible() ) return;

    QT3kDevice* pT3kHandle = getT3kHandle();

    if( bIsDownload )
	{
        cancelKeyCalibration();

        ui->BtnCalibration->setEnabled( false );
        ui->BtnTest->setEnabled( false );
        ui->BtnWriteLogic->setEnabled( false );
        ui->BtnEraseAll->setEnabled( false );

        if( pT3kHandle )
		{
            ui->EditModelName->setText( "" );
            ui->EditFWVer->setText( "" );
		}

        ui->LBState->setText( "<font color='red'>Firmware is being downloaded.</font>" );
	}
	else
	{
        ui->LBState->setText( "<font color='black'>Connected</font>" );

        ui->BtnCalibration->setEnabled( true );
        ui->BtnTest->setEnabled( true );
        ui->BtnWriteLogic->setEnabled( true );
        ui->BtnEraseAll->setEnabled( true );
        ui->BtnCancel->setEnabled( false );

		char szCmd[256];
		sprintf( szCmd, "%s?", cstrFirmwareVersion );
        pT3kHandle->sendCommand( szCmd, true );

		sprintf( szCmd, "%s?", cstrAreaC );
        pT3kHandle->sendCommand( szCmd, true );
	}
}

void TabCalibrationWidget::TPDP_OnMSG(T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short /*ticktime*/, const char *partid, const char *txt)
{
    if( !isVisible() ) return;

    QString strMsg(txt);
    QString strPartid( partid );

    if( strPartid.compare( "SK", Qt::CaseSensitive ) == 0 )
    {
        int nPos = strMsg.indexOf( ':' );
        int nSKNum = strMsg.left( nPos ).toInt();
        int nOnOff = strMsg.right( strMsg.length() - nPos - 1 ).toInt();

        m_wndTestCanvas.setOnOff( nSKNum, nOnOff == 1 ? true : false );
	}
}

void TabCalibrationWidget::TPDP_OnRSP(T3K_DEVICE_INFO /*devInfo*/, ResponsePart Part, unsigned short /*ticktime*/, const char */*partid*/, int /*id*/, bool /*bFinal*/, const char *cmd)
{
    if( !isVisible() ) return;

	const char * buf;

    if ( strstr(cmd, cstrFactorialSoftkey) == cmd )
	{
        buf = cmd + sizeof(cstrFactorialSoftkey) - 1;

        CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

		if( m_bLoadFromSensor )
		{
            m_bLoadFromSensor = false;
            QString strData( buf );
            Keys.load(strData, NULL, NULL);

            T3kCommonData::instance()->getGroupKeys().clear();

            emit updatePreview();
		}
	}

    else if ( strstr(cmd, cstrFactorialSoftkeyPos) == cmd )
	{
        buf = cmd + sizeof(cstrFactorialSoftkey) - 1;

        CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

		if( m_bLoadFromSensor )
		{
            m_bLoadFromSensor = false;
            QString strData( buf );
            Keys.load(strData, NULL, NULL);

            T3kCommonData::instance()->getGroupKeys().clear();

            emit updatePreview();
		}
	}

    else if ( strstr(cmd, cstrAreaC) == cmd )
	{
        buf = cmd + sizeof(cstrAreaC) - 1;

		int nAreaC = atoi(buf);
        m_lAreaC = nAreaC * 2;	//
		if ( m_lAreaC < 300 )
		{
			m_lAreaC = 300;
		}
	}

    else if ( strstr(cmd, cstrFactorialGPIO) == cmd )
	{
        const char* buf = cmd + sizeof(cstrFactorialGPIO) - 1;
		int nLen = (int)strlen( buf );
		m_nSensorGPIOCount = nLen / 2;
	}

	// TODO: *******!!!!!!!!!
    else if ( strstr(cmd, cstrFactorialSoftlogic) == cmd )
	{
        buf = cmd + sizeof(cstrFactorialSoftlogic) - 1;

		if ( m_bLoadFromSensor )
		{
            m_bLoadFromSensor = false;
            m_strBackupSoftlogic = buf;
		}

		//CT3kSoftlogicDlg* pDlg = (CT3kSoftlogicDlg*)AfxGetMainWnd();
		//CSoftlogicArray& Logics = pDlg->GetLogics();

		//Logics.Load( buf, NULL );

		// Save OK

		/*
		if( m_pWndDesignCanvas && *m_pWndDesignCanvas )
			m_pWndDesignCanvas->UpdateKeys();
		*/
	}

    else if ( strstr(cmd, cstrFirmwareVersion) == cmd )
	{		
		switch( Part )
		{
		case MM:
			{
                char szVer[255];
                strncpy( szVer, cmd + sizeof(cstrFirmwareVersion) - 1, 255 );

                if ( g_bScreenShotMode )
                {
                    ui->EditModelName->setText( "T3xxx" );
                    ui->EditFWVer->setText( "x.xx" );
                }
                else
                {
                    QString strFWVer( QString("%1").arg(szVer) );

                    int nS = strFWVer.indexOf( ' ' );
                    if( nS >= 0 )
                    {
                        ui->EditModelName->setText( strFWVer.right( strFWVer.length() - 1 - nS ) );
                        ui->EditFWVer->setText( strFWVer.left( nS ) );
                    }
                }

                float fFirmwareVersion = (float)atof(cmd + sizeof(cstrFirmwareVersion) - 1);
                float fMinDesireFW = (float)MM_MIN_TOOLS_FIRMWARE_VERSION;//MM_MIN_FIRMWARE_VERSION;
                //float fMaxDesireFW = (float)MM_NEXT_FIRMWARE_VERSION;
                if( (fFirmwareVersion < fMinDesireFW) /*|| (fFirmwareVersion >= fMaxDesireFW)*/ )
				{
                    ui->LBState->setText( QString("This program does not support v%1.2f firmware.").arg(fFirmwareVersion, 0, 'f', 2 ) );

                    ui->BtnCalibration->setEnabled( false );
                    ui->BtnTest->setEnabled( false );
                    ui->BtnWriteLogic->setEnabled( false );
                    ui->BtnEraseAll->setEnabled( false );
				}
			}
			break;
        default:
            break;
		}
	}
}

void TabCalibrationWidget::forceMouseEvent( uchar cButtons, char /*cWheel*/, int wX, int wY )
{
    QRect rcWnd( geometry() );
    rcWnd.adjust( 10,10,10,10 );

    QDesktopWidget desktop;
    QRect rcScreen( desktop.screenGeometry( desktop.primaryScreen() ) );

    rcWnd.setLeft( rcWnd.left() * DEV_COORD / rcScreen.width() );
    rcWnd.setTop( rcWnd.top() * DEV_COORD / rcScreen.height() );
    rcWnd.setRight( rcWnd.right() * DEV_COORD / rcScreen.width() );
    rcWnd.setBottom( rcWnd.bottom() * DEV_COORD / rcScreen.height() );

    if( m_wndTestCanvas.isVisible() )
    {
    }
    else
    {

        if( !rcWnd.contains(QPoint(wX, wY)) )
            return;
    }

#if defined(Q_OS_WIN)
	INPUT input;
	memset( &input, 0, sizeof(INPUT) );
	input.type = INPUT_MOUSE;
	if( wX < 0 ) wX = 0;
	if( wX > DEV_COORD ) wX = DEV_COORD;
	if( wY < 0 ) wY = 0;
	if( wY > DEV_COORD ) wY = DEV_COORD;
	input.mi.dx = wX * 65535 / DEV_COORD;
	input.mi.dy = wY * 65535 / DEV_COORD;
	input.mi.dwFlags = MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE;

	if( cButtons & LBUTTON )
	{
		input.mi.dwFlags |= MOUSEEVENTF_LEFTDOWN;
	}
	else
	{
		if( m_cMouseButtons & LBUTTON )
		{
			input.mi.dwFlags |= MOUSEEVENTF_LEFTUP;
		}
	}
	if( cButtons & RBUTTON )
	{
		input.mi.dwFlags |= MOUSEEVENTF_RIGHTDOWN;
	}
	else
	{
		if( m_cMouseButtons & RBUTTON )
		{
			input.mi.dwFlags |= MOUSEEVENTF_RIGHTUP;
		}
	}
	if( cButtons & MBUTTON )
	{
		input.mi.dwFlags |= MOUSEEVENTF_MIDDLEDOWN;
	}
	else
	{
		if( m_cMouseButtons & MBUTTON )
		{
			input.mi.dwFlags |= MOUSEEVENTF_MIDDLEUP;
		}
	}

	if( SendInput( 1, &input, sizeof(INPUT) ) == 0 )
	{
        qDebug( "Error - SendInput: %d", (int)GetLastError() );
	}
#elif defined(Q_OS_LINUX)

#elif defined(Q_OS_MAC)

#endif

	m_cMouseButtons = cButtons;
}

void TabCalibrationWidget::TPDP_OnDVC(T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short /*ticktime*/, const char */*partid*/, t3kpacket::_body::_dvc *device)
{
    if( !isVisible() ) return;

    if( !(device->flag & T3K_DEVICE_TOUCH) ) return;

//	if( device.touch_obj_cnt != 0 )
//	{
//		//TRACE( _T("DVC: touch%d, (%f, %f)\r\n"), device->touch_obj_cnt, device->touch_x, device->touch_y );
//	}

	bool bTestMode = false;
    if( m_wndTestCanvas.isVisible() )
        bTestMode = true;

	if ( !bTestMode )
	{
        emit displayPreviewTouchCount( device->touch_obj_cnt );
	}

    if( device->touch_obj_cnt > 1 )
	{
		return;
	}

    if( device->flag & T3K_DEVICE_TOUCH )
	{
        if ( device->touch_obj_cnt == 1 )
			m_bTouchCheck = true;
	}
	else
	{
		m_bTouchCheck = true;
	}

    m_nTouchCount = device->touch_obj_cnt;

	if ( bTestMode )
	{
        if( device->flag & T3K_DEVICE_MOUSE )
		{
            m_ptTouch.setX( device->mouse_x );
            m_ptTouch.setY( device->mouse_y );
		}
		else
		{
            if ( device->touch_obj_cnt != 1 )
				return;

            float fX = device->touch_x;
            float fY = device->touch_y;

			long lX = (long)(fX * DEV_COORD);
			long lY = (long)(fY * DEV_COORD);

			m_ptTouch.setX( lX );
			m_ptTouch.setY( lY );
		}
        forceMouseEvent( LBUTTON, 0, m_ptTouch.x(), m_ptTouch.y() );
	}
	else
	{
        if ( device->touch_obj_cnt != 1 )
			return;
        float fX = device->touch_x;
        float fY = device->touch_y;

		long lX = (long)(fX * DEV_COORD);
		long lY = (long)(fY * DEV_COORD);

		m_ptTouch.setX( lX );
		m_ptTouch.setY( lY );

        forceMouseEvent( LBUTTON, 0, m_ptTouch.x(), m_ptTouch.y() );
    }

	if( bTestMode )
	{
        float fX = device->touch_x;
        float fY = device->touch_y;

		//TRACE( _T("%d %x, fX: %f, fY: %f\r\n"), device->touch_obj_cnt, device->mouse_buttons, fX, fY );

		long lX = (long)(fX * DEV_COORD);
		long lY = (long)(fY * DEV_COORD);

        if ( device->touch_obj_cnt > 0 )
            m_wndTestCanvas.viewTouchPoint( lX, lY, true );
	}

    m_TickPoint.start();
}

bool TabCalibrationWidget::writeToSensor( bool bLogicOnly )
{
	CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();
	CSoftlogicArray& Logics = T3kCommonData::instance()->getLogics();

    QT3kDevice* pT3kHandle = getT3kHandle();

	bool bOK = false;

	if( pT3kHandle )
	{
		QString strRet;
		QString strExtra;
		QString strCmd;
		int nRetry = 0;

		do
		{
			bOK = true;

			if( !bLogicOnly )
			{
				strRet = Keys.save( strExtra, NULL );
                strCmd = QString("%1%2").arg(cstrFactorialSoftkey).arg(strRet);
                if( !pT3kHandle->sendCommand( strCmd, false ) )
				{
                    bOK = false;
					nRetry++;
					continue;
				}
			}

            strRet = Keys.saveBindInfo();
            strCmd = QString("%1%2").arg(cstrFactorialSoftkeyBind).arg(strRet);
            if( !pT3kHandle->sendCommand( strCmd, false ) )
			{
                bOK = false;
				nRetry++;
				continue;
			}

            strRet = Keys.saveGPIOInfo();
            strCmd = QString("%1%2").arg(cstrFactorialGPIO).arg(strRet);
            if ( !pT3kHandle->sendCommand( strCmd, false ) )
			{
                bOK = false;
				nRetry++;
				continue;
			}

            strRet = Logics.save(NULL);
            strCmd = QString("%1%2").arg(cstrFactorialSoftlogic).arg(strRet);
            if( !pT3kHandle->sendCommand( strCmd, false ) )
			{
                bOK = false;
				nRetry++;
				continue;
			}

            strCmd = QString("%1*").arg(cstrSoftlogic);
            if( !pT3kHandle->sendCommand( strCmd, false ) )
			{
                bOK = false;
				nRetry++;
				continue;
			}

		} while( !bOK && (nRetry < 3) );
	}

	return bOK;
}

void TabCalibrationWidget::showEvent(QShowEvent *)
{
    QT3kDevice* pT3kHandle = getT3kHandle();

    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

    double dScrnDimWidth, dScrnDimHeight;
    Keys.getScreenDimension( dScrnDimWidth, dScrnDimHeight );

    if ( dScrnDimWidth != 0.0 && dScrnDimHeight != 0.0 )
    {
        ui->BtnWriteNoCali->setEnabled( true );
    }
    else
    {
        ui->BtnWriteNoCali->setEnabled( false );
    }

    if ( isT3kConnected() )
    {

        if ( m_bFirmwareDownload )
        {
            ui->BtnCalibration->setEnabled( false );
            ui->BtnTest->setEnabled( false );
            ui->BtnWriteLogic->setEnabled( false );
            ui->BtnEraseAll->setEnabled( false );

            if( pT3kHandle )
            {
                ui->EditModelName->setText( "" );
                ui->EditFWVer->setText( "" );
            }

            ui->LBState->setText( "<font color='red'>Firmware is being Downloaded.</font>" );
        }
        else
        {
            if ( isT3kInvalidFirmware() )
            {
                ui->LBState->setText( "<font color='red'>Invalid firmware</font>" );

                ui->BtnCalibration->setEnabled( false );
                ui->BtnTest->setEnabled( false );
                ui->BtnWriteLogic->setEnabled( false );
                ui->BtnEraseAll->setEnabled( false );
                ui->BtnCancel->setEnabled( false );

                char szCmd[256];
                sprintf( szCmd, "%s?", cstrFirmwareVersion );
                pT3kHandle->sendCommand( szCmd, true );
            }
            else
            {
                ui->LBState->setText( "<font color='black'>Connected</font>" );

                ui->BtnCalibration->setEnabled( true );
                ui->BtnTest->setEnabled( true );
                ui->BtnWriteLogic->setEnabled( true );
                ui->BtnEraseAll->setEnabled( true );
                ui->BtnCancel->setEnabled( false );

                char szCmd[256];
                sprintf( szCmd, "%s?", cstrFirmwareVersion );
                pT3kHandle->sendCommand( szCmd, true );

                sprintf( szCmd, "%s?", cstrAreaC );
                pT3kHandle->sendCommand( szCmd, true );
            }
        }
    }
    else
    {
        ui->BtnCalibration->setEnabled( false );
        ui->BtnTest->setEnabled( false );
        ui->BtnWriteLogic->setEnabled( false );
        ui->BtnEraseAll->setEnabled( false );

        if( pT3kHandle )
        {
            ui->EditModelName->setText( "" );
            ui->EditFWVer->setText( "" );
        }

        ui->LBState->setText( "<font color='red'>The device is not connected.</font>" );
    }

    changeFileAssociateBtnTitle();
}

void TabCalibrationWidget::closeEvent(QCloseEvent *)
{
    cancelKeyCalibration();

    if ( m_wndTestCanvas.isVisible() )
    {
        close();
    }
}

void TabCalibrationWidget::playBuzzer( BuzzerType eType )
{
    int nCat = 0, nType = 0;

	switch( eType )
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
		//MessageBeep( MB_ICONASTERISK );
		nCat = 1, nType = 1;
		break;
	case BuzzerNextPoint:
		nCat = 5, nType = 1;
		break;
	}

    QT3kDevice* pT3kHandle = getT3kHandle();

    if( pT3kHandle )
	{
		char szCmd[256];
		sprintf( szCmd, "%s%d,%d", cstrBuzzerPlay, nCat, nType );
        pT3kHandle->sendCommand( szCmd, true );
	}
}

void TabCalibrationWidget::keyCalibration()
{
	if( m_bCalibrationMode ) return;

    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();
    GroupKeyArray& GroupKeys = T3kCommonData::instance()->getGroupKeys();

    if( Keys.getSize() == 0 )
	{
        if( onCalibrationFinish() )
        {
            QMessageBox::information( this, "Information", "Save Complete", QMessageBox::Ok );
        }
        else
        {
            QMessageBox::critical( this, "Error", "Save Failure", QMessageBox::Ok );
        }
		return;
	}

    m_NoGroupKeys.clear();

    m_nCalibrationMaxStep = (int)GroupKeys.count();
    for( int i=0 ; i<Keys.getSize() ; i++ )
	{
		if( Keys[i]->getGroup() == NULL )
		{
			m_nCalibrationMaxStep++;
            m_NoGroupKeys.push_back( Keys[i] );
		}
	}

    m_bCalibrationMode = true;

    QT3kDevice* pT3kHandle = getT3kHandle();

    if( pT3kHandle )
        pT3kHandle->setInstantMode( T3K_HID_MODE_COMMAND|T3K_HID_MODE_DEVICE, 5000, 0 );

	m_lAveDiffX = m_lAveDiffY = 0;
	m_nPointCount = 0;
    m_bPointOK = false;

	m_nCalibrationStep = 0;
	m_nCalibrationCheckPoint = 0;

    ui->BtnCalibration->setEnabled( false );
    ui->BtnTest->setEnabled( false );
    ui->BtnWriteLogic->setEnabled( false );
    ui->BtnEraseAll->setEnabled( false );
    ui->BtnCancel->setEnabled( true );

    ui->BtnHSK->setEnabled( false );

    emit enableControls( false );

    updateDesignCanvas( true, m_nCalibrationCheckPoint );

    if( !m_nTimerCheckPoint )
        m_nTimerCheckPoint = startTimer( 10 );

    playBuzzer( BuzzerEnterCalibration );
}

void TabCalibrationWidget::cancelKeyCalibration()
{
	if( !m_bCalibrationMode ) return;

    m_NoGroupKeys.clear();

    m_bCalibrationMode = false;

    playBuzzer( BuzzerCancelCalibration );

    QT3kDevice* pT3kHandle = getT3kHandle();

    if( pT3kHandle )
        pT3kHandle->setInstantMode( T3K_HID_MODE_COMMAND, 5000, 0 );

    ui->BtnCalibration->setEnabled( true );
    ui->BtnTest->setEnabled( true );
    ui->BtnWriteLogic->setEnabled( true );
    ui->BtnEraseAll->setEnabled( true );
    ui->BtnCancel->setEnabled( false );

    ui->BtnHSK->setEnabled( true );

    emit enableControls( true );

    if( m_nTimerCheckPoint )
    {
        killTimer( m_nTimerCheckPoint );
        m_nTimerCheckPoint = 0;
    }

    T3kCommonData::instance()->resetCalibrationData();
    emit updatePreview();

    updateDesignCanvas( false );
}

bool TabCalibrationWidget::prepareKeyCalibration()
{
    QT3kDevice* pT3kHandle = getT3kHandle();

    bool bOK = false;

    if( pT3kHandle )
	{
        QString strRet;
        QString strExtra;
        QString strCmd;
		int nRetry = 0;

		do
		{
            bOK = true;

            strCmd = QString("%1*").arg(cstrFactorialSoftkey);
            if( !pT3kHandle->sendCommand( strCmd, false ) )
			{
                bOK = false;
			}

            strCmd = QString("%1**").arg(cstrFactorialSoftlogic);
            if( !pT3kHandle->sendCommand( strCmd, false ) )
			{
                bOK = false;
			}

            strCmd = QString("%1*").arg(cstrSoftlogic);
            if( !pT3kHandle->sendCommand( strCmd, false ) )
			{
                bOK = false;
			}

			nRetry++;

		} while( !bOK && (nRetry < 3) );
	}

	if( !bOK )
	{
        return false;
	}

    return true;
}

bool TabCalibrationWidget::verifyGPIO( int &nSensorGPIOCount )
{
    QT3kDevice* pT3kHandle = getT3kHandle();
    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

	m_nSensorGPIOCount = 0;
	char szCmd[256];
	sprintf( szCmd, "%s?", cstrFactorialGPIO );
	int nRetry = 0;
	do
	{
        if ( pT3kHandle->sendCommand( szCmd, false ) )
			break;
		nRetry++;
	} while( nRetry < 3 );

	nSensorGPIOCount = m_nSensorGPIOCount;
    if ( Keys.getGPIOCount() > nSensorGPIOCount )
	{
        return false;
	}

    return true;
}

void TabCalibrationWidget::keyPressEvent(QKeyEvent *evt)
{
    if( evt->key() == Qt::Key_Escape && m_bCalibrationMode )
    {
        cancelKeyCalibration();
        return;
    }
}

struct FPOINT
{
	float x;
	float y;
};

inline FPOINT getPoint( const QPoint& pt1, const QPoint& pt2, float fV )
{
	FPOINT ptR;

    ptR.x = pt1.x() + (pt2.x()-pt1.x()) * fV;
    ptR.y = pt1.y() + (pt2.y()-pt1.y()) * fV;

	return ptR;
}

inline FPOINT getPoint( const FPOINT& pt1, const FPOINT& pt2, float fV )
{
	FPOINT ptR;

    ptR.x = pt1.x + (pt2.x-pt1.x) * fV;
	ptR.y = pt1.y + (pt2.y-pt1.y) * fV;

	return ptR;
}

bool TabCalibrationWidget::verifyCalibrationPoint( bool bIsGroup )
{
	if( bIsGroup )
	{
		if( !(
            (m_ptCalibration[0].x() < m_ptCalibration[1].x()) &&
            (m_ptCalibration[3].x() < m_ptCalibration[2].x()) &&
            (m_ptCalibration[0].y() < m_ptCalibration[3].y()) &&
            (m_ptCalibration[1].y() < m_ptCalibration[2].y())
			) )
		{
            return false;
		}
	}
	else
	{
        if( m_ptCalibration[2].x() < m_ptCalibration[0].x() ||
            m_ptCalibration[2].y() < m_ptCalibration[0].y() )
		{
            return false;
		}
	}

    return true;
}

bool TabCalibrationWidget::doCalibration( GroupKey* pGroup )
{
    QRect rcOld;
    for( int nG=0 ; nG<pGroup->getCount() ; nG++ )
	{
        CSoftkey* key = pGroup->getAt(nG);
        rcOld = rcOld.united( key->getPosition() );
	}

    QPoint& ptA = m_ptCalibration[0];
    QPoint& ptB = m_ptCalibration[1];
    QPoint& ptC = m_ptCalibration[2];
    QPoint& ptD = m_ptCalibration[3];

	FPOINT ptE, ptF, ptLT, ptRB;
	float fV, fU;

    if( !verifyCalibrationPoint( true ) )
	{
        cancelKeyCalibration();
        return false;
	}

    for( int nI=0 ; nI<pGroup->getCount() ; nI++ )
	{
        CSoftkey* pKey = pGroup->getAt(nI);

        QRect rcKey = pKey->getPosition();

        QPoint ptLeftTop = rcKey.topLeft();
        fV = (float)(ptLeftTop.y() - rcOld.top()) / rcOld.height();
        ptE = getPoint( ptA, ptD, fV );
        ptF = getPoint( ptB, ptC, fV );
        fU = (float)(ptLeftTop.x() - rcOld.left()) / rcOld.width();
        ptLT = getPoint( ptE, ptF, fU );

        QPoint ptRightBottom = rcKey.bottomRight();
        fV = (float)(ptRightBottom.y() - rcOld.top()) / rcOld.height();
        ptE = getPoint( ptA, ptD, fV );
        ptF = getPoint( ptB, ptC, fV );
        fU = (float)(ptRightBottom.x() - rcOld.left()) / rcOld.width();
        ptRB = getPoint( ptE, ptF, fU );

        rcKey = QRect( (int)(ptLT.x+.5f), (int)(ptLT.y+.5f), (int)((ptRB.x+.5f)-(ptLT.x+.5f)), (int)((ptRB.y+.5f)-(ptLT.y+.5f)) );

		pKey->setPosition( rcKey );
	}

    emit updatePreview();

    return true;
}

bool TabCalibrationWidget::doCalibration( CSoftkey* key )
{
    if( !verifyCalibrationPoint( false ) )
	{
        cancelKeyCalibration();
        return false;
	}
    QRect rcNew( m_ptCalibration[0], m_ptCalibration[2] );

	key->setPosition( rcNew );

    emit updatePreview();

    return true;
}

void TabCalibrationWidget::updateDesignCanvas( bool bCalibrate, int nCalPos/*=0*/, bool bSet/*=false*/ )
{
	if( bCalibrate )
	{
        GroupKeyArray& GroupKeys = T3kCommonData::instance()->getGroupKeys();

        if( m_nCalibrationStep < GroupKeys.count() )
		{
            emit updateCalibrationStep( GroupKeys[m_nCalibrationStep], NULL, nCalPos, bSet );
		}
		else
		{
            CSoftkey* key = m_NoGroupKeys.at( m_nCalibrationStep - GroupKeys.count() );
            emit updateCalibrationStep( NULL, key, nCalPos, bSet );
		}
	}
	else
	{
        emit updateCalibrationStep( NULL, NULL, 0, bSet );
	}
}

void TabCalibrationWidget::onCalibrationPoint( QPoint ptCal )
{
	if( !(m_nCalibrationCheckPoint >= 0 && m_nCalibrationCheckPoint < 4) )
		return;

    GroupKeyArray& GroupKeys = T3kCommonData::instance()->getGroupKeys();

    bool bGroupCalibration = m_nCalibrationStep < GroupKeys.count() ? true : false;

	m_ptCalibration[m_nCalibrationCheckPoint++] = ptCal;
	if( !bGroupCalibration )
	{
		m_nCalibrationCheckPoint ++;
	}

	if( m_nCalibrationCheckPoint == 4 )
	{
		if( bGroupCalibration )
		{
			GroupKey* pGroup = GroupKeys[m_nCalibrationStep];

            if( !doCalibration( pGroup ) )
			{
                cancelKeyCalibration();
				return;
			}
		}
		else
		{
            CSoftkey* key = m_NoGroupKeys.at( m_nCalibrationStep - GroupKeys.count() );

            if( !doCalibration( key ) )
			{
                cancelKeyCalibration();
				return;
			}
		}

		if( (m_nCalibrationStep == m_nCalibrationMaxStep-1) )
		{
			// Finish
            if( onCalibrationFinish() )
            {
                updateDesignCanvas( false );
                QMessageBox::information( this, "Information", "Save Complete", QMessageBox::Ok );
            }
            else
            {
                updateDesignCanvas( false );
                QMessageBox::critical( this, "Error", "Save Failure", QMessageBox::Ok );
            }
			return;
		}

		m_nCalibrationCheckPoint = 0;
		m_nCalibrationStep++;
	}

    qDebug("====%d, %d, %d", m_nCalibrationCheckPoint, m_nCalibrationStep, m_nCalibrationMaxStep );

    playBuzzer( BuzzerNextPoint );

    updateDesignCanvas( true, m_nCalibrationCheckPoint );
}

bool TabCalibrationWidget::onCalibrationFinish()
{
    if( m_nTimerCheckPoint )
    {
        killTimer( m_nTimerCheckPoint );
        m_nTimerCheckPoint = 0;
    }

    m_NoGroupKeys.clear();

    m_bCalibrationMode = false;

    playBuzzer( BuzzerCalibrationSucces );

    QT3kDevice* pT3kHandle = getT3kHandle();

    if( pT3kHandle )
        pT3kHandle->setInstantMode( T3K_HID_MODE_COMMAND, 5000, 0 );

    ui->BtnCalibration->setEnabled( true );
    ui->BtnCancel->setEnabled( false );

    ui->BtnTest->setEnabled( true );
    ui->BtnWriteLogic->setEnabled( true );
    ui->BtnEraseAll->setEnabled( true );
    ui->BtnHSK->setEnabled( true );

    emit enableControls( true );

    return writeToSensor( false );
}

void TabCalibrationWidget::timerEvent(QTimerEvent *evt)
{
    if( evt->timerId() == m_nTimerRecheckPoint )
    {
        m_lAveDiffX = m_lAveDiffY = 0;
        m_nPointCount = 0;
        m_bPointOK = false;

        if( m_nTimerRecheckPoint )
        {
            killTimer( m_nTimerRecheckPoint );
            m_nTimerRecheckPoint = 0;
        }
        updateDesignCanvas( true, m_nCalibrationCheckPoint, false );
    }

    if( evt->timerId() == m_nTimerCheckPoint )
    {
        ulong dwTick = m_TickPoint.elapsed(); //GetTickCount();

        bool bTouch = false;
        if( dwTick < 80 && m_nTouchCount > 0 )		// !!!!!!!!!!!!!!!
            bTouch = true;

        if( m_bCalibrationMode )
        {
            if( bTouch && !m_bPointOK )
            {
                if( m_nPointCount == 0 )
                {
                    m_ptPrevTouch = m_ptTouch;
                }
                else
                {
                    long lDx = abs(m_ptTouch.x() - m_ptPrevTouch.x());
                    long lDy = abs(m_ptTouch.y() - m_ptPrevTouch.y());

                    m_lAveDiffX += lDx;
                    m_lAveDiffY += lDy;

                    m_ptPrevTouch = m_ptTouch;
                }

                m_nPointCount++;
                qDebug( "%d", m_nPointCount );

                if( m_nPointCount > 100 )
                {
                    m_lAveDiffX /= m_nPointCount;
                    m_lAveDiffY /= m_nPointCount;

                    if( (m_lAveDiffX >= 0 && m_lAveDiffX < m_lAreaC) &&
                        (m_lAveDiffY >= 0 && m_lAveDiffY < m_lAreaC) )
                    {
                        if( !m_nTimerRecheckPoint )
                            m_nTimerRecheckPoint = startTimer( 1500 );

                        playBuzzer( BuzzerClick );
                        m_ptTickPos = m_ptTouch;

                        m_bPointOK = true;


                        /* ***** */
                        updateDesignCanvas( true, m_nCalibrationCheckPoint, true );
                    }
                    else
                    {
                        m_bPointOK = false;
                        m_nPointCount = 0;
                        m_lAveDiffX = m_lAveDiffY = 0;
                    }
                }
            }
            if( !bTouch  && m_bTouchCheck )
            {
                m_bTouchCheck = false;
                //TRACE( _T("UP!!!!!!!!!!\r\n") );

                emit displayPreviewTouchCount( 0 );

                forceMouseEvent( 0, 0, m_ptTouch.x(), m_ptTouch.y() );
                if( m_bPointOK )
                {
                    if( m_nTimerRecheckPoint )
                    {
                        killTimer( m_nTimerRecheckPoint );
                        m_nTimerRecheckPoint = 0;
                    }
                    qDebug( "Point OK: %d, %d", m_ptTouch.x(), m_ptTouch.y() );
                    onCalibrationPoint( m_ptTickPos );
                }
                m_bPointOK = false;
                m_nPointCount = 0;
                m_lAveDiffX = m_lAveDiffY = 0;
            }
        }
        else
        {
            if( !bTouch && m_bTouchCheck )
            {
                m_bTouchCheck = false;
                //TRACE( _T("UP!!!!!!!!!!\r\n") );

                emit displayPreviewTouchCount( 0 );

                forceMouseEvent( 0, 0, m_ptTouch.x(), m_ptTouch.y() );
                if( m_wndTestCanvas.isVisible() )
                {
                    m_wndTestCanvas.viewTouchPoint( m_ptTouch.x(), m_ptTouch.y(),false );
                }
            }
        }
    }
    else if( evt->timerId() == m_nTimerCheckAssociated )
    {
        killTimer( m_nTimerCheckAssociated );
        m_nTimerCheckAssociated = 0;

        changeFileAssociateBtnTitle();
    }
}

void TabCalibrationWidget::changeFileAssociateBtnTitle()
{
    if ( isAssociateFileExt() )
	{
        ui->BtnHSK->setText( "Remove associated file extension(\".hsk\")" );
	}
	else
	{
        ui->BtnHSK->setText( "Associate T3kSoftlogic with \".hsk\"" );
	}
}

void TabCalibrationWidget::on_BtnCalibration_clicked()
{
    int nSensorGPIOCount;
    if ( !verifyGPIO( nSensorGPIOCount ) )
    {
        QString strMsg = QString("It's different from the number of GPIO which the Sensor supports.\nSensor supports: %1").arg(nSensorGPIOCount);
        QMessageBox::critical( this, "Error", strMsg, QMessageBox::Ok );
        return;
    }

    if( !prepareKeyCalibration() )
    {
        QMessageBox::critical( this, "Error", "Erase Softlogic, Softkey Failure", QMessageBox::Ok );
        return;
    }

    T3kCommonData::instance()->resetCalibrationData();
    emit updatePreview();
    keyCalibration();
}

void TabCalibrationWidget::on_BtnCancel_clicked()
{
    cancelKeyCalibration();
}

void TabCalibrationWidget::on_BtnWriteNoCali_clicked()
{
    playBuzzer( BuzzerCalibrationSucces );

    QT3kDevice* pT3kHandle = getT3kHandle();

    if( pT3kHandle )
        pT3kHandle->setInstantMode( T3K_HID_MODE_COMMAND, 5000, 0 );

    if( writeToSensor( false ) )
    {
        QMessageBox::information( this, "Information", "Save Complete", QMessageBox::Ok );
    }
    else
    {
        QMessageBox::critical( this, "Error", "Save Failure", QMessageBox::Ok );
    }
}

void TabCalibrationWidget::on_BtnTest_clicked()
{
    QT3kDevice* pT3kHandle = getT3kHandle();

    T3kCommonData::instance()->setKeyDataMode( T3kCommonData::KeyDataModeTest );

    if( pT3kHandle )
    {
        m_bLoadFromSensor = true;
        char szCmd[256];
        sprintf( szCmd, "%s?", cstrFactorialSoftkeyPos );
        if( !pT3kHandle->sendCommand( szCmd, false ) )
        {
            m_bLoadFromSensor = false;
            QMessageBox::critical( this, "Error", "Cannot retrieve the Softkey information from the sensor.", QMessageBox::Ok );
            return;
        }

        m_strBackupSoftlogic.clear();
        m_bLoadFromSensor = true;
        sprintf( szCmd, "%s?", cstrFactorialSoftlogic );
        if( !pT3kHandle->sendCommand( szCmd, false ) )
        {
            m_bLoadFromSensor = false;
            QMessageBox::critical( this, "Error", "Cannot retrieve the Softlogic information from the sensor.", QMessageBox::Ok );
            return;
        }

        sprintf( szCmd, "%s**", cstrFactorialSoftlogic );
        if( !pT3kHandle->sendCommand( szCmd, false, 2000 ) )
        {
            QMessageBox::critical( this, "Error", "Cannot erase the Softlogic information from the sensor.", QMessageBox::Ok );
            return;
        }

        sprintf( szCmd, "%s*", cstrSoftlogic );
        if( !pT3kHandle->sendCommand( szCmd, false, 2000 ) )
        {
            QMessageBox::critical( this, "Error", "Cannot erase the Softlogic information from the sensor.", QMessageBox::Ok );
            return;
        }
    }

    QDesktopWidget desktop;
    QRect rcSelMonitor( desktop.screenGeometry( desktop.primaryScreen() ) );

    m_wndTestCanvas.setGeometry( rcSelMonitor );
    m_wndTestCanvas.setScreenMode( QKeyDesignWidget::ScreenModeTest );
    m_wndTestCanvas.setFont( font() );
    m_wndTestCanvas.init();
    m_wndTestCanvas.showFullScreen();
    m_wndTestCanvas.setFocus();

    if( pT3kHandle )
        pT3kHandle->setInstantMode( T3K_HID_MODE_COMMAND|T3K_HID_MODE_MESSAGE|T3K_HID_MODE_DEVICE, 5000, 0 );

    if( !m_nTimerCheckPoint )
        m_nTimerCheckPoint = startTimer( 10 );
}

void TabCalibrationWidget::on_BtnWriteLogic_clicked()
{
    if( QMessageBox::warning( this, "Warning", "Would you like to apply softlogic to sensor?", QMessageBox::Yes|QMessageBox::No ) == QMessageBox::No )
    {
        return;
    }

    if( writeToSensor( true ) )
    {
        QMessageBox::information( this, "Information", "Save Complete", QMessageBox::Ok );
    }
    else
    {
        QMessageBox::critical( this, "Error", "Save Failure", QMessageBox::Ok );
    }
}

void TabCalibrationWidget::on_BtnEraseAll_clicked()
{
    if( QMessageBox::warning( this, "Warning", "Would you like to erase all data?", QMessageBox::Yes|QMessageBox::No ) == QMessageBox::No )
    {
        return;
    }

    QT3kDevice* pT3kHandle = getT3kHandle();

    bool bOK = false;

    if( pT3kHandle )
    {
        QString strRet;
        QString strExtra;
        QString strCmd;
        int nRetry = 0;

        do
        {
            bOK = true;

            strCmd = QString("%1**").arg(cstrFactorialSoftkey );
            if( !pT3kHandle->sendCommand( strCmd, false ) )
            {
                bOK = false;
            }
            strCmd = QString("%1**").arg(cstrFactorialSoftlogic );
            if( !pT3kHandle->sendCommand( strCmd, false ) )
            {
                bOK = false;
            }

            strCmd = QString("%1*").arg(cstrSoftlogic);
            if( !pT3kHandle->sendCommand( strCmd, false ) )
            {
                bOK = false;
            }

            nRetry++;

        } while( !bOK && (nRetry < 3) );
    }

    if( !bOK )
    {
        QMessageBox::critical( this, "Error", "Erase Failure", QMessageBox::Ok );
    }
}

void TabCalibrationWidget::on_BtnHSK_clicked()
{
#ifdef Q_OS_WIN
    if( isProcessElevated() ) // cross platform
    {
        if ( isAssociateFileExt() )
            emit doRemoveFileExtAssociation();
        else
            emit doAssociateFileExt();

        changeFileAssociateBtnTitle();
    }
    else
    {
        ui->BtnCalibration->setEnabled( false );
        ui->BtnCancel->setEnabled( false );

        ui->BtnTest->setEnabled( false );
        ui->BtnWriteLogic->setEnabled( false );
        ui->BtnEraseAll->setEnabled( false );
        ui->BtnHSK->setEnabled( false );

        emit enableControls( false );

        do
        {            
            TCHAR szPath[_MAX_PATH];
            if (GetModuleFileName(NULL, szPath, sizeof(szPath)))
            {
                // Launch itself as administrator.
                SHELLEXECUTEINFO sei;
                memset( &sei, 0, sizeof(SHELLEXECUTEINFO) );
                sei.lpVerb = L"runas";
                sei.lpFile = L"C:\\Windows\\System32\\notepad.exe";
                sei.hwnd = (HWND)winId();
                sei.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_DDEWAIT;
                sei.nShow = SW_HIDE;
                if ( !isAssociateFileExt() )
                    sei.lpParameters = L"/e /exe:assoc_file_ext";
                else
                    sei.lpParameters = L"/e /exe:remove_file_ext";


                if (!ShellExecute((HWND)winId(), L"runas", szPath, isAssociateFileExt() ? L"/e /exe:remove_file_ext" : L"/e /exe:assoc_file_ext", 0, SW_HIDE))
                //if (!ShellExecuteEx(&sei))
                {
                    DWORD dwError = GetLastError();
                    if (dwError == ERROR_CANCELLED)
                    {
                        break;
                    }
                }
                else
                {
//                    DWORD dwExitCode = 0;
//                    BOOL bDone = false;
//                    MSG msg;
//                    while ( !bDone )
//                    {
//                        ::GetExitCodeProcess( sei.hProcess, &dwExitCode );
//                        if ( dwExitCode != STILL_ACTIVE )
//                            bDone = TRUE;
//                        else
//                        {
//                            if ( ::PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
//                            {
//                                ::TranslateMessage( &msg );
//                                ::DispatchMessage( &msg );
//                            }
//                        }
//                    }
                }
            }
        } while ( false );


        ui->BtnCalibration->setEnabled( true );
        ui->BtnCancel->setEnabled( true );

        ui->BtnTest->setEnabled( true );
        ui->BtnWriteLogic->setEnabled( true );
        ui->BtnEraseAll->setEnabled( true );
        ui->BtnHSK->setEnabled( true );

        emit enableControls( true );

        if( m_nTimerCheckAssociated )
            killTimer( m_nTimerCheckAssociated );
        m_nTimerCheckAssociated = startTimer( 500 );
    }
#endif
}

void TabCalibrationWidget::on_ChkBackground_toggled(bool checked)
{
    m_bCheckInvertDrawing = checked;
    QSettings settings( "Habilience", "T3kSoftlogic" );
    settings.beginGroup( "APP_SETTING" );
    settings.setValue( "INVERT_DRAWING", m_bCheckInvertDrawing );
    settings.endGroup();

    emit invertDrawing( m_bCheckInvertDrawing );
}

void TabCalibrationWidget::onCloseTestWidget()
{
    if( m_nTimerCheckPoint )
    {
        killTimer( m_nTimerCheckPoint );
        m_nTimerCheckPoint = 0;
    }

    T3kCommonData::instance()->setKeyDataMode( T3kCommonData::KeyDataModeCalibration );
    emit updatePreview();

    QT3kDevice* pT3kHandle = getT3kHandle();
    if( pT3kHandle )
    {
        pT3kHandle->setInstantMode( T3K_HID_MODE_COMMAND, 5000, 0 );

        int nRetry = 0;
        char szCmd[1024];
        if ( !m_strBackupSoftlogic.isEmpty() )
        {
            sprintf( szCmd, "%s%s", cstrFactorialSoftlogic, m_strBackupSoftlogic.toUtf8().data() );

            do
            {
                if( pT3kHandle->sendCommand( szCmd, false ) )
                {
                    break;
                }
            } while ( nRetry < 3 );

            if ( nRetry >= 3 )
            {
                QMessageBox::critical( this, "Error", "Cannot write the Softlogic information to the sensor.", QMessageBox::Ok );
            }

            sprintf( szCmd, "%s*", cstrSoftlogic );
            nRetry = 0;
            do
            {
                if( pT3kHandle->sendCommand( szCmd, false ) )
                {
                    break;
                }
            } while ( nRetry < 3 );

            if ( nRetry >= 3 )
            {
                QMessageBox::critical( this, "Error", "Cannot write the Softlogic information to the sensor.", QMessageBox::Ok );
            }
        }
    }
}
