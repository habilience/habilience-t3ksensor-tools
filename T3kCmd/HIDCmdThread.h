#ifndef CHIDCMD_H
#define CHIDCMD_H

#include "../common/TPDPEventMultiCaster.h"

#include <QObject>
#include <QMutex>
#include <QFile>
#include <QFuture>

#define PROMPT_MAX  3


class CHIDCmd : public QObject, public TPDPEventMultiCaster::ITPDPEventListener
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

    void OnDeviceConnected(T3K_HANDLE hDevice);
    void OnDeviceDisconnected(T3K_HANDLE hDevice);

    // T3kHIDNotify::IT3kEventListener
    virtual void OnOpenT3kDevice(T3K_HANDLE hDevice);
    virtual void OnCloseT3kDevice(T3K_HANDLE hDevice);
    virtual void OnMSG(ResponsePart, ushort, const char *, const char *);
    virtual void OnRSP(ResponsePart, ushort, const char *, long, bool, const char *);
    virtual void OnRSE(ResponsePart, ushort, const char *, long, bool, const char *);
    virtual void OnSTT(ResponsePart, ushort, const char *, const char *);
    virtual void OnVER(ResponsePart, ushort, const char *, T3kVER &);

protected:
    T3kHandle*          m_pT3kHandle;
    QMutex              m_csTextOut;

    char                m_szInstantMode[100];

    uint                m_tmStart;
    bool                m_bTextOut;
    bool                m_bInstantMode;

    bool				m_bIsConnect;
    ulong				m_dwTimeCheck;

    QFuture<void>       m_ftCmdThread;

signals:

public slots:

};

#endif //CHIDCMD_H
