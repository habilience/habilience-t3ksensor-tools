#include "QIniFormat.h"

#include <QFileInfo>

QIniFormat::QIniFormat(const QString strExtension) :
    m_strExtension(strExtension)
{
    m_pFile = NULL;
    m_pSelectedGroup = NULL;
}

QIniFormat::~QIniFormat()
{
    clearDatamap();

    if( m_pFile )
    {
        if( m_pFile->isOpen() )
            m_pFile->close();
        delete m_pFile;
        m_pFile = NULL;
    }

}

bool QIniFormat::open(const QString &fileName)
{
    if( QFileInfo( fileName ).suffix() != m_strExtension ) return false;
    if( !QFile::exists( fileName ) ) return false;

    if( !m_pFile )
        m_pFile = new QFile();

    m_pFile->setFileName(fileName);
    if( !m_pFile->open(QIODevice::ReadWrite) ) return false;

    syncIniFile();

    m_pFile->close();

    return true;
}

bool QIniFormat::save(const QString &fileName)
{
    if( m_mapIniData.count() == 0 ) return false;

    if( !m_pFile )
        m_pFile = new QFile();

    m_pFile->setFileName( fileName );
    if( !m_pFile->open(QIODevice::WriteOnly) ) return false;

    syncMapData();

    m_pFile->close();

    return true;
}

void QIniFormat::clearDatamap()
{
    foreach( IniDataGroup* g, m_mapIniData.values() )
    {
        if( g != NULL )
            delete g;
    }

    m_mapIniData.clear();
}

void QIniFormat::syncIniFile()
{
    if( !isOpen() ) return;

    clearDatamap();

    QString strCurrentGroup;
    while( !m_pFile->atEnd() )
    {
        QByteArray fBytes = m_pFile->readLine();
        fBytes.trimmed();
        if( fBytes.isEmpty() ) continue;
        char ch = fBytes.at(0);
        if( ch == ';' ) continue;

        char escapeChar = fBytes.at( fBytes.length()-1 );
        while( escapeChar == '\r' || escapeChar == '\n' )
        {
            fBytes.remove( fBytes.length()-1, 1 );
            if( fBytes.size() == 0 ) break;

            escapeChar = fBytes.at( fBytes.length()-1 );
        }

        int nPos = 0;
        if( ch == '[' && ((nPos = fBytes.indexOf( ']')) == fBytes.length()-1) )
        {
            strCurrentGroup = fBytes.mid( 1, nPos-1 ).trimmed();
            m_mapIniData.insert( strCurrentGroup, new IniDataGroup() );
            continue;
        }

        if( (nPos = fBytes.indexOf( '=' )) <= 0 ) continue;

        QString strKey( fBytes.left( nPos ).trimmed() );
        QString strValue( fBytes.mid( nPos+1 ).trimmed() );

        IniDataGroup* pGroup = m_mapIniData.value( strCurrentGroup, NULL );
        if( !pGroup )
        {
            m_mapIniData.insert( "", new IniDataGroup() );
            pGroup = m_mapIniData.value( strCurrentGroup, NULL );
            Q_ASSERT( pGroup );
        }

        pGroup->insert( strKey, strValue );
    }
}
#include <QDebug>
void QIniFormat::syncMapData()
{
    if( !isOpen() ) return;

    IniDataMap::iterator iterNoGroup = m_mapIniData.find( "" );
    if( iterNoGroup != m_mapIniData.end() )
    {
        IniDataGroup* pNoGroup = m_mapIniData.value( "", NULL );
        for( IniDataGroup::iterator iter = pNoGroup->begin(); iter != pNoGroup->end(); ++iter )
        {
            m_pFile->write( QString(iter.key() + "=" + iter.value() + "\r\n").toUtf8() );
        }

        m_pFile->write( "\r\n" );
    }

    for( IniDataMap::iterator iter = m_mapIniData.begin(); iter != m_mapIniData.end(); ++iter )
    {
        if( iter.key() == "" ) continue;

        IniDataGroup* pGroup = iter.value();
        m_pFile->write( QString("[" + iter.key() + "]\r\n").toUtf8() );
        for( IniDataGroup::iterator iter = pGroup->begin(); iter != pGroup->end(); ++iter )
        {
            m_pFile->write( QString(iter.key() + "=" + iter.value() + "\r\n").toUtf8() );

        }
    }
}

void QIniFormat::beginGroup(const QString strGroup)
{
    if( !m_mapIniData.contains( strGroup ) )
    {
        m_strNewGroup = strGroup;
        return;
    }

    m_pSelectedGroup = m_mapIniData.value( strGroup, NULL );
    if( !m_pSelectedGroup ) return;
}

void QIniFormat::endGroup()
{
    m_pSelectedGroup = NULL;
    m_strNewGroup.clear();
}

QString QIniFormat::getValue(const QString strKey, const QString &strDefault /*= QString()*/ )
{
    IniDataGroup* pGroup = m_pSelectedGroup;
    if( !pGroup )
    {
        pGroup = m_mapIniData.value( "", NULL );
        Q_ASSERT( pGroup );
    }

    return pGroup->value( strKey, strDefault );
}

bool QIniFormat::setValue(const QString strKey, const QString strValue)
{
    IniDataGroup* pGroup = m_pSelectedGroup;
    if( !pGroup )
    {
        if( m_strNewGroup.isEmpty() )
        {
            pGroup = m_mapIniData.value( "", NULL );
            if( !pGroup )
            {
                m_mapIniData.insert( "", new IniDataGroup() );
                pGroup = m_mapIniData.value( "", NULL );
            }
        }
        else
        {
            m_mapIniData.insert( m_strNewGroup, new IniDataGroup() );
            m_pSelectedGroup = m_mapIniData.value( m_strNewGroup, NULL );
            m_strNewGroup.clear();

            pGroup = m_pSelectedGroup;
        }
    }

    Q_ASSERT( pGroup );

    pGroup->insert( strKey, strValue );

    qDebug() << strKey + "=" + strValue;

    return true;
}
