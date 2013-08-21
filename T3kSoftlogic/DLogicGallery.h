#pragma once

#include "Softkey.h"
#include "afxwin.h"
// DLogicGallery dialog

class DLogicGallery : public CDialog
{
	DECLARE_DYNAMIC(DLogicGallery)

public:
	DLogicGallery(CWnd* pParent = NULL);   // standard constructor
	virtual ~DLogicGallery();

	void SetInfo( CSoftlogic::Port portKey, CPoint ptMouse );

// Dialog Data
	enum { IDD = IDD_LOGIC_GALLERY };

protected:
	CSoftlogic::Port	m_portKey;
	CPoint				m_ptMouse;

	virtual void OnOK();
	virtual void OnCancel();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	int m_nGalleryItemType;
	afx_msg void OnBnClickedOk();
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	CComboBox m_cbSelectGPIO;
	afx_msg void OnBnClickedRadioGalleryItem();
};
