#ifndef QSENSORINITDATACFG_H
#define QSENSORINITDATACFG_H

#include <QString>
#include <QDateTime>
#include <QVector>
#include <QMap>

class QSensorInitDataCfg
{
public:
    class Enumerator
    {
    public:
        Enumerator() {}
    protected:
        struct stCfgData
        {
            QString     strFileName;
            QDateTime   tmDate;
        };
        QVector<stCfgData> m_aryCfgData;
    public:
        void enumCfgData();
        int getCfgDataCount();
        bool getCfgDataAt( int nIndex, QString& strFileName, QDateTime& tmDate );
        bool findCfgData( const QString& strFileName );
    };

    class _gc
    {
    public:
        _gc();
        ~_gc();
    };
    friend class _gc;

    struct SoftkeyData
    {
        QString strKey;
        QString strKeyBind;
        QString strGPIO;
        QString strLogic;
    };

public:
    static QSensorInitDataCfg* instance();

    Enumerator* getEnumerator() { return &m_Enumerator; }

    bool load( const QString& strDataFileName );
    bool save( const QString& strDataFileName );

    void reset();

    bool isLoaded() const { return m_bIsLoaded; }
    bool isSoftkeyLoaded() const { return m_bIsSoftkeyLoaded; }
    const SoftkeyData& getSoftkeyData() const { return m_SoftkeyData; }
    const QString& getPrevLoadedFileName() const { return m_strPrevFileName; }
    const QString& getFileName() const { return m_strFileName; }

    bool getValue(const QString &strCmd, QString &strValue );
    bool setValue(const QString &strCmd, const QString &strValue );

    int getItemCount();
    bool getItemAt( int nIndex, QString& strCmd, QString& strValue );

protected:
    bool loadSoftkeyFile( QString& strSoftkeyPathName );

    Enumerator m_Enumerator;
    bool m_bIsLoaded;
    bool m_bIsSoftkeyLoaded;

    SoftkeyData m_SoftkeyData;

    QVector<QString>    m_aryCmd;
    QMap<QString, QString> m_mapValue;
    QString m_strPrevFileName;
    QString m_strFileName;

private:
    QSensorInitDataCfg();
    ~QSensorInitDataCfg();

    static QSensorInitDataCfg* m_pThis;
};

#endif // QSENSORINITDATACFG_H
