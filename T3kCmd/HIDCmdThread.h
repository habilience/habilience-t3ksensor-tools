#ifndef CHIDCMDTHREAD_H
#define CHIDCMDTHREAD_H

#include "T3kHIDNotify.h"

#include <QThread>
#include <QMutex>
#include <QFile>

#include <QEventLoop>

#define PROMPT_MAX  3

class CHIDCmdThread : public QThread, public T3kHIDNotify::IT3kEventListener
{
    Q_OBJECT

public:
    explicit CHIDCmdThread(QObject *parent = 0);
    virtual ~CHIDCmdThread();

    void Start();
    void Stop();

    QEventLoop* loop;

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
    T3kHandle*  m_pT3kHandle;

    QMutex      m_csTextOut;

	char m_szInstantMode[100];

    uint m_tmStart;
    bool m_bTextOut;
    bool m_bInstantMode;

    bool				m_bIsConnect;
    bool IsHIDConnect() { return m_bIsConnect; }

    bool OpenT3kHandle();

    ulong				m_dwTimeCheck;

    void OnDeviceConnected(T3K_HANDLE hDevice);
    void OnDeviceDisconnected(T3K_HANDLE hDevice);

    // T3kHIDNotify::IT3kEventListener
    virtual void OnOpenT3kDevice(T3K_HANDLE hDevice);
    virtual void OnCloseT3kDevice(T3K_HANDLE hDevice);
    virtual void OnMSG(short, const char *, const char *);
    virtual void OnRSP(short, const char *, long, bool, const char *);
    virtual void OnRSE(short, const char *, long, bool, const char *);
    virtual void OnSTT(short, const char *, const char *);
    virtual void OnVER(short, const char *, T3kVER &);

    virtual int onReceiveRawData(void* /*pContext*/) { return 0; }
    virtual void onReceiveRawDataFlag(bool /*bReceive*/) {}

    // QThread
    virtual void run();

    void OnThread();

signals:
    void ProcessEvents();

public slots:
    void onStop();
    void onProcessEvents();
};

#endif //CHIDCMDTHREAD_H
