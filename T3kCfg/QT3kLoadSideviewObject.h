#ifndef QT3KLOADSIDEVIEWOBJECT_H
#define QT3KLOADSIDEVIEWOBJECT_H

#include <QObject>
#include "QT3kHIDObject.h"

#include "LogDataDef.h"


class QT3kLoadSideviewObject : public QT3kHIDObject
{
    Q_OBJECT
public:
    explicit QT3kLoadSideviewObject(QT3kDevice*& pHandle, QObject *parent = 0);
    virtual ~QT3kLoadSideviewObject();

    void SetSavePath( QString strSavePath ) { m_strSavePath = strSavePath; }

    // QT3kHIDObject
    virtual bool Start(SensorLogData *pStorage);

protected:
    void NextSideView(int nCamNo);
    void FinishSideView();

    void FillEmptySideview(QImage *pImg, int nWidth, int nHeight);

protected:
    // QT3kDeviceEventHandler::IListener
    virtual void TPDP_OnPRV(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int total, int offset, const unsigned char *data, int cnt);
    virtual void TPDP_OnRSE(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);

protected:
    QString             m_strSavePath;

    int                 m_nLoadCamNo;

    // Data
    QImage*             m_pSideViewImage;
    int                 m_nCountAskCam;

    uchar*              m_pImageBuffer;
    uchar*              m_pCheckBufferY;

    SensorLogData*      m_pStorageHandle;

signals:

public slots:
    void on_CheckTimeOut();
};

#endif // QT3KLOADSIDEVIEWOBJECT_H
