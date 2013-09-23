#ifndef QLANGMANAGER_H
#define QLANGMANAGER_H

#ifdef ZIP_LANGUAGE
#include "QZLangRes.h"
#else
#include "QLangRes.h"
#endif
#include "QSingletone.h"

#include <QVector>

class QLangManager : public QSingleton<QLangManager>
{
public:
    class ILangChangeNotify
    {
    public:
        ILangChangeNotify() { QLangManager::instance()->registerNotify(this); }
        ~ILangChangeNotify() { QLangManager::instance()->unregisterNotify(this); }
        virtual void onChangeLanguage() = 0;
    };

public:
    QLangManager();
    ~QLangManager();

    static QLangRes& getResource() { return QLangManager::instance()->m_LanguageResource; }

    static QString getResUTF8String( QString lpszSection, QString lpszItem );//

    bool setRootPath( const QString& strPath ) { return m_LanguageResource.setRootPath(strPath); }
#ifdef ZIP_LANGUAGE
    bool setZipResource( const QString& strPathName ) { return m_LanguageResource.setZipResource(strPathName); }
#endif

    int getFirstAvailableLanguage() { return m_LanguageResource.getFirstAvailableLanguage(); }
    int getNextAvailableLanguage() { return m_LanguageResource.getNextAvailableLanguage(); }
    QString getLanguageName( int nPos ) { return m_LanguageResource.getLanguageName(nPos); }

    bool setLanguage( int nIndex );
    int getActiveLanguage() { return m_LanguageResource.getActiveLanguage(); }
#ifndef ZIP_LANGUAGE
    int getDefaultLanguage() { return m_LanguageResource.getDefaultLanguage(); } //
#endif

    int getAvailableLanguageCount() { return m_LanguageResource.getAvailableLanguageCount(); }

    void registerNotify( ILangChangeNotify* pNotify );
    void unregisterNotify( ILangChangeNotify* pNotify );

protected:
    QLangRes		m_LanguageResource;
    QVector<ILangChangeNotify*>	m_Notifiers;
};

#endif // QLANGMANAGER_H
