#ifndef QLangRes_H
#define QLangRes_H

#include <QString>
#include "QIni.h"
#include "QLangZipFile.h"

class QLangManager;
class QLangRes
{
    friend class QLangManager;
public:
    struct LocalizationDefinition
    {
        const char*     szAbbreviatedName;
        int             nCodePage;
        const char*     szLocalLangName;
        const char*     szResFileName;
        bool            bR2L;
        bool            bExistFile;
    };
public:
    bool setRootPath( const QString& strPath );
    bool setZipResource( const QString& strPathName );

    QString getResString( const QString& strSection, const QString& strItem );
    bool isR2L();

protected:
    int getFirstAvailableLanguage();
    int getNextAvailableLanguage();
    QString getLanguageName( int nPos );

    int getAvailableLanguageCount() { return m_nAvailableLanguageCount; }

    bool setLanguage( int nIndex );

    int getActiveLanguage() { return m_nDefaultLanguage; }

    void checkAllLanguageFiles( const QString& strPath );
    bool verifyLanguage( int nIndex );
    bool loadLanguageFile( int nIndex );
    bool loadDefaultLanguageFile();

    QString getSectionData( QIni& ini, const QString& strSection, const QString& strItem );
private:
    int     m_nAvailCheckPos;
    int     m_nAvailableLanguageCount;

    QString     m_strRootPath;
    QIni        m_LanguageFile;
    QIni        m_DefLanguageFile;
    int         m_nDefaultLanguage;
    bool        m_bIsDefineLanguage;

    bool        m_bIsR2L;

    QLangZipFile m_LangZipFile;

private:
    QLangRes();
    ~QLangRes();
};

#endif // QLangRes_H
