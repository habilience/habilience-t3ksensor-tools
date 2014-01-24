#include "dialog.h"
#include "qsingleapplication/qtsingleapplication.h"

const static QString UniqueID = "T3kUpgrade@Habilience";

#ifdef Q_OS_WIN
#include <windows.h>
#endif


int main(int argc, char *argv[])
{
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
    
    return a.exec();
}
