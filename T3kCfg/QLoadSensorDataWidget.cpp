#include "QLoadSensorDataWidget.h"
#include "ui_QLoadSensorDataWidget.h"

#include "QWidgetCloseEventManager.h"

#include "QPrintLogData.h"

#include "QCompressToZip.h"

#include "QT3kLoadSideviewObject.h"
#include "QT3kLoadDetectionObject.h"
#include "QT3kLoadSensorDataObject.h"
#include "QT3kLoadEnvironmentObject.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#include <QDesktopWidget>
#include <QtEvents>

#include <QDir>
#include <QProcess>
#include <QDesktopServices>

#define ZIP_STATEREPORT_NAME   "T3kStateReport"

//static const char cstrSytaxError[] = "systax error";


QLoadSensorDataWidget::QLoadSensorDataWidget(T3kHandle*& pHandle, QWidget *parent) :
    QDialog(parent), ui(new Ui::QLoadSensorDataWidget), m_pT3kHandle(pHandle)
{
    setWindowFlags( Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint );

    ui->setupUi(this);

    setFont( parent->font() );

    m_nTimerLoadStep        = 0;
    m_nLoadProgStep         = LDS_SIDEVIEW;

    m_pHIDObject            = NULL;

    m_pSensorLogData        = NULL;

    m_bSaveError            = false;

    setFocusPolicy( Qt::StrongFocus );

    OnChangeLanguage();
}

QLoadSensorDataWidget::~QLoadSensorDataWidget()
{
    CloseWidget();
}

void QLoadSensorDataWidget::MoveWidgetToCenter()
{
    QDesktopWidget DeskWidget;
    int nPrimary = DeskWidget.primaryScreen();
    const QRect rcPrimaryMon = DeskWidget.screenGeometry( nPrimary );
    QPoint ptCenter( rcPrimaryMon.center() );
    ptCenter.setX( ptCenter.x() - width()/2 );
    ptCenter.setY( ptCenter.y() - height()/2 );
    move( ptCenter );

#ifdef Q_OS_WIN
    SetWindowPos( (HWND)winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE );
    SetForegroundWindow( (HWND)winId() );
#else
    raise();
    activateWindow();
#endif
}

void QLoadSensorDataWidget::CreateSavePath()
{
    QDir tempDir;
    QString strPath = QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation )
            + "/" + ZIP_STATEREPORT_NAME;

    if( !tempDir.exists( strPath ) )
        tempDir.mkpath( strPath );
    if( tempDir.exists( strPath + "/data" ) )
        RemoveDirectoryR( strPath + "/data" );

    tempDir.mkdir( strPath + "/data" );

    m_strSavePath = strPath;
}

void QLoadSensorDataWidget::ResetStorage()
{
    if( m_pSensorLogData )
    {
        foreach(CMLogDataGroup* pData, m_pSensorLogData->CM)
        {
            if( pData )
            {
                delete pData;
                pData = NULL;
            }
        }

        delete m_pSensorLogData;
        m_pSensorLogData = NULL;
    }

    m_pSensorLogData = new SensorLogData;
    m_pSensorLogData->CM.clear();
}

void QLoadSensorDataWidget::Start()
{
    QTPDPEventMultiCaster::GetPtr()->SetSingleListener( this );

    ResetStorage();

    MoveWidgetToCenter();
    CreateSavePath();

    if( !m_pT3kHandle->GetReportCommand() )
        m_pT3kHandle->SetReportCommand( true );

    on_InsertProgressLog( "[" + QLangManager::GetPtr()->GetResource().GetResString(QString::fromUtf8("SENSOR DIAGNOSIS"), QString::fromUtf8("LOG_START")) + "]" );

    ui->LBProgressingIcon->Start();

    m_nLoadProgStep = LDS_SIDEVIEW;
    m_nTimerLoadStep = startTimer( 1 );
}

void QLoadSensorDataWidget::CloseWidget()
{
    ClearProgressLog();

    if( m_nTimerLoadStep )
    {
        killTimer( m_nTimerLoadStep );
        m_nTimerLoadStep = 0;
    }

    m_pT3kHandle->SendCommand( (const char*)"cam1/mode=detection", true );
    m_pT3kHandle->SendCommand( (const char*)"cam2/mode=detection", true );

    if( !m_pT3kHandle->GetReportCommand() )
        m_pT3kHandle->SetReportCommand( true );
    if( m_pT3kHandle->GetReportView() )
        m_pT3kHandle->SetReportView( false );
    if( m_pT3kHandle->GetReportDevice() )
        m_pT3kHandle->SetReportDevice( false );

    ResetStorage();
}

void QLoadSensorDataWidget::OnChangeLanguage()
{
    if( !winId() ) return;

    QLangRes& Res = QLangManager::GetPtr()->GetResource();
    setWindowTitle( Res.GetResString(QString::fromUtf8("SENSOR DIAGNOSIS"), QString::fromUtf8("BTN_CAPTION_SAVELOG")) );
    ui->BTOK->setText( Res.GetResString(QString::fromUtf8("EDIT PROFILE ITEM"),QString::fromUtf8("BTN_CAPTION_CANCEL")) );
}

void QLoadSensorDataWidget::on_InsertProgressLog(QString strMessage, bool bError/*=false*/)
{
    QListWidgetItem* pItem = new QListWidgetItem();
    pItem->setText( strMessage );
    if( bError )
        pItem->setForeground( QBrush( Qt::red ) );

    ui->LWProgressLog->addItem( pItem );

    ui->LWProgressLog->setCurrentRow( ui->LWProgressLog->count()-1 );
}

void QLoadSensorDataWidget::ClearProgressLog()
{
    for( int i=0; i<ui->LWProgressLog->count(); i++ )
    {
        QListWidgetItem* pItem = ui->LWProgressLog->item( i );
        if( pItem )
            delete pItem;
    }

    ui->LWProgressLog->clear();
}

void QLoadSensorDataWidget::timerEvent(QTimerEvent *evt)
{
    if( evt->timerId() == m_nTimerLoadStep )
    {
        killTimer( m_nTimerLoadStep );
        m_nTimerLoadStep = 0;

        switch( m_nLoadProgStep )
        {
        case LDS_SIDEVIEW:
            {
                QString strLog( "[" + QLangManager::GetResUTF8String("SENSOR DIAGNOSIS", "LOG_PROGRESS") + "] " );
                strLog += QLangManager::GetResUTF8String( "SENSOR DIAGNOSIS", "LOG_SIDEVIEW" );
                on_InsertProgressLog( strLog );

                if( !m_pT3kHandle->GetReportCommand() )
                    m_pT3kHandle->SetReportCommand( true );
                LoadSideView();
            }
            break;
        case LDS_DETECTION:
            {
                QString strLog( "[" + QLangManager::GetResUTF8String( "SENSOR DIAGNOSIS", "LOG_PROGRESS" ) + "] " );
                strLog += QLangManager::GetResUTF8String( "SENSOR DIAGNOSIS", "LOG_DETECTION" );
                on_InsertProgressLog( strLog );

                if( !m_pT3kHandle->GetReportCommand() )
                    m_pT3kHandle->SetReportCommand( true );
                LoadDetection();
            }
            break;
        case LDS_SENSORDATA:
            {
                QString strLog( "[" + QLangManager::GetResUTF8String( "SENSOR DIAGNOSIS", "LOG_PROGRESS" ) + "] " );
                strLog += QLangManager::GetResUTF8String( "SENSOR DIAGNOSIS", "LOG_DATA" );
                on_InsertProgressLog( strLog );

                if( !m_pT3kHandle->GetReportCommand() )
                    m_pT3kHandle->SetReportCommand( true );
                LoadData();
            }
            break;
        case LDS_ENVIRONMENT:
            {
                LoadEnvironment();
            }
            break;
        case LDS_SAVE:
            {
                QString strLog( "[" + QLangManager::GetResUTF8String( "SENSOR DIAGNOSIS", "LOG_PROGRESS" ) + "] " );
                strLog += QLangManager::GetResUTF8String( "SENSOR DIAGNOSIS", "LOG_SAVING" );
                on_InsertProgressLog( strLog );

                if( SaveLogToFile() && !m_bSaveError )
                    on_BTOK_clicked();

                ui->LBProgressingIcon->Complete();
            }
            break;
        default:
            Q_ASSERT( false );
            // exit error
            break;
        }
    }

    QDialog::timerEvent(evt);
}

void QLoadSensorDataWidget::showEvent(QShowEvent *)
{
    Start();
}

void QLoadSensorDataWidget::hideEvent(QHideEvent *)
{
    CloseWidget();
}

void QLoadSensorDataWidget::closeEvent(QCloseEvent *)
{
    CloseWidget();
}

void QLoadSensorDataWidget::keyPressEvent(QKeyEvent *evt)
{
    if( evt->type() == QEvent::KeyPress )
    {
        if( evt->key() == Qt::Key_Escape || evt->key() == Qt::Key_Control || evt->key() == Qt::Key_Meta || evt->key() == Qt::Key_Alt )
        {
            return;
        }
    }

    QDialog::keyPressEvent(evt);
}

void QLoadSensorDataWidget::LoadSideView()
{
    if( m_pHIDObject )
    {
        delete m_pHIDObject;
        m_pHIDObject = NULL;
    }
    m_pHIDObject = new QT3kLoadSideviewObject( m_pT3kHandle, this );
    connect( m_pHIDObject, SIGNAL(Complete(int)), this, SLOT(onLoadComplete(int)), Qt::QueuedConnection );
    connect( m_pHIDObject, SIGNAL(PrintProgreeLog(QString,bool)), this, SLOT(on_InsertProgressLog(QString,bool)), Qt::QueuedConnection );

    QT3kLoadSideviewObject* pTempObj = (QT3kLoadSideviewObject*)m_pHIDObject;
    pTempObj->SetSavePath( m_strSavePath + "/data" );

    QTPDPEventMultiCaster::GetPtr()->SetSingleListener( m_pHIDObject );
    m_pHIDObject->Start( m_pSensorLogData );
}

void QLoadSensorDataWidget::LoadDetection()
{
    if( m_pHIDObject )
    {
        delete m_pHIDObject;
        m_pHIDObject = NULL;
    }
    m_pHIDObject = new QT3kLoadDetectionObject( m_pT3kHandle, this );
    connect( m_pHIDObject, SIGNAL(Complete(int)), this, SLOT(onLoadComplete(int)), Qt::QueuedConnection );
    connect( m_pHIDObject, SIGNAL(PrintProgreeLog(QString,bool)), this, SLOT(on_InsertProgressLog(QString,bool)), Qt::QueuedConnection );

    QTPDPEventMultiCaster::GetPtr()->SetSingleListener( m_pHIDObject );
    m_pHIDObject->Start( m_pSensorLogData );
}

void QLoadSensorDataWidget::LoadData()
{
    if( m_pHIDObject )
    {
        delete m_pHIDObject;
        m_pHIDObject = NULL;
    }
    m_pHIDObject = new QT3kLoadSensorDataObject( m_pT3kHandle, this );
    connect( m_pHIDObject, SIGNAL(Complete(int)), this, SLOT(onLoadComplete(int)), Qt::QueuedConnection );
    connect( m_pHIDObject, SIGNAL(PrintProgreeLog(QString,bool)), this, SLOT(on_InsertProgressLog(QString,bool)), Qt::QueuedConnection );

    QTPDPEventMultiCaster::GetPtr()->SetSingleListener( m_pHIDObject );
    m_pHIDObject->Start( m_pSensorLogData );
}

void QLoadSensorDataWidget::LoadEnvironment()
{
#ifdef _DEBUG
    bool bRet =
#endif
    QT3kLoadEnvironmentObject::Start( m_pSensorLogData );

#ifdef _DEBUG
    Q_ASSERT( bRet );
#endif

    onLoadComplete( LDS_ENVIRONMENT );
}

void QLoadSensorDataWidget::onLoadComplete(int nStep)
{
    if( nStep > LDS_SAVE ) return;

    QTPDPEventMultiCaster::GetPtr()->SetSingleListener( this );
    if( m_pHIDObject )
    {
        delete m_pHIDObject;
        m_pHIDObject = NULL;
    }

    m_nLoadProgStep++;
    m_nTimerLoadStep = startTimer( 1 );
}

bool QLoadSensorDataWidget::SaveLogToFile()
{
    bool bRet = false;
    QPrintLogData Print;
    if( Print.Open( m_strSavePath + "/data/T3k.dat", QIODevice::WriteOnly ) )
    {
        if( Print.Print( m_pSensorLogData ) )
        {
            Print.close();

            // StateReport html file
            ExportReportFiles( m_strSavePath );
            //

            QString strFELog( GetLastFELogFileNamePath() );
            QFile::copy( strFELog, m_strSavePath+ "/data/" + strFELog.right( strFELog.length() - strFELog.lastIndexOf('\\') ) );

            //m_strSavePath = m_strSavePath.left( m_strSavePath.lastIndexOf( '/' ) );

            QString strZipFileName( ZIP_STATEREPORT_NAME );
            QDateTime CurrDatTime( QDateTime::currentDateTime() );
            strZipFileName += QString("_%1_%2_%3").
                              arg(CurrDatTime.date().year()).arg(CurrDatTime.date().month()).arg(CurrDatTime.date().day())
                              + ".zip";

            QCompressToZip CompressZip;
            QString strOutputZipPath;
            int nPos = m_strSavePath.lastIndexOf( '/' );
            if( nPos > 0 )
            {
                strOutputZipPath = m_strSavePath.left( nPos );
            }

            if( CompressZip.CompressFolderAll( strOutputZipPath, strZipFileName, m_strSavePath, true/*, "habilience"*/ ) )
            {
                QString strDeskTopPath( QStandardPaths::writableLocation( QStandardPaths::DesktopLocation ) );
#if defined(Q_OS_MAC)
                int nPos = strOutputZipPath.lastIndexOf( "/T3kCfg.app" );
                if( nPos > 0 )
                {
                    strOutputZipPath = strOutputZipPath.left( nPos );
                }

                if( strOutputZipPath != strDeskTopPath )
                {
#endif
                QFile::remove( strDeskTopPath + "/" + strZipFileName );
                QFile::copy( strOutputZipPath + "/" + strZipFileName, strDeskTopPath + "/" + strZipFileName );
                QFile::remove( strOutputZipPath + "/" + strZipFileName );
#if defined(Q_OS_MAC)
                }
#endif
#if defined(Q_OS_WIN)
                QStringList args;
                args << "/select," << QDir::toNativeSeparators( strDeskTopPath + "/" + strZipFileName );
                QProcess::startDetached( "explorer", args );
#elif defined(Q_OS_LINUX)
                QStringList args;
                args << strDeskTopPath;
                QProcess::startDetached( "xdg-open", args );
#elif defined(Q_OS_MAC)
                QStringList args;
                args << "-e";
                args << "tell application \"Finder\"";
                args << "-e";
                args << "activate";
                args << "-e";
                args << "select POSIX file \"" + strDeskTopPath + "/" + strZipFileName + "\"";
                args << "-e";
                args << "end tell";
                QProcess::startDetached( "osascript", args );
#endif
                bRet = true;

                QString strLog( "[" + QLangManager::GetPtr()->GetResource().GetResString(QString::fromUtf8("SENSOR DIAGNOSIS"), QString::fromUtf8("LOG_COMPLETE")) + "] " );
                on_InsertProgressLog( strLog );
            }
            else
            {
                QString strLog( "[" + QLangManager::GetPtr()->GetResource().GetResString(QString::fromUtf8("SENSOR DIAGNOSIS"), QString::fromUtf8("LOG_FAIL")) + "] " );
                strLog += "2";
                on_InsertProgressLog( strLog, true );

                m_bSaveError = true;
            }
        }
        else
        {
            Print.close();

            QString strLog( "[" + QLangManager::GetPtr()->GetResource().GetResString(QString::fromUtf8("SENSOR DIAGNOSIS"), QString::fromUtf8("LOG_FAIL")) + "] " );
            strLog += "1";
            on_InsertProgressLog( strLog, true );

            m_bSaveError = true;
        }
    }

    if( bRet )
    {
        ui->BTOK->setText( QLangManager::GetPtr()->GetResource().GetResString(QString::fromUtf8("SENSOR DIAGNOSIS"), QString::fromUtf8("LOG_OK")) );
    }

    return bRet;
}

bool QLoadSensorDataWidget::ExportReportFiles(QString strOutPutPath)
{
    if( !QDir().exists( strOutPutPath ) )
        QDir().mkdir( strOutPutPath );

    QDir dirResStateReport( ":/T3kCfgRes/T3kStateReport/" );
    QStringList strUsableExtensionList;
    strUsableExtensionList << "*.*";

    dirResStateReport.setNameFilters( strUsableExtensionList );
    dirResStateReport.setFilter( QDir::Files );

    QStringList strLogoFileList = dirResStateReport.entryList();

    foreach( QString str, strLogoFileList )
    {
        //if( QFile::exists( strOutPutPath + "/" + str ) ) continue;
        QFile fResource( ":/T3kCfgRes/T3kStateReport/" + str );
        if( fResource.open( QIODevice::ReadOnly ) )
        {
            QFile fSaveFile( strOutPutPath + "/" + str );
            if( fSaveFile.open( QIODevice::WriteOnly ) )
            {
                fSaveFile.write( fResource.readAll() );
                fSaveFile.close();
            }

            fResource.close();
        }
    }

    return true;
}

void QLoadSensorDataWidget::on_BTOK_clicked()
{
    CloseWidget();
    QTPDPEventMultiCaster::GetPtr()->ClearSingleListener();

    if( ui->BTOK->text() == QLangManager::GetPtr()->GetResource().GetResString(QString::fromUtf8("EDIT PROFILE ITEM"),QString::fromUtf8("BTN_CAPTION_CANCEL")) )
    {
        RemoveDirectoryR( m_strSavePath + "/data" );
    }

    QDialog::accept();
}

QString QLoadSensorDataWidget::GetLastFELogFileNamePath()
{
    QString strPath = QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation );
    strPath += "/T3kCfgFE/Logs/";

    QDir dir( strPath );
    QStringList strUsableExtensionList;
    strUsableExtensionList << "*.txt";

    dir.setNameFilters( strUsableExtensionList );
    dir.setFilter( QDir::Files );

    QStringList strLogoFileList = dir.entryList();

    if( strLogoFileList.count() )
    {
        QString str = strLogoFileList.at(0);
        for( int i=1;i<strLogoFileList.count();i++ )
        {
            if( str.compare( strLogoFileList.at(i) ) < 0 )
            {
                str = strLogoFileList.at(i);
            }
        }
        return strPath+str;
    }

    return "";
}

bool QLoadSensorDataWidget::RemoveDirectoryR( QString strPath )
{
    QDir dir( strPath );

    bool bRet = dir.exists();
    if( bRet )
    {
        QFileInfoList entries = dir.entryInfoList( QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files );
        foreach( QFileInfo entryInfo, entries )
        {
            QString path( entryInfo.absoluteFilePath() );
            if( entryInfo.isDir() )
            {
                if( !RemoveDirectoryR( path ) )
                {
                    bRet = false;
                    break;
                }
            }
            else
            {
                QFile file( path );
                if( !file.remove() )
                {
                    qDebug( "%s", file.errorString().toUtf8().data() );
                    bRet = false;
                    break;
                }
            }
        }
    }

    if( bRet && !dir.rmdir( dir.absolutePath() ) )
        bRet = false;

    return bRet;
}
