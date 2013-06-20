#include "QLangManager.h"
#include <QApplication>

QLangManager::_GC::_GC(void)
{
}

QLangManager::_GC::~_GC(void)
{
    if( QLangManager::s_pThis )
        delete QLangManager::s_pThis;
    QLangManager::s_pThis = NULL;
}
QLangManager::_GC s_LMGC;

QLangManager* QLangManager::s_pThis = NULL;

QLangManager* QLangManager::GetPtr()
{
    if( !s_pThis )
        s_pThis = new QLangManager();
    Q_ASSERT( s_pThis );
    return s_pThis;
}

QLangManager::QLangManager()
{
}

QLangManager::~QLangManager()
{
    m_Notifiers.clear();
}

void QLangManager::RegisterNotify( LangChangeNotify* pNotify )
{
    for( int nI = 0 ; nI < m_Notifiers.size() ; nI ++ )
    {
        if( m_Notifiers.at(nI) == pNotify )
        {
            return;
        }
    }
    m_Notifiers.push_back( pNotify );
}

void QLangManager::ReleaseNotify(LangChangeNotify *pNotify)
{
    for( int nI = 0 ; nI < m_Notifiers.size() ; nI ++ )
    {
        if( m_Notifiers.at(nI) == pNotify )
        {
            m_Notifiers.remove( nI );
        }
    }
}

bool QLangManager::SetLanguage( int nIndex )
{
    if( m_LanguageResource.SetLanguage(nIndex) )
    {
        // notify
        for( int nI = 0 ; nI < m_Notifiers.size() ; nI ++ )
        {
            m_Notifiers.at(nI)->OnChangeLanguage();
        }

        QApplication::setLayoutDirection( m_LanguageResource.IsR2L() ? Qt::RightToLeft : Qt::LeftToRight );
        return true;
    }

    return false;
}

QString QLangManager::GetResUTF8String(QString lpszSection, QString lpszItem)
{
    return GetPtr()->GetResource().GetResString( QString::fromUtf8(lpszSection.toUtf8().data()), QString::fromUtf8(lpszItem.toUtf8().data()) );
}
