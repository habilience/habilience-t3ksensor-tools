#include "HIDCmdThread.h"

#include <QCoreApplication>
#include <QTime>
#include <QFile>

#include "DefineString.h"

#include "t3kcomdef.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <process.h>
#endif

#define SYSTIME_PER_MSEC 10000


CHIDCmd::CHIDCmd(QObject* parent) :
    QObject(parent)
{
    m_pT3kHandle = QT3kDevice::instance();

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
//    TPDPEventMultiCaster::instance()->SetSingleListener( this );

    for ( int d = 0 ; d<COUNT_OF_DEVICE_LIST(APP_DEVICE_LIST) ; d++)
    {
        int nCnt = QT3kDevice::getDeviceCount( APP_DEVICE_LIST[d].nVID, APP_DEVICE_LIST[d].nPID, APP_DEVICE_LIST[d].nMI );
        if( nCnt > 0 )
        {
            bRet = QT3kDevice::instance()->open( APP_DEVICE_LIST[d].nVID, APP_DEVICE_LIST[d].nPID, APP_DEVICE_LIST[d].nMI, 0 );
        }
    }

	if( bRet )
	{
		m_dwTimeCheck = 0;
        OnDeviceConnected();
	}
	else
    {
        m_dwTimeCheck = QDateTime::currentDateTimeUtc().toMSecsSinceEpoch();
	}

	return bRet;
}

bool CHIDCmd::OnCommand( char * cmd, bool * pbSysCmd, bool bAsync )
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
    else if( strcmp(cmd, cstrLicense) == 0 )
    {
        QFile file( ":/T3kCmdRes/resources/license.txt" );
        if( file.open( QFile::ReadOnly ) )
        {
            while( !file.atEnd() )
            {
                QByteArray ba( file.readLine() );
                TextOutRuntime( ba.data() );
            }
            TextOutRuntime("\r\n");
            file.close();
        }
    }
    else if( strcmp(cmd, cstrCls) == 0 )
    {
        system(cstrCls);
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
        if ( SendCommand(cmd, bAsync) )
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

    if( !m_bIsConnect )
        FlushPreCommand();
}

void CHIDCmd::EndT3k()
{
    if( m_pT3kHandle )
    {
        m_pT3kHandle->close();
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

void CHIDCmd::OnDeviceConnected()
{
    m_bIsConnect = true;

    ushort nVID = ::T3kGetDevInfoVendorID( m_pT3kHandle->getDeviceInfo() );
    ushort nPID = ::T3kGetDevInfoProductID( m_pT3kHandle->getDeviceInfo() );

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
    m_pT3kHandle->setInstantMode(T3K_HID_MODE_COMMAND, 5000, 0);
}

void CHIDCmd::OnDeviceDisconnected(T3K_DEVICE_INFO devInfo)
{
    ushort nVID = ::T3kGetDevInfoVendorID( devInfo );
    ushort nPID = ::T3kGetDevInfoProductID( devInfo );

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


bool CHIDCmd::SendCommand( char * szCmd, bool bAsync )
{
	if( !IsHIDConnect() )
	{
		TextOutRuntime(cszIsNotConnected, 0);
        return false;
	}

    Q_ASSERT( m_pT3kHandle != NULL );

	if ( strstr(szCmd, cszInstantMode) == szCmd )
	{
        char szBuffer[1024] = { 0 };
        szCmd += sizeof(cszInstantMode) - 1;
        int len = (int)strlen(szCmd);
        for ( int ni = 0; ni < len; ni++ )
        {
            if ( isspace(*szCmd) )
                szCmd++;
            else
                break;
        }
        char * pD = strchr(szCmd, ',');
        if ( pD == NULL )
            pD = szCmd + strlen(szCmd);
        strcpy(szBuffer, cszInstantMode);
        bool bCmd = false;
        bool bQuery = false;
        while ( szCmd < pD )
        {
            if ( *szCmd == 'c' || *szCmd == 'C' )
                bCmd = true;
            if ( *szCmd == '*' || *szCmd == '!' || *szCmd == '?' )
            {
                bQuery = true;
            }
            szBuffer[strlen(szBuffer)] = *szCmd++;
        }
        if ( !bQuery && !bCmd )
        {
            szBuffer[strlen(szBuffer)] = 'C';
        }
        strcat(szBuffer, pD);
        szCmd = szBuffer;

        m_bInstantMode = true;
    }

    m_tmStart = QDateTime::currentDateTimeUtc().toMSecsSinceEpoch();
    m_pT3kHandle->sendCommand(szCmd, bAsync);

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
    if ( (int)time == -1 )
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
                if ( (int)ticktime != -1 )
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

void CHIDCmd::TPDP_OnDisconnected(T3K_DEVICE_INFO devInfo)
{
    OnDeviceDisconnected( devInfo );
    m_pT3kHandle->close();
    m_bIsConnect = false;
    ::memset( m_szInstantMode, 0, sizeof(char)*100 );
}

void CHIDCmd::TPDP_OnMSG(T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short ticktime, const char *partid, const char *txt)
{
    TextOutConsole(ticktime, "%s: %s\r\n", partid, txt);
}

void CHIDCmd::TPDP_OnVER(T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short ticktime, const char *partid, t3kpacket::_body::_ver *ver)
{
	char szVersion[128] = { 0, };
	char date[T3K_VER_DATE_LEN + 1];
	char time[T3K_VER_TIME_LEN + 1];

    memcpy(date, ver->date, T3K_VER_DATE_LEN); date[T3K_VER_DATE_LEN] = 0;
    memcpy(time, ver->time, T3K_VER_TIME_LEN); time[T3K_VER_TIME_LEN] = 0;

    if ( ver->nv == 0 && ver->major == 0 && ver->minor == 0 )	// IAP
	{
        if ( partid[3] == 0 )
		{
            sprintf(szVersion, "Model: %X (IAP)", ver->model);
		}
		else
            sprintf(szVersion, "Model: T%X (IAP)", ver->model);
	}
	else
	{
        if ( partid[3] == 0 )
		{
            sprintf(szVersion, "NV: %d, Ver: %X.%X, Model: %X  %s %s", ver->nv, ver->major, ver->minor, ver->model, date, time);
		}
		else
            sprintf(szVersion, "NV: %d, Ver: %X.%X, Model: T%X  %s %s", ver->nv, ver->major, ver->minor, ver->model, date, time);
	}
    TextOutConsole(ticktime, "%s: %s\r\n", partid, szVersion);
}

void CHIDCmd::TPDP_OnSTT(T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short ticktime, const char *partid, const char *status)
{
    TextOutConsole(ticktime, "%s: %s\r\n", partid, status);
}

void CHIDCmd::TPDP_OnRSP(T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short ticktime, const char *partid, int id, bool /*bFinal*/, const char *cmd)
{
    bool bSend = false;

    if ( strstr(cmd, cszInstantMode) == cmd )
	{
        if ( strcmp( m_szInstantMode, cmd ) != 0 )
		{
            strncpy( m_szInstantMode, cmd, 100 );
			if ( !m_bInstantMode )
			{
                TextOutConsole(ticktime, "%s: %s\r\n", partid, cmd);
			}
		}
		if ( m_bInstantMode )
		{
            TextOutConsole(ticktime, "%s: %s\r\n", partid, cmd);
            m_bInstantMode = false;
		}
	}
	else
	{
		char szBuf[2048];

		if ( m_eGetNv > enFalse )
		{
            if ( m_nGetNvId != id )
				return;
            if ( strstr( cmd, "no cam" ) == cmd || strstr( cmd, "syntax error" ) == cmd )
			{
                bSend = GetNv_SendNext( true );
			}
			else
			{
                if ( cmd[0] != 0 )
				{
					if ( m_nNvIdx < 0 )
                        TextOutConsole(ticktime, "%s: [%s]\r\n", partid, cmd);
					else
                        TextOutConsole(ticktime, "%s: %s\r\n", partid, cmd);

					if ( m_pFileGetNv )
					{
						if ( m_nNvIdx < 0 )
						{
                            int nPartIdIdx = (int)strlen( partid )-1;

                            if( partid[nPartIdIdx-1] == '-' )
							{
								nPartIdIdx -= 2;
							}
							int nCamSub = 0;

                            switch( partid[nPartIdIdx] )
							{
							default:
							case 0:
                                sprintf(szBuf, "[%s]\r\n", cmd);
								break;
							case '1':
                                nCamSub = (int)(partid[nPartIdIdx + 2]-'0');
								if ( nCamSub > 0 )
                                    sprintf(szBuf, "[cam1/sub/%s]\r\n", cmd);
								else
                                    sprintf(szBuf, "[cam1/%s]\r\n", cmd);
								break;
							case '2':
                                nCamSub = (int)(partid[nPartIdIdx + 2]-'0');
								if ( nCamSub > 0 )
                                    sprintf(szBuf, "[cam2/sub/%s]\r\n", cmd);
								else
                                    sprintf(szBuf, "[cam2/%s]\r\n", cmd);
								break;
							}
							fwrite(szBuf, sizeof(char), strlen(szBuf), m_pFileGetNv);
						}
						else
						{
                            int nPartIdIdx = (int)strlen( partid )-1;

                            if( partid[nPartIdIdx-1] == '-' )
							{
								nPartIdIdx -= 2;
							}
							int nCamSub = 0;

                            switch( partid[nPartIdIdx] )
							{
							default:
							case 0:
                                sprintf(szBuf, "%s\r\n", cmd);
								break;
							case '1':
                                nCamSub = (int)(partid[nPartIdIdx + 2]-'0');
								if ( nCamSub > 0 )
                                    sprintf(szBuf, "cam1/sub/%s\r\n", cmd);
								else
                                    sprintf(szBuf, "cam1/%s\r\n", cmd);
								break;
							case '2':
                                nCamSub = (int)(partid[nPartIdIdx + 2]-'0');
								if ( nCamSub > 0 )
                                    sprintf(szBuf, "cam2/sub/%s\r\n", cmd);
								else
                                    sprintf(szBuf, "cam2/%s\r\n", cmd);
								break;
							}
							fwrite(szBuf, sizeof(char), strlen(szBuf), m_pFileGetNv);
						}
					}
				}

                bSend = GetNv_SendNext(cmd[0] == 0);
			}
		}
		else if ( m_pFileGetNv )
		{
            switch ( partid[3] )
			{
			default:
			case 0:
                sprintf(szBuf, ";%s\r\n", cmd);
				break;
			case '1':
                sprintf(szBuf, ";cam1/%s\r\n", cmd);
				break;
			case '2':
                sprintf(szBuf, ";cam2/%s\r\n", cmd);
				break;
			}
            //int strn = strlen(szBuf);
            //qDebug( "[0x%02x][0x%02x]\r\n", szBuf[strn-2], szBuf[strn-1] );
			fwrite(szBuf, sizeof(char), strlen(szBuf), m_pFileGetNv);
		}
		else
		{
            TextOutConsole(ticktime, "%s: %s\r\n", partid, cmd);
		}
	}

	if ( !bSend )
	{
		FlushPreCommand();
	}
}

void CHIDCmd::TPDP_OnRSE(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd)
{
    TPDP_OnRSP(devInfo, Part, ticktime, partid, id, bFinal, cmd);
}

void CHIDCmd::FlushPreCommand()
{
    bool bSysCmd = false;
	while ( m_nPreCommands > 0 )
	{
        bool bCmd = OnCommand(m_ppPreCommands[0], &bSysCmd, true);

        qDebug() << m_nPreCommands << " : " << m_ppPreCommands[0];
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
    if ( m_pT3kHandle->queryFirmwareVersion(PKT_ADDR_MM, &nMode) )
	{
		if ( nMode == MODE_MM_APP )
		{
            if ( m_pT3kHandle->queryFirmwareVersion(PKT_ADDR_CM1, &nMode) )
			{
				//if ( nMode == MODE_CM_APP )
                m_pT3kHandle->queryFirmwareVersion(PKT_ADDR_CM1 | PKT_ADDR_CM_SUB, &nMode);
			}
            if ( m_pT3kHandle->queryFirmwareVersion(PKT_ADDR_CM2, &nMode) )
			{
				//if ( nMode == MODE_CM_APP )
                m_pT3kHandle->queryFirmwareVersion(PKT_ADDR_CM2 | PKT_ADDR_CM_SUB, &nMode);
			}
		}
	}
#else
    m_pT3kHandle->sendCommand("firmware_version=?");
    m_pT3kHandle->sendCommand("cam1/firmware_version=?");
    m_pT3kHandle->sendCommand("cam1/sub/firmware_version=?");
    m_pT3kHandle->sendCommand("cam2/firmware_version=?");
    m_pT3kHandle->sendCommand("cam2/sub/firmware_version=?");
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

    int nId = m_pT3kHandle->sendCommand(szCmd, true);
	m_nGetNvId = nId;
	//TRACE( "Send cmd[%d]: %s\r\n", nId, szCmd );

    return true;
}
