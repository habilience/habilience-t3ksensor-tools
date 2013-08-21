#ifndef QT3KLOADSIDEVIEWOBJECT_H
#define QT3KLOADSIDEVIEWOBJECT_H

#include <QObject>
#include "QT3kHIDObject.h"

#include "LogDataDef.h"


class QT3kLoadSideviewObject : public QT3kHIDObject
{
    Q_OBJECT
public:
    explicit QT3kLoadSideviewObject(T3kHandle*& pHandle, QObject *parent = 0);
    virtual ~QT3kLoadSideviewObject();

    void SetSavePath( QString strSavePath ) { m_strSavePath = strSavePath; }

    // QT3kHIDObject
    virtual bool Start(SensorLogData *pStorage);


protected:
    void NextSideView(int nCamNo);
    void FinishSideView();

    void FillEmptySideview(QImage *pImg, int nWidth, int nHeight);

    // TPDPEventMultiCaster::ITPDPEventListener
protected:
    virtual void OnPRV(ResponsePart, ushort, const char *, int, int, int, unsigned char *);
    virtual void OnRSE(ResponsePart, ushort, const char *, long, bool, const char *);

protected:
    QString             m_strSavePath;

    int                 m_nLoadCamNo;

    // Data
    QImage*             m_pSideViewImage;
    int                 m_nCountAskCam;

    SensorLogData*      m_pStorageHandle;

signals:

public slots:
    void on_CheckTimeOut();
};

#endif // QT3KLOADSIDEVIEWOBJECT_H
