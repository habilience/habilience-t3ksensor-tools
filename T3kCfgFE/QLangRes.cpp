#include "QLangRes.h"
#include <QDir>
#include <QApplication>
#include <QFile>
#include "../common/QUtils.h"
#include "LocalizeDef.h"
#include <QSettings>

QLangRes::QLangRes()
{
    QSettings settings( "Habilience", qApp->applicationName() );
    settings.beginGroup("LANGUAGE");
    m_nDefaultLanguage = settings.value( "DEFAULT", 0 ).toInt();
    m_bIsDefineLanguage = settings.value( "IS_DEFINED", false ).toBool();
    settings.endGroup();

    m_nAvailCheckPos = -1;
    m_nAvailableLanguageCount = 0;
}

QLangRes::~QLangRes()
{

}

bool QLangRes::setRootPath( const QString& strPath )
{
    m_strRootPath = strPath;
    m_strRootPath = rstrip(m_strRootPath, "/\\");
    m_strRootPath += '/';

    checkAllLanguageFiles(m_strRootPath);

    return true;
}

bool QLangRes::setZipResource( const QString& strPathName )
{
    m_LangZipFile.close();
    m_strRootPath = "";

    if ( !m_LangZipFile.open(strPathName) )
        return false;

    int nCnt = 0;
    int nDefCnt = sizeof(s_LocalizationDefs)/sizeof(LocalizationDefinition);
    for ( int i = 0 ; i<nDefCnt ; i++ )
    {
        s_LocalizationDefs[i].bExistFile = false;
        if ( m_LangZipFile.isExist(s_LocalizationDefs[i].szResFileName) )
        {
            if ( !m_bIsDefineLanguage )	// default language
            {
                m_nDefaultLanguage = 0;
                m_bIsDefineLanguage = true;
            }

            s_LocalizationDefs[i].bExistFile = true;
            nCnt ++;
        }

        qDebug( "[QLangRes] language-file [%s] : %d", s_LocalizationDefs[i].szResFileName, s_LocalizationDefs[i].bExistFile );
    }

    qDebug( "[QLangRes] total language-file: %d", nCnt );

    m_nAvailableLanguageCount = nCnt;

    if (m_bIsDefineLanguage)
    {
        if (!verifyLanguage(m_nDefaultLanguage))
        {
            m_nDefaultLanguage = 0;
            m_bIsDefineLanguage = false;
        }
    }

    m_nDefaultLanguage = 0;

    loadLanguageFile( m_nDefaultLanguage );
    loadDefaultLanguageFile();

    QSettings settings( "Habilience", qApp->applicationName() );
    settings.beginGroup("LANGUAGE");
    settings.setValue( "DEFAULT", m_nDefaultLanguage );
    settings.setValue( "IS_DEFINED", m_bIsDefineLanguage );
    settings.endGroup();

    return true;
}

QString QLangRes::getResString( const QString& strSection, const QString& strItem )
{
    QString strData;
    strData = getSectionData( m_LanguageFile, strSection, strItem );
    if (strData.isEmpty())
        strData = getSectionData( m_DefLanguageFile, strSection, strItem );
    strData.replace( "\\n", "\n" );
    strData.replace( "\\r", "\r" );
    strData.replace( "\\t", "\t" );
    return strData;
}

bool QLangRes::isR2L()
{
    return s_LocalizationDefs[m_nDefaultLanguage].bR2L;
}

QString QLangRes::getSectionData( QIni& ini, const QString& strSection, const QString& strItem )
{
    QIni::QSection* pSection;
    if ( strSection.isEmpty() )
    {	// root
        pSection = ini.getSection(strSection);
        Q_ASSERT( pSection );
    }
    else
    {	// data
        pSection = ini.getSectionNoCase(strSection);
        if ( pSection == NULL )
            return QString("");
    }

    QString strEntry;
    strEntry = strItem;
    int idxData = -1;

    idxData = pSection->getDataIndexNoCase(strEntry, idxData);
    if ( idxData < 0 )
    {
        return QString("");
    }

    QString strData = pSection->getData(idxData);

    return strData;
}

void QLangRes::checkAllLanguageFiles( const QString& strPath )
{
    QString strFullPathName;
    int nCnt = 0;
    int nDefCnt = sizeof(s_LocalizationDefs)/sizeof(LocalizationDefinition);
    for ( int i = 0 ; i<nDefCnt ; i++ )
    {
        strFullPathName = strPath;
        s_LocalizationDefs[i].bExistFile = false;
        strFullPathName += s_LocalizationDefs[i].szResFileName;
        if ( QFile(strFullPathName).exists() )
        {
            if ( !m_bIsDefineLanguage )	// default language
            {
                m_nDefaultLanguage = 0;
                m_bIsDefineLanguage = true;
            }

            s_LocalizationDefs[i].bExistFile = true;
            nCnt ++;
        }

        qDebug( "[QLangRes] language-file [%s] : %d", s_LocalizationDefs[i].szResFileName, s_LocalizationDefs[i].bExistFile );
    }

    qDebug( "[QLangRes] total language-file: %d", nCnt );

    m_nAvailableLanguageCount = nCnt;

    if ( m_bIsDefineLanguage )
    {
        if ( !verifyLanguage( m_nDefaultLanguage ) )
        {
            m_nDefaultLanguage = 0;
            m_bIsDefineLanguage = false;
        }
    }

    m_nDefaultLanguage = 0;

    loadLanguageFile( m_nDefaultLanguage );
    loadDefaultLanguageFile();

    QSettings settings( "Habilience", qApp->applicationName() );
    settings.beginGroup("LANGUAGE");
    settings.setValue( "DEFAULT", m_nDefaultLanguage );
    settings.setValue( "IS_DEFINED", m_bIsDefineLanguage );
    settings.endGroup();
}

bool QLangRes::verifyLanguage( int nIndex )
{
    int nDefCnt = sizeof(s_LocalizationDefs)/sizeof(LocalizationDefinition);

    if ( nIndex < nDefCnt && nIndex >= 0 )
    {
        if ( !s_LocalizationDefs[nIndex].bExistFile )
        {
            qDebug( "[QLangRes] Error - language file is not exist" );
            return false;
        }
    }
    else
    {
        qDebug( "[QLangRes] Error - invalid language file index" );
        return false;
    }
    return true;
}

int QLangRes::getFirstAvailableLanguage()
{
    int nDefCnt = sizeof(s_LocalizationDefs)/sizeof(LocalizationDefinition);
    for ( int i = 0 ; i<nDefCnt ; i++ )
    {
        if ( s_LocalizationDefs[i].bExistFile )
        {
            m_nAvailCheckPos = i+1;
            return i;
        }
    }
    m_nAvailCheckPos = -1;
    return -1;
}

int QLangRes::getNextAvailableLanguage()
{
    Q_ASSERT( m_nAvailCheckPos >= 0 );

    int nDefCnt = sizeof(s_LocalizationDefs)/sizeof(LocalizationDefinition);
    for ( int i = m_nAvailCheckPos ; i<nDefCnt ; i++ )
    {
        if ( s_LocalizationDefs[i].bExistFile )
        {
            m_nAvailCheckPos = i+1;
            return i;
        }
    }
    m_nAvailCheckPos = -1;
    return -1;
}

QString QLangRes::getLanguageName( int nPos )
{
    int nDefCnt = sizeof(s_LocalizationDefs)/sizeof(LocalizationDefinition);

    if ( nPos < nDefCnt && nPos >= 0 )
    {
        return QString(s_LocalizationDefs[nPos].szLocalLangName);
    }
    return QString("");
}

bool QLangRes::setLanguage( int nIndex )
{
    if( !verifyLanguage(nIndex) )
        return false;

    m_bIsDefineLanguage = true;
    m_nDefaultLanguage = nIndex;

    QSettings settings( "Habilience", qApp->applicationName() );
    settings.beginGroup("LANGUAGE");
    settings.setValue( "DEFAULT", m_nDefaultLanguage );
    settings.setValue( "IS_DEFINED", m_bIsDefineLanguage );
    settings.endGroup();

    if( !loadLanguageFile( nIndex ) )
    {
        qDebug( "[QLangRes] Error - Cannot read language-file" );
        return false;
    }

    return true;
}

bool QLangRes::loadLanguageFile( int nIndex )
{
    if ( m_strRootPath.isEmpty() && m_LangZipFile.isOpen() )
    {
        m_LanguageFile.close();
        unsigned long dwBufSize = m_LangZipFile.readFile( s_LocalizationDefs[nIndex].szResFileName, NULL, 0 );
        if ( dwBufSize > 0 )
        {
            char* pResBuffer = new char[dwBufSize];
            if ( m_LangZipFile.readFile( s_LocalizationDefs[nIndex].szResFileName, pResBuffer, dwBufSize ) == dwBufSize )
            {
                if ( !m_LanguageFile.load( pResBuffer, dwBufSize ) )
                {
                    // load from resource
                    if( !m_LanguageFile.load( ":/T3kCfgFERes/Languages/english.txt" ) )
                    {
                        return false;
                    }
                }
            }
            delete[] pResBuffer;
        }
    }
    else
    {
        QString strFullPathName;
        strFullPathName = m_strRootPath;
        strFullPathName += s_LocalizationDefs[nIndex].szResFileName;

        m_LanguageFile.close();

        if ( !m_LanguageFile.load(strFullPathName) )
        {
            // load from resource
            if( !m_LanguageFile.load( ":/T3kCfgFERes/Languages/english.txt" ) )
            {
                return false;
            }
        }
    }

    return true;
}

bool QLangRes::loadDefaultLanguageFile()
{
    if ( m_strRootPath.isEmpty() && m_LangZipFile.isOpen() )
    {
        m_DefLanguageFile.close();
        unsigned long dwBufSize = m_LangZipFile.readFile( s_LocalizationDefs[0].szResFileName, NULL, 0 );
        if ( dwBufSize > 0 )
        {
            char* pResBuffer = new char[dwBufSize];
            if ( m_LangZipFile.readFile( s_LocalizationDefs[0].szResFileName, pResBuffer, dwBufSize ) == dwBufSize )
            {
                if ( !m_DefLanguageFile.load( pResBuffer, dwBufSize ) )
                {
                    // load from resource
                    if( !m_LanguageFile.load( ":/T3kCfgFERes/Languages/english.txt" ) )
                    {
                        return false;
                    }
                }
            }
            delete[] pResBuffer;
        }
    }
    else
    {
        QString strFullPathName;
        strFullPathName = m_strRootPath;
        strFullPathName += s_LocalizationDefs[0].szResFileName;

        m_DefLanguageFile.close();

        if ( !m_DefLanguageFile.load(strFullPathName) )
        {
            // load from resource
            if( !m_LanguageFile.load( ":/T3kCfgFERes/Languages/english.txt" ) )
            {
                return false;
            }
        }
    }

    return true;
}
