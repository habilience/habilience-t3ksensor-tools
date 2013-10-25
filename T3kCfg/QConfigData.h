#ifndef QCONFIGDATA_H
#define QCONFIGDATA_H

#include <QObject>
#include "../common/QSingletone.h"

class QSettings;
class QConfigData : public QObject, public QSingleton<QConfigData>
{
    Q_OBJECT
public:
    explicit QConfigData(QObject *parent = 0);
    virtual ~QConfigData();

    bool load(QString strPathFile);

    QVariant getData(QString strGroup, QString strKey, QVariant defaultValue);

protected:
    QSettings*      m_pSettings;

signals:

public slots:

};

#endif // QCONFIGDATA_H
