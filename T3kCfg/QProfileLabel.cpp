#include "stdInclude.h"
#include "QProfileLabel.h"

#include <QSettings>
#include <QObject>
#include <QFile>
#include <QCoreApplication>

QProfileLabel::QProfileLabel()
{
    m_vProfileLabels.reserve( 5 );
    m_vProfileLabels.clear();

    Load();
}

void QProfileLabel::Default()
{
    m_vProfileLabels.clear();
    m_vProfileLabels.push_back( QProfileName1 );
    m_vProfileLabels.push_back( QProfileName2 );
    m_vProfileLabels.push_back( QProfileName3 );
    m_vProfileLabels.push_back( QProfileName4 );
    m_vProfileLabels.push_back( QProfileName5 );
}

void QProfileLabel::Load()
{
    m_vProfileLabels.clear();

    QSettings ReadProfileNames( "Habilience", "T3kCfg" );
    ReadProfileNames.beginGroup( "ProfileNames" );
    m_vProfileLabels.push_back( ReadProfileNames.value( "Profile1", QProfileName1 ).toString() );
    m_vProfileLabels.push_back( ReadProfileNames.value( "Profile2", QProfileName2 ).toString() );
    m_vProfileLabels.push_back( ReadProfileNames.value( "Profile3", QProfileName3 ).toString() );
    m_vProfileLabels.push_back( ReadProfileNames.value( "Profile4", QProfileName4 ).toString() );
    m_vProfileLabels.push_back( ReadProfileNames.value( "Profile5", QProfileName5 ).toString() );
    ReadProfileNames.endGroup();
}

void QProfileLabel::Save()
{
    QSettings SaveProfileNames( "Habilience", "T3kCfg" );
    SaveProfileNames.beginGroup( "ProfileNames" );
    QString str;
    int i = 0;
    foreach( str, m_vProfileLabels )
    {
        i++;
        SaveProfileNames.setValue( QString("Profile%1").arg(i), str );
    }
    SaveProfileNames.endGroup();
}

const QString QProfileLabel::GetLabel( int nProfileIndex )
{
//    Q_ASSERT( m_vProfileLabels.size() );
    if( !m_vProfileLabels.size() && (nProfileIndex < 0 || nProfileIndex >= m_vProfileLabels.size()) )
        return QObject::tr("");
    return m_vProfileLabels.at(nProfileIndex);
}

void QProfileLabel::SetLabel( int nProfileIndex, QString& strNewLabel )
{
    if( nProfileIndex < 0 || nProfileIndex >= m_vProfileLabels.size() )
        return;

    m_vProfileLabels.replace( nProfileIndex, strNewLabel );
}
