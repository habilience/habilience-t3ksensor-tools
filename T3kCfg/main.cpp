#include <QApplication>
#include "T3kCfgWnd.h"
#include <QSettings>
#include <QSharedMemory>
#include <QMessageBox>
#include "stdInclude.h"

// Tag
#include "T3k_ver.h"

#include "QLangManager.h"
#include "QT3kUserData.h"
#include "QConfigData.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif


bool g_bIsScreenShotMode = false;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    /* check screenshot mode */
    if ( argc > 1 )
    {
        if ( strcmp( argv[1], "/screenshot" ) == 0 )
        {
            g_bIsScreenShotMode = true;
        }
    }

#ifdef Q_OS_WIN
    HWND hOtherAppWnd = FindWindow( T3KFE_DIALOG_CLASSNAME, NULL );
    if( hOtherAppWnd )
    {
        ::MessageBox( NULL, L"T3k Factory-Edition Program(\"T3kCfe.exe\") is running.", L"Error", MB_OK );
        ::SetForegroundWindow( hOtherAppWnd );
        return 0;
    }
#endif

    QSharedMemory CheckDuplicateRuns( "Habilience T3k Series Configure" );
    if( CheckDuplicateRuns.isAttached() || CheckDuplicateRuns.attach( QSharedMemory::ReadWrite ) )
    {
        CheckDuplicateRuns.lock();
        void* pData = CheckDuplicateRuns.data();
        ST_SHAREDMEMORY* stSM = (ST_SHAREDMEMORY*)pData;

        if( stSM->szRunningFE == 1 )
        {
            QMessageBox::critical( NULL, "Error", "T3k Factory-Edition Program(\"T3kCfe.exe\") is running.", QMessageBox::Ok );
            CheckDuplicateRuns.unlock();
            return 0;
        }

        QString strPath = QCoreApplication::applicationDirPath();
        if( strPath.at( strPath.size()-1 ) == '/' )
            strPath.remove( strPath.size()-1, 1 );

        QSettings RegOption( "Habilience", "T3kCfg" );
        RegOption.beginGroup( "Options" );
        if( RegOption.value( "Exec Path", "" ).toString().compare( strPath ) )
        {
            RegOption.setValue( "TrayIcon", false );
            RegOption.setValue( "Exec Path", strPath );
            stSM->szDuplicateRuns = 2;
            CheckDuplicateRuns.unlock();
        }
        else
        {
            stSM->szDuplicateRuns = 1;
            CheckDuplicateRuns.unlock();
            return 0;
        }
    }
    else if( !CheckDuplicateRuns.create( sizeof(ST_SHAREDMEMORY) ) )
        return -1;

    QApplication::setQuitOnLastWindowClosed( false );
    QApplication::setWindowIcon( QIcon(":/T3kCfgRes/resources/T3kCfg.png") );

    QString strAppVer( T3000_VERSION );
    int nRPos = -1;
    //strAppVer = strAppVer.left( strAppVer.lastIndexOf( '.' ) );
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

    QString strPath = QCoreApplication::applicationDirPath();
    strPath += "/Languages/";

    QLangManager::instance()->setRootPath( strPath );
    QApplication::setLayoutDirection( QLangManager::instance()->getResource().isR2L() ? Qt::RightToLeft : Qt::LeftToRight );
    QConfigData::instance()->load( QCoreApplication::applicationDirPath() + "/config/config.ini" );

    T3kCfgWnd w;
    w.show();

    return a.exec();
}
