#pragma once


// CKeyEdit

#define			WM_KEYEDIT_VALUE_CHANGE		(WM_USER+200)

class CKeyEdit : public CEdit
{
	DECLARE_DYNAMIC(CKeyEdit)

public:
	CKeyEdit();
	virtual ~CKeyEdit();

	void Reset();
	void SetKeyValue( WORD wKeyValue );
	WORD GetKeyValue() { return m_wKeyValue; }

protected:
	WORD	m_wKeyValue;
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};


