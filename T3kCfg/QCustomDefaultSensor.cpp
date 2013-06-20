#include "QCustomDefaultSensor.h"

#include <QApplication>
#include <QFile>
#include <QDir>

#ifdef Q_OS_WIN
#include "../../WindowApp/Common/T30xConstStr.h"
#else
#include "Common/T30xConstStr.h"
#endif


QCustomDefaultSensor* QCustomDefaultSensor::s_pInstance = NULL;

QCustomDefaultSensor::QCustomDefaultSensor(QObject *parent) :
    QObject(parent)
{
    m_vCommandStr.clear();
    m_bLoaded = false;

    Reload();
}

QString QCustomDefaultSensor::GetDefaultData(const char *pContStr, QString strDefault)
{
    if( !m_bLoaded ) return strDefault;

    for( int i=0; i<m_vCommandStr.size(); i++ )
    {
        QString str = m_vCommandStr.at( i );
        if( str.contains( pContStr ) )
        {
            int nIdx = str.indexOf( '=' );
            if( nIdx < 0 ) continue;
            QString strVal = str.mid( nIdx+1 );
            return strVal;
        }
    }

    return strDefault;
}

bool QCustomDefaultSensor::Reload()
{
    m_bLoaded = false;
    m_vCommandStr.clear();

    QString strFilePath = QApplication::applicationDirPath() + "/Config/";
    QDir dirResStateReport( strFilePath );
    QStringList strUsableExtensionList;
    strUsableExtensionList << "*.cfg";

    dirResStateReport.setNameFilters( strUsableExtensionList );
    dirResStateReport.setFilter( QDir::Files );

    QStringList strCfgFileList = dirResStateReport.entryList();
    if( !strCfgFileList.size() )
        return false;

    strFilePath += strCfgFileList.at(0);

    QFile fConfig( strFilePath );
    if( !fConfig.open( QIODevice::ReadOnly | QIODevice::Text ) )
        return false;

    while( !fConfig.atEnd() )
    {
        QByteArray strLine = fConfig.readLine();
        if( strLine.isEmpty() || strLine.isNull() )
            continue;

        strLine = strLine.trimmed();
        if( strLine.at( 0 ) == '#' )
            continue;

        if( strLine.contains(cstrMouseProfile) )
        {
            m_vCommandStr.push_back( strLine );
            continue;
        }
        else if( strLine.contains(cstrMouseProfile1) )
        {
            m_vCommandStr.push_back( strLine );
            continue;
        }
        else if( strLine.contains(cstrMouseProfile2) )
        {
            m_vCommandStr.push_back( strLine );
            continue;
        }
        else if( strLine.contains(cstrMouseProfile3) )
        {
            m_vCommandStr.push_back( strLine );
            continue;
        }
        else if( strLine.contains(cstrMouseProfile4) )
        {
            m_vCommandStr.push_back( strLine );
            continue;
        }
        else if( strLine.contains(cstrMouseProfile5) )
        {
            m_vCommandStr.push_back( strLine );
            continue;
        }
        else if( strLine.contains(cstrInputMode) )
        {
            m_vCommandStr.push_back( strLine );
            continue;
        }
    }

    qDebug( "Custom Default Sensor : %d", m_vCommandStr.size() );

    m_bLoaded = true;
    return true;
}
