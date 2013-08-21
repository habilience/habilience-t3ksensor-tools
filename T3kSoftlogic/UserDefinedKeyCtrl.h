#pragma once

#include "GdipWndExt.h"

#include "KeyEdit.h"

#define			WM_KEYEDIT_CHANGE		(WM_USER+100)

// CUserDefinedKeyCtrl

class CUserDefinedKeyCtrl : public UIExt::CGdipStatic
{
	DECLARE_DYNAMIC(CUserDefinedKeyCtrl)

public:
	enum KeyType { KeyTypeNormal, KeyTypeFunction, KeyTypeMouse };

	CUserDefinedKeyCtrl();
	virtual ~CUserDefinedKeyCtrl();

	void Reset();
	void SetKeyValue( WORD wKeyValue );
	WORD GetKeyValue();

	void SetKeyType( KeyType eType );
	KeyType GetKeyType() const { return m_eKeyType; }

protected:
	KeyType		m_eKeyType;

	WORD		m_wKeyValue;
	CKeyEdit	m_wndEdit;
	CComboBox	m_wndComboFuncKey;
	CComboBox	m_wndComboMouse;

	Image*		m_pIconBtn[4];
	WORD		m_wIconKeyValue[4];

	Image*		m_pIconClearBtn;
	Rect		m_rectIconClearBtn;

	Rect		m_rectIconBtn[4];

	int			m_nIconTotalWidth;
	int			m_nIconTotalHeight;

	void _SetMouseValue( WORD wKeyValue );
	void _SetFunctionKeyValue( WORD wKeyValue );

	virtual void OnDraw( Graphics& g );
	virtual void OnGdipEraseBkgnd( Graphics& g );

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnKeyValueChange( WPARAM wParam, LPARAM lParam );
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnCbnSelchangeComboFuncKey();
	afx_msg void OnCbnSelchangeComboMouse();
	afx_msg void OnUpdateUIState(UINT /*nAction*/, UINT /*nUIElement*/);
};


