#ifndef QT3KLOADSENSORDATAOBJECT_H
#define QT3KLOADSENSORDATAOBJECT_H

#include <QObject>

#include "QT3kHIDObject.h"
#include "LogDataDef.h"

class QT3kLoadSensorDataObject : public QT3kHIDObject
{
    Q_OBJECT
public:
    explicit QT3kLoadSensorDataObject(QT3kDevice*& pHandle, QObject *parent = 0);
    virtual ~QT3kLoadSensorDataObject();

    enum eDataPart { DP_CM1 = 1, DP_CM2, DP_CM1_1, DP_CM2_1, DP_MM, DP_END };

    // QT3kHIDObject
    virtual bool Start( SensorLogData* pStorage );

protected:
    void LoadNextCamData( eDataPart ePart );
    void NextCommand();

    void Push_Data(ResponsePart Part, PairRSP &stRSP, const char* sCmd);

protected:
    // QT3kDeviceEventHandler::IListener
    virtual void TPDP_OnRSP(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);
    virtual void TPDP_OnRSE(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);
    virtual void TPDP_OnVER(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, t3kpacket::_body::_ver *ver);

protected:
    long                        m_nNVIndex;
    bool                        m_bFactoryCmd;
    eDataPart                   m_eDataPart;

    SensorLogData*              m_pStorageHandle;

signals:

public slots:

};

#endif // QT3KLOADSENSORDATAOBJECT_H
