#ifndef QLANGMANAGER_H
#define QLANGMANAGER_H

#include "QLangRes.h"
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

    QLangRes& getResource() { return m_LanguageResource; }

    bool setRootPath( const QString& strPath ) { return m_LanguageResource.setRootPath(strPath); }
    bool setZipResource( const QString& strPathName ) { return m_LanguageResource.setZipResource(strPathName); }

    int getFirstAvailableLanguage() { return m_LanguageResource.getFirstAvailableLanguage(); }
    int getNextAvailableLanguage() { return m_LanguageResource.getNextAvailableLanguage(); }
    QString getLanguageName( int nPos ) { return m_LanguageResource.getLanguageName(nPos); }

    bool setLanguage( int nIndex );
    int getActiveLanguage() { return m_LanguageResource.getActiveLanguage(); }

    int getAvailableLanguageCount() { return m_LanguageResource.getAvailableLanguageCount(); }

    void registerNotify( ILangChangeNotify* pNotify );
    void unregisterNotify( ILangChangeNotify* pNotify );

protected:
    QLangRes		m_LanguageResource;
    QVector<ILangChangeNotify*>	m_Notifiers;
};

#endif // QLANGMANAGER_H
