#ifndef QT3KLOADDETECTIONOBJECT_H
#define QT3KLOADDETECTIONOBJECT_H

#include <QObject>
#include "QT3kHIDObject.h"


class QTimer;

class QT3kLoadDetectionObject : public QT3kHIDObject
{
    Q_OBJECT
public:
    explicit QT3kLoadDetectionObject(QT3kDeviceR*& pHandle, QObject *parent = 0);
    virtual ~QT3kLoadDetectionObject();

    // QT3kHIDObject
    virtual bool Start(SensorLogData *pStorage);

protected:
    // QT3kDeviceREventHandler::IListener
    virtual void TPDP_OnIRD(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int total, int offset, const unsigned char *data, int cnt);
    virtual void TPDP_OnITD(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int total, int offset, const unsigned char *data, int cnt);

protected:
    void InitCamDataForDetection();
    void InitDetectionData();

protected:
    QTimer                      m_DetectionWaitTimer;
    bool                        m_bMainCamDetection;

    SensorLogData*              m_pStorageHandle;

    typedef struct _DetectionData
    {
        uchar*                  pIRD;
        uchar*                  pITD;
        int                     nIRD;
    } DetectionData;

    QVector<DetectionData>      m_vDetectionData;

signals:

public slots:
    void on_DetectionCheckTimer();
};

#endif // QT3KLOADDETECTIONOBJECT_H
