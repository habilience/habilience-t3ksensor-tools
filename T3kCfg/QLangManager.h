#ifndef QLANGMANAGER_H
#define QLANGMANAGER_H

#include <QVector>
#include "QLangRes.h"

class QLangManager
{
public:
    QLangManager();
    ~QLangManager();

    class LangChangeNotify
    {
    public:
        LangChangeNotify() { QLangManager::GetPtr()->RegisterNotify( this ); }
        ~LangChangeNotify() { QLangManager::GetPtr()->ReleaseNotify( this ); }
        virtual void OnChangeLanguage() = 0;
    };

    class _GC
    {
    public:
            _GC(void);
            ~_GC(void);
    };
    friend class _GC;

    static QLangManager* GetPtr();
    static QString GetResUTF8String( QString lpszSection, QString lpszItem );

    QLangRes& GetResource() { return m_LanguageResource; }

    bool SetRootPath( QString lpszPath ) { return m_LanguageResource.SetRootPath(lpszPath); }

    int GetFirstAvailableLanguage() { return m_LanguageResource.GetFirstAvailableLanguage(); }
    int GetNextAvailableLanguage( int nPos ) { return m_LanguageResource.GetNextAvailableLanguage(nPos); }
    QString GetLanguageName( int nPos ) { return m_LanguageResource.GetLanguageName(nPos); }

    bool SetLanguage( int nIndex );
    int GetActiveLanguage() { return m_LanguageResource.GetActiveLanguage(); }
    int GetDefaultLanguage() { return m_LanguageResource.GetDefaultLanguage(); }

    int GetAvailableLanguageCount() { return m_LanguageResource.GetAvailableLanguageCount(); }
//    void GenerateLanguageSetting() { m_LanguageResource.GenerateLanguageSetting(); }

    void RegisterNotify( LangChangeNotify* pNotify );
    void ReleaseNotify( LangChangeNotify* pNotify );

protected:
    QLangRes                        m_LanguageResource;
    QVector<LangChangeNotify*>      m_Notifiers;
private:
    static QLangManager*            s_pThis;
};

#endif // QLANGMANAGER_H
