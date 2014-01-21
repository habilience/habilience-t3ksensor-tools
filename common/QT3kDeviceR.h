#ifndef QT3KDEVICEREMOTE_H
#define QT3KDEVICEREMOTE_H

#include "QT3kDevice.h"

class QT3kDeviceREventHandler;
class QT3kDeviceR : public QT3kDevice
{
    Q_OBJECT
public:
    explicit QT3kDeviceR(QObject *parent = 0);

    char* getRawDataPacket( int& nRetBytes );

    void setEventHandler( QT3kDeviceREventHandler* pHandler );
    void setBothSensorData( bool bBoth ) { m_bGetBothSensorData = bBoth ;}

protected:
    // Only RawData
    char*                                   m_pRawDataBuffer;
    int                                     m_nTotalRawBytes;
    QMutex                                  m_RawDataMutex;
    QQueue<unsigned short>                  m_qBytes;
    QQueue<char*>                           m_qBuffers;

    bool                                    m_bGetBothSensorData;
    QMutex                                  m_RemoteStatusMutex;
    bool                                    m_bRemoting;

    QT3kDeviceREventHandler*                m_pT3kREventHandler;

signals:
    int receiveRawData(void* pContext);
    void receiveRwDataFlag(bool);

public slots:
//    virtual void onDisconnected();
//    virtual void onDownloadingFirmware( bool bIsDownload );
    virtual int onReceiveRawData(unsigned char* pBuffer, unsigned short nBytes);
    virtual void onReceiveRawDataFlag(bool);

};

#endif // QT3KDEVICEREMOTE_H
