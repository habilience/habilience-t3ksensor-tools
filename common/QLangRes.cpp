#include "QLangRes.h"

#include <QSettings>
#include <QCoreApplication>
#include <QFile>
#include <QLocale>
#include <QLocale>
#include <QStringList>

#include "LocalizeDef.h"

QLangRes::QLangRes()
{
    m_pLanguageFile = NULL;
    m_pDefLanguageFile = NULL;

    QLocale Lang;
    int nLanguageID = Lang.system().language();//Lang. query( QSystemLocale::LanguageId, 0 ).toInt();
    m_strSystemLangAbbreviatedName = QLocale::languageToString( (QLocale::Language)nLanguageID ).toLower();

    {
    QSettings regLanguage( "Habilience", "T3kCfg" );
    regLanguage.beginGroup( "Language" );
    m_nLocalLanguage = regLanguage.value( "Local", 0 ).toInt();
    m_nDefaultLanguage = regLanguage.value( "Default", 0 ).toInt();
    m_bIsDefineLanguage = regLanguage.value( "IsDefined", false ).toBool();
    regLanguage.endGroup();
    }

    m_nAvailCheckPos = -1;
    m_nAvailableLanguageCount = 0;
}

QLangRes::~QLangRes()
{
    if( m_pLanguageFile )
    {
        delete m_pLanguageFile;
        m_pLanguageFile = NULL;
    }

    if( m_pDefLanguageFile )
    {
        delete m_pDefLanguageFile;
        m_pDefLanguageFile = NULL;
    }
}

bool QLangRes::setRootPath( QString lpszPath )
{
    //Q_ASSERT( !m_strRootPath.size() );

    m_strRootPath = lpszPath;

    m_strRootPath.trimmed();
    if( m_strRootPath.at( m_strRootPath.length() - 1 ) != '/' )
        m_strRootPath += '/';

    checkAllLanguageFiles( m_strRootPath );

    return true;
}

bool QLangRes::load( int nIndex )
{
    bool bRet = false;

    if( m_pLanguageFile )
    {
        delete m_pLanguageFile;
        m_pLanguageFile = NULL;
    }

    QString strSelect( m_strRootPath + s_LocalizationDefs[nIndex].szResFileName );
    if( QFile::exists( strSelect ) )
    {
        m_pLanguageFile = new QSettings( strSelect, QSettings::IniFormat );
        m_pLanguageFile->setIniCodec( "UTF-8" );
    }

    if( m_pDefLanguageFile )
    {
        delete m_pDefLanguageFile;
        m_pDefLanguageFile = NULL;
    }

    m_pDefLanguageFile = new QSettings( ":/T3kCfgRes/Languages/english.txt", QSettings::IniFormat );
    m_pDefLanguageFile->setIniCodec( "UTF-8" );

    if( QString::compare( s_LocalizationDefs[nIndex].szResFileName, "english.txt", Qt::CaseInsensitive ) == 0 || m_pLanguageFile )
        bRet = true;

    return bRet;
}

QString QLangRes::getResString( QString lpszSection, QString lpszItem )
{
    QString str;
    if( m_pLanguageFile )
    {
        m_pLanguageFile->beginGroup( lpszSection );
        QVariant var = m_pLanguageFile->value( lpszItem, "" );
        switch( var.type() )
        {
        case QVariant::StringList:
            {
                QStringList strList( var.toStringList() );
                str = strList.at(0);
                for( int i=1; i<strList.count(); i++ )
                    str += ", " + strList[i];
            }
            break;
        case QVariant::String:
            str = var.toString();
            break;
        default:
            break;
        }
        m_pLanguageFile->endGroup();
    }

    if( !str.size() && m_pDefLanguageFile )
    {
        m_pDefLanguageFile->beginGroup( lpszSection );
        QVariant var = m_pDefLanguageFile->value( lpszItem, "" );
        switch( var.type() )
        {
        case QVariant::StringList:
            {
                QStringList strList( var.toStringList() );
                str = strList.at(0);
                for( int i=1; i<strList.count(); i++ )
                    str += ", " + strList[i];
            }
            break;
        case QVariant::String:
            str = var.toString();
            break;
        default:
            break;
        }

        m_pDefLanguageFile->endGroup();
    }
    return str;
}

bool QLangRes::isR2L()
{
    return s_LocalizationDefs[m_nDefaultLanguage].bIsR2L;
}

bool QLangRes::setLanguage( int nIndex )
{
    if( !verifyLanguage(nIndex) )
        return false;

    m_bIsDefineLanguage = true;
    m_nDefaultLanguage = nIndex;

    {
    QSettings regLanguage( "Habilience", "T3kCfg" );
    regLanguage.beginGroup( "Language" );
    regLanguage.setValue( "Default", m_nDefaultLanguage );
    regLanguage.setValue( "IsDefined", m_bIsDefineLanguage );
    regLanguage.endGroup();
    }

    if( !load( nIndex ) )
    {
        qDebug( "[CLangRes] Error - Cannot read language-file" );
        return false;
    }

    return true;
}

bool QLangRes::verifyLanguage( int nIndex )
{
    int nDefCnt = sizeof(s_LocalizationDefs)/sizeof(LocalizationDefinition);

    if( nIndex < nDefCnt && nIndex >= 0 )
    {
        if( !s_LocalizationDefs[nIndex].bExistFile )
        {
            qDebug( "[CLangRes] Error - language file is not exist\r\n" );
            return false;
        }
    }
    else
    {
        qDebug( "[CLangRes] Error - invalid language file index\r\n" );
        return false;
    }
    return true;
}

int QLangRes::getFirstAvailableLanguage()
{
    Q_ASSERT( m_nAvailCheckPos < 0 );

    int nDefCnt = sizeof(s_LocalizationDefs)/sizeof(LocalizationDefinition);
    for( int i = 0 ; i<nDefCnt ; i++ )
    {
        if( s_LocalizationDefs[i].bExistFile )
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
    for( int i = m_nAvailCheckPos ; i<nDefCnt ; i++ )
    {
        if( s_LocalizationDefs[i].bExistFile )
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

    if( nPos < nDefCnt && nPos >= 0 )
    {
        return QString::fromUtf8(s_LocalizationDefs[nPos].szLocalLangName);
    }
    return "";
}

void QLangRes::checkAllLanguageFiles( QString strPath )
{
    QString strFullPathName;
    int nCnt = 0;
    int nDefCnt = sizeof(s_LocalizationDefs)/sizeof(LocalizationDefinition);
    for( int i = 0 ; i<nDefCnt ; i++ )
    {
        strFullPathName = strPath;
        s_LocalizationDefs[i].bExistFile = false;
        strFullPathName += s_LocalizationDefs[i].szResFileName;
        if( QString::compare( s_LocalizationDefs[i].szResFileName, "english.txt", Qt::CaseInsensitive ) == 0 ||
            QFile::exists( strFullPathName ) )
        {
            if( !m_bIsDefineLanguage )	// default language
            {
                if( s_LocalizationDefs[i].szAbbreviatedName[0] != '=' && m_strSystemLangAbbreviatedName.size() )
                {/*
                    if( ( (s_LocalizationDefs[i].szAbbreviatedName[0] == '=') || (m_strSystemLangAbbreviatedName.at(0) == s_LocalizationDefs[i].szAbbreviatedName[0]) ) &&
                        ( (s_LocalizationDefs[i].szAbbreviatedName[1] == '=') || (m_strSystemLangAbbreviatedName.at(1) == s_LocalizationDefs[i].szAbbreviatedName[1]) ) &&
                        ( (s_LocalizationDefs[i].szAbbreviatedName[2] == '=') || (m_strSystemLangAbbreviatedName.at(2) == s_LocalizationDefs[i].szAbbreviatedName[2]) ) )
                        */
                    QString str( QString( s_LocalizationDefs[i].szResFileName ).remove( QString::fromUtf8(".txt") ) );
                    if( !m_strSystemLangAbbreviatedName.compare( str ) )
                    {
                        m_nLocalLanguage = i;
                        m_nDefaultLanguage = i;
                        m_bIsDefineLanguage = true;
                    }
                }
            }

            s_LocalizationDefs[i].bExistFile = true;
            nCnt ++;
        }

        qDebug( "[CLangRes] language-file [%s] : %d\r\n", s_LocalizationDefs[i].szResFileName, s_LocalizationDefs[i].bExistFile );
    }

    qDebug( "[CLangRes] total language-file: %d\r\n", nCnt );

    m_nAvailableLanguageCount = nCnt;

    if( m_bIsDefineLanguage )
    {
        if( !verifyLanguage( m_nDefaultLanguage ) )
        {
            m_nDefaultLanguage = 0;
            m_bIsDefineLanguage = false;
        }
    }
    else
        m_nLocalLanguage = 0;

    load( m_nDefaultLanguage );

    {
    QSettings regLanguage( "Habilience", "T3kCfg" );
    regLanguage.beginGroup( "Language" );
    regLanguage.setValue( "Local", m_nLocalLanguage );
    regLanguage.setValue( "Default", m_nDefaultLanguage );
    regLanguage.setValue( "IsDefined", m_bIsDefineLanguage );
    regLanguage.endGroup();
    }
}
