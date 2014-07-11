#include "T3kSoftlogicDlg.h"

#include "qsingleapplication/qtsingleapplication.h"
#include "QUtils.h"
#include "QGUIUtils.h"
#include "../T3k_ver.h"
#include <QStringList>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#ifdef Q_OS_LINUX
#include <QFile>
#include <QProcess>
#endif

#define T3K_SOFTLOGIC_DIALOG_CLASSNAME			"Habilience T3k Softlogic Dialog"


bool g_bScreenShotMode = false;

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
    HWND hWnd = ::FindWindowA( T3K_SOFTLOGIC_DIALOG_CLASSNAME, NULL );
    if (hWnd)
    {
        ::ShowWindow(hWnd, SW_SHOWNORMAL);
        ::SetForegroundWindow(hWnd);

        return -1;
    }
#endif
    QtSingleApplication a(T3K_SOFTLOGIC_DIALOG_CLASSNAME, argc, argv);

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

    QString strAppVer( T3K_VERSION );
    int nRPos = -1;
    for( int i=strAppVer.length()-1 ; i>=3 ; i-=2 )
    {
        if( strAppVer.at(i-1) == '.' &&
            strAppVer.at(i) == '0' )
        {
            nRPos = i-1;
        }
        else
        {
            break;
        }
    }

    if( nRPos >= 0 )
        strAppVer = strAppVer.left(nRPos);

    QCoreApplication::setApplicationVersion( strAppVer );

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
    
    int nRet = a.exec();

#ifdef Q_OS_LINUX
    if( QFile::exists( "/etc/udev/rules.d/51-t3ksensors.rules" ) && bCreateRules )
        QFile::remove( "/etc/udev/rules.d/51-t3ksensors.rules" );
#endif

    return nRet;
}
