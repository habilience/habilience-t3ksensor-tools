#include "QSensorInitDataCfg.h"
#include <QFileInfo>
#include <QSettings>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include "QUtils.h"
#include "conf.h"
#include <QStandardPaths>

QSensorInitDataCfg::_gc::_gc() {}
QSensorInitDataCfg::_gc::~_gc()
{
    if (QSensorInitDataCfg::m_pThis)
        delete QSensorInitDataCfg::m_pThis;
    QSensorInitDataCfg::m_pThis = NULL;
}
static QSensorInitDataCfg::_gc s_gc;

void QSensorInitDataCfg::Enumerator::enumCfgData()
{
    m_aryCfgData.clear();

#ifndef CREATE_FILE_TO_DOCUMENTS_LOCATION
    QString strDir = QCoreApplication::applicationDirPath();
    strDir = rstrip(strDir, "/\\");
    QString strPath = strDir + QDir::separator() +"SensorData" + QDir::separator();
#else
    QString strDocuments = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString strDir = rstrip( strDocuments, "/\\" );
    QString strPath = strDir + QDir::separator() + "T3kCfgFE" + QDir::separator() + "SensorData" + QDir::separator();
#endif

    QDir currentDir(strPath);
    QStringList files;
    QString fileName = "*.cfg";
    files = currentDir.entryList(QStringList(fileName),
                                 QDir::Files | QDir::NoSymLinks);

    QString strFileName;
    QString strPathName;
    int nPos = 0;
    stCfgData cfgData;
    for (int i=0 ; i<files.size() ; i++)
    {
        strFileName = files.at(i);
        nPos = strFileName.lastIndexOf( '.' );
        if ( nPos < 0 ) continue;
        strFileName = strFileName.left( nPos );
        qDebug( "%s", (const char*)strFileName.toUtf8() );
        cfgData.strFileName = strFileName;

        strPathName = strPath + strFileName + ".cfg";
        QFileInfo fileInfo(strPathName);
        cfgData.tmDate = fileInfo.lastModified();

        m_aryCfgData.push_back(cfgData);
    }
}

bool QSensorInitDataCfg::Enumerator::findCfgData(const QString &strFileName )
{
    for ( int i=0 ; i<m_aryCfgData.size() ; i++ )
    {
        const stCfgData & cfgData = m_aryCfgData.at(i);
        if (cfgData.strFileName.compare(strFileName, Qt::CaseInsensitive) == 0)
            return true;
    }
    return false;
}

int QSensorInitDataCfg::Enumerator::getCfgDataCount()
{
    return (int)m_aryCfgData.size();
}

bool QSensorInitDataCfg::Enumerator::getCfgDataAt( int nIndex, QString& strFileName, QDateTime& tmDate )
{
    if ( m_aryCfgData.size() <= nIndex || nIndex < 0 )
        return false;

    const stCfgData& cfgData = m_aryCfgData.at(nIndex);
    strFileName = cfgData.strFileName;
    tmDate = cfgData.tmDate;

    return true;
}


static const char InitDataFileName[] = "SensorInitData.cfg";

QSensorInitDataCfg* QSensorInitDataCfg::m_pThis = NULL;
QSensorInitDataCfg::QSensorInitDataCfg() :
    m_bIsLoaded(false), m_bIsSoftkeyLoaded(false)
{
    m_strPrevFileName = "";
    m_strFileName = "";
}

QSensorInitDataCfg::~QSensorInitDataCfg()
{
}

QSensorInitDataCfg* QSensorInitDataCfg::instance()
{
    if ( m_pThis ) return m_pThis;
    m_pThis = new QSensorInitDataCfg();
    return m_pThis;
}

void QSensorInitDataCfg::reset()
{
    m_bIsLoaded = false;
    m_bIsSoftkeyLoaded = false;
    m_mapValue.clear();
    m_aryCmd.clear();
    m_SoftkeyData.strKey = "";
    m_SoftkeyData.strKeyBind = "";
    m_SoftkeyData.strGPIO = "";
    m_SoftkeyData.strLogic = "";
}

bool QSensorInitDataCfg::loadSoftkeyFile(QString &strSoftkeyPathName )
{
    if (!QFile::exists(strSoftkeyPathName))
        return false;

    QSettings iniSoftkey( strSoftkeyPathName, QSettings::IniFormat );

    QString strSoftkey, strSoftkeyBind, strSoftkeyGPIO, strSoftlogic;
    strSoftkey = iniSoftkey.value( "softkey" ).toString();
    strSoftkeyBind = iniSoftkey.value( "softkey_bind_info" ).toString();
    strSoftkeyGPIO = iniSoftkey.value( "softkey_gpio_info" ).toString();
    strSoftlogic = iniSoftkey.value( "softlogic" ).toString();

    qDebug( "softkey: %s", (const char*)strSoftkey.toLatin1() );
    qDebug( "softkey bind: %s", (const char*)strSoftkeyBind.toLatin1() );
    qDebug( "softkey gpio: %s", (const char*)strSoftkeyGPIO.toLatin1() );
    qDebug( "softlogic: %s", (const char*)strSoftlogic.toLatin1() );

    m_SoftkeyData.strKey = strSoftkey;
    m_SoftkeyData.strKeyBind = strSoftkeyBind;
    m_SoftkeyData.strGPIO = strSoftkeyGPIO;
    m_SoftkeyData.strLogic = strSoftlogic;

    return true;
}

bool QSensorInitDataCfg::load(const QString &strDataFileName )
{
    reset();

    m_strPrevFileName = "";
    m_strFileName = "";

#ifndef CREATE_FILE_TO_DOCUMENTS_LOCATION
    QString strDir = QCoreApplication::applicationDirPath();
    strDir = rstrip(strDir, "/\\");
#else
    QString strDocuments = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    strDocuments = rstrip( strDocuments, "/\\" );
    QString strDir = strDocuments + QDir::separator() + "T3kCfgFE";
#endif

    QString strSoftkeyPath;
    QString strFile = strDir + QDir::separator() + "SensorData" + QDir::separator();
    strFile += strDataFileName;
    strSoftkeyPath = strFile;
    strFile += ".cfg";
    strSoftkeyPath += ".hsk";

    QFile file(strFile);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QTextStream in(&file);

    QString strLine;
    while (!in.atEnd())
    {
        strLine = in.readLine();
        strLine = trim(strLine);

        if (strLine.at(0).toLatin1() == '#') // skip comment
        {
            m_aryCmd.push_back( strLine );
            continue;
        }

        int nSP = strLine.indexOf( '=' );
        if ( nSP < 0 )
        {
            qDebug( "syntax error - skip this line \"%s\"", (const char*)strLine.toLatin1());
            continue;
        }
        QString strCmd = strLine.left(nSP);
        QString strValue = strLine.right( strLine.size() - nSP - 1 );

        strCmd = trim(strCmd);
        strValue = trim(strValue);

        qDebug( "sensor init data: %s = %s", (const char*)strCmd.toLatin1(), (const char*)strValue.toLatin1() );
        m_aryCmd.push_back( strCmd );
        m_mapValue.insert(strCmd, strValue);
    }

    file.close();

    if ( !strSoftkeyPath.isEmpty() )
    {
        if ( loadSoftkeyFile( strSoftkeyPath ) )
        {
            m_bIsSoftkeyLoaded = true;
        }
    }

    m_bIsLoaded = true;
    m_strPrevFileName = strDataFileName;
    m_strFileName = strDataFileName;

    return true;
}

bool QSensorInitDataCfg::save(const QString &strDataFileName )
{
#ifndef CREATE_FILE_TO_DOCUMENTS_LOCATION
    QString strDir = QCoreApplication::applicationDirPath();
    strDir = rstrip(strDir, "/\\");

    QString strFile = strDir + QDir::separator() + "SensorData" + QDir::separator();
    makeDirectory( strFile );
#else
    QString strDocuments = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    strDocuments = rstrip( strDocuments, "/\\" );
    QString strFile = strDocuments + QDir::separator() + "T3kCfgFE" + QDir::separator() + "SensorData" + QDir::separator();
    makeDirectory( strFile );
#endif
    strFile += strDataFileName;
    strFile += ".cfg";

    QFile file(strFile);
    if (!file.open(QIODevice::WriteOnly))
        return false;
    QTextStream out(&file);

    QString strCmd, strValue;
    QString strLine;
    for ( int i=0 ; i<m_aryCmd.size() ; i++ )
    {
        strCmd = m_aryCmd.at(i);

        strLine = "";
        if ( strCmd.at(0).toLatin1() == '#' )	// comment
        {
            strLine = strCmd;
        }
        else
        {
            strValue = m_mapValue.value( strCmd );
            if (!strValue.isEmpty())
                strLine = strCmd + "=" + strValue;
        }

        if ( !strLine.isEmpty() )
        {
            out << strLine << "\n";
        }
    }

    file.close();

    m_bIsLoaded = true;
    m_strPrevFileName = strDataFileName;
    m_strFileName = strDataFileName;

    return true;
}

int QSensorInitDataCfg::getItemCount()
{
    return (int)m_aryCmd.size();
}

bool QSensorInitDataCfg::getItemAt( int nIndex, QString& strCmd, QString& strValue )
{
    Q_ASSERT( nIndex < getItemCount() );
    QString str = m_aryCmd.at( nIndex );
    if ( str.at(0).toLatin1() == '#' )
    {
        strCmd = str;
        strValue = "";
        return true;
    }
    else
    {
        strValue = m_mapValue.value( str );
        if ( strValue.isEmpty() )
        {
            return false;
        }
        else
        {
            strCmd = str;
        }
    }
    return true;
}

bool QSensorInitDataCfg::getValue( const QString& strCmd, QString& strValue )
{
    strValue = m_mapValue.value( strCmd );
    if ( strValue.isEmpty() )
        return false;

    return true;
}

bool QSensorInitDataCfg::setValue( const QString& strCmd, const QString& strValue )
{
    m_mapValue.insert( strCmd, strValue );
    m_aryCmd.push_back(strCmd);
    return true;
}
