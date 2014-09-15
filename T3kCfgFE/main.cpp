#include "dialog.h"
#include "QMyApplication.h"
#include "AppData.h"
#include "../common/T3kSMDef.h"

#include <QString>
#include <QSharedMemory>

AppData g_AppData;
QMyApplication* g_pApp = NULL;

#ifdef Q_OS_WIN
#include <windows.h>
#include "QShowMessageBox.h"
#endif

#ifdef Q_OS_LINUX
#include <QFile>
#include <QProcess>
#include <sys/unistd.h>
#endif

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
#ifdef Q_OS_WIN
    HWND hWnd = ::FindWindowA( "Habilience T3000 Factory-Edition Dialog", NULL );
    if (hWnd)
    {
        ::ShowWindow(hWnd, SW_SHOWNORMAL);
        ::SetForegroundWindow(hWnd);

        return -1;
    }
#endif
    g_AppData.bMaximizeToVirtualScreen = false;
    g_AppData.bScreenShotMode = false;
    g_AppData.bUpgradeFW = false;
    g_AppData.bDelayStart = false;

    if (argc > 1)
    {
        QString strArg;
        for (int c = 1; c < argc; c++)
        {
            strArg = argv[c];
            if (strArg.compare("/screenshot", Qt::CaseInsensitive) == 0)
                g_AppData.bScreenShotMode = true;
            if (strArg.compare("-screenshot", Qt::CaseInsensitive) == 0)
                g_AppData.bScreenShotMode = true;
            if (strArg.compare("--screenshot", Qt::CaseInsensitive) == 0)
                g_AppData.bScreenShotMode = true;

            if (strArg.compare("/virtualscreen", Qt::CaseInsensitive) == 0)
                g_AppData.bMaximizeToVirtualScreen = true;
            if (strArg.compare("-virtualscreen", Qt::CaseInsensitive) == 0)
                g_AppData.bMaximizeToVirtualScreen = true;
            if (strArg.compare("--virtualscreen", Qt::CaseInsensitive) == 0)
                g_AppData.bMaximizeToVirtualScreen = true;
            if (strArg.compare("/upgradeFW", Qt::CaseInsensitive) == 0)
                g_AppData.bUpgradeFW = true;
            if (strArg.compare("/delayStart", Qt::CaseInsensitive) == 0)
                g_AppData.bDelayStart = true;
        }
    }

#ifdef Q_OS_LINUX
    if( g_AppData.bDelayStart )
        sleep( 1 );
#endif

    QMyApplication a( "Habilience T3000 Factory-Edition Dialog", argc, argv );
    g_pApp = &a;
    Dialog w;

    QObject::connect( &a, &QtSingleApplication::messageReceived, &w, &Dialog::onHandleMessage );

    if (a.isRunning())
    {
        a.sendMessage("");
        return 0;
    }

    QSharedMemory CheckDuplicateRuns( T3K_SM_NAME );
    CheckDuplicateRuns.create( sizeof(T3K_SHAREDMEMORY) );
    if( CheckDuplicateRuns.isAttached() || CheckDuplicateRuns.attach( QSharedMemory::ReadWrite ) )
    {
        CheckDuplicateRuns.lock();
        T3K_SHAREDMEMORY* stSM = (T3K_SHAREDMEMORY*)CheckDuplicateRuns.data();

        stSM->szRunningFE = 1;
        CheckDuplicateRuns.unlock();
    }

    w.show();

    int nExit = a.exec();

    if( CheckDuplicateRuns.isAttached() || CheckDuplicateRuns.attach( QSharedMemory::ReadWrite ) )
    {
        CheckDuplicateRuns.lock();
        T3K_SHAREDMEMORY* stSM = (T3K_SHAREDMEMORY*)CheckDuplicateRuns.data();

        stSM->szRunningFE = 0;
        CheckDuplicateRuns.unlock();
    }

#ifdef Q_OS_LINUX
    if( QFile::exists( "/etc/udev/rules.d/51-t3ksensors.rules" ) && bCreateRules )
        QFile::remove( "/etc/udev/rules.d/51-t3ksensors.rules" );
#endif

    return nExit;
}
