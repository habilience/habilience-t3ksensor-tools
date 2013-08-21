#pragma once
#include "afxwin.h"
#include "userdefinedkeyctrl.h"

#include "Softkey.h"
#include "borderstyleedit.h"

// DEditGate 대화 상자입니다.
class CLogicDesignerWnd;
class DEditGate : public CDialog
{
	DECLARE_DYNAMIC(DEditGate)

public:
	DEditGate(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~DEditGate();

	BOOL Create(CWnd* pParentWnd);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_EDIT_GATE };

	void SetLogic( CSoftlogic* pSoftlogic, BOOL bShow );

	CSoftlogic* GetLogic() { return m_pSoftlogic; }

protected:
	enum EditMode { ModeNone, ModeUserDefined, ModeFunctionKey };

	CLogicDesignerWnd*		m_pWndLogicCanvas;
	CSoftlogic*			m_pSoftlogic;
	BOOL				m_bShowLogic;

	CFont				m_fntFixed;

	void UpdateItems();
	void UpdateOutputPort( CSoftlogic::PortType );
	void UpdateOutputPortType( int nGateType );

	void ApplyItems();
	void ApplyHiddenLogics();

	void DrawGate( CDC* pDC, CRect rcBody, int nGateType );

	void OnUpdateKeyValue( WORD wKeyValue );
	void SetEditMode( EditMode Mode, WORD wKeyValue );

	void UpdateInputCombo( CSoftlogic::Port port, CComboBox& cbInput );

	void DisplayKeyValue( WORD wKeyValue );

	virtual void OnOK() {}
	virtual void OnCancel() {}

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	int m_nGateType;
	CComboBox m_cbControlPort;
	CComboBox m_cbTrigger;
	BOOL m_bInvertInputPort1;
	BOOL m_bInvertInputPort2;
	CComboBox m_cbOutputPortType;
	CComboBox m_cbOutputPort;
	BOOL m_bInvertControlPort;
	CBorderStyleEdit m_edtReportId;
	CComboBox m_cbInputPort1;
	CComboBox m_cbInputPort2;
	CComboBox m_cbLogicShowHide;
	CUserDefinedKeyCtrl m_wndUserDefinedKey;

public:
	afx_msg void OnClose();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboControlPort();
	afx_msg void OnCbnSelchangeComboControlTrigger();
	afx_msg void OnCbnSelchangeComboOutputPort();
	afx_msg void OnCbnSelchangeComboOutputType();
	afx_msg void OnBnClickedRadioGateType();
	afx_msg LRESULT OnChangeKeyEdit( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnChangeReportIdEdit( WPARAM wParam, LPARAM lParam );
	afx_msg void OnBnClickedCheckInvert();
	afx_msg void OnCbnSelchangeComboShowhide();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnCbnSelchangeComboInputPort1();
	afx_msg void OnCbnSelchangeComboInputPort2();
};
