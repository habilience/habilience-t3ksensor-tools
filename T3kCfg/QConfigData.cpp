#include "QConfigData.h"

#include <QSettings>
#include <QFile>

QConfigData::QConfigData(QObject *parent) :
    QObject(parent)
{
    m_pSettings = NULL;
}

QConfigData::~QConfigData()
{
    if( m_pSettings )
    {
        delete m_pSettings;
        m_pSettings = NULL;
    }
}

bool QConfigData::load(QString strPathFile)
{
    if( !QFile::exists( strPathFile ) ) return false;

    if( m_pSettings )
    {
        delete m_pSettings;
        m_pSettings = NULL;
    }

    m_pSettings = new QSettings( strPathFile, QSettings::IniFormat );
    m_pSettings->setIniCodec( "UTF-8" );

    return true;
}

QVariant QConfigData::getData(QString strGroup, QString strKey, QVariant defaultValue)
{
    if( !m_pSettings ) return defaultValue;

    QVariant value;

    if( strGroup.isEmpty() )
    {
        value = m_pSettings->value( strKey, defaultValue );
    }
    else
    {
        m_pSettings->beginGroup( strGroup );

        value = m_pSettings->value( strKey, defaultValue );

        m_pSettings->endGroup();
    }

    return value;
}
