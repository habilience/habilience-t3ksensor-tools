#ifndef QT3KLOADENVIRONMENTOBJECT_H
#define QT3KLOADENVIRONMENTOBJECT_H

#include <QObject>

#include "LogDataDef.h"

class QT3kLoadEnvironmentObject : public QObject
{
    Q_OBJECT
public:
    explicit QT3kLoadEnvironmentObject(QObject *parent = 0);

    static bool Start( SensorLogData* pStorage );

    //
    static QString GetOSDisplayString();
#ifdef Q_OS_WIN
    static bool IsServiceRunning( QString strServiceName );
    static QVector<PairRSP> GetHIDState();
    static unsigned long GetScreenOrientation();
#endif
signals:

public slots:

};

#endif // QT3KLOADENVIRONMENTOBJECT_H
