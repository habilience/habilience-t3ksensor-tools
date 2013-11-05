#include "dialog.h"
#include "qsingleapplication/qtsingleapplication.h"

const static QString UniqueID = "T3kUpgrade@Habilience";

int main(int argc, char *argv[])
{
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
