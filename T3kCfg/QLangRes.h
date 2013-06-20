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
        const char*	szLocalLangName;
        const char*	szResFileName;
		bool            bIsR2L;
        bool            bExistFile;
    };

public:
    QString GetResString( QString lpszSection, QString lpszItem );

    bool SetRootPath( QString lpszPath );
    bool IsR2L();
protected:
    bool Load( int nIndex );

    int GetFirstAvailableLanguage();
    int GetNextAvailableLanguage( int nPos );
    QString GetLanguageName( int nPos );

    int GetAvailableLanguageCount() { return m_nAvailableLanguageCount; }

    bool SetLanguage( int nIndex );

    bool VerifyLanguage( int nIndex );

    void CheckAllLanguageFiles( QString strPath );

    int GetActiveLanguage() { return m_nDefaultLanguage; }
    int GetDefaultLanguage() { return m_nLocalLanguage; }
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
    QSettings*		m_pLanguageFile;
    QSettings*		m_pDefLanguageFile;
    QString		m_strSystemLangAbbreviatedName;
    int                 m_nLocalLanguage;
    int			m_nDefaultLanguage;
    bool		m_bIsDefineLanguage;
};

#endif // QLANGRES_H
