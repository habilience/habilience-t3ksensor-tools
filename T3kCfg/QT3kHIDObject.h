#ifndef QT3KHIDOBJECT_H
#define QT3KHIDOBJECT_H

#include "TPDPEventMultiCaster.h"
#include "QLangManager.h"

#include "LogDataDef.h"

#include <QObject>
#include <QTimer>

#define MAX_CAM_COUNT           4

#define LDS_SIDEVIEW        0
#define LDS_DETECTION       1
#define LDS_SENSORDATA      2
#define LDS_ENVIRONMENT     3
#define LDS_SAVE            4

class QT3kHIDObject : public QObject, public QLangManager::ILangChangeNotify, public TPDPEventMultiCaster::ITPDPEventListener
{
    Q_OBJECT
public:
    explicit QT3kHIDObject(T3kHandle*& pHandle, QObject *parent = 0);
    virtual ~QT3kHIDObject();

    virtual bool Start( SensorLogData* pStorage ) = 0;

    // QLangManager::LangChangeNotify
protected:
    virtual void onChangeLanguage();

protected:
    void StartAsyncTimeoutChecker( long lTimeOut );
    void StopAsyncTimeoutChecker();

protected:
    T3kHandle*&        m_pT3kHandle;

    QTimer              m_Timer;
    QString             m_strSendCmd;
    int                 m_nSendCmdID;
    int                 m_nTimeOutCount;

    bool                m_bStart;

signals:
    void Complete( int nStep );
    void PrintProgreeLog( QString strLog, bool bError = false );
    void CheckTimeOut();

public slots:

protected slots:
    void on_AsyncTimeout();
};

#endif // QT3KHIDOBJECT_H
