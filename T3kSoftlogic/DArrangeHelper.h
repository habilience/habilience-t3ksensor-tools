#pragma once

#include "GdiplusExt.h"
#include "borderstyleedit.h"
// DArrangeHelper ��ȭ �����Դϴ�.
class CDesignCanvasWnd;
class DArrangeHelper : public CDialog
{
	DECLARE_DYNAMIC(DArrangeHelper)

public:
	DArrangeHelper(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~DArrangeHelper();

	INT_PTR DoModal( CWnd* pParentWnd );

	void SetUnit( ScreenUnit eUnit, double dScaleWidth, double dScaleHeight );


// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ARRANGE_HELPER };

protected:
	double m_dD2PScaleWidth;
	double m_dD2PScaleHeight;

	ScreenUnit			m_eUnit;

	void UpdateUnit();

	CDesignCanvasWnd*	m_pWndDesignCanvas;
	Image*				m_pImageArrangePicture;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	void MYDDX_Text(CDataExchange* pDX, int nIDC, double& dValue, BOOL bIsScaleX);

	virtual void OnOK() {}

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnApply();
	afx_msg void OnBnClickedBtnCancel();
	afx_msg void OnBnClickedRadioHorizontalVertical();
protected:
	int m_nArrangeType;
	double m_dWidth;
	double m_dHeight;
	double m_dInterval;
	int m_nCount;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
protected:
	CBorderStyleEdit m_edtKeyCount;
	CBorderStyleEdit m_edtKeyWidth;
	CBorderStyleEdit m_edtKeyHeight;
	CBorderStyleEdit m_edtKeyInterval;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
