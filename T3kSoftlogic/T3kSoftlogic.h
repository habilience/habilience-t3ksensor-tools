// T3kSoftlogic.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// CT3kSoftlogicApp:
// See T3kSoftlogic.cpp for the implementation of this class
//

class CT3kSoftlogicApp : public CWinApp
{
public:
	CT3kSoftlogicApp();

	BOOL m_bScreenShotMode;
protected:
//	void RegisterExt();
//	BOOL IsOS64();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

extern CT3kSoftlogicApp theApp;