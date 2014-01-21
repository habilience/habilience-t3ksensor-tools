#ifndef QT3KDEVICEINCLUDEDREMOTE_H
#define QT3KDEVICEINCLUDEDREMOTE_H

#include "QT3kDeviceEventHandler.h"
#include "QT3kDeviceR.h"

#include <QTcpSocket>

class QT3kDeviceREventHandler : public QT3kDeviceEventHandler
{
public:
    QT3kDeviceREventHandler();
    virtual ~QT3kDeviceREventHandler() {}

    void sendRemoteNotifyPacket(int nType);
    void sendRemoteRawDataPacket(int nType, const char* pData, qint64 nDataSize);

    void setSocket(QTcpSocket* socket);

    friend class QT3kDeviceR;
protected:
    // QT3kDeviceEventHandler
    virtual void _onDisconnected( T3K_DEVICE_INFO devInfo );
    virtual void _onDownloadingFirmware( T3K_DEVICE_INFO devInfo, bool bIsDownload );

public:
    // Remote
    QTcpSocket*         m_pSocket;
    bool                m_bRemoteMode;

public slots:
    virtual int onReceiveRawData(void* pContext);
    virtual void onReceiveRawDataFlag(bool bReceive);
//    virtual void onDownloadingFirmware(int bDownload);
};



#endif // QT3KDEVICEINCLUDEDREMOTE_H
