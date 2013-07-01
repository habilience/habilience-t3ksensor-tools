#pragma once

#include <T30xHandle.h>

#define PROMPT_MAX  3

class CHIDCmdThread : public CT30xHandle::ITPDPNotify
{
protected:
	HANDLE		m_hExitEvent;
	HANDLE		m_hThread;

	CT30xHandle * m_pT30xHandle;

	CRITICAL_SECTION	m_csTextOut;

	char m_szInstantMode[100];

public:
	void Start();
	void Stop();

	BOOL SendCommand( char * szCmd );

public:
	enum Prompt
	{
		promptNil,
		promptSystemTime,
		promptT3000Time,
	};
	Prompt m_prompt[PROMPT_MAX];
	void AddPrompt( Prompt p );

public:
	char ** m_ppPreCommands;
	int     m_nPreCommands;
	void AddPreCommand( const char * szCmd );
	void FlushPreCommand();

public:
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
		enCM1,
	} m_eGetNv;
	void GetNv( BOOL bFactory, const char * szFile );
	BOOL GetNv_SendNext( BOOL bEndOfGetNv );

protected:
	UINT_PTR m_tmStart;
	BOOL m_bTextOut;
	BOOL m_bInstantMode;
public:
	void LockTextOut();
	void UnlockTextOut();
	void TextOut( DWORD ticktime, const char * szFormat, ... );
	void TextOutRuntime( const char * szCmd, UINT_PTR time = -1, DWORD ticktime = -1 );

	inline UINT_PTR GetCurrentSystime()
	{
		FILETIME f;
		::GetSystemTimeAsFileTime(&f);
		unsigned __int64 t;
		t = f.dwLowDateTime | (((unsigned __int64)f.dwHighDateTime) << 32);
		return (UINT_PTR)(t / 10000);
	}

protected:
	BOOL				m_bIsConnect;
	BOOL IsHIDConnect() { return m_bIsConnect; }

	BOOL OpenT30xHandle();

	DWORD				m_dwTimeCheck;

	void OnDeviceConnected(T3K_DEVICE_INFO devInfo);
	void OnDeviceDisconnected(T3K_DEVICE_INFO devInfo);

	// CT30xHandle::ITPDPNotify
	virtual void TPDP_OnConnected( T3K_DEVICE_INFO devInfo );
	virtual void TPDP_OnDisconnected( T3K_DEVICE_INFO devInfo );
	virtual void TPDP_OnMSG( T3K_DEVICE_INFO devInfo, WORD ticktime, LPCSTR partid, const char * txt );
	virtual void TPDP_OnRSP( T3K_DEVICE_INFO devInfo, WORD ticktime, LPCSTR partid, int id, BOOL bFinal, const char * cmd );
	virtual void TPDP_OnSTT( T3K_DEVICE_INFO devInfo, WORD ticktime, LPCSTR partid, const char * status );
	virtual void TPDP_OnVER( T3K_DEVICE_INFO devInfo, WORD ticktime, LPCSTR partid, t3kpacket::_body::_ver* ver );

	static unsigned int __stdcall _OnThread( LPVOID lpParam );
	void OnThread();
public:
	CHIDCmdThread(void);
	~CHIDCmdThread(void);
};
