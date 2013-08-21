#pragma once
#include "afxwin.h"


// DSelectModel 대화 상자입니다.

class DSelectModel : public CDialog
{
	DECLARE_DYNAMIC(DSelectModel)

public:
	DSelectModel(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~DSelectModel();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SELECT_MODEL };

	LPCTSTR GetPanelPathName();

protected:

	CString				m_strPanelPathName;
	CArray<CString>		m_aryModelFileNames;

	CFont m_fntList;

	void RefreshList();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnDelete();
	virtual BOOL OnInitDialog();
protected:
	CListBox m_lstModelName;
public:
	afx_msg void OnLbnDblclkListModel();
	afx_msg void OnLbnSelchangeListModel();
};
