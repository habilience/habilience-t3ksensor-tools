#ifndef QINIFORMAT_H
#define QINIFORMAT_H

#include <QFile>
#include <QMap>

// customize wIni format
class QIniFormat
{
public:
    QIniFormat(const QString strExtension);
    ~QIniFormat();

    bool isOpen() { return (m_pFile != NULL) && m_pFile->isOpen(); }

    bool open(const QString &fileName);
    bool save(const QString &fileName);

    void beginGroup( const QString strGroup );
    void endGroup();

    QString getValue( const QString strKey, const QString& strDefault = QString() );
    bool setValue( const QString strKey, const QString strValue );

protected:

    void syncIniFile();
    void syncMapData();
    void clearDatamap();

protected:
    typedef QMap<QString, QString>             IniDataGroup;
    typedef QMap<QString, IniDataGroup*>       IniDataMap;
    QFile*      m_pFile;

    IniDataMap      m_mapIniData;
    IniDataGroup*   m_pSelectedGroup;

    QString         m_strNewGroup;
    QString         m_strExtension;
};

#endif // QINIFORMAT_H
