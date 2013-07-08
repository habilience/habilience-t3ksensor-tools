#ifndef INCLUDEREMOTENOTIFY_H
#define INCLUDEREMOTENOTIFY_H

#include "T3kHandle.h"

#include <QTcpSocket>

class IncludeRemoteNotify : public ITPDPT3kNotify
{
public:
    IncludeRemoteNotify();
    virtual ~IncludeRemoteNotify() {}

    void SendRemoteNotifyPacket(int nType);
    void SendRemoteRawDataPacket(int nType, const char* pData, qint64 nDataSize);

    void SetSocket(QTcpSocket* socket);

protected:
    // Remote
    QTcpSocket*         m_pSocket;
    bool                m_bRemoteMode;

public slots:
    virtual void onSensorDisconnect(T3K_HANDLE hDevice);
    virtual int onReceiveRawData(void* pContext);
    virtual void onReceiveRawDataFlag(bool bReceive);
    virtual void onDownloadingFirmware(int bDownload);
};



#endif // INCLUDEREMOTENOTIFY_H
