// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0500		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0500		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0500 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0400	// Change this to the appropriate value to target IE 5.0 or later.
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <T30xConstStr.h>

#include <TPDPEventMulticaster.h>
#include "wIni.h"

CT30xHandle* GetT30xHandle();

enum ScreenUnit { UnitRes, UnitMM };


#if 1
#define DEFAULT_SOFTKEY		_T("000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000")
#define DEFAULT_SOFTLOGIC	_T("00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000")
#else
#define DEFAULT_SOFTKEY		_T("@>Co:X:1_oM@[<o:XA?ooMA84o:XHMooMATmo:XO\\OoMB1fo:XVj_oMBN`o:X^8ooMBkXo:XeFooMCHRo:XlUOoM0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000")
#define DEFAULT_SOFTLOGIC	_T("10P02T@0Q010H0R00PH0S00;@0T0<GU0U02\\I0V03XE`W0<CW@W00C`0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000P8LCW")
#endif

#define DEV_COORD		(0x7fff)

CString GetExecuteDirectory( void );
void MakeDirectory( LPCTSTR lpszDir );
BOOL CALLBACK MonitorEnumProc( HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData );

#define LBUTTON		(0x01)
#define RBUTTON		(0x02)
#define MBUTTON		(0x04)

#define USE_SOFTLOGIC_OUTPUT_MOUSE
#define USE_SOFTLOGIC_OUTPUT_AUDIO_CONTROL

// for consumer device

#define V_KEY_VOLUME_INC_VALUE		0x74
#define V_KEY_VOLUME_DEC_VALUE		0x75
#define V_KEY_VOLUME_MUTE_VALUE		0x76
#define V_KEY_TRANSPORT_PLAY		0x77
#define V_KEY_TRANSPORT_PAUSE		0x78
#define V_KEY_TRANSPORT_FFORWARD	0x79
#define V_KEY_TRANSPORT_REWIND		0x80
#define V_KEY_TRANSPORT_NEXT_TRACK	0x81
#define V_KEY_TRANSPORT_PREV_TRACK	0x82
#define V_KEY_TRANSPORT_STOP		0x83
#define V_KEY_TRANSPORT_EJECT		0x84
#define V_KEY_TRANSPORT_PLAYPAUSE	0x85

#define V_KEY_AL_EMAIL_VIEWER		0x95
#define V_KEY_AL_CALCULATOR			0x96
#define V_KEY_AL_MY_COMPUTER		0x97
#define V_KEY_AL_WEB_BROWSER		0x98

#include "../T3000_ver.h"
#define VERSION T3000_VERSION

CString GetProgVersion( void );
