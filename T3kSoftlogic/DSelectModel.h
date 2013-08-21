#pragma once
#include "afxwin.h"


// DSelectModel ��ȭ �����Դϴ�.

class DSelectModel : public CDialog
{
	DECLARE_DYNAMIC(DSelectModel)

public:
	DSelectModel(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~DSelectModel();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SELECT_MODEL };

	LPCTSTR GetPanelPathName();

protected:

	CString				m_strPanelPathName;
	CArray<CString>		m_aryModelFileNames;

	CFont m_fntList;

	void RefreshList();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
