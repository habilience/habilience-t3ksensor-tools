// LogicDesignerWnd.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "T3kSoftlogic.h"
#include ".\LogicDesignerWnd.h"
#include "T3kSoftlogicDlg.h"
#include "KeyMapStr.h"

#define LOGIC_HEIGHT  4

IMPLEMENT_DYNAMIC(CLogicDesignerWnd, CFullScrnWnd)
CLogicDesignerWnd::CLogicDesignerWnd()
	: CFullScrnWnd()
{
	m_SelPart = epqNill;
	m_bShowNumber = FALSE;
	m_pBitmap = NULL;
	m_pSoftlogicForRemove = NULL;
	m_eScreenMode = ScreenModeLogicDesign;

	m_bDownCloseButton = FALSE;

	m_pIconConsumerButton = NULL;

	m_ptSoftkeyPositionBackup.x = -1.f;
	m_ptSoftkeyPositionBackup.y = -1.f;

	m_pToBindKey = NULL;
	m_pCheckKey = NULL;

	SetInvertDrawing( FALSE );

}

CLogicDesignerWnd::~CLogicDesignerWnd()
{
	if( m_pBitmap )
		delete m_pBitmap;
	m_pBitmap = NULL;

	if( m_pIconConsumerButton )
		delete m_pIconConsumerButton;
}

void CLogicDesignerWnd::SetInvertDrawing( BOOL bInvert )
{
	if ( bInvert )
	{
		m_clrBackground = RGB(255, 255, 255);
		m_clrCloseBtnBg = RGB(230, 230, 230);
		m_clrCloseBtnFg = RGB(100, 100, 100);
		m_clrGrid = RGB(200, 200, 200);
		m_clrBindNormal = RGB(149, 179, 215);
		m_clrBindSelect = RGB(108, 150, 200);

		m_clrKeyFg = RGB(50, 50, 50);
		m_clrKeyBg = RGB(255, 255, 255);
		m_clrKeyBorderActive = RGB(0, 180, 0);
		m_clrKeyBorderNormal = RGB(56, 93, 138);
		m_clrKeyBorderBind = RGB(180, 0, 0);
		m_clrConnector = RGB(56, 93, 138);

		m_clrLogicBorderNormal = RGB(192, 80, 77);
		m_clrLogicBorderActive = RGB(0, 180, 0);
		m_clrLogicFg = RGB(50, 50, 50);
	}
	else
	{
		m_clrBackground = RGB(0, 0, 0);
		m_clrCloseBtnBg = RGB(100, 100, 100);
		m_clrCloseBtnFg = RGB(0, 0, 0);
		m_clrGrid = RGB(50, 50, 50);
		m_clrBindNormal = RGB(40, 40, 40);
		m_clrBindSelect = RGB(70, 70, 70);

		m_clrKeyFg = RGB(200, 200, 200);
		m_clrKeyBg = RGB(0, 0, 0);
		m_clrKeyBorderActive = RGB(50, 200, 50);
		m_clrKeyBorderNormal = RGB(50, 50, 200);
		m_clrKeyBorderBind = RGB(200, 50, 50);
		m_clrConnector = RGB(200, 200, 200);

		m_clrLogicBorderNormal = RGB(200, 50, 50);
		m_clrLogicBorderActive = RGB(50, 200, 50);
		m_clrLogicFg = RGB(200, 200, 200);
	}
}

BEGIN_MESSAGE_MAP(CLogicDesignerWnd, CFullScrnWnd)
	ON_WM_CLOSE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_SETCURSOR()
	ON_WM_CREATE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SYSKEYDOWN()
	ON_WM_SYSKEYUP()
	ON_WM_DESTROY()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CLogicDesignerWnd 메시지 처리기입니다.

void CLogicDesignerWnd::OnCancel()
{
	DestroyWindow();
}

BOOL CLogicDesignerWnd::OnEraseBkgnd(CDC* pDC)
{
	/*
	if( !m_pBitmap ) return CWnd::OnEraseBkgnd(pDC);
	if( !m_MemDC.GetSafeHdc() ) return CWnd::OnEraseBkgnd(pDC);
	CBitmap * pOldBitmap = m_MemDC.SelectObject(m_pBitmap);
	CRect rect;
	GetClientRect(rect);

	m_MemDC.FillSolidRect(rect, RGB(0,0,0));
	*/

	return TRUE;
}

#define LOGIC_SIZE  15

void CLogicDesignerWnd::OnPaint()
{
#define SYSTIME_PER_MSEC 10000
	FILETIME f;

	::GetSystemTimeAsFileTime(&f);
	DWORD tmStart = (DWORD)((f.dwLowDateTime | (((LONGLONG)f.dwHighDateTime) << 32)) / SYSTIME_PER_MSEC);

	CPaintDC dc(this);

	CRect rcClient;
	GetClientRect( rcClient );

	if( m_MemDC.GetSafeHdc() == NULL )
	{
		m_MemDC.CreateCompatibleDC( &dc );
	}

	BOOL bInit = FALSE;
	if( !m_pBitmap )
	{
		m_pBitmap = new CBitmap;
		ASSERT( m_pBitmap );
		VERIFY( m_pBitmap->CreateCompatibleBitmap( &dc, rcClient.Width(), rcClient.Height() ) );
		bInit = TRUE;
	}

	ASSERT( m_pBitmap );
	CBitmap* pOldBitmap = m_MemDC.SelectObject( m_pBitmap );
	if( bInit )
	{
		dc.FillSolidRect(&rcClient, m_clrBackground);
	}

	OnDraw( &m_MemDC );

	m_MemDC.SetMapMode( MM_TEXT );
	m_MemDC.SetViewportOrg( 0, 0 );

	dc.BitBlt( rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), 
		&m_MemDC, 0, 0, SRCCOPY );

	m_MemDC.SelectObject( pOldBitmap );

	::GetSystemTimeAsFileTime(&f);
	DWORD tmCur = (DWORD)((f.dwLowDateTime | (((LONGLONG)f.dwHighDateTime) << 32)) / SYSTIME_PER_MSEC) - tmStart;


	//TRACE( _T("OnDraw: %dms\r\n"), tmCur );
}

void CLogicDesignerWnd::DrawCloseButton( CDC* pDC, CRect rcClose )
{
	if( m_eScreenMode == ScreenModePreview )
		return;

	CPen penClose( PS_SOLID, 1, RGB(200, 200, 200) );
	CBrush brushClose;
	brushClose.CreateSolidBrush( m_bDownCloseButton ? RGB(237, 28, 36) : m_clrCloseBtnBg );
	CBrush brushX( m_clrCloseBtnFg );
	CPen* pOldPen = pDC->SelectObject( &penClose );
	CBrush* pOldBrush = pDC->SelectObject( &brushClose );

	pDC->RoundRect( rcClose, CPoint(7, 7) );


	CPoint ptC = rcClose.CenterPoint();
	int nWH = rcClose.Width() / 4;
	int nC = 4;
	pDC->BeginPath();
	pDC->MoveTo( ptC.x-nWH+nC, ptC.y-nWH );
	pDC->LineTo( ptC.x, ptC.y-nC );
	pDC->LineTo( ptC.x+nWH-nC, ptC.y-nWH );
	pDC->LineTo( ptC.x+nWH, ptC.y-nWH+nC );
	pDC->LineTo( ptC.x+nC, ptC.y );
	pDC->LineTo( ptC.x+nWH, ptC.y+nWH-nC );
	pDC->LineTo( ptC.x+nWH-nC, ptC.y+nWH );
	pDC->LineTo( ptC.x, ptC.y+nC );
	pDC->LineTo( ptC.x-nWH+nC, ptC.y+nWH );
	pDC->LineTo( ptC.x-nWH, ptC.y+nWH-nC );
	pDC->LineTo( ptC.x-nC, ptC.y );
	pDC->LineTo( ptC.x-nWH, ptC.y-nWH+nC );
	pDC->EndPath();

	pDC->SelectObject( &brushX );
	pDC->StrokeAndFillPath();

	pDC->SelectObject( pOldPen );
	pDC->SelectObject( pOldBrush );
}

void CLogicDesignerWnd::OnDraw( CDC* pDC )
{
	CRect rcClient;
	GetClientRect(&rcClient);
	int logic_size = rcClient.Width() / LOGIC_SIZE;

	pDC->FillSolidRect( rcClient, m_clrBackground );

	CSize step(rcClient.Width() / 50, rcClient.Height() / 50);
	for ( int ny = rcClient.top + step.cy; ny < rcClient.bottom; ny += step.cy )
	{
		for ( int nx = rcClient.left + step.cx; nx < rcClient.right; nx += step.cx )
			pDC->SetPixel(nx, ny, m_clrGrid);
	}

	CT3kSoftlogicDlg * pDlg = (CT3kSoftlogicDlg *)AfxGetMainWnd();
	CSoftkeyArray & Keys = pDlg->GetKeys();
	CSoftlogicArray & Logics = pDlg->GetLogics();


	m_bShowText = FALSE;
	m_bShowText = (logic_size * 2 / 5) > 20 ? TRUE : FALSE;

	int ni;

	for ( ni = 0; ni < Keys.GetSize(); ni++ )
	{
		if ( m_pCheckKey == Keys[ni] ) continue;
		DrawKeyBind(pDC, rcClient, Keys[ni], ni, logic_size);
	}
	if ( m_pCheckKey )
	{
		DrawKeyBind(pDC, rcClient, m_pCheckKey, ni, logic_size);
	}
	int nbid = -1;
	for ( ni = 0; ni < Keys.GetSize(); ni++ )
	{
		if ( Keys[ni]->getBind() )
			nbid = Keys.IndexFromSoftkey( Keys[ni]->getBind() );
		else
			nbid = -1;
		DrawKey(pDC, rcClient, Keys[ni], ni, nbid, logic_size,
			(ni == m_port_hit - MM_SOFTLOGIC_PORT_SOFTKEY_START));
	}

	for ( ni = 0; ni < Logics.GetSize(); ni++ )
	{
		DrawLogic(pDC, rcClient, Logics[ni], ni < Logics.GetShowSize(), logic_size,
			(ni == m_port_hit - MM_SOFTLOGIC_PORT_SOFTLOGIC_START),
			(m_bShowNumber ? ni + 1 : -1));
	}

	CRect rcClose;
	int nCloseWH = rcClient.Width() / 36;
	rcClose.left = rcClient.right - nCloseWH;
	rcClose.right = rcClient.right;
	rcClose.top = rcClient.top;
	rcClose.bottom = rcClient.top + nCloseWH;

	rcClose.OffsetRect( -20, 20 );

	m_rcCloseButton = rcClose;
	DrawCloseButton( pDC, rcClose );

}

void CLogicDesignerWnd::DrawKeyBind( CDC * pDC, CRect rcClient, CSoftkey * key, int id, int size )
{
	ASSERT( pDC );
	if ( key == NULL )
		return;
	CSoftkey * keyTo = key->getBind();
	if ( keyTo == NULL )
		return;

	CPoint ptPos((int)(rcClient.Width() * key->m_ptPosition.x) - size / 2, (int)(rcClient.Height() * key->m_ptPosition.y));
	CPoint ptPosTo((int)(rcClient.Width() * keyTo->m_ptPosition.x) - size / 2, (int)(rcClient.Height() * keyTo->m_ptPosition.y));

	Graphics g(pDC->GetSafeHdc());
	g.SetSmoothingMode( SmoothingModeHighQuality );
	Color clrLine;
	clrLine.SetFromCOLORREF( m_clrBindNormal );
	Pen LinePen(clrLine, size/8.0f);
	if ( m_pCheckKey == key )
	{
		clrLine.SetFromCOLORREF( m_clrBindSelect );
		LinePen.SetColor( clrLine );
	}
	LinePen.SetStartCap( LineCapRoundAnchor );
	LinePen.SetEndCap( LineCapArrowAnchor );
	LinePen.SetLineJoin( LineJoinRound );
	_DrawLine2(g, LinePen, ptPos, ptPosTo, 0.5f, size/2 - size/8);
	/*
	CPen * penOld = pDC->GetCurrentPen();
	CPen penBind;
	penBind.CreatePen(PS_SOLID, size / 5, RGB(80, 80, 80));
	pDC->SelectObject(&penBind);

	_DrawLine(pDC, ptPos, ptPosTo, 0.5f);

	pDC->SelectObject(penOld);
	*/
}

void CLogicDesignerWnd::DrawKey( CDC * pDC, CRect rcClient, CSoftkey * key, int id, int nbid, int size, BOOL bBound )
{
	ASSERT( pDC );
	if ( key == NULL )
		return;

	CPoint ptPos((int)(rcClient.Width() * key->m_ptPosition.x), (int)(rcClient.Height() * key->m_ptPosition.y));

	CPen * penOld = pDC->GetCurrentPen();
	CBrush* brushOld = pDC->GetCurrentBrush();
	CPen penKey;
	CBrush brushKey;
	brushKey.CreateSolidBrush( m_clrKeyBg );

	if ( m_pToBindKey == key || m_pCheckKey == key )
	{
		if ( m_pToBindKey )
			penKey.CreatePen(PS_SOLID, 3, m_clrKeyBorderBind);
		else
		{
			if ( key->getBind() )
			{
				penKey.CreatePen(PS_SOLID, 2, m_clrKeyBorderNormal);
				//pDC->SelectStockObject(BLACK_BRUSH);
			}
			else
			{
				penKey.CreatePen(PS_SOLID, 1, m_clrKeyBorderNormal);
				//pDC->SelectStockObject(BLACK_BRUSH);
			}
		}
		pDC->SelectStockObject(NULL_BRUSH);
	}
	else
	{
		if ( key->getBind() )
		{
			penKey.CreatePen(PS_SOLID, 2, m_clrKeyBorderNormal);
			pDC->SelectObject( &brushKey );
		}
		else
		{
			if ( m_pCheckKey && (m_pCheckKey->getBind() == key) )
			{
				penKey.CreatePen(PS_SOLID, 1, m_clrKeyBorderActive);
				pDC->SelectStockObject(NULL_BRUSH);
			}
			else
			{
				penKey.CreatePen(PS_SOLID, 1, m_clrKeyBorderNormal);
				pDC->SelectObject( &brushKey );
			}
		}
	}

	pDC->SelectObject(&penKey);
	pDC->SetBkMode( TRANSPARENT );
	pDC->SetTextColor( m_clrKeyFg );
	CRect rcKey(ptPos.x - size, ptPos.y - size / 5, ptPos.x, ptPos.y + size / 5);
	pDC->Rectangle(&rcKey);
	if ( !key->getShow() )
	{
		CBrush brsh;
		brsh.CreateHatchBrush(HS_BDIAGONAL, RGB(127,127,127));
		CBrush * brshOld = pDC->SelectObject(&brsh);
		pDC->Rectangle(&rcKey);
		pDC->SelectObject(brshOld);
	}

	if( m_bShowText )
	{
		int nHeightLine = rcKey.Height() / 3;
		rcKey.bottom -= nHeightLine;

		CFont font;
		font.CreateFont(size / 6, 0, 0, 0, 0, FALSE, FALSE, 0, 0, 0, 0, 0, 0, _T("arial"));
		CFont * fontOld = pDC->SelectObject(&font);
		pDC->SetBkMode(TRANSPARENT);
		CString str;
		str.Format(_T("%d"), id + 1);
		pDC->DrawText(str, &rcKey, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		rcKey.OffsetRect(0, nHeightLine);
		str = key->getName();
		pDC->DrawText(str, &rcKey, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

		if ( nbid >= 0 )
		{
			CFont fntS;
			fntS.CreateFont( size / 8, 0, 0, 0, 0, FALSE, FALSE, 0, 0, 0, 0, 0, 0, _T("Arial") );
			pDC->SelectObject( &fntS );

			CString strBind;
			strBind.Format( _T("%d"), nbid + 1 );
			rcKey.OffsetRect(rcKey.Width(), nHeightLine);
			pDC->DrawText( strBind, rcKey, DT_SINGLELINE | DT_LEFT | DT_VCENTER );
		}

		pDC->SelectObject(fontOld);
	}

	if ( bBound )
	{
		pDC->DrawFocusRect(CRect(ptPos.x - size, ptPos.y - size / LOGIC_HEIGHT, ptPos.x, ptPos.y + size / LOGIC_HEIGHT));
	}

	pDC->SelectObject(penOld);
	pDC->SelectObject(brushOld);
}

BOOL CLogicDesignerWnd::IsAssociate( CSoftlogic* pLogicCheck, CSoftlogic* pLogic )
{
	if ( !pLogicCheck || !pLogic )
		return FALSE;

	CT3kSoftlogicDlg * pDlg = (CT3kSoftlogicDlg *)AfxGetMainWnd();

	CSoftlogicArray& Logics = pDlg->GetLogics();

	CSoftlogic::Port epIn1 = pLogicCheck->getIn1Port();
	if ( epIn1 != CSoftlogic::epNoGate )
	{
		if ( epIn1 >= CSoftlogic::epSoftLogic0 )
		{
			CSoftlogic* pAssociateLogic = Logics[epIn1 - CSoftlogic::epSoftLogic0];

			if ( pAssociateLogic == pLogic )
				return TRUE;
		}
	}

	CSoftlogic::Port epIn2 = pLogicCheck->getIn2Port();
	if ( epIn2 != CSoftlogic::epNoGate )
	{
		if ( epIn2 >= CSoftlogic::epSoftLogic0 )
		{
			CSoftlogic* pAssociateLogic = Logics[epIn2 - CSoftlogic::epSoftLogic0];

			if ( pAssociateLogic == pLogic )
				return TRUE;
		}
	}

	return FALSE;
}

BOOL CLogicDesignerWnd::IsAssociate( CSoftkey* pSoftkey, CSoftlogic* pLogic )
{
	if ( !pSoftkey || !pLogic )
		return FALSE;

	CT3kSoftlogicDlg * pDlg = (CT3kSoftlogicDlg *)AfxGetMainWnd();

	// Check Input port 1
	CSoftlogic::Port port1 = pLogic->getIn1Port();

	if ( port1 >= CSoftlogic::epSoftkey0 && port1 < CSoftlogic::epSoftLogic0 )
	{
		// is key
		int idx = port1 - CSoftlogic::epSoftkey0;
		CSoftkeyArray & Keys = pDlg->GetKeys();
		CSoftkey* pInSoftkey = Keys[idx];
		if( pInSoftkey == pSoftkey )
			return TRUE;
	}
	else if ( port1 >= CSoftlogic::epSoftLogic0 )
	{
		int idx = port1 - MM_SOFTLOGIC_PORT_SOFTLOGIC_START;
		if ( idx < SOFT_LOGIC_MAX )
		{
			CSoftlogicArray & Logics = pDlg->GetLogics();
			CSoftlogic * pSoftlogic = Logics[idx];
			if ( pLogic == pSoftlogic )
				goto goto_CheckPort2;
			if ( pSoftlogic )
			{
				if ( IsAssociate(pSoftkey, pSoftlogic) )
					return TRUE;
			}
		}
	}

goto_CheckPort2:

	// Check Input port 2
	CSoftlogic::Port port2 = pLogic->getIn2Port();

	if ( port2 >= CSoftlogic::epSoftkey0 && port2 < CSoftlogic::epSoftkeyAll )
	{
		// is key
		int idx = port2 - CSoftlogic::epSoftkey0;
		CSoftkeyArray & Keys = pDlg->GetKeys();
		CSoftkey* pInSoftkey = Keys[idx];
		if( pInSoftkey == pSoftkey )
			return TRUE;
	}
	else if ( port2 >= CSoftlogic::epSoftLogic0 )
	{
		int idx = port2 - MM_SOFTLOGIC_PORT_SOFTLOGIC_START;
		if ( idx < SOFT_LOGIC_MAX )
		{
			CSoftlogicArray & Logics = pDlg->GetLogics();
			CSoftlogic * pSoftlogic = Logics[idx];
			if ( pSoftlogic )
			{
				if ( pLogic == pSoftlogic )
					return FALSE;
				if ( IsAssociate(pSoftkey, pSoftlogic) )
					return TRUE;
			}
		}
	}

	return FALSE;
}

void CLogicDesignerWnd::DrawLogic( CDC * pDC, CRect rcClient, CSoftlogic * logic, BOOL bShow, int size, BOOL bBound, int ID )
{
	ASSERT( pDC );
	if ( logic == NULL )
		return;

	if ( logic->getLogicType() == CSoftlogic::eltNILL )
		return;

	CPoint ptPos((int)(rcClient.Width() * logic->m_ptPosition.x), (int)(rcClient.Height() * logic->m_ptPosition.y));

	pDC->SelectStockObject(NULL_BRUSH);
	CPen * penOld = pDC->GetCurrentPen();
	CPen penLogic;
	CPen penLine;

	penLine.CreatePen(PS_SOLID, 1, m_clrConnector);

	if( (m_wndEditGate && m_wndEditGate.IsWindowVisible() && (m_wndEditGate.GetLogic() == logic))
		|| (m_pSoftlogicForRemove == logic) || 
		IsAssociate(m_pCheckKey, logic) )
	{
		penLogic.CreatePen(PS_SOLID, 1, m_clrLogicBorderActive);
	}
	else
	{
		penLogic.CreatePen(PS_SOLID, 1, m_clrLogicBorderNormal);
	}

	CPoint ptOutPort;

	switch ( logic->getLogicType() )
	{
	default:
	case CSoftlogic::eltNILL:
		return;

	case CSoftlogic::eltAND:
		pDC->SelectObject(&penLogic);
		pDC->SetTextColor(m_clrLogicFg);
		_DrawAndGate(pDC, ptPos, size, logic->getEnablePort(), logic->getEnableNot(), logic->getIn1Not(), logic->getIn2Not(), bShow);
		switch ( logic->getOutPortType() )
		{
		default:
		case CSoftlogic::eptKey:
			_DrawOutKey(pDC, ptPos, size, logic->getOutKey1ConditionKeys(), logic->getOutKey1());
			pDC->SelectObject(&penLine);
			_DrawTrigger(pDC, ptPos, size, logic->getLogicTrigger());
			break;
		case CSoftlogic::eptMouse:
			_DrawOutMouse(pDC, ptPos, size, logic->getOutKey1ConditionKeys(), logic->getOutMouse());
			pDC->SelectObject(&penLine);
			_DrawTrigger(pDC, ptPos, size, logic->getLogicTrigger());
			break;
		case CSoftlogic::eptState:
			_DrawOutState(pDC, ptPos, size, logic->getOutPort());
			pDC->SelectObject(&penLine);
			_DrawTrigger(pDC, ptPos, size, logic->getLogicTrigger());
			break;
		case CSoftlogic::eptGPIO:
			_DrawOutGPIO(pDC, ptPos, size, logic->getOutPort());
			pDC->SelectObject(&penLine);
			_DrawTrigger(pDC, ptPos, size, logic->getLogicTrigger());
			break;
		case CSoftlogic::eptReport:
			_DrawOutReport(pDC, ptPos, size, logic->getReportId());
			pDC->SelectObject(&penLine);
			_DrawTrigger(pDC, ptPos, size, logic->getLogicTrigger());
			break;
		}
		if ( GetOutPortPosI(logic->getIn1Port(), &ptOutPort, &rcClient) )
			_DrawLine(pDC, ptOutPort, CPoint(ptPos.x - size, (ptPos.y - size / 4) + size / 8), logic->m_fLines[0]);
		else
		{
			pDC->SelectObject(&penLogic);
			_DrawInPort(pDC, logic->getIn1Port(), CPoint(ptPos.x - size, (ptPos.y - size / 4) + size / 8), size, TRUE, TRUE, TRUE);
			pDC->SelectObject(&penLine);
		}
		if ( GetOutPortPosI(logic->getIn2Port(), &ptOutPort, &rcClient) )
			_DrawLine(pDC, ptOutPort, CPoint(ptPos.x - size, (ptPos.y + size / 4) - size / 8), logic->m_fLines[1]);
		else
		{
			pDC->SelectObject(&penLogic);
			_DrawInPort(pDC, logic->getIn2Port(), CPoint(ptPos.x - size, (ptPos.y + size / 4) - size / 8), size, FALSE, TRUE, TRUE);
		}
		break;

	case CSoftlogic::eltOR:
		pDC->SelectObject(&penLogic);
		pDC->SetTextColor(m_clrLogicFg);
		_DrawOrGate(pDC, ptPos, size, logic->getEnablePort(), logic->getEnableNot(), logic->getIn1Not(), logic->getIn2Not(), bShow);
		switch ( logic->getOutPortType() )
		{
		default:
		case CSoftlogic::eptKey:
			_DrawOutKey(pDC, ptPos, size, logic->getOutKey1ConditionKeys(), logic->getOutKey1());
			pDC->SelectObject(&penLine);
			_DrawTrigger(pDC, ptPos, size, logic->getLogicTrigger());
			break;
		case CSoftlogic::eptMouse:
			pDC->SelectObject(&penLine);
			_DrawTrigger(pDC, ptPos, size, logic->getLogicTrigger());
			break;
		case CSoftlogic::eptState:
			_DrawOutState(pDC, ptPos, size, logic->getOutPort());
			pDC->SelectObject(&penLine);
			_DrawTrigger(pDC, ptPos, size, logic->getLogicTrigger());
			break;
		case CSoftlogic::eptGPIO:
			_DrawOutGPIO(pDC, ptPos, size, logic->getOutPort());
			pDC->SelectObject(&penLine);
			_DrawTrigger(pDC, ptPos, size, logic->getLogicTrigger());
			break;
		case CSoftlogic::eptReport:
			_DrawOutReport(pDC, ptPos, size, logic->getReportId());
			pDC->SelectObject(&penLine);
			_DrawTrigger(pDC, ptPos, size, logic->getLogicTrigger());
			break;
		}
		if ( GetOutPortPosI(logic->getIn1Port(), &ptOutPort, &rcClient) )
			_DrawLine(pDC, ptOutPort, CPoint(ptPos.x - size, (ptPos.y - size / 4) + size / 8), logic->m_fLines[0]);
		else
		{
			pDC->SelectObject(&penLogic);
			_DrawInPort(pDC, logic->getIn1Port(), CPoint(ptPos.x - size, (ptPos.y - size / 4) + size / 8), size, TRUE, FALSE);
			pDC->SelectObject(&penLine);
		}
		if ( GetOutPortPosI(logic->getIn2Port(), &ptOutPort, &rcClient) )
			_DrawLine(pDC, ptOutPort, CPoint(ptPos.x - size, (ptPos.y + size / 4) - size / 8), logic->m_fLines[1]);
		else
		{
			pDC->SelectObject(&penLogic);
			_DrawInPort(pDC, logic->getIn2Port(), CPoint(ptPos.x - size, (ptPos.y + size / 4) - size / 8), size, FALSE, FALSE);
		}
		break;

	case CSoftlogic::eltXOR:
		pDC->SelectObject(&penLogic);
		pDC->SetTextColor(m_clrLogicFg);
		_DrawXorGate(pDC, ptPos, size, logic->getEnablePort(), logic->getEnableNot(), logic->getIn1Not(), logic->getIn2Not(), bShow);
		switch ( logic->getOutPortType() )
		{
		default:
		case CSoftlogic::eptKey:
			_DrawOutKey(pDC, ptPos, size, logic->getOutKey1ConditionKeys(), logic->getOutKey1());
			pDC->SelectObject(&penLine);
			_DrawTrigger(pDC, ptPos, size, logic->getLogicTrigger());
			break;
		case CSoftlogic::eptMouse:
			pDC->SelectObject(&penLine);
			_DrawTrigger(pDC, ptPos, size, logic->getLogicTrigger());
			break;
		case CSoftlogic::eptState:
			_DrawOutState(pDC, ptPos, size, logic->getOutPort());
			pDC->SelectObject(&penLine);
			_DrawTrigger(pDC, ptPos, size, logic->getLogicTrigger());
			break;
		case CSoftlogic::eptGPIO:
			_DrawOutGPIO(pDC, ptPos, size, logic->getOutPort());
			pDC->SelectObject(&penLine);
			_DrawTrigger(pDC, ptPos, size, logic->getLogicTrigger());
			break;
		case CSoftlogic::eptReport:
			_DrawOutReport(pDC, ptPos, size, logic->getReportId());
			pDC->SelectObject(&penLine);
			_DrawTrigger(pDC, ptPos, size, logic->getLogicTrigger());
			break;
		}
		if ( GetOutPortPosI(logic->getIn1Port(), &ptOutPort, &rcClient) )
			_DrawLine(pDC, ptOutPort, CPoint(ptPos.x - size, (ptPos.y - size / 4) + size / 8), logic->m_fLines[0]);
		else
		{
			pDC->SelectObject(&penLogic);
			_DrawInPort(pDC, logic->getIn1Port(), CPoint(ptPos.x - size, (ptPos.y - size / 4) + size / 8), size, TRUE, FALSE);
			pDC->SelectObject(&penLine);
		}
		if ( GetOutPortPosI(logic->getIn2Port(), &ptOutPort, &rcClient) )
			_DrawLine(pDC, ptOutPort, CPoint(ptPos.x - size, (ptPos.y + size / 4) - size / 8), logic->m_fLines[1]);
		else
		{
			pDC->SelectObject(&penLogic);
			_DrawInPort(pDC, logic->getIn2Port(), CPoint(ptPos.x - size, (ptPos.y + size / 4) - size / 8), size, FALSE, FALSE, TRUE);
		}
		break;

	case CSoftlogic::eltAND_RISINGEDGE:
		pDC->SelectObject(&penLogic);
		pDC->SetTextColor(m_clrLogicFg);
		_DrawAndRisingedgeGate(pDC, ptPos, size, logic->getEnablePort(), logic->getEnableNot(), logic->getIn1Not(), logic->getIn2Not(), bShow);
		switch ( logic->getOutPortType() )
		{
		default:
		case CSoftlogic::eptKey:
			_DrawOutKey(pDC, ptPos, size, logic->getOutKey1ConditionKeys(), logic->getOutKey1());
			pDC->SelectObject(&penLine);
			_DrawTrigger(pDC, ptPos, size, logic->getLogicTrigger());
			break;
		case CSoftlogic::eptMouse:
			_DrawOutMouse(pDC, ptPos, size, logic->getOutKey1ConditionKeys(), logic->getOutMouse());
			pDC->SelectObject(&penLine);
			_DrawTrigger(pDC, ptPos, size, logic->getLogicTrigger());
			break;
		case CSoftlogic::eptState:
			_DrawOutState(pDC, ptPos, size, logic->getOutPort());
			pDC->SelectObject(&penLine);
			_DrawTrigger(pDC, ptPos, size, logic->getLogicTrigger());
			break;
		case CSoftlogic::eptGPIO:
			_DrawOutGPIO(pDC, ptPos, size, logic->getOutPort());
			pDC->SelectObject(&penLine);
			_DrawTrigger(pDC, ptPos, size, logic->getLogicTrigger());
			break;
		case CSoftlogic::eptReport:
			_DrawOutReport(pDC, ptPos, size, logic->getReportId());
			pDC->SelectObject(&penLine);
			_DrawTrigger(pDC, ptPos, size, logic->getLogicTrigger());
			break;
		}
		if ( GetOutPortPosI(logic->getIn1Port(), &ptOutPort, &rcClient) )
			_DrawLine(pDC, ptOutPort, CPoint(ptPos.x - size, (ptPos.y - size / 4) + size / 8), logic->m_fLines[0]);
		else
		{
			pDC->SelectObject(&penLogic);
			_DrawInPort(pDC, logic->getIn1Port(), CPoint(ptPos.x - size, (ptPos.y - size / 4) + size / 8), size, TRUE, TRUE, TRUE);
			pDC->SelectObject(&penLine);
		}
		if ( GetOutPortPosI(logic->getIn2Port(), &ptOutPort, &rcClient) )
			_DrawLine(pDC, ptOutPort, CPoint(ptPos.x - size, (ptPos.y + size / 4) - size / 8), logic->m_fLines[1]);
		else
		{
			pDC->SelectObject(&penLogic);
			_DrawInPort(pDC, logic->getIn2Port(), CPoint(ptPos.x - size, (ptPos.y + size / 4) - size / 8), size, FALSE, TRUE, TRUE);
		}
		break;
	}

	if ( bBound )
	{
		if ( m_SelPart == epgIn1 )
		{
			if ( m_pt_dst.x >= 0 )
			{
				pDC->SelectObject(&penLine);
				_DrawLine(pDC, m_pt_dst, CPoint(ptPos.x - size, (ptPos.y - size / 4) + size / 8), 0.5f);
			}
		}
		else if ( m_SelPart == epgIn2 )
		{
			if ( m_pt_dst.x >= 0 )
			{
				pDC->SelectObject(&penLine);
				_DrawLine(pDC, m_pt_dst, CPoint(ptPos.x - size, (ptPos.y + size / 4) - size / 8), 0.5f);
			}
		}

		const int xsize = size / 20;
		const int ysize = size / 20;

		switch ( m_SelPart )
		{
		default:
			break;
		case egpBody:
			pDC->DrawFocusRect(CRect(ptPos.x - size, ptPos.y - size / LOGIC_HEIGHT,
									 ptPos.x+1, ptPos.y + size / LOGIC_HEIGHT + 1));
			break;
		case epgIn1:
			pDC->DrawFocusRect(CRect(ptPos.x - size - xsize * 2, ptPos.y - size / LOGIC_HEIGHT + ysize,
									 ptPos.x - size + xsize * 4, ptPos.y - ysize));
			break;
		case epgIn2:
			pDC->DrawFocusRect(CRect(ptPos.x - size - xsize * 2, ptPos.y + ysize,
									 ptPos.x - size + xsize * 4, ptPos.y + size / LOGIC_HEIGHT - ysize));
			break;
		}
	}

	if ( ID >= 0 )
	{
		if( m_bShowText )
		{
			CFont font;
			font.CreateFont(size / 2, 0, 0, 0, 0, FALSE, FALSE, 0, 0, 0, 0, 0, 0, _T("arial"));
			CFont * fontOld = pDC->SelectObject(&font);
			pDC->SetBkMode(TRANSPARENT);

			CString str;
			str.Format(_T("%d"), ID);
			pDC->DrawText(str,
				CRect(ptPos.x - size, ptPos.y - size / LOGIC_HEIGHT, ptPos.x, ptPos.y + size / LOGIC_HEIGHT),
				DT_SINGLELINE | DT_CENTER | DT_VCENTER);

			pDC->SelectObject(fontOld);
		}
	}

	pDC->SelectObject(penOld);
}

BOOL CLogicDesignerWnd::GetOutPortPosI( CSoftlogic::Port port, POINT * ptPos, CRect rcClient )
{
	if ( port == CSoftlogic::epNoGate )
		return FALSE;

	CT3kSoftlogicDlg * pDlg = (CT3kSoftlogicDlg *)AfxGetMainWnd();

	ASSERT( ptPos );

	int idx;

	if ( port < CSoftlogic::epGpio0 )
	{	// state
		return FALSE;
	}
	else if ( port < CSoftlogic::epSoftkey0 )
	{	// GPIO
		//ASSERT(0); //!!
		return FALSE;
	}
	else if ( port < CSoftlogic::epSoftkeyAll )
	{	// softkey
		idx = port - MM_SOFTLOGIC_PORT_SOFTKEY_START;
		if ( idx < SOFT_KEY_MAX )
		{
			CSoftkeyArray & Keys = pDlg->GetKeys();
			CSoftkey * pSoftkey = Keys[idx];
			if ( pSoftkey )
			{
				ptPos->x = (int)(pSoftkey->m_ptPosition.x * rcClient.Width()) + rcClient.left;
				ptPos->y = (int)(pSoftkey->m_ptPosition.y * rcClient.Height()) + rcClient.top;
				return TRUE;
			}
		}
	}
	else
	{ // softlogic
		idx = port - MM_SOFTLOGIC_PORT_SOFTLOGIC_START;
		if ( idx < SOFT_LOGIC_MAX )
		{
			CSoftlogicArray & Logics = pDlg->GetLogics();
			CSoftlogic * pSoftlogic = Logics[idx];
			if ( pSoftlogic )
			{
				ptPos->x = (int)(pSoftlogic->m_ptPosition.x * rcClient.Width()) + rcClient.left;
				ptPos->y = (int)(pSoftlogic->m_ptPosition.y * rcClient.Height()) + rcClient.top;
				return TRUE;
			}
		}
	}

	return FALSE;
}

void CLogicDesignerWnd::_DrawEnable( CDC * pdc, CSoftlogic::Port enable_port, BOOL enable_not,
								 int xe, int ye, int xsize, int ysize )
{
	if ( enable_port == CSoftlogic::epNoGate )
		return;

	if ( enable_not )
	{
		pdc->Ellipse(xe - xsize, ye - xsize * 2, xe + xsize, ye);
		ye -= xsize * 2;
	}

	pdc->MoveTo(xe, ye);
	pdc->LineTo(xe, ye - ysize);
	pdc->Ellipse(xe - xsize * 3, ye - ysize * 4 - ysize, xe + xsize * 3, ye - ysize);

	if( m_bShowText )
	{
		CFont font;
		font.CreateFont(ysize * 3, 0, 0, 0, 0, FALSE, FALSE, 0, 0, 0, 0, 0, 0, _T("arial"));
		CFont * fontOld = pdc->SelectObject(&font);
		pdc->SetBkMode(TRANSPARENT);
		CString strE;
		switch ( enable_port )
		{
		default:
			break;
		case CSoftlogic::epStateWorking:
			strE = _T("W"); break;
		case CSoftlogic::epStateTouch:
			strE = _T("Tch"); break;
		case CSoftlogic::epStateCalibration:
			strE = _T("Cal"); break;
		case CSoftlogic::epStateTouchScreen:
			strE = _T("TS"); break;
		case CSoftlogic::epStateMultiTouchDevice:
			strE = _T("MTD"); break;
		case CSoftlogic::epStateInvertDetection:
			strE = _T("IVT"); break;
		}
		pdc->DrawText(strE,
			CRect(xe - xsize * 3, ye - ysize * 4 - ysize, xe + xsize * 3, ye - ysize),
			DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		pdc->SelectObject(fontOld);
	}
}

void CLogicDesignerWnd::_DrawBufferGate( CDC * pdc, CPoint pt, int size,
									 CSoftlogic::Port enable_port, BOOL enable_not, BOOL in_not, BOOL bShow )
{
	CRect rcLogic(pt.x - size * 4 / 5, pt.y - size / LOGIC_HEIGHT, pt.x - size * 1 / 5, pt.y + size / LOGIC_HEIGHT);
	const int xsize = size / 20;
	const int ysize = size / 20;

	pdc->BeginPath();
	pdc->MoveTo(rcLogic.left, rcLogic.top);
	pdc->LineTo(rcLogic.left, rcLogic.bottom);
	pdc->LineTo(rcLogic.right, rcLogic.top + rcLogic.Height() / 2);
	pdc->LineTo(rcLogic.left, rcLogic.top);
	pdc->EndPath();
	if ( !bShow )
	{
		CBrush brsh;
		brsh.CreateHatchBrush(HS_BDIAGONAL, RGB(127,127,127));
		CBrush * brshOld = pdc->SelectObject(&brsh);
		pdc->StrokeAndFillPath();
		pdc->SelectObject(brshOld);
	}
	else
		pdc->StrokePath();

	pdc->MoveTo(rcLogic.right, rcLogic.top + rcLogic.Height() / 2);
	pdc->LineTo(pt.x, rcLogic.top + rcLogic.Height() / 2);

	int inx = rcLogic.left;
	if ( in_not )
	{
		pdc->Ellipse(inx - xsize * 2, rcLogic.top + rcLogic.Height() / 2 - ysize, inx, rcLogic.top + rcLogic.Height() / 2 + ysize);
		inx -= xsize * 2;
	}
	pdc->MoveTo(inx, rcLogic.top + rcLogic.Height() / 2);
	pdc->LineTo(pt.x - size, rcLogic.top + rcLogic.Height() / 2);

	int xe = rcLogic.left + rcLogic.Width() / 2 - xsize;
	int ye = rcLogic.top + ysize * 2;
	_DrawEnable(pdc, enable_port, enable_not, xe, ye, xsize, ysize);
}

void CLogicDesignerWnd::_DrawAndGate( CDC * pdc, CPoint pt, int size,
								  CSoftlogic::Port enable_port, BOOL enable_not,
								  BOOL in1_not, BOOL in2_not, BOOL bShow )
{
	CRect rcLogic(pt.x - size * 4 / 5, pt.y - size / LOGIC_HEIGHT, pt.x - size * 1 / 5, pt.y + size / LOGIC_HEIGHT);
	const int xsize = size / 20;
	const int ysize = size / 20;

	pdc->BeginPath();
	pdc->MoveTo(rcLogic.left + rcLogic.Width() / 2, rcLogic.top);
	pdc->LineTo(rcLogic.left, rcLogic.top);
	pdc->LineTo(rcLogic.left, rcLogic.bottom);
	pdc->LineTo(rcLogic.left + rcLogic.Width() / 2, rcLogic.bottom);
	pdc->ArcTo(rcLogic.left, rcLogic.top, rcLogic.right, rcLogic.bottom,
		rcLogic.left + rcLogic.Width() / 2, rcLogic.bottom, rcLogic.left + rcLogic.Width() / 2, rcLogic.top);
	pdc->EndPath();
	if ( !bShow )
	{
		CBrush brsh;
		brsh.CreateHatchBrush(HS_BDIAGONAL, RGB(127,127,127));
		CBrush * brshOld = pdc->SelectObject(&brsh);
		pdc->StrokeAndFillPath();
		pdc->SelectObject(brshOld);
	}
	else
		pdc->StrokePath();

	pdc->MoveTo(rcLogic.right, rcLogic.top + rcLogic.Height() / 2);
	pdc->LineTo(pt.x, rcLogic.top + rcLogic.Height() / 2);

	int inx = rcLogic.left;
	if ( in1_not )
	{
		pdc->Ellipse(inx - xsize * 2, rcLogic.top + rcLogic.Height() / 4 - ysize, inx, rcLogic.top + rcLogic.Height() / 4 + ysize);
		inx -= xsize * 2;
	}
	pdc->MoveTo(inx, rcLogic.top + rcLogic.Height() / 4);
	pdc->LineTo(pt.x - size, rcLogic.top + rcLogic.Height() / 4);

	inx = rcLogic.left;
	if ( in2_not )
	{
		pdc->Ellipse(inx - xsize * 2, rcLogic.top + rcLogic.Height() * 3 / 4 - ysize, inx, rcLogic.top + rcLogic.Height() * 3 / 4 + ysize);
		inx -= xsize * 2;
	}
	pdc->MoveTo(inx, rcLogic.bottom - rcLogic.Height() / 4);
	pdc->LineTo(pt.x - size, rcLogic.bottom - rcLogic.Height() / 4);

	int xe = rcLogic.left + rcLogic.Width() / 2 - xsize;
	int ye = rcLogic.top;
	_DrawEnable(pdc, enable_port, enable_not, xe, ye, xsize, ysize);
}

void CLogicDesignerWnd::_DrawOrGate( CDC * pdc, CPoint pt, int size,
								 CSoftlogic::Port enable_port, BOOL enable_not, BOOL in1_not, BOOL in2_not, BOOL bShow )
{
	CRect rcLogic(pt.x - size * 4 / 5, pt.y - size / LOGIC_HEIGHT, pt.x - size * 1 / 5, pt.y + size / LOGIC_HEIGHT);
	const float cfup = 0.18f;
	const int xsize = size / 20;
	const int ysize = size / 20;

	pdc->BeginPath();
	pdc->MoveTo(rcLogic.left, rcLogic.bottom);
	pdc->ArcTo(rcLogic.left - (int)(rcLogic.Width() * (1.f + cfup)), rcLogic.top - rcLogic.Height(), rcLogic.right + (int)(rcLogic.Width() * cfup), rcLogic.bottom,
		rcLogic.left, rcLogic.bottom, rcLogic.right, rcLogic.top + rcLogic.Height() / 2);
	pdc->ArcTo(rcLogic.left - (int)(rcLogic.Width() * (1.f + cfup)), rcLogic.top, rcLogic.right + (int)(rcLogic.Width() * cfup), rcLogic.bottom + rcLogic.Height(),
		rcLogic.right, rcLogic.top + rcLogic.Height() / 2, rcLogic.left, rcLogic.top);
	CPoint pts[3];
	pts[0] = CPoint(rcLogic.left, rcLogic.top);
	pts[1] = CPoint(rcLogic.left + (int)(rcLogic.Width() * 0.1f), (rcLogic.top + rcLogic.bottom) / 2);
	pts[2] = CPoint(rcLogic.left, rcLogic.bottom);
	pdc->PolyBezierTo(pts, 3);
	pdc->EndPath();
	if ( !bShow )
	{
		CBrush brsh;
		brsh.CreateHatchBrush(HS_BDIAGONAL, RGB(127,127,127));
		CBrush * brshOld = pdc->SelectObject(&brsh);
		pdc->StrokeAndFillPath();
		pdc->SelectObject(brshOld);
	}
	else
		pdc->StrokePath();

	pdc->MoveTo(rcLogic.right, rcLogic.top + rcLogic.Height() / 2);
	pdc->LineTo(pt.x, rcLogic.top + rcLogic.Height() / 2);

	int inx = rcLogic.left;
	if ( in1_not )
	{
		pdc->Ellipse(inx - xsize * 2, rcLogic.top + rcLogic.Height() / 4 - ysize, inx, rcLogic.top + rcLogic.Height() / 4 + ysize);
		inx -= xsize * 2;
	}
	pdc->MoveTo(inx, rcLogic.top + rcLogic.Height() / 4);
	pdc->LineTo(pt.x - size, rcLogic.top + rcLogic.Height() / 4);

	inx = rcLogic.left;
	if ( in2_not )
	{
		pdc->Ellipse(inx - xsize * 2, rcLogic.bottom - rcLogic.Height() / 4 - ysize, inx, rcLogic.bottom - rcLogic.Height() / 4 + ysize);
		inx -= xsize * 2;
	}
	pdc->MoveTo(inx, rcLogic.bottom - rcLogic.Height() / 4);
	pdc->LineTo(pt.x - size, rcLogic.bottom - rcLogic.Height() / 4);

	int xe = rcLogic.left + rcLogic.Width() / 2 - xsize;
	int ye = rcLogic.top + (int)(ysize / 1.5f);
	_DrawEnable(pdc, enable_port, enable_not, xe, ye, xsize, ysize);
}

void CLogicDesignerWnd::_DrawXorGate( CDC * pdc, CPoint pt, int size,
								  CSoftlogic::Port enable_port, BOOL enable_not, BOOL in1_not, BOOL in2_not, BOOL bShow )
{
	CRect rcLogic(pt.x - size * 4 / 5, pt.y - size / LOGIC_HEIGHT, pt.x - size * 1 / 5, pt.y + size / LOGIC_HEIGHT);
	const float cfup = 0.18f;
	const int xsize = size / 20;
	const int ysize = size / 20;

	pdc->BeginPath();
	pdc->MoveTo(rcLogic.left, rcLogic.bottom);
	pdc->ArcTo(rcLogic.left - (int)(rcLogic.Width() * (1.f + cfup)), rcLogic.top - rcLogic.Height(), rcLogic.right + (int)(rcLogic.Width() * cfup), rcLogic.bottom,
		rcLogic.left, rcLogic.bottom, rcLogic.right, rcLogic.top + rcLogic.Height() / 2);
	pdc->ArcTo(rcLogic.left - (int)(rcLogic.Width() * (1.f + cfup)), rcLogic.top, rcLogic.right + (int)(rcLogic.Width() * cfup), rcLogic.bottom + rcLogic.Height(),
		rcLogic.right, rcLogic.top + rcLogic.Height() / 2, rcLogic.left, rcLogic.top);
	CPoint pts[3];
	pts[0] = CPoint(rcLogic.left, rcLogic.top);
	pts[1] = CPoint(rcLogic.left + (int)(rcLogic.Width() * 0.1f), (rcLogic.top + rcLogic.bottom) / 2);
	pts[2] = CPoint(rcLogic.left, rcLogic.bottom);
	pdc->PolyBezierTo(pts, 3);
	pdc->MoveTo(rcLogic.left + xsize, rcLogic.top);
	pdc->EndPath();
	pts[0] = CPoint(rcLogic.left + xsize, rcLogic.top);
	pts[1] = CPoint(rcLogic.left + xsize + (int)(rcLogic.Width() * 0.1f), (rcLogic.top + rcLogic.bottom) / 2);
	pts[2] = CPoint(rcLogic.left + xsize, rcLogic.bottom);
	if ( !bShow )
	{
		CBrush brsh;
		brsh.CreateHatchBrush(HS_BDIAGONAL, RGB(127,127,127));
		CBrush * brshOld = pdc->SelectObject(&brsh);
		pdc->StrokeAndFillPath();
		pdc->SelectObject(brshOld);
	}
	else
		pdc->StrokePath();
	pdc->PolyBezierTo(pts, 3);

	pdc->MoveTo(rcLogic.right, rcLogic.top + rcLogic.Height() / 2);
	pdc->LineTo(pt.x, rcLogic.top + rcLogic.Height() / 2);

	int inx = rcLogic.left;
	if ( in1_not )
	{
		pdc->Ellipse(inx - xsize * 2, rcLogic.top + rcLogic.Height() / 4 - ysize, inx, rcLogic.top + rcLogic.Height() / 4 + ysize);
		inx -= xsize * 2;
	}
	pdc->MoveTo(inx, rcLogic.top + rcLogic.Height() / 4);
	pdc->LineTo(pt.x - size, rcLogic.top + rcLogic.Height() / 4);

	inx = rcLogic.left;
	if ( in2_not )
	{
		pdc->Ellipse(inx - xsize * 2, rcLogic.bottom - rcLogic.Height() / 4 - ysize, inx, rcLogic.bottom - rcLogic.Height() / 4 + ysize);
		inx -= xsize * 2;
	}
	pdc->MoveTo(inx, rcLogic.bottom - rcLogic.Height() / 4);
	pdc->LineTo(pt.x - size, rcLogic.bottom - rcLogic.Height() / 4);

	int xe = rcLogic.left + rcLogic.Width() / 2 - xsize;
	int ye = rcLogic.top + (int)(ysize / 1.5f);
	_DrawEnable(pdc, enable_port, enable_not, xe, ye, xsize, ysize);
}

void CLogicDesignerWnd::_DrawAndRisingedgeGate( CDC * pdc, CPoint pt, int size,
											CSoftlogic::Port enable_port, BOOL enable_not,
											BOOL in1_not, BOOL in2_not, BOOL bShow )
{
	CRect rcLogic(pt.x - size * 4 / 5, pt.y - size / LOGIC_HEIGHT, pt.x - size * 1 / 5, pt.y + size / LOGIC_HEIGHT);
	const int xsize = size / 20;
	const int ysize = size / 20;

	pdc->BeginPath();
	pdc->MoveTo(rcLogic.left + rcLogic.Width() / 2, rcLogic.top);
	pdc->LineTo(rcLogic.left, rcLogic.top);
	pdc->LineTo(rcLogic.left, rcLogic.bottom);
	pdc->LineTo(rcLogic.left + rcLogic.Width() / 2, rcLogic.bottom);
	pdc->ArcTo(rcLogic.left + xsize, rcLogic.top, rcLogic.right - xsize, rcLogic.bottom,
		rcLogic.left + rcLogic.Width() / 2, rcLogic.bottom, rcLogic.left + rcLogic.Width() / 2, rcLogic.top);
	pdc->EndPath();
	if ( !bShow )
	{
		CBrush brsh;
		brsh.CreateHatchBrush(HS_BDIAGONAL, RGB(127,127,127));
		CBrush * brshOld = pdc->SelectObject(&brsh);
		pdc->StrokeAndFillPath();
		pdc->SelectObject(brshOld);
	}
	else
		pdc->StrokePath();
	pdc->MoveTo(rcLogic.right - xsize, rcLogic.top);
	pdc->LineTo(rcLogic.right - xsize, rcLogic.bottom);
	pdc->MoveTo(rcLogic.right, rcLogic.top);
	pdc->LineTo(rcLogic.right, rcLogic.bottom);

	pdc->MoveTo(rcLogic.right, rcLogic.top + rcLogic.Height() / 2);
	pdc->LineTo(pt.x, rcLogic.top + rcLogic.Height() / 2);

	int inx = rcLogic.left;
	if ( in1_not )
	{
		pdc->Ellipse(inx - xsize * 2, rcLogic.top + rcLogic.Height() / 4 - ysize, inx, rcLogic.top + rcLogic.Height() / 4 + ysize);
		inx -= xsize * 2;
	}
	pdc->MoveTo(inx, rcLogic.top + rcLogic.Height() / 4);
	pdc->LineTo(pt.x - size, rcLogic.top + rcLogic.Height() / 4);

	inx = rcLogic.left;
	if ( in2_not )
	{
		pdc->Ellipse(inx - xsize * 2, rcLogic.top + rcLogic.Height() * 3 / 4 - ysize, inx, rcLogic.top + rcLogic.Height() * 3 / 4 + ysize);
		inx -= xsize * 2;
	}
	pdc->MoveTo(inx, rcLogic.bottom - rcLogic.Height() / 4);
	pdc->LineTo(pt.x - size, rcLogic.bottom - rcLogic.Height() / 4);

	int xe = rcLogic.left + rcLogic.Width() / 2 - xsize;
	int ye = rcLogic.top;
	_DrawEnable(pdc, enable_port, enable_not, xe, ye, xsize, ysize);
}

void CLogicDesignerWnd::_DrawTrigger( CDC * pdc, CPoint pt, int size, CSoftlogic::Trigger trigger )
{
	if ( trigger == CSoftlogic::etNoTrigger )
		return;

	CRect rcLogic(pt.x - size * 4 / 5, pt.y - size / LOGIC_HEIGHT, pt.x - size * 1 / 5, pt.y + size / LOGIC_HEIGHT);
	const int xsize = size / 20;
	const int ysize = size / 20;

	pdc->MoveTo(rcLogic.left + rcLogic.Width() / 2 - xsize * 2, rcLogic.bottom - ysize * 3);
	pdc->LineTo(rcLogic.left + rcLogic.Width() / 2 - xsize * 2, rcLogic.top + ysize * 3);
	pdc->LineTo(rcLogic.left + rcLogic.Width() / 2 - xsize * 3, rcLogic.top + ysize * 4);

	pdc->MoveTo(rcLogic.left + rcLogic.Width() / 2 - xsize, rcLogic.top + ysize * 3);
	pdc->LineTo(rcLogic.left + rcLogic.Width() / 2 - xsize, rcLogic.bottom - ysize * 3);
	pdc->LineTo(rcLogic.left + rcLogic.Width() / 2, rcLogic.bottom - ysize * 4);

	if( m_bShowText )
	{
		CFont font;
		font.CreateFont((int)(ysize * 3), xsize, 0, 0, 0, FALSE, FALSE, 0, 0, 0, 0, 0, 0, _T("arial"));
		CFont * fontOld = pdc->SelectObject(&font);
		pdc->SetBkMode(TRANSPARENT);

		switch ( trigger )
		{
		default:
		case CSoftlogic::etNoTrigger:
			break;

		case CSoftlogic::etTrigger_U100_D100:
//			pdc->DrawText(_T("100"), 3,
			pdc->DrawText(_T("128"), 3,
				CRect(rcLogic.left - size, rcLogic.top, rcLogic.left + rcLogic.Width() / 2 - xsize * 5 / 2, rcLogic.bottom),
				DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
//			pdc->DrawText(_T("100"), 3,
			pdc->DrawText(_T("128"), 3,
				CRect(rcLogic.left + rcLogic.Width() / 2 - xsize / 2, rcLogic.top, rcLogic.right + size, rcLogic.bottom),
				DT_SINGLELINE | DT_LEFT | DT_VCENTER);
			break;

		case CSoftlogic::etTrigger_U250_D250:
//			pdc->DrawText(_T("250"), 3,
			pdc->DrawText(_T("256"), 3,
				CRect(rcLogic.left - size, rcLogic.top, rcLogic.left + rcLogic.Width() / 2 - xsize * 5 / 2, rcLogic.bottom),
				DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
//			pdc->DrawText(_T("250"), 3,
			pdc->DrawText(_T("256"), 3,
				CRect(rcLogic.left + rcLogic.Width() / 2 - xsize / 2, rcLogic.top, rcLogic.right + size, rcLogic.bottom),
				DT_SINGLELINE | DT_LEFT | DT_VCENTER);
			break;

		case CSoftlogic::etTrigger_U500_D500:
//			pdc->DrawText(_T("500"), 3,
			pdc->DrawText(_T("512"), 3,
				CRect(rcLogic.left - size, rcLogic.top, rcLogic.left + rcLogic.Width() / 2 - xsize * 5 / 2, rcLogic.bottom),
				DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
//			pdc->DrawText(_T("500"), 3,
			pdc->DrawText(_T("512"), 3,
				CRect(rcLogic.left + rcLogic.Width() / 2 - xsize / 2, rcLogic.top, rcLogic.right + size, rcLogic.bottom),
				DT_SINGLELINE | DT_LEFT | DT_VCENTER);
			break;

		case CSoftlogic::etTrigger_U1000_D1000:
//			pdc->DrawText(_T("1000"), 4,
			pdc->DrawText(_T("1024"), 4,
				CRect(rcLogic.left - size, rcLogic.top, rcLogic.left + rcLogic.Width() / 2 - xsize * 5 / 2, rcLogic.bottom),
				DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
//			pdc->DrawText(_T("1000"), 4,
			pdc->DrawText(_T("1024"), 4,
				CRect(rcLogic.left + rcLogic.Width() / 2 - xsize / 2, rcLogic.top, rcLogic.right + size, rcLogic.bottom),
				DT_SINGLELINE | DT_LEFT | DT_VCENTER);
			break;

		case CSoftlogic::etTrigger_U2000_D2000:
//			pdc->DrawText(_T("2000"), 4,
			pdc->DrawText(_T("2048"), 4,
				CRect(rcLogic.left - size, rcLogic.top, rcLogic.left + rcLogic.Width() / 2 - xsize * 5 / 2, rcLogic.bottom),
				DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
//			pdc->DrawText(_T("2000"), 4,
			pdc->DrawText(_T("2048"), 4,
				CRect(rcLogic.left + rcLogic.Width() / 2 - xsize / 2, rcLogic.top, rcLogic.right + size, rcLogic.bottom),
				DT_SINGLELINE | DT_LEFT | DT_VCENTER);
			break;

		case CSoftlogic::etTrigger_U250_D0:
//			pdc->DrawText(_T("250"), 3,
			pdc->DrawText(_T("256"), 3,
				CRect(rcLogic.left - size, rcLogic.top, rcLogic.left + rcLogic.Width() / 2 - xsize * 5 / 2, rcLogic.bottom),
				DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
			pdc->DrawText(_T("0"), 1,
				CRect(rcLogic.left + rcLogic.Width() / 2 - xsize / 2, rcLogic.top, rcLogic.right + size, rcLogic.bottom),
				DT_SINGLELINE | DT_LEFT | DT_VCENTER);
			break;

		case CSoftlogic::etTrigger_U500_D0:
//			pdc->DrawText(_T("500"), 3,
			pdc->DrawText(_T("512"), 3,
				CRect(rcLogic.left - size, rcLogic.top, rcLogic.left + rcLogic.Width() / 2 - xsize * 5 / 2, rcLogic.bottom),
				DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
			pdc->DrawText(_T("0"), 1,
				CRect(rcLogic.left + rcLogic.Width() / 2 - xsize / 2, rcLogic.top, rcLogic.right + size, rcLogic.bottom),
				DT_SINGLELINE | DT_LEFT | DT_VCENTER);
			break;

		case CSoftlogic::etTrigger_U1000_D0:
//			pdc->DrawText(_T("1000"), 4,
			pdc->DrawText(_T("1024"), 4,
				CRect(rcLogic.left - size, rcLogic.top, rcLogic.left + rcLogic.Width() / 2 - xsize * 5 / 2, rcLogic.bottom),
				DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
			pdc->DrawText(_T("0"), 1,
				CRect(rcLogic.left + rcLogic.Width() / 2 - xsize / 2, rcLogic.top, rcLogic.right + size, rcLogic.bottom),
				DT_SINGLELINE | DT_LEFT | DT_VCENTER);
			break;

		case CSoftlogic::etTrigger_U2000_D0:
//			pdc->DrawText(_T("2000"), 4,
			pdc->DrawText(_T("2048"), 4,
				CRect(rcLogic.left - size, rcLogic.top, rcLogic.left + rcLogic.Width() / 2 - xsize * 5 / 2, rcLogic.bottom),
				DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
			pdc->DrawText(_T("0"), 1,
				CRect(rcLogic.left + rcLogic.Width() / 2 - xsize / 2, rcLogic.top, rcLogic.right + size, rcLogic.bottom),
				DT_SINGLELINE | DT_LEFT | DT_VCENTER);
			break;

		case CSoftlogic::etTrigger_U3000_D0:
//			pdc->DrawText(_T("3000"), 4,
			pdc->DrawText(_T("3072"), 4,
				CRect(rcLogic.left - size, rcLogic.top, rcLogic.left + rcLogic.Width() / 2 - xsize * 5 / 2, rcLogic.bottom),
				DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
			pdc->DrawText(_T("0"), 1,
				CRect(rcLogic.left + rcLogic.Width() / 2 - xsize / 2, rcLogic.top, rcLogic.right + size, rcLogic.bottom),
				DT_SINGLELINE | DT_LEFT | DT_VCENTER);
			break;

		case CSoftlogic::etTrigger_U200_D50:
//			pdc->DrawText(_T("200"), 3,
			pdc->DrawText(_T("192"), 3,
				CRect(rcLogic.left - size, rcLogic.top, rcLogic.left + rcLogic.Width() / 2 - xsize * 5 / 2, rcLogic.bottom),
				DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
//			pdc->DrawText(_T("50"), 2,
			pdc->DrawText(_T("64"), 2,
				CRect(rcLogic.left + rcLogic.Width() / 2 - xsize / 2, rcLogic.top, rcLogic.right + size, rcLogic.bottom),
				DT_SINGLELINE | DT_LEFT | DT_VCENTER);
			break;

		case CSoftlogic::etTrigger_U400_D100:
//			pdc->DrawText(_T("400"), 3,
			pdc->DrawText(_T("384"), 3,
				CRect(rcLogic.left - size, rcLogic.top, rcLogic.left + rcLogic.Width() / 2 - xsize * 5 / 2, rcLogic.bottom),
				DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
//			pdc->DrawText(_T("100"), 3,
			pdc->DrawText(_T("128"), 3,
				CRect(rcLogic.left + rcLogic.Width() / 2 - xsize / 2, rcLogic.top, rcLogic.right + size, rcLogic.bottom),
				DT_SINGLELINE | DT_LEFT | DT_VCENTER);
			break;

		case CSoftlogic::etTrigger_U750_D250:
//			pdc->DrawText(_T("750"), 3,
			pdc->DrawText(_T("768"), 3,
				CRect(rcLogic.left - size, rcLogic.top, rcLogic.left + rcLogic.Width() / 2 - xsize * 5 / 2, rcLogic.bottom),
				DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
//			pdc->DrawText(_T("250"), 3,
			pdc->DrawText(_T("256"), 3,
				CRect(rcLogic.left + rcLogic.Width() / 2 - xsize / 2, rcLogic.top, rcLogic.right + size, rcLogic.bottom),
				DT_SINGLELINE | DT_LEFT | DT_VCENTER);
		break;

		case CSoftlogic::etTrigger_U1500_D500:
//			pdc->DrawText(_T("1500"), 4,
			pdc->DrawText(_T("1536"), 4,
				CRect(rcLogic.left - size, rcLogic.top, rcLogic.left + rcLogic.Width() / 2 - xsize * 5 / 2, rcLogic.bottom),
				DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
//			pdc->DrawText(_T("500"), 3,
			pdc->DrawText(_T("512"), 3,
				CRect(rcLogic.left + rcLogic.Width() / 2 - xsize / 2, rcLogic.top, rcLogic.right + size, rcLogic.bottom),
				DT_SINGLELINE | DT_LEFT | DT_VCENTER);
			break;

		case CSoftlogic::etTrigger_U2000_D1000:
//			pdc->DrawText(_T("2000"), 4,
			pdc->DrawText(_T("2048"), 4,
				CRect(rcLogic.left - size, rcLogic.top, rcLogic.left + rcLogic.Width() / 2 - xsize * 5 / 2, rcLogic.bottom),
				DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
//			pdc->DrawText(_T("1000"), 4,
			pdc->DrawText(_T("1024"), 4,
				CRect(rcLogic.left + rcLogic.Width() / 2 - xsize / 2, rcLogic.top, rcLogic.right + size, rcLogic.bottom),
				DT_SINGLELINE | DT_LEFT | DT_VCENTER);
			break;
		}

		pdc->SelectObject(fontOld);
	}
}

void CLogicDesignerWnd::_DrawLine2( Graphics& g, Pen& LinePen, CPoint pt1, CPoint pt2, float fD, int nThXY )
{
	Point pts[4];

	if ( pt1.x < pt2.x )
	{
		int xD = pt1.x + (int)((pt2.x - pt1.x) * fD + .5f);
		pts[0].X = pt1.x;	pts[0].Y = pt1.y;
		pts[1].X = xD;		pts[1].Y = pt1.y;
		pts[2].X = xD;		pts[2].Y = pt2.y;
		pts[3].X = pt2.x;	pts[3].Y = pt2.y;
	}
	else
	{
		int yD = pt1.y + (int)((pt2.y - pt1.y) * fD + .5f);
		pts[0].X = pt1.x;	pts[0].Y = pt1.y;
		pts[1].X = pt1.x;	pts[1].Y = yD;
		pts[2].X = pt2.x;	pts[2].Y = yD;
		pts[3].X = pt2.x;	pts[3].Y = pt2.y;
	}

	g.DrawLines( &LinePen, pts, 4 );
}

void CLogicDesignerWnd::_DrawLine( CDC * pdc, CPoint pt1, CPoint pt2, float fD )
{
	if ( pt1.x < pt2.x )
	{
		int xD = pt1.x + (int)((pt2.x - pt1.x) * fD);
		pdc->MoveTo(pt1.x, pt1.y);
		pdc->LineTo(xD, pt1.y);
		pdc->LineTo(xD, pt2.y);
		pdc->LineTo(pt2.x, pt2.y);
	}
	else
	{
		int yD = pt1.y + (int)((pt2.y - pt1.y) * fD);
		pdc->MoveTo(pt1.x, pt1.y);
		pdc->LineTo(pt1.x, yD);
		pdc->LineTo(pt2.x, yD);
		pdc->LineTo(pt2.x, pt2.y);
	}
}

void CLogicDesignerWnd::_DrawInPort( CDC * pdc, CSoftlogic::Port port, CPoint pt, int size, BOOL bUp, BOOL bDefaultTrue, BOOL bNoGateOK )
{
	const int xsize = size / 20;
	const int ysize = size / 20;

	CFont font;
	font.CreateFont(ysize * 3, 0, 0, 0, 0, FALSE, FALSE, 0, 0, 0, 0, 0, 0, _T("arial"));
	CFont * fontOld = pdc->SelectObject(&font);
	pdc->SetBkMode(TRANSPARENT);

	CRect rc;

	if ( port == CSoftlogic::epNoGate )
	{
		rc = CRect(pt.x - xsize * 4, pt.y - ysize * 2, pt.x, pt.y + ysize * 2);
		pdc->Ellipse(&rc);
		if ( bNoGateOK )
		{
			if( m_bShowText )
			{
				if ( bDefaultTrue )
					pdc->DrawText(_T("1"), 1, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
				else
					pdc->DrawText(_T("0"), 1, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			}
		}
		else
		{
			pdc->MoveTo(pt.x - xsize * 4, pt.y - ysize * 2);
			pdc->LineTo(pt.x, pt.y + ysize * 2);
			pdc->MoveTo(pt.x, pt.y - ysize * 2);
			pdc->LineTo(pt.x - xsize * 4, pt.y + ysize * 2);
		}
	}
	else if ( port < CSoftlogic::epGpio0 )
	{	// state
		CString strE;
		switch ( port )
		{
		default:
			goto jmp_x;
		case CSoftlogic::epStateWorking:
			strE = _T("W"); break;
		case CSoftlogic::epStateTouch:
			strE = _T("Tch"); break;
		case CSoftlogic::epStateCalibration:
			strE = _T("Cal"); break;
		case CSoftlogic::epStateTouchScreen:
			strE = _T("TS"); break;
		case CSoftlogic::epStateMultiTouchDevice:
			strE = _T("MTD"); break;
		case CSoftlogic::epStateInvertDetection:
			strE = _T("IVT"); break;
		}
		pdc->MoveTo(pt.x, pt.y);
		if ( bUp )
		{
			pdc->LineTo(pt.x, pt.y - ysize);
			rc = CRect(pt.x - xsize * 3, pt.y - ysize - ysize * 4, pt.x + xsize * 3, pt.y - ysize);
		}
		else
		{
			pdc->LineTo(pt.x, pt.y + ysize);
			rc = CRect(pt.x - xsize * 3, pt.y + ysize, pt.x + xsize * 3, pt.y + ysize + ysize * 4);
		}
		pdc->Ellipse(&rc);
		if( m_bShowText )
			pdc->DrawText(strE, rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	}
	else if ( port < CSoftlogic::epSoftkey0 )
	{	// GPIO
		//ASSERT(0); //!!
		rc = CRect(pt.x - xsize * 4, pt.y - ysize * 2, pt.x, pt.y + ysize * 2);
		CPen penKey;
		penKey.CreatePen(PS_SOLID, 1, m_clrKeyBorderNormal);
		CPen* pOldPen = pdc->SelectObject( &penKey );
		pdc->Rectangle(&rc);
		CString strGPIOText;
		strGPIOText.Format( _T("IO%d"), (port - CSoftlogic::epGpio0) + 1 );
		if( m_bShowText )
			pdc->DrawText(strGPIOText, 3, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		pdc->SelectObject( pOldPen );
	}
	else if ( port == CSoftlogic::epSoftkeyAll )
	{
		rc = CRect(pt.x - xsize * 4, pt.y - ysize * 2, pt.x, pt.y + ysize * 2);
		CPen penKey;
		penKey.CreatePen(PS_SOLID, 1, m_clrKeyBorderNormal);
		CPen* pOldPen = pdc->SelectObject( &penKey );
		pdc->Rectangle(&rc);
		if( m_bShowText )
			pdc->DrawText(_T("All"), 3, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		pdc->SelectObject( pOldPen );
	}
	else
	{	// softkey, softlogic
jmp_x:
		rc = CRect(pt.x - xsize * 4, pt.y - ysize * 2, pt.x, pt.y + ysize * 2);
		pdc->Ellipse(&rc);
		if( m_bShowText )
			pdc->DrawText(_T("?"), 1, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	}

	pdc->SelectObject(fontOld);
}

void CLogicDesignerWnd::_DrawOutKey( CDC * pdc, CPoint pt, int size, BYTE ckey, BYTE key )
{
	const int xsize = size / 20;
	const int ysize = size / 20;

	pdc->MoveTo(pt.x, pt.y);
	pdc->LineTo(pt.x, pt.y - ysize);
	if ( (ckey & (MM_SOFTLOGIC_CKEY_CTRL | MM_SOFTLOGIC_CKEY_ALT | MM_SOFTLOGIC_CKEY_SHIFT | MM_SOFTLOGIC_CKEY_WINDOW)) != 0x0 )
		pdc->Rectangle(pt.x - xsize * 4, pt.y - ysize * 6 - ysize, pt.x + xsize * 4, pt.y - ysize);
	else
		pdc->Rectangle(pt.x - xsize * 4, pt.y - ysize * 4 - ysize, pt.x + xsize * 4, pt.y - ysize);

	if( m_bShowText )
	{
		CFont font;
		font.CreateFont(ysize * 3, 0, 0, 0, 0, FALSE, FALSE, 0, 0, 0, 0, 0, 0, _T("arial"));
		CFont * fontOld = pdc->SelectObject(&font);
		pdc->SetBkMode(TRANSPARENT);
		CString str;
		if ( (ckey & (MM_SOFTLOGIC_CKEY_CTRL | MM_SOFTLOGIC_CKEY_ALT | MM_SOFTLOGIC_CKEY_SHIFT | MM_SOFTLOGIC_CKEY_WINDOW)) != 0x0 )
		{
			str = _T("[");
			if ( (ckey & MM_SOFTLOGIC_CKEY_CTRL) != 0x0 ) str += 'C';
			if ( (ckey & MM_SOFTLOGIC_CKEY_ALT) != 0x0 ) str += 'A';
			if ( (ckey & MM_SOFTLOGIC_CKEY_SHIFT) != 0x0 ) str += 'S';
			if ( (ckey & MM_SOFTLOGIC_CKEY_WINDOW) != 0x0 ) str += 'W';
			str += _T("]");
			pdc->DrawText(str,
				CRect(pt.x - xsize * 5, pt.y - ysize * 7 - ysize, pt.x + xsize * 5, pt.y - ysize * 3),
				DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		}
		int vkey = 0;
		Graphics g(pdc->GetSafeHdc());
		int nIX, nIY;
#define ICON_WIDTH (16)
#define ICON_HEIGHT (16)
		nIX = pt.x - ICON_WIDTH / 2;
		nIY = pt.y - ysize - ysize * 2 - ICON_HEIGHT / 2;
		switch( key )
		{
		case V_KEY_VOLUME_INC_VALUE:
			pdc->FillSolidRect( CRect(pt.x - xsize * 4+1, pt.y - ysize * 4 - ysize+1, pt.x + xsize * 4-1, pt.y - ysize-1), RGB(220, 180, 180) );
			g.DrawImage( m_pIconConsumerButton, Rect(nIX, nIY, ICON_WIDTH, ICON_HEIGHT), 0*ICON_WIDTH, 0, ICON_WIDTH, ICON_HEIGHT, UnitPixel );
			break;
		case V_KEY_VOLUME_DEC_VALUE:
			pdc->FillSolidRect( CRect(pt.x - xsize * 4+1, pt.y - ysize * 4 - ysize+1, pt.x + xsize * 4-1, pt.y - ysize-1), RGB(220, 180, 180) );
			g.DrawImage( m_pIconConsumerButton, Rect(nIX, nIY, ICON_WIDTH, ICON_HEIGHT), 1*ICON_WIDTH, 0, ICON_WIDTH, ICON_HEIGHT, UnitPixel );
			break;
		case V_KEY_VOLUME_MUTE_VALUE:
			pdc->FillSolidRect( CRect(pt.x - xsize * 4+1, pt.y - ysize * 4 - ysize+1, pt.x + xsize * 4-1, pt.y - ysize-1), RGB(220, 180, 180) );
			g.DrawImage( m_pIconConsumerButton, Rect(nIX, nIY, ICON_WIDTH, ICON_HEIGHT), 2*ICON_WIDTH, 0, ICON_WIDTH, ICON_HEIGHT, UnitPixel );
			break;
		case V_KEY_TRANSPORT_PLAY:
			pdc->FillSolidRect( CRect(pt.x - xsize * 4+1, pt.y - ysize * 4 - ysize+1, pt.x + xsize * 4-1, pt.y - ysize-1), RGB(220, 180, 180) );
			g.DrawImage( m_pIconConsumerButton, Rect(nIX, nIY, ICON_WIDTH, ICON_HEIGHT), 3*ICON_WIDTH, 0, ICON_WIDTH, ICON_HEIGHT, UnitPixel );
			break;
		case V_KEY_TRANSPORT_PAUSE:
			pdc->FillSolidRect( CRect(pt.x - xsize * 4+1, pt.y - ysize * 4 - ysize+1, pt.x + xsize * 4-1, pt.y - ysize-1), RGB(220, 180, 180) );
			g.DrawImage( m_pIconConsumerButton, Rect(nIX, nIY, ICON_WIDTH, ICON_HEIGHT), 4*ICON_WIDTH, 0, ICON_WIDTH, ICON_HEIGHT, UnitPixel );
			break;
		case V_KEY_TRANSPORT_FFORWARD:
			pdc->FillSolidRect( CRect(pt.x - xsize * 4+1, pt.y - ysize * 4 - ysize+1, pt.x + xsize * 4-1, pt.y - ysize-1), RGB(220, 180, 180) );
			g.DrawImage( m_pIconConsumerButton, Rect(nIX, nIY, ICON_WIDTH, ICON_HEIGHT), 5*ICON_WIDTH, 0, ICON_WIDTH, ICON_HEIGHT, UnitPixel );
			break;
		case V_KEY_TRANSPORT_REWIND:
			pdc->FillSolidRect( CRect(pt.x - xsize * 4+1, pt.y - ysize * 4 - ysize+1, pt.x + xsize * 4-1, pt.y - ysize-1), RGB(220, 180, 180) );
			g.DrawImage( m_pIconConsumerButton, Rect(nIX, nIY, ICON_WIDTH, ICON_HEIGHT), 6*ICON_WIDTH, 0, ICON_WIDTH, ICON_HEIGHT, UnitPixel );
			break;
		case V_KEY_TRANSPORT_NEXT_TRACK:
			pdc->FillSolidRect( CRect(pt.x - xsize * 4+1, pt.y - ysize * 4 - ysize+1, pt.x + xsize * 4-1, pt.y - ysize-1), RGB(220, 180, 180) );
			g.DrawImage( m_pIconConsumerButton, Rect(nIX, nIY, ICON_WIDTH, ICON_HEIGHT), 7*ICON_WIDTH, 0, ICON_WIDTH, ICON_HEIGHT, UnitPixel );
			break;
		case V_KEY_TRANSPORT_PREV_TRACK:
			pdc->FillSolidRect( CRect(pt.x - xsize * 4+1, pt.y - ysize * 4 - ysize+1, pt.x + xsize * 4-1, pt.y - ysize-1), RGB(220, 180, 180) );
			g.DrawImage( m_pIconConsumerButton, Rect(nIX, nIY, ICON_WIDTH, ICON_HEIGHT), 8*ICON_WIDTH, 0, ICON_WIDTH, ICON_HEIGHT, UnitPixel );
			break;
		case V_KEY_TRANSPORT_STOP:
			pdc->FillSolidRect( CRect(pt.x - xsize * 4+1, pt.y - ysize * 4 - ysize+1, pt.x + xsize * 4-1, pt.y - ysize-1), RGB(220, 180, 180) );
			g.DrawImage( m_pIconConsumerButton, Rect(nIX, nIY, ICON_WIDTH, ICON_HEIGHT), 9*ICON_WIDTH, 0, ICON_WIDTH, ICON_HEIGHT, UnitPixel );
			break;
		case V_KEY_TRANSPORT_EJECT:
			pdc->FillSolidRect( CRect(pt.x - xsize * 4+1, pt.y - ysize * 4 - ysize+1, pt.x + xsize * 4-1, pt.y - ysize-1), RGB(220, 180, 180) );
			g.DrawImage( m_pIconConsumerButton, Rect(nIX, nIY, ICON_WIDTH, ICON_HEIGHT), 10*ICON_WIDTH, 0, ICON_WIDTH, ICON_HEIGHT, UnitPixel );
			break;
		case V_KEY_TRANSPORT_PLAYPAUSE:
			pdc->FillSolidRect( CRect(pt.x - xsize * 4+1, pt.y - ysize * 4 - ysize+1, pt.x + xsize * 4-1, pt.y - ysize-1), RGB(220, 180, 180) );
			g.DrawImage( m_pIconConsumerButton, Rect(nIX-ICON_WIDTH/3, nIY, ICON_WIDTH, ICON_HEIGHT), 3*ICON_WIDTH, 0, ICON_WIDTH, ICON_HEIGHT, UnitPixel );
			g.DrawImage( m_pIconConsumerButton, Rect(nIX+ICON_WIDTH/3, nIY, ICON_WIDTH, ICON_HEIGHT), 4*ICON_WIDTH, 0, ICON_WIDTH, ICON_HEIGHT, UnitPixel );
			break;
		case V_KEY_AL_EMAIL_VIEWER:
			pdc->FillSolidRect( CRect(pt.x - xsize * 4+1, pt.y - ysize * 4 - ysize+1, pt.x + xsize * 4-1, pt.y - ysize-1), RGB(180, 180, 220) );
			g.DrawImage( m_pIconConsumerButton, Rect(nIX, nIY, ICON_WIDTH, ICON_HEIGHT), 12*ICON_WIDTH, 0, ICON_WIDTH, ICON_HEIGHT, UnitPixel );
			break;
		case V_KEY_AL_CALCULATOR:
			pdc->FillSolidRect( CRect(pt.x - xsize * 4+1, pt.y - ysize * 4 - ysize+1, pt.x + xsize * 4-1, pt.y - ysize-1), RGB(180, 180, 220) );
			g.DrawImage( m_pIconConsumerButton, Rect(nIX, nIY, ICON_WIDTH, ICON_HEIGHT), 13*ICON_WIDTH, 0, ICON_WIDTH, ICON_HEIGHT, UnitPixel );
			break;
		case V_KEY_AL_MY_COMPUTER:
			pdc->FillSolidRect( CRect(pt.x - xsize * 4+1, pt.y - ysize * 4 - ysize+1, pt.x + xsize * 4-1, pt.y - ysize-1), RGB(180, 180, 220) );
			g.DrawImage( m_pIconConsumerButton, Rect(nIX, nIY, ICON_WIDTH, ICON_HEIGHT), 14*ICON_WIDTH, 0, ICON_WIDTH, ICON_HEIGHT, UnitPixel );
			break;
		case V_KEY_AL_WEB_BROWSER:
			pdc->FillSolidRect( CRect(pt.x - xsize * 4+1, pt.y - ysize * 4 - ysize+1, pt.x + xsize * 4-1, pt.y - ysize-1), RGB(180, 180, 220) );
			g.DrawImage( m_pIconConsumerButton, Rect(nIX, nIY, ICON_WIDTH, ICON_HEIGHT), 15*ICON_WIDTH, 0, ICON_WIDTH, ICON_HEIGHT, UnitPixel );
			break;
		default:
			FindScanCode(key, vkey);
			vkey = ::MapVirtualKey(vkey, 1);
			str = VirtualKeyToString(vkey);
			pdc->DrawText(str,
				CRect(pt.x - xsize * 4, pt.y - ysize * 4 - ysize, pt.x + xsize * 4, pt.y - ysize),
				DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			break;
		}
		pdc->SelectObject(fontOld);
	}
}

void CLogicDesignerWnd::_DrawOutMouse( CDC * pdc, CPoint pt, int size, BYTE ckey, BYTE mkey )
{
	const int xsize = size / 20;
	const int ysize = size / 20;

	pdc->MoveTo(pt.x, pt.y);
	pdc->LineTo(pt.x, pt.y - ysize);
	if ( (ckey & (MM_SOFTLOGIC_CKEY_CTRL | MM_SOFTLOGIC_CKEY_ALT | MM_SOFTLOGIC_CKEY_SHIFT | MM_SOFTLOGIC_CKEY_WINDOW)) != 0x0 )
		pdc->Rectangle(pt.x - xsize * 4, pt.y - ysize * 6 - ysize, pt.x + xsize * 4, pt.y - ysize);
	else
		pdc->Rectangle(pt.x - xsize * 4, pt.y - ysize * 4 - ysize, pt.x + xsize * 4, pt.y - ysize);
	pdc->MoveTo(pt.x - xsize * 4, pt.y - (int)(ysize * 0.9f));
	pdc->LineTo(pt.x + xsize * 4, pt.y - (int)(ysize * 0.9f));

	if( m_bShowText )
	{
		CFont font;
		font.CreateFont(ysize * 3, 0, 0, 0, 0, FALSE, FALSE, 0, 0, 0, 0, 0, 0, _T("arial"));
		CFont * fontOld = pdc->SelectObject(&font);
		pdc->SetBkMode(TRANSPARENT);
		CString str;
		if ( (ckey & (MM_SOFTLOGIC_CKEY_CTRL | MM_SOFTLOGIC_CKEY_ALT | MM_SOFTLOGIC_CKEY_SHIFT | MM_SOFTLOGIC_CKEY_WINDOW)) != 0x0 )
		{
			str = _T("[");
			if ( (ckey & MM_SOFTLOGIC_CKEY_CTRL) != 0x0 ) str += 'C';
			if ( (ckey & MM_SOFTLOGIC_CKEY_ALT) != 0x0 ) str += 'A';
			if ( (ckey & MM_SOFTLOGIC_CKEY_SHIFT) != 0x0 ) str += 'S';
			if ( (ckey & MM_SOFTLOGIC_CKEY_WINDOW) != 0x0 ) str += 'W';
			str += _T("]");
			pdc->DrawText(str,
				CRect(pt.x - xsize * 5, pt.y - ysize * 7 - ysize, pt.x + xsize * 5, pt.y - ysize * 3),
				DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		}
		str = _T("(");
		if ( (mkey & MM_SOFTLOGIC_OPEX_MKEY_ACT_LBTN) != 0x0 ) str += 'L';
		if ( (mkey & MM_SOFTLOGIC_OPEX_MKEY_ACT_RBTN) != 0x0 ) str += 'R';
		if ( (mkey & MM_SOFTLOGIC_OPEX_MKEY_ACT_MBTN) != 0x0 ) str += 'M';
		str += _T(")");
		pdc->DrawText(str,
			CRect(pt.x - xsize * 4, pt.y - ysize * 4 - ysize, pt.x + xsize * 4, pt.y - ysize),
			DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		pdc->SelectObject(fontOld);
	}
}

void CLogicDesignerWnd::_DrawOutState( CDC * pdc, CPoint pt, int size, CSoftlogic::Port out_port )
{
	const int xsize = size / 20;
	const int ysize = size / 20;

	if ( out_port == CSoftlogic::epNoGate )
		return;

	pdc->MoveTo(pt.x, pt.y);
	pdc->LineTo(pt.x, pt.y - ysize);
	pdc->Ellipse(pt.x - xsize * 4, pt.y - ysize * 4 - ysize, pt.x + xsize * 4, pt.y - ysize);

	if( m_bShowText )
	{
		CFont font;
		font.CreateFont(ysize * 3, 0, 0, 0, 0, FALSE, FALSE, 0, 0, 0, 0, 0, 0, _T("arial"));
		CFont * fontOld = pdc->SelectObject(&font);
		pdc->SetBkMode(TRANSPARENT);
		CString strE;
		switch ( out_port )
		{
		default:
			break;
		case CSoftlogic::epStateWorking:
			strE = _T("Beep1"); break;
		case CSoftlogic::epStateTouch:
			strE = _T("Beep2"); break;
		case CSoftlogic::epStateCalibration:
			strE = _T("Cal"); break;
		case CSoftlogic::epStateTouchScreen:
			strE = _T("TS"); break;
		case CSoftlogic::epStateMultiTouchDevice:
			strE = _T("MTD"); break;
		case CSoftlogic::epStateInvertDetection:
			strE = _T("IVT"); break;
		}
		pdc->DrawText(strE,
			CRect(pt.x - xsize * 5, pt.y - ysize * 4 - ysize, pt.x + xsize * 5, pt.y - ysize),
			DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		pdc->SelectObject(fontOld);
	}
}

void CLogicDesignerWnd::_DrawOutGPIO( CDC * pdc, CPoint pt, int size, CSoftlogic::Port out_port )
{
	CT3kSoftlogicDlg * pDlg = (CT3kSoftlogicDlg *)AfxGetMainWnd();
	CSoftkeyArray& Keys = pDlg->GetKeys();

	GPIOInfo* pInfo = Keys.GetGPIOInfo(out_port-CSoftlogic::epGpio0);
	if ( !pInfo ) return;

	if ( !pInfo->bEnable || !pInfo->bOutput ) return;

	const int xsize = size / 20;
	const int ysize = size / 20;

	pdc->MoveTo(pt.x, pt.y);
	pdc->LineTo(pt.x, pt.y - ysize);
	pdc->Ellipse(pt.x - xsize * 4, pt.y - ysize * 6 - ysize, pt.x + xsize * 4, pt.y - ysize);

	if( m_bShowText )
	{
		CFont font;
		font.CreateFont((int)(ysize * 3), 0, 0, 0, 0, FALSE, FALSE, 0, 0, 0, 0, 0, 0, _T("arial"));
		CFont * fontOld = pdc->SelectObject(&font);
		pdc->SetBkMode(TRANSPARENT);
		CString str;
		if ( out_port >= CSoftlogic::epGpio0 && out_port < CSoftlogic::epSoftkey0 )
		{
			str.Format(_T("%d"), (out_port - CSoftlogic::epGpio0)+1);
		}
		pdc->DrawText(_T("GPIO"), 4,
			CRect(pt.x - xsize * 4, pt.y - ysize * 5 - ysize, pt.x + xsize * 4, pt.y - ysize * 3),
			DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		pdc->DrawText(str,
			CRect(pt.x - xsize * 4, pt.y - ysize * 3 - ysize, pt.x + xsize * 4, pt.y - ysize),
			DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		pdc->SelectObject(fontOld);
	}
}

void CLogicDesignerWnd::_DrawOutReport( CDC * pdc, CPoint pt, int size, int id )
{
	const int xsize = size / 20;
	const int ysize = size / 20;

	pdc->MoveTo(pt.x, pt.y);
	pdc->LineTo(pt.x, pt.y - ysize);
	pdc->Ellipse(pt.x - (int)(xsize * 4.5f), pt.y - ysize * 6 - ysize, pt.x + (int)(xsize * 4.5f), pt.y - ysize);

	if( m_bShowText )
	{
		CFont font;
		font.CreateFont((int)(ysize * 3), 0, 0, 0, 0, FALSE, FALSE, 0, 0, 0, 0, 0, 0, _T("arial"));
		CFont * fontOld = pdc->SelectObject(&font);
		pdc->SetBkMode(TRANSPARENT);
		CString str;
		str.Format(_T("%d"), id);
		pdc->DrawText(_T("Repo"), 4,
			CRect(pt.x - xsize * 4, pt.y - ysize * 5 - ysize, pt.x + xsize * 4, pt.y - ysize * 3),
			DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		pdc->DrawText(str,
			CRect(pt.x - xsize * 4, pt.y - ysize * 3 - ysize, pt.x + xsize * 4, pt.y - ysize),
			DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		pdc->SelectObject(fontOld);
	}
}

CSoftlogic::Port CLogicDesignerWnd::HitTest( CPoint pt, POINT * ptPos, GatePart * part, CSoftkey* pDontCheckKey/*=NULL*/ )
{
	CRect rcClient;
	GetClientRect(&rcClient);

	int logic_size = rcClient.Width() / LOGIC_SIZE;
	CRect rcLogic(0 - logic_size, 0 - logic_size / LOGIC_HEIGHT, 0, 0 + logic_size / LOGIC_HEIGHT);
	const int xsize = logic_size / 20;
	const int ysize = logic_size / 20;

	CRect rcIn1(0 - logic_size - xsize * 2, 0 - logic_size / LOGIC_HEIGHT + ysize,
				0 - logic_size + xsize * 4, 0 - ysize);
	CRect rcIn2(0 - logic_size - xsize * 2, 0 + ysize,
				0 - logic_size + xsize * 4, 0 + logic_size / LOGIC_HEIGHT - ysize);

	CT3kSoftlogicDlg * pDlg = (CT3kSoftlogicDlg *)AfxGetMainWnd();

	int ni;
	CRect rc;

	CSoftlogicArray & Logics = pDlg->GetLogics();
	for ( ni = Logics.GetSize() - 1; ni >= 0; ni-- )
	{
		CSoftlogic * pSoftlogic = Logics[ni];
		if ( pSoftlogic == NULL )
			continue;

		int x = (int)(pSoftlogic->m_ptPosition.x * rcClient.Width()) + rcClient.left;
		int y = (int)(pSoftlogic->m_ptPosition.y * rcClient.Height()) + rcClient.top;

		rc = rcIn1 + CPoint(x, y);
		if ( rc.PtInRect(pt) )
		{
			if ( ptPos )
				*ptPos = CPoint(x, y);
			if ( part )
				*part = epgIn1;
			return (CSoftlogic::Port)(ni + MM_SOFTLOGIC_PORT_SOFTLOGIC_START);
		}

		rc = rcIn2 + CPoint(x, y);
		if ( rc.PtInRect(pt) )
		{
			if ( ptPos )
				*ptPos = CPoint(x, y);
			if ( part )
				*part = epgIn2;
			return (CSoftlogic::Port)(ni + MM_SOFTLOGIC_PORT_SOFTLOGIC_START);
		}

		rc = rcLogic + CPoint(x, y);
		if ( rc.PtInRect(pt) )
		{
			if ( ptPos )
				*ptPos = CPoint(x, y);
			if ( part )
				*part = egpBody;
			return (CSoftlogic::Port)(ni + MM_SOFTLOGIC_PORT_SOFTLOGIC_START);
		}
	}

	CSoftkeyArray & Keys = pDlg->GetKeys();
	for ( ni = Keys.GetSize() - 1; ni >= 0; ni-- )
	{
		CSoftkey * pSoftkey = Keys[ni];
		if ( pSoftkey == NULL )
			continue;
		if ( pSoftkey == pDontCheckKey )
			continue;

		int x = (int)(pSoftkey->m_ptPosition.x * rcClient.Width()) + rcClient.left;
		int y = (int)(pSoftkey->m_ptPosition.y * rcClient.Height()) + rcClient.top;
		rc = rcLogic + CPoint(x, y);
		if ( rc.PtInRect(pt) )
		{
			if ( ptPos )
				*ptPos = CPoint(x, y);
			if ( part )
				*part = egpBody;
			return (CSoftlogic::Port)(ni + MM_SOFTLOGIC_PORT_SOFTKEY_START);
		}
	}

	return CSoftlogic::epNoGate;
}

CRect CLogicDesignerWnd::GetKeyBound( CSoftkey* pSoftkey, BOOL bFocusRect )
{
	CRect rcClient;
	GetClientRect( rcClient );
	int logic_size = rcClient.Width() / LOGIC_SIZE;
	CPoint ptX((int)(rcClient.Width() * pSoftkey->m_ptPosition.x), (int)(rcClient.Height() * pSoftkey->m_ptPosition.y));
	CRect rcKey;
	if ( bFocusRect )
	{
		rcKey = CRect(ptX.x - logic_size, ptX.y - logic_size / LOGIC_HEIGHT, ptX.x, ptX.y + logic_size / LOGIC_HEIGHT);
		rcKey.InflateRect( 12, 1 );
		return rcKey;
	}
	rcKey = CRect(ptX.x - logic_size, ptX.y - logic_size / 5, ptX.x, ptX.y + logic_size / 5);
	rcKey.InflateRect( 1, 1 );
	return rcKey;
}

CRect CLogicDesignerWnd::GetLogicBound( CSoftlogic* pLogic )
{
	if ( pLogic == NULL )
		return CRect(0,0,0,0);

	CRect rcClient;
	GetClientRect( rcClient );
	int logic_size = rcClient.Width() / LOGIC_SIZE;

	CPoint pt((int)(rcClient.Width() * pLogic->m_ptPosition.x), (int)(rcClient.Height() * pLogic->m_ptPosition.y));

	CRect rcLogic(pt.x - logic_size * 4 / 5, pt.y - logic_size / LOGIC_HEIGHT, pt.x - logic_size * 1 / 5, pt.y + logic_size / LOGIC_HEIGHT);

	const int xsize = logic_size / 20;
	const int ysize = logic_size / 20;

	CRect rcOutPort(pt.x - xsize * 4, pt.y - ysize * 6 - ysize, pt.x + xsize * 4, pt.y - ysize);

	int xe = rcLogic.left + rcLogic.Width() / 2 - xsize;
	int ye = rcLogic.top;
	ye -= xsize * 2;	// not
	CRect rcEnablePort = CRect(xe - xsize * 3, ye - ysize * 4 - ysize, xe + xsize * 3, ye - ysize);


	CPoint ptIn1(pt.x - logic_size, (pt.y - logic_size / 4) + logic_size / 8);
	CPoint ptIn2(pt.x - logic_size, (pt.y + logic_size / 4) - logic_size / 8);
	CRect rcInPort1 = CRect(ptIn1.x - xsize * 4, ptIn1.y - ysize * 2, ptIn1.x, ptIn1.y + ysize * 2);
	CRect rcInPort2 = CRect(ptIn2.x - xsize * 4, ptIn2.y - ysize * 2, ptIn2.x, ptIn2.y + ysize * 2);

	rcOutPort.InflateRect( 2, 2 );

	rcLogic.UnionRect( rcLogic, rcOutPort );
	rcLogic.UnionRect( rcLogic, rcEnablePort );
	rcLogic.UnionRect( rcLogic, rcInPort1 );
	rcLogic.UnionRect( rcLogic, rcInPort2 );

	rcLogic.InflateRect( 1, 1 );

	return rcLogic;
}

void CLogicDesignerWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if ( GetCapture() == this )
	{
		CT3kSoftlogicDlg * pDlg = (CT3kSoftlogicDlg *)AfxGetMainWnd();
		CRect rcClient;
		GetClientRect(&rcClient);

		if ( m_SelPart == egpBody )
		{
			CSoftkeyArray & Keys = pDlg->GetKeys();
			CSoftlogicArray& Logics = pDlg->GetLogics();

			if ( m_port_hit <= CSoftlogic::epNoGate )
			{
			}
			else if ( m_port_hit < CSoftlogic::epGpio0 )
			{
			}
			else if ( m_port_hit < CSoftlogic::epSoftkey0 )
			{
			}
			else if ( m_port_hit < CSoftlogic::epSoftkeyAll )
			{
				//CSoftkey * pSoftkey = Keys[m_port_hit - CSoftlogic::epSoftkey0];
				int logic_size = rcClient.Width() / LOGIC_SIZE;

				if ( m_pCheckKey != NULL )
				{
					CRect rcUpdate;
					rcUpdate.SetRectEmpty();

					GatePart gp;
					CRect rcKey = GetKeyBound( m_pCheckKey, TRUE );
					rcUpdate = rcKey;

					if ( m_pToBindKey )
					{
						rcUpdate.UnionRect( rcUpdate, GetKeyBound( m_pToBindKey, TRUE ) );
					}

					m_pToBindKey = NULL;

					BOOL bPressCtrlkey = HIWORD(::GetKeyState( VK_CONTROL )) & 0x01 ? TRUE : FALSE;
					if( !bPressCtrlkey )
					{
						CPoint ptCheck = rcKey.CenterPoint();
						CSoftlogic::Port over_port_hit = HitTest( ptCheck, NULL, &gp, m_pCheckKey );
						if ( over_port_hit >= CSoftlogic::epSoftkey0 && over_port_hit < CSoftlogic::epSoftkeyAll )
						{
							CSoftkey * pOverKey = Keys[over_port_hit - CSoftlogic::epSoftkey0];
							if ( pOverKey )
							{
								m_pToBindKey = pOverKey;
							}
						}
					}

					POINTF ptOld = m_pCheckKey->m_ptPosition;

					point += m_SelOffset;
					m_pCheckKey->m_ptPosition.x = (float)point.x / rcClient.Width();
					m_pCheckKey->m_ptPosition.x = (float)(int)(m_pCheckKey->m_ptPosition.x * 200 + 0.5f) / 200;
					m_pCheckKey->m_ptPosition.y = (float)point.y / rcClient.Height();
					m_pCheckKey->m_ptPosition.y = (float)(int)(m_pCheckKey->m_ptPosition.y * 200 + 0.5f) / 200;

					// adjustment position
					{
						CRect rcKey = GetKeyBound( m_pCheckKey, FALSE );

						CPoint ptN = point;
						if ( rcKey.left < rcClient.left ) ptN.x += (rcClient.left-rcKey.left);
						if ( rcKey.top < rcClient.top ) ptN.y += (rcClient.top-rcKey.top);
						if ( rcKey.right > rcClient.right ) ptN.x += (rcClient.right-rcKey.right);
						if ( rcKey.bottom > rcClient.bottom ) ptN.y += (rcClient.bottom-rcKey.bottom);

						if( ptN != point )
						{
							m_pCheckKey->m_ptPosition.x = (float)ptN.x / rcClient.Width();
							m_pCheckKey->m_ptPosition.x = (float)(int)(m_pCheckKey->m_ptPosition.x * 200 + 0.5f) / 200;
							m_pCheckKey->m_ptPosition.y = (float)ptN.y / rcClient.Height();
							m_pCheckKey->m_ptPosition.y = (float)(int)(m_pCheckKey->m_ptPosition.y * 200 + 0.5f) / 200;
						}
					}

					rcUpdate.UnionRect( rcUpdate, GetKeyBound(m_pCheckKey, TRUE) );

					float fdx = m_pCheckKey->m_ptPosition.x - ptOld.x;
					float fdy = m_pCheckKey->m_ptPosition.y - ptOld.y;

					for ( int i=0 ; i<Logics.GetSize() ; i++ )
					{
						CSoftlogic* pSoftlogic = Logics[i];

						if( IsAssociate(m_pCheckKey, pSoftlogic) )
						{
							rcUpdate.UnionRect( rcUpdate, GetLogicBound(pSoftlogic) );
							pSoftlogic->m_ptPosition.x += fdx;
							pSoftlogic->m_ptPosition.y += fdy;
							rcUpdate.UnionRect( rcUpdate, GetLogicBound(pSoftlogic) );
						}
					}

					if ( m_pToBindKey )
					{
						rcUpdate.UnionRect( rcUpdate, GetKeyBound(m_pToBindKey, TRUE) );
					}

					if ( m_pCheckKey->getBind() )
					{
						rcUpdate.UnionRect( rcUpdate, GetKeyBound(m_pCheckKey->getBind(), FALSE) );
					}

					for ( int i=0 ; i<Keys.GetSize() ; i++ )
					{
						CSoftkey* pKey = Keys[i];
						if ( pKey->getBind() == m_pCheckKey )
						{
							rcUpdate.UnionRect( rcUpdate, GetKeyBound( pKey, FALSE) );
						}
					}

					InvalidateRect( rcUpdate, FALSE );

				}
				//Invalidate(FALSE);
			}
			else
			{	// softlogic
				CSoftlogicArray & Logics = pDlg->GetLogics();
				CSoftlogic * pSoftlogic = Logics[m_port_hit - CSoftlogic::epSoftLogic0];
				if ( pSoftlogic != NULL )
				{
					CRect rcUpdate;
					rcUpdate = GetLogicBound( pSoftlogic );

					point += m_SelOffset;
					pSoftlogic->m_ptPosition.x = (float)point.x / rcClient.Width();
					pSoftlogic->m_ptPosition.x = (float)(int)(pSoftlogic->m_ptPosition.x * 200 + 0.5f) / 200;
					pSoftlogic->m_ptPosition.y = (float)point.y / rcClient.Height();
					pSoftlogic->m_ptPosition.y = (float)(int)(pSoftlogic->m_ptPosition.y * 200 + 0.5f) / 200;

					rcUpdate.UnionRect( rcUpdate, GetLogicBound(pSoftlogic) );

					CSoftlogic::Port epIn1 = pSoftlogic->getIn1Port();
					if ( epIn1 != CSoftlogic::epNoGate )
					{
						if ( epIn1 >= CSoftlogic::epSoftkey0 && epIn1 < CSoftlogic::epSoftkeyAll )
						{
							CSoftkey* pAssociateKey = Keys[epIn1 - CSoftlogic::epSoftkey0];
							rcUpdate.UnionRect( rcUpdate, GetKeyBound(pAssociateKey, FALSE) );
						}
						else if ( epIn1 >= CSoftlogic::epSoftLogic0 )
						{
							CSoftlogic* pAssociateLogic = Logics[epIn1 - CSoftlogic::epSoftLogic0];
							rcUpdate.UnionRect( rcUpdate, GetLogicBound(pAssociateLogic) );
						}
					}

					CSoftlogic::Port epIn2 = pSoftlogic->getIn2Port();
					if ( epIn2 != CSoftlogic::epNoGate )
					{
						if ( epIn2 >= CSoftlogic::epSoftkey0 && epIn2 < CSoftlogic::epSoftkeyAll )
						{
							CSoftkey* pAssociateKey = Keys[epIn2 - CSoftlogic::epSoftkey0];
							rcUpdate.UnionRect( rcUpdate, GetKeyBound(pAssociateKey, FALSE) );
						}
						else if ( epIn2 >= CSoftlogic::epSoftLogic0 )
						{
							CSoftlogic* pAssociateLogic = Logics[epIn2 - CSoftlogic::epSoftLogic0];
							rcUpdate.UnionRect( rcUpdate, GetLogicBound(pAssociateLogic) );
						}
					}

					CSoftlogic::PortType eptOut = pSoftlogic->getOutPortType();
					if ( eptOut == CSoftlogic::eptState && pSoftlogic->getOutPort() == CSoftlogic::epNoGate )
					{
						for ( int i=0 ; i<Logics.GetSize() ; i++ )
						{
							CSoftlogic* pSoftlogicCheck = Logics[i];

							if( IsAssociate(pSoftlogicCheck, pSoftlogic) )
							{
								rcUpdate.UnionRect( rcUpdate, GetLogicBound(pSoftlogicCheck) );
							}
						}
					}

					InvalidateRect( rcUpdate, FALSE );
				}
				//Invalidate(FALSE);
			}
		} // body
		else // in port
		{
			CSoftlogicArray & Logics = pDlg->GetLogics();
			CSoftlogic * logic = Logics[m_port_hit - CSoftlogic::epSoftLogic0];

			if ( logic )
			{
				CPoint ptPos((int)(rcClient.Width() * logic->m_ptPosition.x), (int)(rcClient.Height() * logic->m_ptPosition.y));
				int logic_size = rcClient.Width() / LOGIC_SIZE;

				CRect rcUpdate;
				if ( m_SelPart == epgIn1 )
				{
					if ( m_pt_dst.x >= 0 )
					{
						CPoint ptX(ptPos.x - logic_size, (ptPos.y - logic_size / 4) + logic_size / 8);
						rcUpdate = CRect( m_pt_dst.x, m_pt_dst.y, ptX.x, ptX.y );
					}
				}
				else if ( m_SelPart == epgIn2 )
				{
					if ( m_pt_dst.x >= 0 )
					{
						CPoint ptX(ptPos.x - logic_size, (ptPos.y + logic_size / 4) - logic_size / 8);
						rcUpdate = CRect( m_pt_dst.x, m_pt_dst.y, ptX.x, ptX.y );
					}
				}

				rcUpdate.NormalizeRect();
				rcUpdate.InflateRect( 1, 1 );

				InvalidateRect( rcUpdate, FALSE );
			}

			CPoint pt;
			GatePart gp;
			CSoftlogic::Port port_hit = HitTest(point, &pt, &gp);
			if ( port_hit != CSoftlogic::epNoGate )
				m_pt_dst = pt;
			else
				m_pt_dst = point;
			m_port_dst = port_hit;

			if ( logic )
			{
				CPoint ptPos((int)(rcClient.Width() * logic->m_ptPosition.x), (int)(rcClient.Height() * logic->m_ptPosition.y));
				int logic_size = rcClient.Width() / LOGIC_SIZE;

				CRect rcUpdate;
				if ( m_SelPart == epgIn1 )
				{
					if ( m_pt_dst.x >= 0 )
					{
						CPoint ptX(ptPos.x - logic_size, (ptPos.y - logic_size / 4) + logic_size / 8);
						rcUpdate = CRect( m_pt_dst.x, m_pt_dst.y, ptX.x, ptX.y );
					}
				}
				else if ( m_SelPart == epgIn2 )
				{
					if ( m_pt_dst.x >= 0 )
					{
						CPoint ptX(ptPos.x - logic_size, (ptPos.y + logic_size / 4) - logic_size / 8);
						rcUpdate = CRect( m_pt_dst.x, m_pt_dst.y, ptX.x, ptX.y );
					}
				}

				rcUpdate.NormalizeRect();
				rcUpdate.InflateRect( 1, 1 );

				InvalidateRect( rcUpdate, FALSE );
			}
		}
	}
	else	// not capture
	{
		GatePart gp;
		CSoftlogic::Port port_hit = HitTest(point, NULL, &gp);
		if ( m_port_hit != port_hit || m_SelPart != gp )
		{
			m_port_hit = port_hit;
			m_SelPart = gp;
			m_pt_dst.x = m_pt_dst.y = -1;
			Invalidate(FALSE);
		}
	}

	CFullScrnWnd::OnMouseMove(nFlags, point);
}

void CLogicDesignerWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_ptSoftkeyPositionBackup.x = m_ptSoftkeyPositionBackup.y = -1.f;

	if( m_rcCloseButton.PtInRect(point) )
	{
		m_bDownCloseButton = TRUE;
		if( GetCapture() != this )
			SetCapture();
		InvalidateRect( m_rcCloseButton );
		return;
	}

	CT3kSoftlogicDlg * pDlg = (CT3kSoftlogicDlg *)AfxGetMainWnd();

	CPoint pt;
	CSoftlogic::Port port_hit = HitTest(point, &pt, &m_SelPart);
	if ( port_hit != CSoftlogic::epNoGate )
	{
		m_SelOffset = pt - point;
		m_port_dst = CSoftlogic::epNoGate;
		SetCapture();

		if ( m_SelPart == egpBody )
		{
			if ( m_port_hit >= CSoftlogic::epSoftkey0 && m_port_hit < CSoftlogic::epSoftkeyAll )
			{
				CSoftkeyArray & Keys = pDlg->GetKeys();
				CSoftkey * pSoftkey = Keys[m_port_hit - CSoftlogic::epSoftkey0];
				if ( pSoftkey != NULL )
				{
					m_pCheckKey = pSoftkey;
					m_ptSoftkeyPositionBackup = pSoftkey->m_ptPosition;
					CRect rcUpdate = GetKeyBound( m_pCheckKey, TRUE );

					CSoftlogicArray& Logics = pDlg->GetLogics();
					for ( int i=0 ; i<Logics.GetSize() ; i++ )
					{
						CSoftlogic* pSoftlogic = Logics[i];

						if( IsAssociate(m_pCheckKey, pSoftlogic) )
						{
							rcUpdate.UnionRect( rcUpdate, GetLogicBound(pSoftlogic) );
						}
					}

					if ( pSoftkey->getBind() )
					{
						rcUpdate.UnionRect( rcUpdate, GetKeyBound(pSoftkey->getBind(), FALSE) );
					}

					InvalidateRect( rcUpdate, FALSE );
				}
			}
		}
	}

	if ( m_wndEditGate && m_wndEditGate.IsWindowVisible() )
	{
		m_wndEditGate.ShowWindow( SW_HIDE );
	}
	if ( m_wndLogicGallery && m_wndEditGate.IsWindowVisible() )
	{
		m_wndLogicGallery.ShowWindow( SW_HIDE );
	}

	CFullScrnWnd::OnLButtonDown(nFlags, point);
}

void CLogicDesignerWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	CT3kSoftlogicDlg * pDlg = (CT3kSoftlogicDlg *)AfxGetMainWnd();
	CRect rcClient;
	GetClientRect( rcClient );
	int logic_size = rcClient.Width() / LOGIC_SIZE;

	if( m_bDownCloseButton )
	{
		if( GetCapture() == this )
		{
			m_bDownCloseButton = FALSE;
			InvalidateRect( m_rcCloseButton );		
			ReleaseCapture();
			if( m_rcCloseButton.PtInRect(point) )
			{
				PostMessage( WM_CLOSE, 0, 0 );
			}
			return;
		}
	}

	if ( GetCapture() == this )
	{
		if ( m_SelPart == epgIn1 )
		{
			int idx_logic = m_port_hit - MM_SOFTLOGIC_PORT_SOFTLOGIC_START;
			CT3kSoftlogicDlg * pDlg = (CT3kSoftlogicDlg *)AfxGetMainWnd();
			CSoftlogicArray & Logics = pDlg->GetLogics();
			CSoftlogic * pSoftlogic = Logics[idx_logic];
			if ( pSoftlogic )
			{
				pSoftlogic->setIn1Port(m_port_dst);
				Invalidate(FALSE);
				goto goto_return;
			}
		}
		else if ( m_SelPart == epgIn2 )
		{
			int idx_logic = m_port_hit - MM_SOFTLOGIC_PORT_SOFTLOGIC_START;
			CT3kSoftlogicDlg * pDlg = (CT3kSoftlogicDlg *)AfxGetMainWnd();
			CSoftlogicArray & Logics = pDlg->GetLogics();
			CSoftlogic * pSoftlogic = Logics[idx_logic];
			if ( pSoftlogic )
			{
				pSoftlogic->setIn2Port(m_port_dst);
				Invalidate(FALSE);
				goto goto_return;
			}
		}

		if ( m_pToBindKey )
		{
			m_pCheckKey->setBind( m_pToBindKey );
			if ( m_pCheckKey )
			{
				float fdx = m_pCheckKey->m_ptPosition.x - m_ptSoftkeyPositionBackup.x;
				float fdy = m_pCheckKey->m_ptPosition.y - m_ptSoftkeyPositionBackup.y;
				m_pCheckKey->m_ptPosition = m_ptSoftkeyPositionBackup;

				// restore logic position
				CSoftlogicArray & Logics = pDlg->GetLogics();
				for ( int i=0 ; i<Logics.GetSize() ; i++ )
				{
					CSoftlogic* pSoftlogic = Logics[i];

					if( IsAssociate(m_pCheckKey, pSoftlogic) )
					{
						pSoftlogic->m_ptPosition.x -= fdx;
						pSoftlogic->m_ptPosition.y -= fdy;
					}
				}
			}
			m_pToBindKey = NULL;
			Invalidate( FALSE );
		}
		else
		{
			if ( m_pCheckKey )
			{
				CSoftlogicArray & Logics = pDlg->GetLogics();
				for ( int i=0 ; i<Logics.GetSize() ; i++ )
				{
					CSoftlogic* pSoftlogic = Logics[i];

					if( IsAssociate(m_pCheckKey, pSoftlogic) )
					{
						// adjustment position
						CPoint ptX((int)(rcClient.Width() * pSoftlogic->m_ptPosition.x), (int)(rcClient.Height() * pSoftlogic->m_ptPosition.y));
						CRect rcLogic(ptX.x - logic_size * 4 / 5, ptX.y - logic_size / LOGIC_HEIGHT, ptX.x - logic_size * 1 / 5, ptX.y + logic_size / LOGIC_HEIGHT);

						CPoint ptN = ptX;
						if ( rcLogic.left < rcClient.left ) ptN.x += (rcClient.left-rcLogic.left);
						if ( rcLogic.top < rcClient.top ) ptN.y += (rcClient.top-rcLogic.top);
						if ( rcLogic.right > rcClient.right ) ptN.x += (rcClient.right-rcLogic.right);
						if ( rcLogic.bottom > rcClient.bottom ) ptN.y += (rcClient.bottom-rcLogic.bottom);

						if( ptN != ptX )
						{
							float fX = (float)(ptX.x - ptN.x) / rcClient.Width();
							float fY = (float)(ptX.y - ptN.y) / rcClient.Height();
							pSoftlogic->m_ptPosition.x -= fX;
							pSoftlogic->m_ptPosition.y -= fY;
						}
					}
				}
			}
		}

		if ( m_pCheckKey )
		{
			m_pCheckKey = NULL;
			Invalidate( FALSE );
		}

goto_return:
		ReleaseCapture();
	}

	CFullScrnWnd::OnLButtonUp(nFlags, point);
}

void CLogicDesignerWnd::AddLogicFromGallery( int nGalleryItemType, CSoftlogic::Port portKey, CPoint ptMouse )
{
	CT3kSoftlogicDlg * pDlg = (CT3kSoftlogicDlg *)AfxGetMainWnd();

	CRect rcClient;
	GetClientRect( &rcClient );
	int logic_size = rcClient.Width() / LOGIC_SIZE;
	float fLogicSize = (float)logic_size / rcClient.Width();

	CSoftlogicArray & Logics = pDlg->GetLogics();
	int nLogicIndex;
	CSoftlogic* pLogic;

	if ( portKey >= CSoftlogic::epGpio0 && portKey < CSoftlogic::epSoftkey0 )		// GPIO type
	{
		switch ( nGalleryItemType )
		{
		case 3:
			// Calibration with GPIO
			nLogicIndex = Logics.AddSoftlogic_State( 
				CSoftlogic::eltAND_RISINGEDGE, 
				TRUE, CSoftlogic::epStateCalibration, 
				TRUE, portKey, 
				FALSE, CSoftlogic::epNoGate, 
				CSoftlogic::epStateCalibration,
				CSoftlogic::etTrigger_U1000_D0, TRUE );

			pLogic = Logics[nLogicIndex];
			pLogic->m_ptPosition.x = (float)ptMouse.x / rcClient.Width() + fLogicSize*1.5f;
			pLogic->m_ptPosition.y = (float)ptMouse.y / rcClient.Height() - fLogicSize*0.45f;

			nLogicIndex = Logics.AddSoftlogic_State( 
				CSoftlogic::eltAND_RISINGEDGE, 
				FALSE, CSoftlogic::epStateCalibration, 
				TRUE, portKey, 
				FALSE, CSoftlogic::epNoGate, 
				CSoftlogic::epStateCalibration,
				CSoftlogic::etTrigger_U100_D100, TRUE );

			pLogic = Logics[nLogicIndex];
			pLogic->m_ptPosition.x = (float)ptMouse.x / rcClient.Width() + fLogicSize*1.5f;
			pLogic->m_ptPosition.y = (float)ptMouse.y / rcClient.Height() + fLogicSize*0.8f;
			break;
		case 4:
			// Touch ON/OFF with GPIO
			nLogicIndex = Logics.AddSoftlogic_State( 
				CSoftlogic::eltAND_RISINGEDGE, 
				FALSE, CSoftlogic::epNoGate, 
				TRUE, portKey, 
				FALSE, CSoftlogic::epStateTouchScreen, 
				CSoftlogic::epStateTouchScreen,
				CSoftlogic::etTrigger_U100_D100, TRUE );

			pLogic = Logics[nLogicIndex];
			pLogic->m_ptPosition.x = (float)ptMouse.x / rcClient.Width() + fLogicSize*1.5f;
			pLogic->m_ptPosition.y = (float)ptMouse.y / rcClient.Height() - fLogicSize*0.45f;

			nLogicIndex = Logics.AddSoftlogic_State( 
				CSoftlogic::eltAND_RISINGEDGE, 
				FALSE, CSoftlogic::epNoGate, 
				FALSE, portKey, 
				TRUE, CSoftlogic::epStateTouchScreen, 
				CSoftlogic::epStateTouchScreen,
				CSoftlogic::etTrigger_U100_D100, TRUE );

			pLogic = Logics[nLogicIndex];
			pLogic->m_ptPosition.x = (float)ptMouse.x / rcClient.Width() + fLogicSize*1.5f;
			pLogic->m_ptPosition.y = (float)ptMouse.y / rcClient.Height() + fLogicSize*0.8f;
			break;
		case 5:
			// IR Pen Mode with GPIO
			{
				CSoftlogic::Port port1, port2;
				nLogicIndex = Logics.AddSoftlogic_State( 
					CSoftlogic::eltAND_RISINGEDGE, 
					FALSE, CSoftlogic::epNoGate, 
					TRUE, portKey, 
					FALSE, CSoftlogic::epStateInvertDetection, 
					CSoftlogic::epStateInvertDetection,
					CSoftlogic::etTrigger_U100_D100, TRUE );

				port1 = (CSoftlogic::Port)(nLogicIndex+MM_SOFTLOGIC_PORT_SOFTLOGIC_START);

				pLogic = Logics[nLogicIndex];
				pLogic->m_ptPosition.x = (float)ptMouse.x / rcClient.Width() + fLogicSize*1.5f;
				pLogic->m_ptPosition.y = (float)ptMouse.y / rcClient.Height() - fLogicSize*0.45f;

				nLogicIndex = Logics.AddSoftlogic_State( 
					CSoftlogic::eltAND_RISINGEDGE, 
					FALSE, CSoftlogic::epNoGate, 
					FALSE, portKey, 
					TRUE, CSoftlogic::epStateInvertDetection, 
					CSoftlogic::epStateInvertDetection,
					CSoftlogic::etTrigger_U100_D100, TRUE );

				port2 = (CSoftlogic::Port)(nLogicIndex+MM_SOFTLOGIC_PORT_SOFTLOGIC_START);

				pLogic = Logics[nLogicIndex];
				pLogic->m_ptPosition.x = (float)ptMouse.x / rcClient.Width() + fLogicSize*1.5f;
				pLogic->m_ptPosition.y = (float)ptMouse.y / rcClient.Height() + fLogicSize*0.8f;

				nLogicIndex = Logics.AddSoftlogic_State( 
					CSoftlogic::eltOR, 
					FALSE, CSoftlogic::epNoGate, 
					FALSE, port1, 
					FALSE, port2, 
					CSoftlogic::epStateWorking,
					CSoftlogic::etNoTrigger, TRUE );

				pLogic = Logics[nLogicIndex];
				pLogic->m_ptPosition.x = (float)ptMouse.x / rcClient.Width() + fLogicSize*3.0f;
				pLogic->m_ptPosition.y = (float)ptMouse.y / rcClient.Height() + fLogicSize*0.1f;
			}
			break;
		}
	}
	else
	{
		ASSERT( (portKey >= CSoftlogic::epSoftkey0) && (portKey <= CSoftlogic::epSoftkey29) );

		CSoftkeyArray & Keys = pDlg->GetKeys();
		CSoftkey* pSoftkey = Keys[(int)(portKey-CSoftlogic::epSoftkey0)];
		CRect rcKey = GetKeyBound( pSoftkey, FALSE );

		switch ( nGalleryItemType )
		{
		case 0:
			// 1초 딜레이 터치 ON/OFF
			nLogicIndex = Logics.AddSoftlogic_State( 
				CSoftlogic::eltAND_RISINGEDGE, 
				TRUE, CSoftlogic::epStateCalibration, 
				FALSE, portKey, 
				FALSE, CSoftlogic::epNoGate, 
				CSoftlogic::epStateTouchScreen,
				CSoftlogic::etTrigger_U1000_D0, TRUE );

			pLogic = Logics[nLogicIndex];
			pLogic->m_ptPosition.x = (float)rcKey.right / rcClient.Width() + fLogicSize*1.5f;
			pLogic->m_ptPosition.y = (float)rcKey.CenterPoint().y / rcClient.Height();
			break;
		case 1:
			// 3초 딜레이 좌표보정 진입, 
			nLogicIndex = Logics.AddSoftlogic_State( CSoftlogic::eltAND_RISINGEDGE, 
				TRUE, CSoftlogic::epStateCalibration, 
				FALSE, portKey, 
				FALSE, CSoftlogic::epNoGate, 
				CSoftlogic::epStateCalibration,
				CSoftlogic::etTrigger_U3000_D0, TRUE );

			pLogic = Logics[nLogicIndex];
			pLogic->m_ptPosition.x = (float)rcKey.right / rcClient.Width() + fLogicSize*1.5f;
			pLogic->m_ptPosition.y = (float)rcKey.CenterPoint().y / rcClient.Height() - fLogicSize*0.45f;

			nLogicIndex = Logics.AddSoftlogic_State( CSoftlogic::eltAND_RISINGEDGE, 
				FALSE, CSoftlogic::epStateCalibration, 
				FALSE, portKey, 
				FALSE, CSoftlogic::epNoGate, 
				CSoftlogic::epStateCalibration,
				CSoftlogic::etTrigger_U2000_D0, TRUE );

			pLogic = Logics[nLogicIndex];
			pLogic->m_ptPosition.x = (float)rcKey.right / rcClient.Width() + fLogicSize*1.5f;
			pLogic->m_ptPosition.y = (float)rcKey.CenterPoint().y / rcClient.Height() + fLogicSize*0.8f;
			break;
		case 2:
			// TaskSwitch
			{
				// MTD Gate
				nLogicIndex = Logics.AddSoftlogic_State( CSoftlogic::eltAND, 
					FALSE, CSoftlogic::epStateMultiTouchDevice, 
					FALSE, CSoftlogic::epNoGate, 
					FALSE, portKey, 
					CSoftlogic::epNoGate,
					CSoftlogic::etNoTrigger, TRUE );

				CSoftlogic::Port portMTD = (CSoftlogic::Port)(nLogicIndex+MM_SOFTLOGIC_PORT_SOFTLOGIC_START);
				pLogic = Logics[nLogicIndex];
				pLogic->m_ptPosition.x = (float)rcKey.right / rcClient.Width() + fLogicSize*1.5f;
				pLogic->m_ptPosition.y = (float)rcKey.CenterPoint().y / rcClient.Height() + fLogicSize*1.5f;

				// OR Gate
				nLogicIndex = Logics.AddSoftlogic_State( CSoftlogic::eltOR, 
					FALSE, CSoftlogic::epNoGate, 
					TRUE, portKey, 
					FALSE, portMTD, 
					CSoftlogic::epNoGate,
					CSoftlogic::etTrigger_U1000_D0, TRUE );

				CSoftlogic::Port portORGate = (CSoftlogic::Port)(nLogicIndex+MM_SOFTLOGIC_PORT_SOFTLOGIC_START);

				pLogic = Logics[nLogicIndex];
				pLogic->m_ptPosition.x = (float)rcKey.right / rcClient.Width() + fLogicSize*2.3f;
				pLogic->m_ptPosition.y = (float)rcKey.CenterPoint().y / rcClient.Height() + fLogicSize*0.5f;


				// Tab
				nLogicIndex = Logics.AddSoftlogic_Key( CSoftlogic::eltAND_RISINGEDGE,
					FALSE, portKey, 
					TRUE, portMTD, 
					0x00, 0x2b,			//0x2b = tab
					CSoftlogic::etNoTrigger, TRUE );
				pLogic = Logics[nLogicIndex];
				pLogic->m_ptPosition.x = (float)rcKey.right / rcClient.Width() + fLogicSize*3.8f;
				pLogic->m_ptPosition.y = (float)rcKey.CenterPoint().y / rcClient.Height() - fLogicSize*0.5f;

				// Alt
				nLogicIndex = Logics.AddSoftlogic_Key( CSoftlogic::eltAND,
					TRUE, portORGate, 
					FALSE, CSoftlogic::epNoGate, 
					0x04, 0x00,			//0x04 = Alt
					CSoftlogic::etNoTrigger, TRUE );
				pLogic = Logics[nLogicIndex];
				pLogic->m_ptPosition.x = (float)rcKey.right / rcClient.Width() + fLogicSize*3.8f;
				pLogic->m_ptPosition.y = (float)rcKey.CenterPoint().y / rcClient.Height() + fLogicSize*0.5f;


				// Ctrl+Win+Tab
				nLogicIndex = Logics.AddSoftlogic_Key( CSoftlogic::eltAND_RISINGEDGE,
					FALSE, portMTD, 
					FALSE, CSoftlogic::epNoGate, 
					0x01|0x08, 0x2b,			//0x01 = Ctrl, 0x08 = Win, 0x2b = tab
					CSoftlogic::etNoTrigger, TRUE );
				pLogic = Logics[nLogicIndex];
				pLogic->m_ptPosition.x = (float)rcKey.right / rcClient.Width() + fLogicSize*3.8f;
				pLogic->m_ptPosition.y = (float)rcKey.CenterPoint().y / rcClient.Height() + fLogicSize*1.5f;
			}
			break;
		} // end of switch ( nGalleryItemType )
	}


	Invalidate();
}

void CLogicDesignerWnd::OnRButtonUp(UINT nFlags, CPoint point)
{
	CT3kSoftlogicDlg * pDlg = (CT3kSoftlogicDlg *)AfxGetMainWnd();

	if ( m_wndEditGate && m_wndEditGate.IsWindowVisible() )
	{
		m_wndEditGate.ShowWindow( SW_HIDE );
	}

	if ( m_port_hit <= CSoftlogic::epNoGate )
	{
		if ( m_wndLogicGallery )
		{
			m_wndLogicGallery.SetInfo( CSoftlogic::epNoGate, point );
			m_wndLogicGallery.ShowWindow( SW_SHOW );
		}
		return;
	}

	if ( m_port_hit >= CSoftlogic::epSoftkey0 && m_port_hit < CSoftlogic::epSoftkeyAll )
	{
		// unbind key

		CSoftkeyArray & Keys = pDlg->GetKeys();
		CSoftkey * pSoftkey = Keys[m_port_hit - CSoftlogic::epSoftkey0];
		if ( pSoftkey != NULL )
		{
			if ( pSoftkey->getBind() )
			{
				pSoftkey->setBind( NULL );
				Invalidate( FALSE );
			}
		}
		return;
	}

	int idx_logic = m_port_hit - MM_SOFTLOGIC_PORT_SOFTLOGIC_START;

	if( idx_logic < 0 ) return;

	CSoftlogicArray & Logics = pDlg->GetLogics();
	CSoftlogic * pSoftlogic = Logics[idx_logic];


	m_pSoftlogicForRemove = pSoftlogic;

	Invalidate();

	if( ::MessageBox( GetSafeHwnd(), _T("Would you like to delete the logic gate?"), _T("Warning"), MB_YESNO|MB_ICONQUESTION ) == IDYES )
	{
		// delete!
		Logics.Remove( pSoftlogic );
	}

	m_pSoftlogicForRemove = NULL;
	Invalidate();
}

BOOL CLogicDesignerWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if( m_eScreenMode != ScreenModePreview )
	{
		if ( m_port_hit != CSoftlogic::epNoGate )
		{
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEALL));
			return TRUE;
		}
	}

	SetCursor( AfxGetApp()->LoadStandardCursor(IDC_ARROW) );

	return CFullScrnWnd::OnSetCursor(pWnd, nHitTest, message);
}

int CLogicDesignerWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFullScrnWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pIconConsumerButton = GdipLoadImageFromRes( AfxGetResourceHandle(), _T("PNG"), PNG_ICON_CONSUMER_BTN );

	if ( !m_wndEditGate.Create(this) )
		return -1;
	if ( !m_wndLogicGallery.Create(DLogicGallery::IDD, this) )
		return -1;

	return 0;
}

void CLogicDesignerWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CT3kSoftlogicDlg * pDlg = (CT3kSoftlogicDlg *)AfxGetMainWnd();
	CSoftlogicArray & Logics = pDlg->GetLogics();

	if ( m_port_hit <= CSoftlogic::epNoGate )
	{
		// new logic!

		int idx_logic = Logics.AddSoftlogic_State( CSoftlogic::eltAND_RISINGEDGE, FALSE, CSoftlogic::epNoGate, 
									FALSE, CSoftlogic::epNoGate,
									FALSE, CSoftlogic::epNoGate,
									CSoftlogic::epNoGate, CSoftlogic::etNoTrigger, TRUE );

		CSoftlogic * pSoftlogic = Logics[idx_logic];
		if ( pSoftlogic )
		{
			CRect rcClient;
			GetClientRect( rcClient );
			pSoftlogic->m_ptPosition.x = (float)point.x / rcClient.Width();
			pSoftlogic->m_ptPosition.y = (float)point.y / rcClient.Height();


			m_wndEditGate.SetWindowText( _T("New Gate") );
			m_wndEditGate.SetLogic( pSoftlogic, (idx_logic) < Logics.GetShowSize() );
			m_wndEditGate.ShowWindow( SW_SHOW );
			Invalidate();
		}

	}
	else if ( m_port_hit >= CSoftlogic::epSoftLogic0 )
	{
		int idx_logic = m_port_hit - MM_SOFTLOGIC_PORT_SOFTLOGIC_START;
		CSoftlogic * pSoftlogic = Logics[idx_logic];

		if ( pSoftlogic )
		{
			// Active!
			m_wndEditGate.SetWindowText( _T("Edit Gate") );
			m_wndEditGate.SetLogic( pSoftlogic, (idx_logic) < Logics.GetShowSize() );
			m_wndEditGate.ShowWindow( SW_SHOW );
			Invalidate();
		}
	}
	else if ( (m_port_hit >= CSoftlogic::epSoftkey0) && (m_port_hit <= CSoftlogic::epSoftkey29) )
	{
		if ( m_wndLogicGallery )
		{
			m_wndLogicGallery.SetInfo( m_port_hit, point );
			m_wndLogicGallery.ShowWindow( SW_SHOW );
		}
	}
}

void CLogicDesignerWnd::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if ( (nFlags & 0x2000) != 0x0 )
	{
		if ( !m_bShowNumber )
			Invalidate();
		m_bShowNumber = TRUE;
	}

	CFullScrnWnd::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

void CLogicDesignerWnd::OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if ( (nFlags & 0x2000) == 0x0 )
	{
		if ( m_bShowNumber )
			Invalidate();
		m_bShowNumber = FALSE;
	}

	CFullScrnWnd::OnSysKeyUp(nChar, nRepCnt, nFlags);
}

void CLogicDesignerWnd::OnDestroy()
{

	CFullScrnWnd::OnDestroy();
	CT3kSoftlogicDlg* pDlg = (CT3kSoftlogicDlg*)AfxGetMainWnd();
	pDlg->OnCloseCanvasWnd();
	if ( m_wndEditGate )
		m_wndEditGate.DestroyWindow();
	if ( m_wndLogicGallery )
		m_wndLogicGallery.DestroyWindow();
}

void CLogicDesignerWnd::OnSize(UINT nType, int cx, int cy)
{
	CFullScrnWnd::OnSize(nType, cx, cy);

	if ( cx <=0 || cy <=0 ) return;

	if ( m_pBitmap != NULL ) delete m_pBitmap;
	m_pBitmap = NULL;
}

BOOL CLogicDesignerWnd::PreTranslateMessage(MSG* pMsg)
{
	if( m_eScreenMode == ScreenModePreview )
	{
		switch( pMsg->message )
		{
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MOUSEMOVE:
			return TRUE;
			break;
		}
		return CFullScrnWnd::PreTranslateMessage(pMsg);
	}
	else
	{

		if( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE )
		{
			PostMessage( WM_CLOSE, 0, 0 );
			return TRUE;
		}
	}

	return CFullScrnWnd::PreTranslateMessage(pMsg);
}
