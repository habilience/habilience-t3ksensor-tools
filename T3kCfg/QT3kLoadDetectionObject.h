#ifndef QT3KLOADDETECTIONOBJECT_H
#define QT3KLOADDETECTIONOBJECT_H

#include <QObject>
#include "QT3kHIDObject.h"


class QTimer;

class QT3kLoadDetectionObject : public QT3kHIDObject
{
    Q_OBJECT
public:
    explicit QT3kLoadDetectionObject(T3kHandle*& pHandle, QObject *parent = 0);
    virtual ~QT3kLoadDetectionObject();

    // QT3kHIDObject
    virtual bool Start(SensorLogData *pStorage);

    // TPDPEventMultiCaster::ITPDPEventListener
protected:
    virtual void OnIRD(ResponsePart, ushort, const char *, int, int, unsigned char *);
    virtual void OnITD(ResponsePart, ushort, const char *, int, int, unsigned char *);

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
