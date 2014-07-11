#include <QCoreApplication>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>

#include <QDebug>
#include <QString>

#include "HIDCmdThread.h"
#include "QExFuncThread.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <conio.h>
#else
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#endif

#include "DefineString.h"
#include "../T3k_ver.h"

#ifdef Q_OS_LINUX
#include <QFile>
#include <QProcess>
#endif

CHIDCmd  g_HIDCmd;

static void OnStart();
static int GetCommandPolling(void* pContext);
static int T3kLoop(void* pContext);
//bool CtrlHandler( unsigned long dwEvent );


int main(int argc, char *argv[])
{
#ifdef Q_OS_LINUX
    bool bCreateRules = false;
    if( !QFile::exists( "/etc/udev/rules.d/51-t3ksensors.rules" ) )
    {
        QFile fResource( ":/T3kCfgFERes/resources/51-t3ksensors.rules" );
        if( fResource.open( QIODevice::ReadOnly ) )
        {
            QFile fSaveFile( "/etc/udev/rules.d/51-t3ksensors.rules" );
            if( fSaveFile.open( QIODevice::WriteOnly ) )
            {
                fSaveFile.write( fResource.readAll() );
                fSaveFile.close();

                bCreateRules = true;
            }

            fResource.close();
        }

        QProcess::startDetached( "/etc/init.d/udev", QStringList("restart") );
    }
#endif
    QCoreApplication a(argc, argv);

    int ni = 1;
    for ( ; ni < argc; ni++ )
    {
        if ( strcmp(argv[ni], cstrHelp0) == 0 )
        {
            printf(QString(cstrTitleOut).arg(T3K_VERSION).toUtf8().data());
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

    int nRet = a.exec();

#ifdef Q_OS_LINUX
    if( QFile::exists( "/etc/udev/rules.d/51-t3ksensors.rules" ) && bCreateRules )
        QFile::remove( "/etc/udev/rules.d/51-t3ksensors.rules" );
#endif

    return nRet;
}

static void OnStart()
{
    g_HIDCmd.TextOutRuntime(QString(cstrTitleOut).arg(T3K_VERSION).toUtf8().data(), 0);
}

int T3kLoop(void* pContext)
{
    g_HIDCmd.InitT3k();

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

#ifndef Q_OS_WIN
int posix_kbhit(void)
{
    struct termios oldt, newt;
    int ch;

    tcgetattr( STDIN_FILENO, &oldt );
    newt = oldt;
    newt.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newt );

    ch = getchar();

    tcsetattr( STDIN_FILENO, TCSANOW, &oldt );

    return ch;
}
#endif


int GetCommandPolling(void* pContext)
{
    QExFuncThread* pSender = (QExFuncThread*)pContext;

    bool bInput = false;
    while ( !pSender->TerminateFlag() )
    {
#ifdef Q_OS_WIN
        int n = kbhit();
#else
        int n = posix_kbhit();
#endif
        if( n == 0 )
        {
            QThread::msleep( 100 );
            continue;
        }

        if( !bInput )
        {
            g_HIDCmd.LockTextOut();
            printf(">");
        }

        char szBuf[2048];
        char* sz = fgets(szBuf, 2048, stdin);
        if( sz[0] == 10 )
        {
            if( bInput )
            {
                bInput = false;
            }
            else
            {
                bInput = true;
                printf(">");
                continue;
            }
        }

        long len = strlen(sz);
        for ( len--; len >= 0; len-- )
        {
            if ( isspace(sz[len]) )
            {
                sz[len] = 0;
                continue;
            }
            break;
        }
        for ( long ni = 0; ni < len; ni++ )
        {
            if ( isspace(*sz) )
            {
                sz++;
                continue;
            }
            break;
        }

        bInput = false;
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
//        SetConsoleCtrlHandler(NULL, true);
//        g_HIDCmdThread.Stop();
//        exit( 0 );
//        break;

//    default:
//        return false ;
//    }

//    return true;
//}
