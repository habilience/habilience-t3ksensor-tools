#include "HIDCmdThread.h"

#include <QCoreApplication>
#include <QTime>

#include "DefineString.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <process.h>
#endif

#define SYSTIME_PER_MSEC 10000


CHIDCmd::CHIDCmd(QObject* parent) :
    QObject(parent)
{
    m_pT3kHandle = new T3kHandle();
    m_bIsConnect = false;

	for ( int ni = 0; ni < PROMPT_MAX; ni++ )
	{
		m_prompt[ni] = promptNil;
	}

	m_ppPreCommands = NULL;
    m_nPreCommands = 0;

	m_pFileGetNv = NULL;
	m_eGetNv = enFalse;

    m_tmStart = QDateTime::currentDateTimeUtc().toMSecsSinceEpoch();
    m_bTextOut = true;
    m_bInstantMode = false;

	m_szInstantMode[0] = '\0';

	m_dwTimeCheck = 0;

    m_nGetNvId = 0;
}

CHIDCmd::~CHIDCmd(void)
{
    EndT3k();

	for ( int ni = 0; ni < m_nPreCommands; ni++ )
	{
		delete [] m_ppPreCommands[ni];
	}
	m_nPreCommands = 0;
	delete [] m_ppPreCommands;
	m_ppPreCommands = NULL;

	if ( m_pFileGetNv )
	{
		fclose(m_pFileGetNv);
		m_pFileGetNv = NULL;
	}
}

struct DEVICE_ID
{
    ushort nVID;
    ushort nPID;
    ushort nMI;
	int nDeviceIndex;
};

bool CHIDCmd::OpenT3kHandle()
{
    bool bRet = false;

    // setnotify
    m_pT3kHandle->SetNotify( T3kHIDNotify::Instance() );

	do
	{
        int nOldT3000DetectCnt = T3kHandle::GetDeviceCount( 0xFFFF, 0x0000, 1 );
		if( nOldT3000DetectCnt > 0 )
		{
            bRet = m_pT3kHandle->OpenWithVIDPID( 0xFFFF, 0x0000, 1, 0 );
			if( bRet ) break;
		}
        int nT3000DetectCnt = T3kHandle::GetDeviceCount( 0x2200, 0x3000, 1 );
		if( nT3000DetectCnt > 0 )
		{
            bRet = m_pT3kHandle->OpenWithVIDPID( 0x2200, 0x3000, 1, 0 );
			if( bRet ) break;
		}
        int nT3100DetectCnt = T3kHandle::GetDeviceCount( 0x2200, 0x3100, 1 );
		if( nT3100DetectCnt > 0 )
		{
            bRet = m_pT3kHandle->OpenWithVIDPID( 0x2200, 0x3100, 1, 0 );
			if( bRet ) break;
		}
        int nT3200DetectCnt = T3kHandle::GetDeviceCount( 0x2200, 0x3200, 1 );
		if( nT3200DetectCnt > 0 )
		{
            bRet = m_pT3kHandle->OpenWithVIDPID( 0x2200, 0x3200, 1, 0 );
			if( bRet ) break;
		}
        int nT3500DetectCnt = T3kHandle::GetDeviceCount( 0x2200, 0x3500, 1 );
		if( nT3500DetectCnt > 0 )
		{
            bRet = m_pT3kHandle->OpenWithVIDPID( 0x2200, 0x3500, 1, 0 );
			if( bRet ) break;
		}
        int nT3900DetectCnt = T3kHandle::GetDeviceCount( 0x2200, 0x3900, 1 );
		if( nT3900DetectCnt > 0 )
		{
            bRet = m_pT3kHandle->OpenWithVIDPID( 0x2200, 0x3900, 1, 0 );
			if( bRet ) break;
		}
        int nT3kVHIDDetectCnt = T3kHandle::GetDeviceCount( 0x2200, 0xFF02, 0 );
		if ( nT3kVHIDDetectCnt > 0 )
		{
            bRet = m_pT3kHandle->OpenWithVIDPID( 0x2200, 0xFF02, 0, 0 );
			break;
		}

    } while( false );

	if( bRet )
	{
		m_dwTimeCheck = 0;
	}
	else
    {
        m_dwTimeCheck = QDateTime::currentDateTimeUtc().toMSecsSinceEpoch();
	}

	return bRet;
}

bool CHIDCmd::OnCommand( char * cmd, bool * pbSysCmd )
{
    char * buf;
    char * file = NULL;

    if ( pbSysCmd )
        *pbSysCmd = true;

    if ( strcmp(cmd, "") == 0 )
    {
        //qDebug("no command");
    }
    else if ( strcmp(cmd, cstrHelp0) == 0 || strcmp(cmd, cstrHelp1) == 0 )
    {
        TextOutRuntime(cstrHelpOut, 0);
    }
    else if( strcmp(cmd, cstrCls) == 0 )
    {
        system("cls");
    }
    else if( strcmp(cmd, cstrExit) == 0 )
    {
        //g_bExit = true;
        TextOutRuntime(cstrExitOut, 0);
        return false;
    }
    else if ( strstr(cmd, cstrGetNv) == cmd )
    {
        bool bFactoryNv = false;

        buf = cmd + sizeof(cstrGetNv) - 1;
        while ( *buf != 0 )
        {
            if ( !isspace(*buf) )
            {
                if ( *buf == '/' )
                {
                    if ( *(++buf) == 'f' )
                    {
                        bFactoryNv = true;
                    }
                    buf++;
                    while ( true )
                    {
                        if ( *buf != 0 )
                        {
                            buf--;
                            break;
                        }
                        if ( isspace(*buf) )
                            break;
                        buf++;
                    }
                }
                else if ( *buf == '>' )
                {
                    file = buf + 1;
                    buf += strlen(buf) - 1;
                }
            }

            buf++;
        }

        if ( file != NULL )
        {
            while ( *file != 0 )
            {
                if ( isspace(*file) )
                {
                    file++;
                    continue;
                }
                else
                    break;
            }
        }

        GetNv(bFactoryNv, file);
    }
    else
    {
        if ( SendCommand(cmd) )
        {
            if ( pbSysCmd )
                *pbSysCmd = false;
        }
    }
    return true;
}

void CHIDCmd::InitT3k()
{
    OpenT3kHandle();

    if ( !m_bIsConnect )
    {
        FlushPreCommand();
    }
}

void CHIDCmd::EndT3k()
{
    if( m_pT3kHandle )
    {
        m_pT3kHandle->Close();
        delete m_pT3kHandle;
        m_pT3kHandle = NULL;
    }
}

bool CHIDCmd::ProcessT3k()
{
    if( !m_pT3kHandle )
    {
        qDebug( "Debug : null t3k handle " );
        return false;
    }

    if( !m_bIsConnect )
    {
        ulong dwCurTimeCheck = QDateTime::currentDateTimeUtc().toMSecsSinceEpoch();

        if( (dwCurTimeCheck - m_dwTimeCheck) > 2000 )		// 2s
        {
            OpenT3kHandle();
        }
    }

    return true;
}

void CHIDCmd::SetFutureHandle(QFuture<void> ft)
{
    m_ftCmdThread = ft;
}

void CHIDCmd::OnDeviceConnected(T3K_HANDLE hDevice)
{
    m_bIsConnect = true;

    ushort nVID = ::T3kGetDevInfoVendorID(::T3kGetDeviceInfoFromHandle(hDevice));
    ushort nPID = ::T3kGetDevInfoProductID(::T3kGetDeviceInfoFromHandle(hDevice));

	if ( nVID == 0xFFFF && nPID == 0x0000 )
	{
        TextOutConsole(-1, "T3000 Open!\r\n");
	}
	else
	{
		switch( nPID )
		{
		case 0x3000:
            TextOutConsole(-1, "T3000 Open!\r\n");
			break;
		case 0x3100:
            TextOutConsole(-1, "T3100 Open!\r\n");
			break;
		case 0x3200:
            TextOutConsole(-1, "T3200 Open!\r\n");
			break;
		case 0x3500:
            TextOutConsole(-1, "T3k A Open!\r\n");
			break;
		case 0x3900:
            TextOutConsole(-1, "T3900 Open!\r\n");
			break;
		default:
            TextOutConsole(-1, "T3k series Open!\r\n");
			break;
		}
	}

    m_bInstantMode = false;
    m_pT3kHandle->SetInstantMode(T3K_HID_MODE_COMMAND, 5000, 0);
}

void CHIDCmd::OnDeviceDisconnected(T3K_HANDLE hDevice)
{
    ushort nVID = ::T3kGetDevInfoVendorID(::T3kGetDeviceInfoFromHandle(hDevice));
    ushort nPID = ::T3kGetDevInfoProductID(::T3kGetDeviceInfoFromHandle(hDevice));

	if ( nVID == 0xFFFF && nPID == 0x0000 )
	{
        TextOutConsole(-1, "T3000 Close!\r\n");
	}
	else
	{
		switch( nPID )
		{
		case 0x3000:
            TextOutConsole(-1, "T3000 Close!\r\n");
			break;
		case 0x3100:
            TextOutConsole(-1, "T3100 Close!\r\n");
			break;
		case 0x3200:
            TextOutConsole(-1, "T3200 Close!\r\n");
			break;
		case 0x3500:
            TextOutConsole(-1, "T3k A Close!\r\n");
			break;
		case 0x3900:
            TextOutConsole(-1, "T3900 Close!\r\n");
			break;
		default:
            TextOutConsole(-1, "T3k series Close!\r\n");
			break;
		}
	}
}


void CHIDCmd::AddPrompt( Prompt p )
{
	for ( int ni = 0; ni < PROMPT_MAX; ni++ )
	{
		if ( m_prompt[ni] == promptNil )
		{
			m_prompt[ni] = p;
			break;
		}
	}
}


void CHIDCmd::AddPreCommand( const char * szCmd )
{
	char ** ppPreCommands = new char *[m_nPreCommands + 1];
	if ( m_ppPreCommands )
	{
		memcpy(ppPreCommands, m_ppPreCommands, m_nPreCommands * sizeof(char *));
		delete [] m_ppPreCommands;
	}
	m_ppPreCommands = ppPreCommands;

	m_ppPreCommands[m_nPreCommands] = new char[strlen(szCmd) + 1];
	strcpy(m_ppPreCommands[m_nPreCommands], szCmd);
	m_nPreCommands++;
}


bool CHIDCmd::SendCommand( char * szCmd )
{
	if( !IsHIDConnect() )
	{
		TextOutRuntime(cszIsNotConnected, 0);
        return false;
	}

    Q_ASSERT( m_pT3kHandle != NULL );

    static const char cstrHidModeChar[] = { 'M', 'C', 'V', '\0', 'O', 'T', 'G', 'D', 'S', 'X' };

	if ( strstr(szCmd, cszInstantMode) == szCmd )
	{
        int nInstantMode = T3K_HID_MODE_COMMAND;
        ushort nTimeout = 5000;
        ulong dwFGstValue = 0xFFFFFFFF;

        QString strCmd( szCmd );
        strCmd.trimmed();
        strCmd = strCmd.remove( 0, strCmd.indexOf( '=' ) + 1 );
        int nP = strCmd.indexOf( ',' );
        QString strMode = strCmd.left( nP ).toUpper();

        m_bInstantMode = true;

        if( strMode.length() > 1 &&
            (strMode.at(0) == '*' || strMode.at(0) == '!' || strMode.at(0) == '?') )
        {

        }
        else
        {
            for( int i=0; i<strMode.length(); i++ )
            {
                if( strMode.at(i) == cstrHidModeChar[i] )
                    nInstantMode |= (0x01 <<i);
            }

            strCmd = strCmd.remove( 0, nP + 1 );
            nP = strCmd.indexOf( ',' );
            if( nP >= 0 )
            {
                nTimeout = strCmd.left( nP ).toUShort();
                dwFGstValue = strCmd.remove( 0, nP + 1 ).toULong();
            }

            m_tmStart = QDateTime::currentDateTimeUtc().toMSecsSinceEpoch();
            m_pT3kHandle->SetInstantMode( nInstantMode, nTimeout, dwFGstValue );

            return true;
        }
    }

    m_tmStart = QDateTime::currentDateTimeUtc().toMSecsSinceEpoch();
    m_pT3kHandle->SendCommand(szCmd);

    return true;
}


void CHIDCmd::LockTextOut()
{
    QMutexLocker Lock( &m_csTextOut );
    m_bTextOut = false;
    //qDebug("LockTextOut\r\n");
}

void CHIDCmd::UnlockTextOut()
{
	//@@ lock textout

    //qDebug("UnlockTextOut\r\n");
    QMutexLocker Lock( &m_csTextOut );
    m_bTextOut = true;
}

void CHIDCmd::TextOutConsole( ulong ticktime, const char * szFormat, ... )
{
	va_list varpars;
	va_start(varpars, szFormat);

	char szCommand[1024];
	vsprintf(szCommand, szFormat, varpars);

    QMutexLocker Lock( &m_csTextOut );

	if ( m_bTextOut )
	{
		TextOutRuntime(szCommand, -1, ticktime);
	}
	else
	{
		//@@ lock textout
        //szCommand;
	}

	va_end(varpars);
}

void CHIDCmd::TextOutRuntime( const char * szCmd, uint time, ulong ticktime )
{
    if ( time == -1 )
	{
        time = QDateTime::currentDateTimeUtc().toMSecsSinceEpoch();
	}

	if ( time == 0 )
	{
		printf(szCmd);
	}
	else
	{
		char szOut[2048] = { 0 };
		char * pCur = szOut;

		// system time
		time -= m_tmStart;
		int sys_msec = (int)(time % 1000);
		int sys_sec = (int)((time / 1000) % 1000);

		// t3000 time
		int t_msec = (int)(ticktime % 1000);
		int t_sec = (int)((ticktime / 1000) % 1000);

		for ( int ni = 0; ni < PROMPT_MAX; ni++ )
		{
			switch ( m_prompt[ni] )
			{
			default:
			case promptNil:
				ni = PROMPT_MAX;
				break;
			case promptSystemTime:
				pCur += sprintf(pCur, "%03d.%03d ", sys_sec, sys_msec);
				break;
            case promptT3kTime:
                if ( ticktime != -1 )
					pCur += sprintf(pCur, "%03d.%03d ", t_sec, t_msec);
				else
                {
                    strcat(pCur, "---.--- ");
					pCur += strlen(pCur);
				}
				break;
			}
		}

		strcat(pCur, szCmd);

		printf(szOut);
	}
}

void CHIDCmd::OnOpenT3kDevice(T3K_HANDLE hDevice)
{
    OnDeviceConnected( hDevice );
}

void CHIDCmd::OnCloseT3kDevice(T3K_HANDLE hDevice)
{
    OnDeviceDisconnected( hDevice );
    m_pT3kHandle->Close();
    m_bIsConnect = false;
    ::memset( m_szInstantMode, 0, sizeof(char)*100 );
}

void CHIDCmd::OnMSG(ushort nTickTime, const char *sPardID, const char *sTxt)
{
    TextOutConsole(nTickTime, "%s: %s\r\n", sPardID, sTxt);
}

void CHIDCmd::OnVER(ushort nTickTime, const char *sPartID, T3kVER &ver)
{
	char szVersion[128] = { 0, };
	char date[T3K_VER_DATE_LEN + 1];
	char time[T3K_VER_TIME_LEN + 1];

    memcpy(date, ver.date, T3K_VER_DATE_LEN); date[T3K_VER_DATE_LEN] = 0;
    memcpy(time, ver.time, T3K_VER_TIME_LEN); time[T3K_VER_TIME_LEN] = 0;

    if ( ver.nv == 0 && ver.major == 0 && ver.minor == 0 )	// IAP
	{
        if ( sPartID[3] == 0 )
		{
            sprintf(szVersion, "Model: %X (IAP)", ver.model);
		}
		else
            sprintf(szVersion, "Model: T%X (IAP)", ver.model);
	}
	else
	{
        if ( sPartID[3] == 0 )
		{
            sprintf(szVersion, "NV: %d, Ver: %X.%X, Model: %X  %s %s", ver.nv, ver.major, ver.minor, ver.model, date, time);
		}
		else
            sprintf(szVersion, "NV: %d, Ver: %X.%X, Model: T%X  %s %s", ver.nv, ver.major, ver.minor, ver.model, date, time);
	}
    TextOutConsole(nTickTime, "%s: %s\r\n", sPartID, szVersion);
}

void CHIDCmd::OnSTT(ushort nTickTime, const char *sPartID, const char *sStatus)
{
    TextOutConsole(nTickTime, "%s: %s\r\n", sPartID, sStatus);
}

void CHIDCmd::OnRSP(ushort nTickTime, const char * sPartID, long lID, bool, const char *sCmd)
{
    bool bSend = false;

    if ( strstr(sCmd, cszInstantMode) == sCmd )
	{
        if ( strcmp( m_szInstantMode, sCmd ) != 0 )
		{
            strncpy( m_szInstantMode, sCmd, 100 );
			if ( !m_bInstantMode )
			{
                TextOutConsole(nTickTime, "%s: %s\r\n", sPartID, sCmd);
			}
		}
		if ( m_bInstantMode )
		{
            TextOutConsole(nTickTime, "%s: %s\r\n", sPartID, sCmd);
            m_bInstantMode = false;
		}
	}
	else
	{
		char szBuf[2048];

		if ( m_eGetNv > enFalse )
		{
            if ( m_nGetNvId != lID )
				return;
            if ( strstr( sCmd, "no cam" ) == sCmd || strstr( sCmd, "syntax error" ) == sCmd )
			{
                bSend = GetNv_SendNext( true );
			}
			else
			{
                if ( sCmd[0] != 0 )
				{
					if ( m_nNvIdx < 0 )
                        TextOutConsole(nTickTime, "%s: [%s]\r\n", sPartID, sCmd);
					else
                        TextOutConsole(nTickTime, "%s: %s\r\n", sPartID, sCmd);

					if ( m_pFileGetNv )
					{
						if ( m_nNvIdx < 0 )
						{
                            int nPartIdIdx = (int)strlen( sPartID )-1;

                            if( sPartID[nPartIdIdx-1] == '-' )
							{
								nPartIdIdx -= 2;
							}
							int nCamSub = 0;

                            switch( sPartID[nPartIdIdx] )
							{
							default:
							case 0:
                                sprintf(szBuf, "[%s]\r\n", sCmd);
								break;
							case '1':
                                nCamSub = (int)(sPartID[nPartIdIdx + 2]-'0');
								if ( nCamSub > 0 )
                                    sprintf(szBuf, "[cam1/sub/%s]\r\n", sCmd);
								else
                                    sprintf(szBuf, "[cam1/%s]\r\n", sCmd);
								break;
							case '2':
                                nCamSub = (int)(sPartID[nPartIdIdx + 2]-'0');
								if ( nCamSub > 0 )
                                    sprintf(szBuf, "[cam2/sub/%s]\r\n", sCmd);
								else
                                    sprintf(szBuf, "[cam2/%s]\r\n", sCmd);
								break;
							}
							fwrite(szBuf, sizeof(char), strlen(szBuf), m_pFileGetNv);
						}
						else
						{
                            int nPartIdIdx = (int)strlen( sPartID )-1;

                            if( sPartID[nPartIdIdx-1] == '-' )
							{
								nPartIdIdx -= 2;
							}
							int nCamSub = 0;

                            switch( sPartID[nPartIdIdx] )
							{
							default:
							case 0:
                                sprintf(szBuf, "%s\r\n", sCmd);
								break;
							case '1':
                                nCamSub = (int)(sPartID[nPartIdIdx + 2]-'0');
								if ( nCamSub > 0 )
                                    sprintf(szBuf, "cam1/sub/%s\r\n", sCmd);
								else
                                    sprintf(szBuf, "cam1/%s\r\n", sCmd);
								break;
							case '2':
                                nCamSub = (int)(sPartID[nPartIdIdx + 2]-'0');
								if ( nCamSub > 0 )
                                    sprintf(szBuf, "cam2/sub/%s\r\n", sCmd);
								else
                                    sprintf(szBuf, "cam2/%s\r\n", sCmd);
								break;
							}
							fwrite(szBuf, sizeof(char), strlen(szBuf), m_pFileGetNv);
						}
					}
				}

                bSend = GetNv_SendNext(sCmd[0] == 0);
			}
		}
		else if ( m_pFileGetNv )
		{
            switch ( sPartID[3] )
			{
			default:
			case 0:
                sprintf(szBuf, ";%s\r\n", sCmd);
				break;
			case '1':
                sprintf(szBuf, ";cam1/%s\r\n", sCmd);
				break;
			case '2':
                sprintf(szBuf, ";cam2/%s\r\n", sCmd);
				break;
			}
            //int strn = strlen(szBuf);
            //qDebug( "[0x%02x][0x%02x]\r\n", szBuf[strn-2], szBuf[strn-1] );
			fwrite(szBuf, sizeof(char), strlen(szBuf), m_pFileGetNv);
		}
		else
		{
            TextOutConsole(nTickTime, "%s: %s\r\n", sPartID, sCmd);
		}
	}

	if ( !bSend )
	{
		FlushPreCommand();
	}
}

void CHIDCmd::OnRSE(ushort ticktime, const char *partid, long id, bool finish, const char *cmd)
{
    OnRSP(ticktime, partid, id, finish, cmd);
}

void CHIDCmd::FlushPreCommand()
{
    bool bSysCmd = false;
	while ( m_nPreCommands > 0 )
	{
        bool bCmd = OnCommand(m_ppPreCommands[0], &bSysCmd);

		m_nPreCommands--;
		delete [] m_ppPreCommands[0];
		if ( m_nPreCommands == 0 )
		{
			delete [] m_ppPreCommands;
			m_ppPreCommands = NULL;
		}
		else
		{
			char ** ppPreCommands = new char *[m_nPreCommands];
			memcpy(ppPreCommands, m_ppPreCommands + 1, m_nPreCommands * sizeof(char *));
			delete [] m_ppPreCommands;
			m_ppPreCommands = ppPreCommands;
		}

		if ( !bCmd )
		{
#ifdef Q_OS_WIN
            HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
            INPUT_RECORD ir;
            ir.EventType = KEY_EVENT;
            ir.Event.KeyEvent.bKeyDown = 1;
            ir.Event.KeyEvent.wRepeatCount = 1;
            ir.Event.KeyEvent.wVirtualKeyCode = VK_SPACE;
            ir.Event.KeyEvent.wVirtualScanCode = 0x39;
            ir.Event.KeyEvent.uChar.AsciiChar = ' ';
            ir.Event.KeyEvent.dwControlKeyState = 0x0;
            DWORD dwWrite;
            WriteConsoleInput(hIn, &ir, 1, &dwWrite);
#endif
			return;
		}
		if ( !bSysCmd )
			break;
	}
}


void CHIDCmd::GetNv( bool bFactory, const char * szFile )
{
	m_eGetNv = enFalse;
	if ( !m_bIsConnect )
	{
		TextOutRuntime(cszIsNotConnected, 0);
		return;
	}

    TextOutRuntime(cstrGetNvString, 0);

	if ( szFile )
	{
		if ( m_pFileGetNv )
		{
			fclose(m_pFileGetNv);
		}
		m_pFileGetNv = fopen(szFile, "wb");
	}

	int nMode;
#if 1
    if ( m_pT3kHandle->QueryFirmwareVersion(PKT_ADDR_MM, &nMode) )
	{
		if ( nMode == MODE_MM_APP )
		{
            if ( m_pT3kHandle->QueryFirmwareVersion(PKT_ADDR_CM1, &nMode) )
			{
				//if ( nMode == MODE_CM_APP )
                m_pT3kHandle->QueryFirmwareVersion(PKT_ADDR_CM1 | PKT_ADDR_CM_SUB, &nMode);
			}
            if ( m_pT3kHandle->QueryFirmwareVersion(PKT_ADDR_CM2, &nMode) )
			{
				//if ( nMode == MODE_CM_APP )
                m_pT3kHandle->QueryFirmwareVersion(PKT_ADDR_CM2 | PKT_ADDR_CM_SUB, &nMode);
			}
		}
	}
#else
    m_pT3kHandle->SendCommand("firmware_version=?");
    m_pT3kHandle->SendCommand("cam1/firmware_version=?");
    m_pT3kHandle->SendCommand("cam1/sub/firmware_version=?");
    m_pT3kHandle->SendCommand("cam2/firmware_version=?");
    m_pT3kHandle->SendCommand("cam2/sub/firmware_version=?");
#endif

	// 
	m_bFactorialNv = bFactory;
	if ( m_bFactorialNv )
		m_nNvIdx = -1;
	else
		m_nNvIdx = 0;
	m_eGetNv = enCM1;
    m_tmStart = QDateTime::currentDateTimeUtc().toMSecsSinceEpoch();

    GetNv_SendNext( false );
}

bool CHIDCmd::GetNv_SendNext( bool bEndOfGetNv )
{
	if ( m_eGetNv == enFalse )
        return false;

	if ( bEndOfGetNv )
	{
		if ( m_nNvIdx < 0 )
			m_nNvIdx = 0;
		else
		{
			m_eGetNv = (tagGetNv)(m_eGetNv - 1);
			if ( m_eGetNv == enFalse )
			{
				if ( m_pFileGetNv )
				{
					fclose(m_pFileGetNv);
					m_pFileGetNv = NULL;
				}

				TextOutRuntime(cszGetNv_End, 0);
                return false;
			}
			else
			{
				if ( m_bFactorialNv )
					m_nNvIdx = -1;
				else
					m_nNvIdx = 0;
			}
		}
	}

	char szCmd[80];
	switch ( m_eGetNv )
	{
	case enCM1:
		sprintf(szCmd, "cam1/get_nv=%d", m_nNvIdx);
		break;

	case enCM1c:
		sprintf(szCmd, "cam1/sub/get_nv=%d", m_nNvIdx);
		break;

	case enCM2:
		sprintf(szCmd, "cam2/get_nv=%d", m_nNvIdx);
		break;

	case enCM2c:
		sprintf(szCmd, "cam2/sub/get_nv=%d", m_nNvIdx);
		break;

	case enMM:
	default:
		sprintf(szCmd, "get_nv=%d", m_nNvIdx);
		break;
	}
	if ( m_nNvIdx < 0 )
		m_nNvIdx--;
	else
		m_nNvIdx++;

    int nId = m_pT3kHandle->SendCommand(szCmd, true);
	m_nGetNvId = nId;
	//TRACE( "Send cmd[%d]: %s\r\n", nId, szCmd );

    return true;
}
