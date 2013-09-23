#include "T3kSoftlogicDlg.h"

#include "qsingleapplication/qtsingleapplication.h"
#include "QUtils.h"
#include <QStringList>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#define T3K_SOFTLOGIC_DIALOG_CLASSNAME			"Habilience T3k Softlogic Dialog"

#include <QMessageBox>
bool g_bScreenShotMode = false;
#include <QVector>
int main(int argc, char *argv[])
{
    QtSingleApplication a(argc, argv);    

    int nActiveTab = -1;
    QString strExecute;
    QString strCmdLine;
    if( argc > 1 )
    {
        for( int i=1 ;i<argc; i++ )
        {
            strCmdLine += argv[i];
            strCmdLine += " ";
        }

        strCmdLine = strCmdLine.trimmed();

        if( strCmdLine.contains("/e") )
        {
            int nIdx = strCmdLine.indexOf( "/tab" );
            if ( nIdx >= 0 && nIdx+5 < strCmdLine.size() )
                nActiveTab = strCmdLine.mid( nIdx + 5 ).toInt();

            nIdx = strCmdLine.indexOf("/exe");
            if ( nIdx >= 0 && nIdx+5 < strCmdLine.size() )
                strExecute = strCmdLine.mid( nIdx + 5 ).trimmed();
        }
        else
        {
            if (a.isRunning()) {
                a.sendMessage(strCmdLine);
                return 0;
            }
        }

        if ( strCmdLine.contains("/screenshot") )
            g_bScreenShotMode = true;
    }

    QApplication::setQuitOnLastWindowClosed( true );

    T3kSoftlogicDlg dlg( NULL, strCmdLine.isEmpty() ? "" : strCmdLine );

    dlg.setFont( getSystemFont(NULL) );

    QObject::connect( &a, &QtSingleApplication::messageReceived, &dlg, &T3kSoftlogicDlg::onHandleMessage );

    if ( !strExecute.isEmpty() )
    {
        dlg.doExecute( strExecute );
        return 0;
    }

    if (a.isRunning()) {
        a.sendMessage(strCmdLine);
        return 0;
    }

    dlg.setActiveTab( nActiveTab );

    dlg.show();
    
    return a.exec();
}
