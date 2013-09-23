#ifndef QLANGRES_H
#define QLANGRES_H

#include <QString>
#include <QSettings>

class QLangManager;

class QLangRes
{
    friend class QLangManager;
public:
    QLangRes();
    ~QLangRes();

    struct LocalizationDefinition
    {
        const char*	szAbbreviatedName;
        int         nCodePage;
        const char*	szLocalLangName;
        const char*	szResFileName;
        bool        bIsR2L;
        bool        bExistFile;
    };

public:
    QString getResString( QString lpszSection, QString lpszItem );

    bool setRootPath( QString lpszPath );
    bool isR2L();
protected:
    bool load( int nIndex );

    int getFirstAvailableLanguage();
    int getNextAvailableLanguage();
    QString getLanguageName( int nPos );

    int getAvailableLanguageCount() { return m_nAvailableLanguageCount; }

    bool setLanguage( int nIndex );

    bool verifyLanguage( int nIndex );

    void checkAllLanguageFiles( QString strPath );

    int getActiveLanguage() { return m_nDefaultLanguage; }
    int getDefaultLanguage() { return m_nLocalLanguage; }
/*
    void GenerateLanguageSetting();

    bool LoadLanguageFile( int nIndex );
    bool LoadDefaultLanguageFile();

    QString GetSectionData( CwIni& wIni, LPCTSTR lpszSection, LPCTSTR lpszItem );
    void CopyDefaultLanguageResource( HINSTANCE hInst, UINT nId, LPCTSTR lpszFileName );*/
private:
    int			m_nAvailCheckPos;

    int			m_nAvailableLanguageCount;

    QString		m_strRootPath;
    QSettings*  m_pLanguageFile;
    QSettings*  m_pDefLanguageFile;
    QString		m_strSystemLangAbbreviatedName;
    int         m_nLocalLanguage;
    int			m_nDefaultLanguage;
    bool		m_bIsDefineLanguage;
};

#endif // QLANGRES_H
