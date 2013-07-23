#include "QLangManager.h"

QLangManager::QLangManager()
{
}

QLangManager::~QLangManager()
{
    m_Notifiers.clear();
}

bool QLangManager::setLanguage( int nIndex )
{
    if ( m_LanguageResource.setLanguage(nIndex) )
    {
        // notify
        for ( int nI = 0 ; nI < m_Notifiers.size() ; nI ++ )
        {
            m_Notifiers.at(nI)->onChangeLanguage();
        }

        return true;
    }

    return false;
}

void QLangManager::registerNotify( ILangChangeNotify* pNotify )
{
    for ( int nI = 0 ; nI < m_Notifiers.size() ; nI ++ )
    {
        if( m_Notifiers.at(nI) == pNotify )
        {
            return;
        }
    }
    m_Notifiers.push_back( pNotify );
}

void QLangManager::unregisterNotify( ILangChangeNotify* pNotify )
{
    for( int nI = 0 ; nI < m_Notifiers.size() ; nI ++ )
    {
        if( m_Notifiers.at(nI) == pNotify )
        {
            m_Notifiers.remove(nI);
            return;
        }
    }
}
