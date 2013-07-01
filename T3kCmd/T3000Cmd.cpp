#include "T3000Cmd.h"
#include "HIDCmdThread.h"
#include <conio.h>

CHIDCmdThread  g_HIDCmdThread;

const char cstrT3000Help[] = "T3k series command interpreter.\r\n\\r\n\T3kCmd [/t] [/T] [/c:Command]\r\n\\r\n\/t\tDisplays the system time of T3k series.\r\n\/T\tDisplays the operation time of PC.\r\n/c:Command\tCarries out the command specified by starting.\r\n";
const char cstrTitleOut[] = "T3kCmd [Version \"VERSION\"]\r\n\Copyright (c) 2012 Habilience. All rights reserved.\r\n\r\n";
const char cstrHelpOut[] = "/?\tProvides Help information for T3k series commands.\r\n\cls\tClears the screen.\r\n\exit\tQuits this program (T3kCmd.exe).\r\n\load_nv [/f] [>FILENAME]\r\n\tDisplays the system values.\r\n\t/f\tDisplays the factorial values.\r\n\t>FILENAME\tSaves the values to PC.\r\n\instant_mode=[C][M]\r\n\tSelects contents to displaying.\r\n\tC\tDisplays Commands and Responses.\r\n\tM\tDisplays Messages.\r\n\help\tProvides Help information for T3k series commands.\r\n\r\n";
const char cstrExitOut[] = "Good-bye.\r\n\r\n";

const char cstrHelp0[] = "/?";
const char cstrHelp1[] = "help";
const char cstrCls[] = "cls";
const char cstrExit[] = "exit";
const char cstrGetNv[] = "load_nv";

bool g_bExit = false;

void ClearScreen()
{
	system("cls");
}


static void OnStart()
{
	g_HIDCmdThread.TextOutRuntime(cstrTitleOut, 0);
}

bool OnCommand( char * cmd, bool * pbSysCmd )
{
	char * buf;
	char * file = NULL;

	if ( pbSysCmd )
		*pbSysCmd = TRUE;

	if ( strcmp(cmd, "") == 0 )
	{
		TRACE("no command");
	}
	else if ( strcmp(cmd, cstrHelp0) == 0 || strcmp(cmd, cstrHelp1) == 0 )
	{
		g_HIDCmdThread.TextOutRuntime(cstrHelpOut, 0);
	}
	else if( strcmp(cmd, cstrCls) == 0 )
	{
		ClearScreen();
	}
	else if( strcmp(cmd, cstrExit) == 0 )
	{
		g_bExit = TRUE;
		g_HIDCmdThread.TextOutRuntime(cstrExitOut, 0);
		return FALSE;
	}
	else if ( strstr(cmd, cstrGetNv) == cmd )
	{
		BOOL bFactoryNv = FALSE;

		buf = cmd + sizeof(cstrGetNv) - 1;
		while ( *buf != 0 )
		{
			if ( !_istspace(*buf) )
			{
				if ( *buf == '/' )
				{
					if ( *(++buf) == 'f' )
					{
						bFactoryNv = TRUE;
					}
					buf++;
					while ( TRUE )
					{
						if ( *buf != 0 )
						{
							buf--;
							break;
						}
						if ( _istspace(*buf) )
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
				if ( _istspace(*file) )
				{
					file++;
					continue;
				}
				else
					break;
			}
		}

		g_HIDCmdThread.GetNv(bFactoryNv, file);
	}
	else
	{
		if ( g_HIDCmdThread.SendCommand(cmd) )
		{
			if ( pbSysCmd )
				*pbSysCmd = FALSE;
		}
	}
	return TRUE;
}

#define INPUT_RECORD_SIZE 512
#define INPUT_BUFFER_SIZE 2048
void GetCommandPolling()
{
	HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
	if( hIn != INVALID_HANDLE_VALUE )
	{
		DWORD fdwSaveOldMode, fdwMode;
		GetConsoleMode(hIn, &fdwSaveOldMode);
		fdwMode = ENABLE_PROCESSED_INPUT | ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT; 
		SetConsoleMode(hIn, fdwMode);

		while ( 1 )
		{
			INPUT_RECORD ir[INPUT_RECORD_SIZE];
			DWORD dw;
			if ( !ReadConsoleInput(hIn, ir, INPUT_RECORD_SIZE, &dw) || dw <= 0 )
				continue;

			if ( g_bExit )
			{
				break;
			}

			if ( ir[0].EventType == KEY_EVENT &&
				ir[0].Event.KeyEvent.wVirtualKeyCode != VK_PROCESSKEY &&
				ir[0].Event.KeyEvent.wVirtualKeyCode != VK_SHIFT &&
				ir[0].Event.KeyEvent.wVirtualKeyCode != VK_CONTROL &&
				ir[0].Event.KeyEvent.wVirtualKeyCode != VK_MENU &&
				ir[0].Event.KeyEvent.wVirtualKeyCode != VK_NUMLOCK &&
				ir[0].Event.KeyEvent.wVirtualKeyCode != VK_SCROLL &&
				ir[0].Event.KeyEvent.wVirtualKeyCode != VK_CAPITAL )
			{
				DWORD dwWrite;
				WriteConsoleInput(hIn, ir, dw, &dwWrite);

				// stop output
				g_HIDCmdThread.LockTextOut();

				Sleep(100);
				printf(">");

				// get command line
				TCHAR szBuf[2048];
				TCHAR * sz = _getts(szBuf);
				size_t len = _tcslen(sz);
				for ( len--; len >= 0; len-- )
				{
					if ( _istspace(sz[len]) )
					{
						sz[len] = 0;
						continue;
					}
					break;
				}
				for ( size_t ni = 0; ni < len; ni++ )
				{
					if ( _istspace(*sz) )
					{
						sz++;
						continue;
					}
					break;
				}

				// start output
				g_HIDCmdThread.UnlockTextOut();

				BOOL bRet = OnCommand(sz);

				if ( !bRet )
					break;
			}
		}

		SetConsoleMode(hIn, fdwSaveOldMode);
	}
}

static BOOL WINAPI CtrlHandler( DWORD dwEvent )
{
	int i = 0;
	DWORD dwRet = 0;

	switch( dwEvent )
	{
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
	case CTRL_CLOSE_EVENT:
		SetConsoleCtrlHandler(NULL, TRUE);
		g_HIDCmdThread.Stop();
		exit( 0 );
		break;

	default:
		return(FALSE);
	}

	return(TRUE);
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	int ni = 1;
	for ( ; ni < argc; ni++ )
	{
		if ( strcmp(argv[ni], cstrHelp0) == 0 )
		{
			printf(cstrTitleOut);
			printf(cstrT3000Help);
			return 0;
		}
		else if ( strcmp(argv[ni], "/t") == 0 )
			g_HIDCmdThread.AddPrompt(CHIDCmdThread::promptT3000Time);
		else if ( strcmp(argv[ni], "/T") == 0 )
			g_HIDCmdThread.AddPrompt(CHIDCmdThread::promptSystemTime);
		else if ( strstr(argv[ni], "/c:") == argv[ni] )
		{
			char * pCmd = argv[ni] + 3;
			g_HIDCmdThread.AddPreCommand(pCmd);
		}
	}

	if( !SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE) ) 
	{
		return nRetCode;
	}

	OnStart();

	g_HIDCmdThread.Start();

	GetCommandPolling();

	g_HIDCmdThread.Stop();

	SetConsoleCtrlHandler(CtrlHandler, FALSE);
	SetConsoleCtrlHandler(NULL, FALSE);

	return nRetCode;
}
