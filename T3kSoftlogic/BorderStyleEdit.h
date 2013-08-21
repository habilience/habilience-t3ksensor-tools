#pragma once


// CBorderStyleEdit

#define WM_EDIT_MODIFIED		(WM_USER+0xED)

class CBorderStyleEdit : public CEdit
{
	DECLARE_DYNAMIC(CBorderStyleEdit)
protected:
	CBrush	m_BgBrush;
	COLORREF	m_dwTextColor;
	COLORREF	m_dwActiveTextColor;
	COLORREF	m_dwBorderColor;
	COLORREF	m_dwBgColor;

	BOOL		m_bShowBorder;
	UINT		m_nBorderWidth;

	BOOL		m_bModified;
	double		m_dOldValue;
	CString		m_strOldValue;

	BOOL		m_bTextStyle;
	BOOL		m_bFloatStyle;

	double		m_dValueRangeMin;
	double		m_dValueRangeMax;

	BOOL		m_bNowEditing;

	CString		m_strAllowString;
	CString		m_strDeniedString;

	UINT_PTR	m_nWarningTimer;
	BOOL		m_bBlink;
	BOOL		m_bWarning;

	void DrawEditFrame( CDC* pDC );

public:
	CBorderStyleEdit();
	virtual ~CBorderStyleEdit();

	void SetColor( COLORREF dwBorderColor, COLORREF dwBgColor );
	void SetFloatStyle( BOOL bFloatStyle, BOOL bAllowMinus=TRUE );
	void SetPathNameStyle();
	void SetRange( double dMin, double dMax ) { m_dValueRangeMin = dMin, m_dValueRangeMax = dMax; }

	void SetWarning( BOOL bWarning );

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
	afx_msg void OnPaint();
	afx_msg void OnKillfocus();
	afx_msg void OnSetfocus();

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnNcPaint();
	afx_msg void OnUpdateUIState(UINT /*nAction*/, UINT /*nUIElement*/);
};


