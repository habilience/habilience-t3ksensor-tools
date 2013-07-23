#ifndef QT3KDEVICEEVENTHANDLER_H
#define QT3KDEVICEEVENTHANDLER_H

#include <QObject>
#include <QVector>
#include <T3kHIDLib.h>
#include "QSingletone.h"

enum ResponsePart { MM, CM1, CM2, CM1_1, CM2_1 };

#define UNUSE_ARG(xxx); xxx=xxx

class QT3kDevice;
class QT3kDeviceEventHandler : public QObject, public QSingleton<QT3kDeviceEventHandler>
{
    Q_OBJECT
public:
    class IListener
    {
    public:
        IListener() { QT3kDeviceEventHandler::instance()->addListener(this); }
        ~IListener() { QT3kDeviceEventHandler::instance()->removeListener(this); }

        virtual void TPDP_OnDisconnected( T3K_DEVICE_INFO devInfo )
            { UNUSE_ARG(devInfo); }
        virtual void TPDP_OnDownloadingFirmware( T3K_DEVICE_INFO devInfo, bool bIsDownload )
            { UNUSE_ARG(devInfo); UNUSE_ARG(bIsDownload); }

        virtual void TPDP_OnMSG( T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime,
                                 const char* partid, const char * txt )
            { UNUSE_ARG(devInfo); UNUSE_ARG(Part); UNUSE_ARG(ticktime); UNUSE_ARG(partid); UNUSE_ARG(txt); }
        virtual void TPDP_OnOBJ( T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime,
                                 const char* partid, unsigned char * layerid, float * start_pos, float * end_pos, int cnt )
            { UNUSE_ARG(devInfo); UNUSE_ARG(Part); UNUSE_ARG(ticktime); UNUSE_ARG(partid); UNUSE_ARG(layerid); UNUSE_ARG(start_pos); UNUSE_ARG(end_pos); UNUSE_ARG(cnt); }
        virtual void TPDP_OnOBC( T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime,
                                 const char* partid, unsigned char * layerid, float * start_pos, float * end_pos, int cnt )
            { UNUSE_ARG(devInfo); UNUSE_ARG(Part); UNUSE_ARG(ticktime); UNUSE_ARG(partid); UNUSE_ARG(layerid); UNUSE_ARG(start_pos); UNUSE_ARG(end_pos); UNUSE_ARG(cnt); }
        virtual void TPDP_OnDTC( T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime,
                                 const char* partid, unsigned char * layerid, unsigned long * start_pos, unsigned long * end_pos, int cnt )
            { UNUSE_ARG(devInfo); UNUSE_ARG(Part); UNUSE_ARG(ticktime); UNUSE_ARG(partid); UNUSE_ARG(layerid); UNUSE_ARG(start_pos); UNUSE_ARG(end_pos); UNUSE_ARG(cnt); }
        virtual void TPDP_OnIRD( T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime,
                                 const char* partid, int total, int offset, const unsigned char * data, int cnt )
            { UNUSE_ARG(devInfo); UNUSE_ARG(Part); UNUSE_ARG(ticktime); UNUSE_ARG(partid); UNUSE_ARG(total); UNUSE_ARG(offset); UNUSE_ARG(data); UNUSE_ARG(cnt); }
        virtual void TPDP_OnITD( T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime,
                                 const char* partid, int total, int offset, const unsigned char * data, int cnt )
            { UNUSE_ARG(devInfo); UNUSE_ARG(Part); UNUSE_ARG(ticktime); UNUSE_ARG(partid); UNUSE_ARG(total); UNUSE_ARG(offset); UNUSE_ARG(data); UNUSE_ARG(cnt); }
        virtual void TPDP_OnPRV( T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime,
                                 const char* partid, int total, int offset, const unsigned char * data, int cnt )
            { UNUSE_ARG(devInfo); UNUSE_ARG(Part); UNUSE_ARG(ticktime); UNUSE_ARG(partid); UNUSE_ARG(total); UNUSE_ARG(offset); UNUSE_ARG(data); UNUSE_ARG(cnt); }
        virtual void TPDP_OnTXE( T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime,
                                 const char* partid, int err_bytes )
            { UNUSE_ARG(devInfo); UNUSE_ARG(Part); UNUSE_ARG(ticktime); UNUSE_ARG(partid); UNUSE_ARG(err_bytes); }
        virtual void TPDP_OnRXE( T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime,
                                 const char* partid, int err_bytes )
            { UNUSE_ARG(devInfo); UNUSE_ARG(Part); UNUSE_ARG(ticktime); UNUSE_ARG(partid); UNUSE_ARG(err_bytes); }
        virtual void TPDP_OnCMD( T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime,
                                 const char* partid, int id, const char * cmd )
            { UNUSE_ARG(devInfo); UNUSE_ARG(Part); UNUSE_ARG(ticktime); UNUSE_ARG(partid); UNUSE_ARG(id); UNUSE_ARG(cmd); }
        virtual void TPDP_OnRSP( T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime,
                                 const char* partid, int id, bool bFinal, const char * cmd )
            { UNUSE_ARG(devInfo); UNUSE_ARG(Part); UNUSE_ARG(ticktime); UNUSE_ARG(partid); UNUSE_ARG(id); UNUSE_ARG(bFinal); UNUSE_ARG(cmd); }
        virtual void TPDP_OnSTT( T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime,
                                 const char* partid, const char * status )
            { UNUSE_ARG(devInfo); UNUSE_ARG(Part); UNUSE_ARG(ticktime); UNUSE_ARG(partid); UNUSE_ARG(status); }
        virtual void TPDP_OnDVC( T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime,
                                 const char* partid, t3kpacket::_body::_dvc * device )
            { UNUSE_ARG(devInfo); UNUSE_ARG(Part); UNUSE_ARG(ticktime); UNUSE_ARG(partid); UNUSE_ARG(device); }
        virtual void TPDP_OnTPT( T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime,
                                 const char* partid, int count, int guess_cnt, t3ktouchpoint* points )
            { UNUSE_ARG(devInfo); UNUSE_ARG(Part); UNUSE_ARG(ticktime); UNUSE_ARG(partid); UNUSE_ARG(count); UNUSE_ARG(guess_cnt); UNUSE_ARG(points); }
        virtual void TPDP_OnGST( T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime,
                                 const char* partid, unsigned char cActionGroup, unsigned char cAction, unsigned short wFeasibleness,
                                 unsigned short x, unsigned short y, unsigned short w, unsigned short h, float fZoom, const char* msg )
            { UNUSE_ARG(devInfo); UNUSE_ARG(Part); UNUSE_ARG(ticktime); UNUSE_ARG(partid);
              UNUSE_ARG(cActionGroup); UNUSE_ARG(cAction); UNUSE_ARG(wFeasibleness);
              UNUSE_ARG(x); UNUSE_ARG(y); UNUSE_ARG(w); UNUSE_ARG(h); UNUSE_ARG(fZoom); UNUSE_ARG(msg); }
        virtual void TPDP_OnVER( T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char* partid, t3kpacket::_body::_ver* ver )
            { UNUSE_ARG(devInfo); UNUSE_ARG(Part); UNUSE_ARG(ticktime); UNUSE_ARG(partid); UNUSE_ARG(ver); }

    };

protected:
    QVector<IListener*>      m_EventListener;

public:
    void addListener(IListener* l);
    void removeListener(IListener* l);


    friend class QT3kDevice;
protected:
    virtual void _onDisconnected( T3K_DEVICE_INFO devInfo );
    virtual void _onDownloadingFirmware( T3K_DEVICE_INFO devInfo, bool bIsDownload );

    virtual void _onMSG( T3K_DEVICE_INFO devInfo, unsigned short ticktime,
                         const char* partid, const char * txt );
    virtual void _onOBJ( T3K_DEVICE_INFO devInfo, unsigned short ticktime,
                         const char* partid, unsigned char * layerid, float * start_pos, float * end_pos, int cnt );
    virtual void _onOBC( T3K_DEVICE_INFO devInfo, unsigned short ticktime,
                         const char* partid, unsigned char * layerid, float * start_pos, float * end_pos, int cnt );
    virtual void _onDTC( T3K_DEVICE_INFO devInfo, unsigned short ticktime,
                         const char* partid, unsigned char * layerid, unsigned long * start_pos, unsigned long * end_pos, int cnt );
    virtual void _onIRD( T3K_DEVICE_INFO devInfo, unsigned short ticktime,
                         const char* partid, int total, int offset, const unsigned char * data, int cnt );
    virtual void _onITD( T3K_DEVICE_INFO devInfo, unsigned short ticktime,
                         const char* partid, int total, int offset, const unsigned char * data, int cnt );
    virtual void _onPRV( T3K_DEVICE_INFO devInfo, unsigned short ticktime,
                         const char* partid, int total, int offset, const unsigned char * data, int cnt );
    virtual void _onTXE( T3K_DEVICE_INFO devInfo, unsigned short ticktime,
                         const char* partid, int err_bytes );
    virtual void _onRXE( T3K_DEVICE_INFO devInfo, unsigned short ticktime,
                         const char* partid, int err_bytes );
    virtual void _onCMD( T3K_DEVICE_INFO devInfo, unsigned short ticktime,
                         const char* partid, int id, const char * cmd );
    virtual void _onRSP( T3K_DEVICE_INFO devInfo, unsigned short ticktime,
                         const char* partid, int id, bool bFinal, const char * cmd );
    virtual void _onSTT( T3K_DEVICE_INFO devInfo, unsigned short ticktime,
                         const char* partid, const char * status );
    virtual void _onDVC( T3K_DEVICE_INFO devInfo, unsigned short ticktime,
                         const char* partid, t3kpacket::_body::_dvc * device );
    virtual void _onTPT( T3K_DEVICE_INFO devInfo, unsigned short ticktime,
                         const char* partid, int count, int guess_cnt, t3ktouchpoint* points );
    virtual void _onGST( T3K_DEVICE_INFO devInfo, unsigned short ticktime,
                             const char* partid, unsigned char cActionGroup, unsigned char cAction, unsigned short wFeasibleness,
                             unsigned short x, unsigned short y, unsigned short w, unsigned short h, float fZoom, const char* msg );
    virtual void _onVER( T3K_DEVICE_INFO devInfo, unsigned short ticktime, const char* partid, t3kpacket::_body::_ver* ver );
public:
    explicit QT3kDeviceEventHandler(QObject *parent=0);
    ~QT3kDeviceEventHandler();

public:
signals:

public slots:
};

ResponsePart getResponsPartFromPartId( const char* partid, const char* cmd );

#endif // QT3KDEVICEEVENTHANDLER_H
