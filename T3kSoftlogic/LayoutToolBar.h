#pragma once

#include "DArrangeHelper.h"

// DLayoutToolBar 대화 상자입니다.
class CDesignCanvasWnd;
class DSoftKeyDesignTool;
class DLayoutToolBar : public CDialog
{
	DECLARE_DYNAMIC(DLayoutToolBar)

public:
	DLayoutToolBar(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~DLayoutToolBar();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_KEY_LAYOUT_TOOL };

	BOOL Create(CDesignCanvasWnd* pDesignCanvasWnd, DSoftKeyDesignTool* pDesignTool);

	void UpdateUIButtonState( int nSelectKeyCount );
	void SetUnit( ScreenUnit eUnit, double dScaleWidth, double dScaleHeight );

protected:
	double m_dD2PScaleWidth;
	double m_dD2PScaleHeight;

	ScreenUnit			m_eUnit;
	CDesignCanvasWnd*	m_pWndDesignCanvas;
	DSoftKeyDesignTool*	m_pWndDesignTool;

	DArrangeHelper		m_wndArrangeHelperDlg;

	virtual void OnOK();
	virtual void OnCancel();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	afx_msg void OnBnClickedBtnAlignLeft();
	afx_msg void OnBnClickedBtnAlignCenter();
	afx_msg void OnBnClickedBtnAlignRight();
	afx_msg void OnBnClickedBtnAlignTop();
	afx_msg void OnBnClickedBtnAlignMiddle();
	afx_msg void OnBnClickedBtnAlignBottom();
	afx_msg void OnBnClickedBtnSameSizeWidth();
	afx_msg void OnBnClickedBtnSameSizeHeight();
	afx_msg void OnBnClickedBtnSameSizeBoth();
	afx_msg void OnBnClickedBtnDistribHorzEqualGap();
	afx_msg void OnBnClickedBtnDistribVertEqualGap();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnHelperReorder();
	afx_msg void OnBnClickedBtnHelperArrange();
};
