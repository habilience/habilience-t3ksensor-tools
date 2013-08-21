#pragma once

#include "GdiplusExt.h"

// CUnderlineTitleStatic

class CUnderlineTitleStatic : public CStatic
{
	DECLARE_DYNAMIC(CUnderlineTitleStatic)

protected:
	Image*	m_pImageIcon;
public:
	void SetIconImage( HMODULE hModule, LPCTSTR lpszResType, UINT nIDResPNG );
	void SetWindowText( LPCTSTR lpszNewText );
	CUnderlineTitleStatic();
	virtual ~CUnderlineTitleStatic();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnUpdateUIState(UINT /*nAction*/, UINT /*nUIElement*/);
};


