#ifndef CHIDCMD_H
#define CHIDCMD_H

#include "QT3kDeviceEventHandler.h"

#include <QObject>
#include <QMutex>
#include <QFile>
#include <QFuture>

#define PROMPT_MAX  3


class CHIDCmd : public QObject, public QT3kDeviceEventHandler::IListener
{
    Q_OBJECT

public:
    CHIDCmd(QObject* parent = NULL);
    virtual ~CHIDCmd();

    void InitT3k();
    void EndT3k();
    bool ProcessT3k();

    void SetFutureHandle( QFuture<void> ft );

    bool SendCommand( char * szCmd );

    bool OnCommand( char* szCmd, bool* pbSysCmd = NULL );

    enum Prompt
    {
        promptNil,
        promptSystemTime,
        promptT3kTime
    };
    Prompt m_prompt[PROMPT_MAX];
    void AddPrompt( Prompt p );

    char ** m_ppPreCommands;
    int     m_nPreCommands;
    void AddPreCommand( const char * szCmd );
    void FlushPreCommand();

    FILE * m_pFileGetNv;
    int  m_bFactorialNv;
    int  m_nNvIdx;
    int  m_nGetNvId;
    enum tagGetNv
    {
        enFalse,
        enMM,
        enCM2c,
        enCM2,
        enCM1c,
        enCM1
    } m_eGetNv;
    void GetNv( bool bFactory, const char * szFile );
    bool GetNv_SendNext( bool bEndOfGetNv );

    void LockTextOut();
    void UnlockTextOut();
    void TextOutConsole( ulong ticktime, const char * szFormat, ... );
    void TextOutRuntime( const char * szCmd, uint time = -1, ulong ticktime = -1 );

protected:
    bool IsHIDConnect() { return m_bIsConnect; }

    bool OpenT3kHandle();

    void OnDeviceConnected();
    void OnDeviceDisconnected(T3K_DEVICE_INFO devInfo);

    // T3kHIDNotify::IT3kEventListener
    virtual void TPDP_OnDisconnected(T3K_DEVICE_INFO devInfo);
    virtual void TPDP_OnMSG(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, const char *txt);
    virtual void TPDP_OnRSP(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);
    virtual void TPDP_OnRSE(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);
    virtual void TPDP_OnSTT(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, const char *status);
    virtual void TPDP_OnVER(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, t3kpacket::_body::_ver *ver);

protected:
    QT3kDevice*         m_pT3kHandle;
    QMutex              m_csTextOut;

    char                m_szInstantMode[100];

    uint                m_tmStart;
    bool                m_bTextOut;
    bool                m_bInstantMode;

    bool				m_bIsConnect;
    ulong				m_dwTimeCheck;

    QFuture<void>       m_ftCmdThread;

signals:
    void connectedT3kDevice();

public slots:
    void onConnectedT3kDevice();

};

#endif //CHIDCMD_H
