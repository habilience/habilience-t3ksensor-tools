#include <QCoreApplication>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>

#include "HIDCmdThread.h"
#include "QExFuncThread.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <conio.h>
#endif

#include "DefineString.h"



CHIDCmd  g_HIDCmd;

static void OnStart();
int GetCommandPolling(void* pContext);
int T3kLoop(void* pContext);
bool CtrlHandler( unsigned long dwEvent );


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

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
            g_HIDCmd.AddPrompt(CHIDCmd::promptT3kTime);
        else if ( strcmp(argv[ni], "/T") == 0 )
            g_HIDCmd.AddPrompt(CHIDCmd::promptSystemTime);
        else if ( strstr(argv[ni], "/c:") == argv[ni] )
        {
            char * pCmd = argv[ni] + 3;
            g_HIDCmd.AddPreCommand(pCmd);
        }
    }

    QExFuncThread T3kThread( &a, T3kLoop, NULL );
    QExFuncThread CmdThread( &a, GetCommandPolling, NULL );

    a.connect( a.instance(), &QCoreApplication::aboutToQuit, &T3kThread, &QExFuncThread::onStop, Qt::DirectConnection );
    a.connect( a.instance(), &QCoreApplication::aboutToQuit, &CmdThread, &QExFuncThread::onStop, Qt::DirectConnection );

    T3kThread.connect( &T3kThread, &QThread::started, &CmdThread, &QExFuncThread::onStart );
    //T3kThread.connect( &T3kThread, &QThread::finished, &CmdThread, &QExFuncThread::onStop );
    T3kThread.connect( &T3kThread, &QThread::finished, &CmdThread, &QExFuncThread::onExit );
    CmdThread.connect( &CmdThread, &QThread::finished, &T3kThread, &QExFuncThread::onStop );
    //CmdThread.connect( &CmdThread, &QThread::finished, &T3kThread, &QExFuncThread::onExit );
    T3kThread.start();
    //CmdThread.start();

    //QFuture<void> ftT3k = QtConcurrent::run( T3kLoop );
    //QFuture<void> ftCmd = QtConcurrent::run( GetCommandPolling );
    //g_HIDCmd.SetFutureHandle( ft );


    //QObject::connect( &a, &QCoreApplication::aboutToQuit, g_HIDCmd, &CHIDCmd::onStop, Qt::DirectConnection );

    //QObject::disconnect(qApp, SIGNAL(aboutToQuit(QPrivateSignal));
    return a.exec();
}

static void OnStart()
{
    g_HIDCmd.TextOutRuntime(cstrTitleOut, 0);
}

int T3kLoop(void* pContext)
{
    OnStart();

    g_HIDCmd.InitT3k();
    g_HIDCmd.ProcessT3k();

    QExFuncThread* pSender = (QExFuncThread*)pContext;

    while( !pSender->TerminateFlag() )
    {
#ifdef _DEBUG
        bool bRet =
#endif
        g_HIDCmd.ProcessT3k();
#ifdef _DEBUG
        //Q_ASSERT( bRet );
#endif
        QThread::msleep( 1 );
    }

    g_HIDCmd.EndT3k();

    return 0;
}

#define INPUT_RECORD_SIZE 512
#define INPUT_BUFFER_SIZE 2048
int GetCommandPolling(void* pContext)
{
    QExFuncThread* pSender = (QExFuncThread*)pContext;
#ifdef Q_OS_WIN
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    if( hIn != INVALID_HANDLE_VALUE )
    {
        DWORD fdwSaveOldMode, fdwMode;
        GetConsoleMode(hIn, &fdwSaveOldMode);
        fdwMode = ENABLE_PROCESSED_INPUT | ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT;
        SetConsoleMode(hIn, fdwMode);

        while ( !pSender->TerminateFlag() )
        {
            if( WaitForSingleObject( hIn, 100 ) != WAIT_OBJECT_0 )
            {
                continue;
            }

            INPUT_RECORD ir[INPUT_RECORD_SIZE];
            DWORD dw;
            if ( !ReadConsoleInput(hIn, ir, INPUT_RECORD_SIZE, &dw) || dw <= 0 )
                continue;

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
                g_HIDCmd.LockTextOut();

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
                g_HIDCmd.UnlockTextOut();

                BOOL bRet = g_HIDCmd.OnCommand(sz);

                if ( !bRet )
                    break;
            }
        }

        SetConsoleMode(hIn, fdwSaveOldMode);
    }
#else

#endif

    return 0;
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
