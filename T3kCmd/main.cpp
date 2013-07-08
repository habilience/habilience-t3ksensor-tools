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
static int GetCommandPolling(void* pContext);
static int T3kLoop(void* pContext);
//bool CtrlHandler( unsigned long dwEvent );


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

    OnStart();

    QExFuncThread T3kThread( &a, T3kLoop, NULL );
    QExFuncThread CmdThread( &a, GetCommandPolling, NULL );

    a.connect( a.instance(), &QCoreApplication::aboutToQuit, &T3kThread, &QExFuncThread::onStop, Qt::DirectConnection );
    a.connect( a.instance(), &QCoreApplication::aboutToQuit, &CmdThread, &QExFuncThread::onStop, Qt::DirectConnection );

    T3kThread.connect( &T3kThread, &QThread::started, &CmdThread, &QExFuncThread::onStart );
    T3kThread.connect( &T3kThread, &QThread::finished, &CmdThread, &QExFuncThread::onExit );
    CmdThread.connect( &CmdThread, &QThread::finished, &T3kThread, &QExFuncThread::onStop );

    T3kThread.start();

    return a.exec();
}

static void OnStart()
{
    g_HIDCmd.TextOutRuntime(cstrTitleOut, 0);
}

int T3kLoop(void* pContext)
{
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
        Q_ASSERT( bRet );
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

    while ( !pSender->TerminateFlag() )
    {
        int n = kbhit();
        if( n == 0 )
        {
            QThread::msleep( 100 );
            continue;
        }

        g_HIDCmd.LockTextOut();
        printf(">");

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

        g_HIDCmd.UnlockTextOut();

        bool bRet = g_HIDCmd.OnCommand(sz);

        if ( !bRet )
            break;
    }

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
