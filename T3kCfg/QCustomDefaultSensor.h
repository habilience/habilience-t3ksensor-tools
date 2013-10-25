#ifndef QCUSTOMDEFAULTSENSOR_
#define QCUSTOMDEFAULTSENSOR_

#include <QObject>
#include <QVector>

class QCustomDefaultSensor : public QObject
{
    Q_OBJECT
public:
    explicit QCustomDefaultSensor(QObject *parent = 0);

    static QCustomDefaultSensor* Instance(bool bReload = false);

    bool IsLoaded() { return m_bLoaded; }

    // T30xConstStr.h
    QString GetDefaultData( const char* pContStr, QString strDefault );

    bool Reload();

protected:
    static QCustomDefaultSensor* s_pInstance;

    QVector<QString>                m_vCommandStr;
    bool                            m_bLoaded;
signals:

public slots:

};

inline QCustomDefaultSensor* QCustomDefaultSensor::Instance(bool bReload/*=false*/)
{
    if( !s_pInstance )
        s_pInstance = new QCustomDefaultSensor();

    if( bReload )
        s_pInstance->Reload();
    return s_pInstance;
}

#endif // QCUTOMDEFAULTSENSOR_
