#ifndef QREQUESTHIDMANAGER_H
#define QREQUESTHIDMANAGER_H

#include <QList>
#include <QString>
#include <QObject>
#include "T3kHandle.h"
#include <QMutex>

class QRequestHIDManager : public QObject
{
    Q_OBJECT

public:
    QRequestHIDManager(QObject* parent = NULL);
    ~QRequestHIDManager();

    enum eRequestPart { MM = 0, CM1, CM2, CM1_1, CM2_1 };

    struct NVData
    {
        eRequestPart	ePart;
        QString         strItem;
        QString         strValue;
    };

    void Start( T3kHandle* pHandle );
    void Pause();
    void Stop();

    void AddItem( const char* szItem, QString strValue, eRequestPart ePart = QRequestHIDManager::MM );
    void RemoveItem( const char* szItem, eRequestPart ePart = QRequestHIDManager::MM );

protected:
    virtual void timerEvent(QTimerEvent *);

protected:
    QList<NVData>		m_RequestItem;
    QMutex                      m_Mutex;

    int                         m_nTimer;

    T3kHandle*                 m_pT3kHandle;

signals:

public slots:

};

#endif // QREQUESTHIDMANAGER_H
