#ifndef QREQUESTHIDMANAGER_H
#define QREQUESTHIDMANAGER_H

#include <QList>
#include <QString>
#include <QObject>
#include "QT3kDeviceEventHandler.h"
#include <QMutex>

class QRequestHIDManager : public QObject
{
    Q_OBJECT

public:
    QRequestHIDManager(QObject* parent = NULL);
    virtual ~QRequestHIDManager();

    struct NVData
    {
        ResponsePart	ePart;
        QString         strItem;
        QString         strValue;
    };

    void Start( QT3kDevice* pHandle );
    void Pause();
    void Stop();

    void AddItem( const char* szItem, QString strValue, ResponsePart ePart = MM );
    void RemoveItem( const char* szItem, ResponsePart ePart = MM );

protected:
    virtual void timerEvent(QTimerEvent *);

protected:
    QList<NVData>           m_RequestItem;
    QMutex                  m_Mutex;

    int                     m_nTimer;

    QT3kDevice*             m_pT3kHandle;
    bool                    m_bStart;

signals:
    void finish();

public slots:

};

#endif // QREQUESTHIDMANAGER_H
