#include <QCoreApplication>
#include <QtConcurrent/QtConcurrent>

#include "HIDCmdThread.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <conio.h>
#endif

#include "DefineString.h"



CHIDCmdThread  g_HIDCmdThread;
QEventLoop loop;
bool g_bExit = false;

static void OnStart();
void GetCommandPolling();
bool CtrlHandler( unsigned long dwEvent );


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    g_HIDCmdThread.loop = &loop;
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
            g_HIDCmdThread.AddPrompt(CHIDCmdThread::promptT3kTime);
        else if ( strcmp(argv[ni], "/T") == 0 )
            g_HIDCmdThread.AddPrompt(CHIDCmdThread::promptSystemTime);
        else if ( strstr(argv[ni], "/c:") == argv[ni] )
        {
            char * pCmd = argv[ni] + 3;
            g_HIDCmdThread.AddPreCommand(pCmd);
        }
    }

    //QObject::connect( &a, SIGNAL(aboutToQuit(QPrivateSignal)), &g_HIDCmdThread, SLOT(onStop(QPrivateSignal)) );

    OnStart();

    g_HIDCmdThread.Start();

    QFuture<void> ft = QtConcurrent::run( GetCommandPolling );

    GetCommandPolling();

    g_HIDCmdThread.Stop();

    //QObject::disconnect(qApp, SIGNAL(aboutToQuit(QPrivateSignal));

    a.exec();

    return nRetCode;
}

static void OnStart()
{
    g_HIDCmdThread.TextOutRuntime(cstrTitleOut, 0);
}

#define INPUT_RECORD_SIZE 512
#define INPUT_BUFFER_SIZE 2048
void GetCommandPolling()
{
#ifdef Q_OS_WIN
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
                char szBuf[2048];
                char* sz = gets(szBuf);
                size_t len = strlen(sz);
                for ( len--; len >= 0; len-- )
                {
                    if ( isspace(sz[len]) )
                    {
                        sz[len] = 0;
                        continue;
                    }
                    break;
                }
                for ( size_t ni = 0; ni < len; ni++ )
                {
                    if ( isspace(*sz) )
                    {
                        sz++;
                        continue;
                    }
                    break;
                }

                // start output
                g_HIDCmdThread.UnlockTextOut();

                BOOL bRet = g_HIDCmdThread.OnCommand(sz);

                if ( !bRet )
                    break;
            }
        }

        SetConsoleMode(hIn, fdwSaveOldMode);
    }
#else

#endif
}

//static bool __stdcall CtrlHandler( unsigned long dwEvent )
//{
//    switch( dwEvent )
//    {
//    case CTRL_C_EVENT:
//    case CTRL_BREAK_EVENT:
//    case CTRL_LOGOFF_EVENT:
//    case CTRL_SHUTDOWN_EVENT:
//    case CTRL_CLOSE_EVENT:
//        SetConsoleCtrlHandler(NULL, TRUE);
//        g_HIDCmdThread.Stop();
//        exit( 0 );
//        break;

//    default:
//        return false ;
//    }

//    return true;
//}
