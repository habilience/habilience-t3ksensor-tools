#include "dialog.h"
#include "QMyApplication.h"
#include "AppData.h"

#include <QString>
#include <QSharedMemory>

AppData g_AppData;
QMyApplication* g_pApp = NULL;

#ifdef Q_OS_WIN
#include <windows.h>
#include "QShowMessageBox.h"
#endif

int main(int argc, char *argv[])
{
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
        }
    }

    QMyApplication a(argc, argv);
    g_pApp = &a;
    Dialog w;

    QObject::connect( &a, &QtSingleApplication::messageReceived, &w, &Dialog::onHandleMessage );

    if (a.isRunning())
    {
        a.sendMessage("");
        return 0;
    }

    typedef struct _ST_SHAREDMEMORY
    {
        char szDuplicateRuns;
        char szRunningFE;
    } ST_SHAREDMEMORY;

    QSharedMemory CheckDuplicateRuns( "Habilience T3k Series Configure" );
    CheckDuplicateRuns.create( sizeof(ST_SHAREDMEMORY) );
    if( CheckDuplicateRuns.isAttached() || CheckDuplicateRuns.attach( QSharedMemory::ReadWrite ) )
    {
        CheckDuplicateRuns.lock();
        void* pData = CheckDuplicateRuns.data();
        ST_SHAREDMEMORY* stSM = (ST_SHAREDMEMORY*)pData;

        stSM->szRunningFE = 1;
        CheckDuplicateRuns.unlock();
    }

    w.show();

    int nExit = a.exec();
    
    if( CheckDuplicateRuns.isAttached() || CheckDuplicateRuns.attach( QSharedMemory::ReadWrite ) )
    {
        CheckDuplicateRuns.lock();
        void* pData = CheckDuplicateRuns.data();
        ST_SHAREDMEMORY* stSM = (ST_SHAREDMEMORY*)pData;

        stSM->szRunningFE = 0;
        CheckDuplicateRuns.unlock();
    }

    return nExit;
}
