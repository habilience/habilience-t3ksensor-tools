#pragma once

#include "Softkey.h"
#include "DEditGate.h"
#include "DLogicGallery.h"
#include "FullScrnWnd.h"

class CLogicDesignerWnd : public CFullScrnWnd
{
	DECLARE_DYNAMIC(CLogicDesignerWnd)

public:
	CLogicDesignerWnd();   // 표준 생성자입니다.
	virtual ~CLogicDesignerWnd();

	enum ScreenMode { ScreenModeLogicDesign, ScreenModePreview };

	void AddLogicFromGallery( int nGalleryItemType, CSoftlogic::Port portKey, CPoint ptMouse );

protected:
	COLORREF			m_clrBackground;
	COLORREF			m_clrCloseBtnBg;
	COLORREF			m_clrCloseBtnFg;
	COLORREF			m_clrGrid;
	COLORREF			m_clrBindSelect;
	COLORREF			m_clrBindNormal;
	COLORREF			m_clrKeyBg;
	COLORREF			m_clrKeyFg;
	COLORREF			m_clrKeyBorderActive;
	COLORREF			m_clrKeyBorderNormal;
	COLORREF			m_clrKeyBorderBind;

	COLORREF			m_clrLogicBorderNormal;
	COLORREF			m_clrLogicBorderActive;
	COLORREF			m_clrLogicFg;
	COLORREF			m_clrLogicBg;

	COLORREF			m_clrConnector;

	DEditGate			m_wndEditGate;
	DLogicGallery		m_wndLogicGallery;

	CBitmap*			m_pBitmap;
	CDC					m_MemDC;

	CSoftlogic*			m_pSoftlogicForRemove;

	ScreenMode			m_eScreenMode;
	CRect				m_rcCloseButton;
	BOOL				m_bDownCloseButton;

	BOOL				m_bShowText;

	Image*				m_pIconConsumerButton;

	POINTF				m_ptSoftkeyPositionBackup;
	CSoftkey*			m_pToBindKey;
	CSoftkey*			m_pCheckKey;

protected:
	void _DrawBufferGate( CDC * pdc, CPoint pt, int size, CSoftlogic::Port enable_port, BOOL enable_not, BOOL in_not, BOOL bShow );
	void _DrawAndGate( CDC * pdc, CPoint pt, int size, CSoftlogic::Port enable_port, BOOL enable_not, BOOL in1_not, BOOL in2_not, BOOL bShow );
	void _DrawOrGate( CDC * pdc, CPoint pt, int size, CSoftlogic::Port enable_port, BOOL enable_not, BOOL in1_not, BOOL in2_not, BOOL bShow );
	void _DrawXorGate( CDC * pdc, CPoint pt, int size, CSoftlogic::Port enable_port, BOOL enable_not, BOOL in1_not, BOOL in2_not, BOOL bShow );
	void _DrawAndRisingedgeGate( CDC * pdc, CPoint pt, int size, CSoftlogic::Port enable_port, BOOL enable_not, BOOL in1_not, BOOL in2_not, BOOL bShow );
	void _DrawTrigger( CDC * pdc, CPoint pt, int size, CSoftlogic::Trigger trigger );
	void _DrawLine( CDC * pdc, CPoint pt1, CPoint pt2, float fD );
	void _DrawLine2( Graphics& g, Pen& LinePen, CPoint pt1, CPoint pt2, float fD, int nThXY );
	void _DrawInPort( CDC * pdc, CSoftlogic::Port port, CPoint pt, int size, BOOL bUp, BOOL bDefaultTrue, BOOL bNoGateOK = FALSE );

	void _DrawEnable( CDC * pdc, CSoftlogic::Port enable_port, BOOL enable_not, int xe, int ye, int xsize, int ysize );
	void _DrawOutKey( CDC * pdc, CPoint pt, int size, BYTE ckey, BYTE key );
	void _DrawOutMouse( CDC * pdc, CPoint pt, int size, BYTE ckey, BYTE mkey );
	void _DrawOutState( CDC * pdc, CPoint pt, int size, CSoftlogic::Port out_port );
	void _DrawOutGPIO( CDC * pdc, CPoint pt, int size, CSoftlogic::Port out_port );
	void _DrawOutReport( CDC * pdc, CPoint pt, int size, int id );

	BOOL GetOutPortPosI( CSoftlogic::Port port, POINT * ptPos, CRect rcClient );

	CSoftlogic::Port m_port_hit;
	CPoint           m_SelOffset;
	enum GatePart
	{
		epqNill, egpBody, epgIn1, epgIn2,
	} m_SelPart;
	CSoftlogic::Port HitTest( CPoint pt, POINT * ptPos, GatePart * part, CSoftkey* pDontCheckKey=NULL );

	CSoftlogic::Port m_port_dst;
	CPoint           m_pt_dst;

	BOOL  m_bShowNumber;

	void DrawCloseButton( CDC* pDC, CRect rcClose );

	BOOL IsAssociate( CSoftkey* pSoftkey, CSoftlogic* pLogic );
	BOOL IsAssociate( CSoftlogic* pLogicCheck, CSoftlogic* pLogic );
	CRect GetKeyBound( CSoftkey* pSoftkey, BOOL bFocusRect );
	CRect GetLogicBound( CSoftlogic* pLogic );

public:
	void DrawKeyBind( CDC * pDC, CRect rcClient, CSoftkey * key, int id, int size );
	void DrawKey( CDC * pDC, CRect rcClient, CSoftkey * key, int id, int nbid, int size, BOOL bBound );
	void DrawLogic( CDC * pDC, CRect rcClient, CSoftlogic * logic, BOOL bShow, int size, BOOL bBound, int ID );

	void OnDraw( CDC* pDC );

	void SetScreenMode( ScreenMode eMode ) { m_eScreenMode = eMode; }

	void SetInvertDrawing( BOOL bInvert );

protected:
	virtual void OnCancel();
	virtual void OnOK() {};

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
