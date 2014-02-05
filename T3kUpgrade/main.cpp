#include "dialog.h"
#include "qsingleapplication/qtsingleapplication.h"

const static QString UniqueID = "T3kUpgrade@Habilience";

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#ifdef Q_OS_LINUX
#include <QFile>
#include <QProcess>
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
    HWND hWnd = ::FindWindowA( "Habilience T3k Downloader Dialog", NULL );
    if (hWnd)
    {
        ::ShowWindow(hWnd, SW_SHOWNORMAL);
        ::SetForegroundWindow(hWnd);

        return -1;
    }
#endif

    QtSingleApplication a(argc, argv);
    QApplication::setQuitOnLastWindowClosed( true );

    Dialog w;

    QObject::connect( &a, &QtSingleApplication::messageReceived, &w, &Dialog::onHandleMessage );

    if (a.isRunning())
    {
        a.sendMessage("");
        return 0;
    }

    w.show();
    
    int nRet = a.exec();

#ifdef Q_OS_LINUX
    if( QFile::exists( "/etc/udev/rules.d/51-t3ksensors.rules" ) && bCreateRules )
        QFile::remove( "/etc/udev/rules.d/51-t3ksensors.rules" );
#endif

    return nRet;
}
