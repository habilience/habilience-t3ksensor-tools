#ifndef QT3KDEVICEEVENTHANDLER_H
#define QT3KDEVICEEVENTHANDLER_H

#include <QObject>
#include <QVector>
#include <T3kHIDLib.h>

enum ResponsePart { MM, CM1, CM2, CM1_1, CM2_1 };

class QT3kDeviceEventHandler : public QObject
{
    Q_OBJECT
public:
    class IListener
    {
    public:
        IListener() { QT3kDeviceEventHandler::getPtr()->addListener(this); }
        ~IListener() { QT3kDeviceEventHandler::getPtr()->removeListener(this); }

        virtual void TPDP_OnDisconnected( T3K_DEVICE_INFO /*devInfo*/ ) {}
        virtual void TPDP_OnDownloadingFirmware( T3K_DEVICE_INFO /*devInfo*/, bool /*bIsDownload*/ ) {}

        virtual void TPDP_OnMSG( T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short /*ticktime*/,
                                 const char* /*partid*/, const char * /*txt*/ ) {}
        virtual void TPDP_OnOBJ( T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short /*ticktime*/,
                                 const char* /*partid*/, unsigned char * /*layerid*/, float * /*start_pos*/, float * /*end_pos*/, int /*cnt*/ ) {}
        virtual void TPDP_OnOBC( T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short /*ticktime*/,
                                 const char* /*partid*/, unsigned char * /*layerid*/, float * /*start_pos*/, float * /*end_pos*/, int /*cnt*/ ) {}
        virtual void TPDP_OnDTC( T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short /*ticktime*/,
                                 const char* /*partid*/, unsigned char * /*layerid*/, unsigned long * /*start_pos*/, unsigned long * /*end_pos*/, int /*cnt*/ ) {}
        virtual void TPDP_OnIRD( T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short /*ticktime*/,
                                 const char* /*partid*/, int /*total*/, int /*offset*/, const unsigned char * /*data*/, int /*cnt*/ ) {}
        virtual void TPDP_OnITD( T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short /*ticktime*/,
                                 const char* /*partid*/, int /*total*/, int /*offset*/, const unsigned char * /*data*/, int /*cnt*/ ) {}
        virtual void TPDP_OnPRV( T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short /*ticktime*/,
                                 const char* /*partid*/, int /*total*/, int /*offset*/, const unsigned char * /*data*/, int /*cnt*/ ) {}
        virtual void TPDP_OnTXE( T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short /*ticktime*/,
                                 const char* /*partid*/, int /*err_bytes*/ ) {}
        virtual void TPDP_OnRXE( T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short /*ticktime*/,
                                 const char* /*partid*/, int /*err_bytes*/ ) {}
        virtual void TPDP_OnCMD( T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short /*ticktime*/,
                                 const char* /*partid*/, int /*id*/, const char * /*cmd*/ ) {}
        virtual void TPDP_OnRSP( T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short /*ticktime*/,
                                 const char* /*partid*/, int /*id*/, bool /*bFinal*/, const char * /*cmd*/ ) {}
        virtual void TPDP_OnSTT( T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short /*ticktime*/,
                                 const char* /*partid*/, const char * /*status*/ ) {}
        virtual void TPDP_OnDVC( T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short /*ticktime*/,
                                 const char* /*partid*/, t3kpacket::_body::_dvc * /*device*/ ) {}
        virtual void TPDP_OnTPT( T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short /*ticktime*/,
                                 const char* /*partid*/, int /*count*/, int /*guess_cnt*/, t3ktouchpoint* /*points*/ ) {}
        virtual void TPDP_OnGST( T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short /*ticktime*/,
                                 const char* /*partid*/, unsigned char /*cActionGroup*/, unsigned char /*cAction*/, unsigned short /*wFeasibleness*/,
                                 unsigned short /*x*/, unsigned short /*y*/, unsigned short /*w*/, unsigned short /*h*/, float /*fZoom*/, const char* /*msg*/ ) {}

    };

    class _gc
    {
    public:
        _gc();
        ~_gc();
    };
    friend class _gc;

private:
    static QT3kDeviceEventHandler* m_pThis;

protected:
    QVector<IListener*>      m_EventListener;

public:
    static QT3kDeviceEventHandler* getPtr();

    void addListener(IListener* l);
    void removeListener(IListener* l);

private:
    explicit QT3kDeviceEventHandler(QObject *parent=0);
    ~QT3kDeviceEventHandler();

public:
signals:

public slots:
};

ResponsePart getResponsPartFromPartId( const char* partid, const char* cmd );

#endif // QT3KDEVICEEVENTHANDLER_H
