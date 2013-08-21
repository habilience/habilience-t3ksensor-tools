// UserDefinedKeyCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "T3kSoftlogic.h"
#include "UserDefinedKeyCtrl.h"
#include "KeyMapStr.h"


#define CTRL		0
#define ALT			1
#define SHIFT		2
#define WIN			3

#define UP			0
#define DOWN		1

#define IDC_EDIT_KEY			1
#define IDC_COMBO_FUNCKEY		2
#define IDC_COMBO_MOUSE			3

// CUserDefinedKeyCtrl

IMPLEMENT_DYNAMIC(CUserDefinedKeyCtrl, CGdipStatic)
CUserDefinedKeyCtrl::CUserDefinedKeyCtrl()
{
	m_pIconBtn[0] =
	m_pIconBtn[1] =
	m_pIconBtn[2] = 
	m_pIconBtn[3] = NULL;

	m_eKeyType = KeyTypeNormal;

	m_pIconClearBtn = NULL;

	m_wIconKeyValue[CTRL] = MM_MOUSE_KEY0_CTRL;
	m_wIconKeyValue[ALT] = MM_MOUSE_KEY0_ALT;
	m_wIconKeyValue[SHIFT] = MM_MOUSE_KEY0_SHIFT;
	m_wIconKeyValue[WIN] = MM_MOUSE_KEY0_WINDOW;

	m_wKeyValue = 0x00;
}

CUserDefinedKeyCtrl::~CUserDefinedKeyCtrl()
{
	for( int i=0 ; i<4 ; i++ )
	{
		if( m_pIconBtn[i] != NULL )
			delete m_pIconBtn[i];
	}

	if( m_pIconClearBtn )
		delete m_pIconClearBtn;
}


BEGIN_MESSAGE_MAP(CUserDefinedKeyCtrl, CGdipStatic)
	ON_WM_SIZE()
	ON_MESSAGE(WM_KEYEDIT_VALUE_CHANGE, OnKeyValueChange)
	ON_CBN_SELCHANGE(IDC_COMBO_FUNCKEY, OnCbnSelchangeComboFuncKey)
	ON_CBN_SELCHANGE(IDC_COMBO_MOUSE, OnCbnSelchangeComboMouse)
	ON_WM_LBUTTONDOWN()
	ON_WM_SETFOCUS()
	ON_WM_UPDATEUISTATE()
END_MESSAGE_MAP()



// CUserDefinedKeyCtrl 메시지 처리기입니다.


void CUserDefinedKeyCtrl::PreSubclassWindow()
{
	ModifyStyle( 0, SS_NOTIFY );
	ModifyStyleEx( WS_EX_STATICEDGE, 0 );

	m_pIconBtn[CTRL]	= GdipLoadImageFromRes( AfxGetInstanceHandle(), _T("PNG"), PNG_ICON_KEY_CTRL );
	m_pIconBtn[ALT]		= GdipLoadImageFromRes( AfxGetInstanceHandle(), _T("PNG"), PNG_ICON_KEY_ALT );
	m_pIconBtn[SHIFT]	= GdipLoadImageFromRes( AfxGetInstanceHandle(), _T("PNG"), PNG_ICON_KEY_SHIFT );
	m_pIconBtn[WIN]		= GdipLoadImageFromRes( AfxGetInstanceHandle(), _T("PNG"), PNG_ICON_KEY_WIN );
	m_pIconClearBtn		= GdipLoadImageFromRes( AfxGetInstanceHandle(), _T("PNG"), PNG_ICON_KEY_CLEAR );

	m_nIconTotalWidth = 0;
	m_nIconTotalWidth += m_pIconBtn[CTRL]->GetWidth() / 2;
	m_nIconTotalWidth += m_pIconBtn[ALT]->GetWidth() / 2;
	m_nIconTotalWidth += m_pIconBtn[SHIFT]->GetWidth() / 2;
	m_nIconTotalWidth += m_pIconBtn[WIN]->GetWidth() / 2;

	m_nIconTotalWidth += 4;

	m_nIconTotalHeight = m_pIconBtn[CTRL]->GetHeight();

	CRect rc;
	GetClientRect( rc );
	//rc.left += m_nIconTotalWidth;
	CRect rcEdit;
	rcEdit = rc;
	rcEdit.top = rc.CenterPoint().y;
	m_wndEdit.CreateEx( WS_EX_CLIENTEDGE, _T("EDIT"), _T(""), WS_CHILD|WS_TABSTOP/*|WS_BORDER*/|ES_AUTOHSCROLL, rcEdit, this, IDC_EDIT_KEY );
	m_wndEdit.SetFont( GetFont() );

	m_wndComboFuncKey.Create( WS_CHILD|WS_VSCROLL|WS_TABSTOP|CBS_DROPDOWNLIST, rcEdit, this, IDC_COMBO_FUNCKEY );
	m_wndComboFuncKey.SetFont( GetFont() );

	m_wndComboMouse.Create( WS_CHILD|WS_VSCROLL|WS_TABSTOP|CBS_DROPDOWNLIST, rcEdit, this, IDC_COMBO_MOUSE );
	m_wndComboMouse.SetFont( GetFont() );

	switch( m_eKeyType )
	{
	case KeyTypeNormal:
		m_wndComboMouse.ShowWindow( SW_HIDE );
		m_wndComboFuncKey.ShowWindow( SW_HIDE );
		m_wndEdit.ShowWindow( SW_SHOW );
		break;
	case KeyTypeFunction:
		m_wndEdit.ShowWindow( SW_HIDE );
		m_wndComboMouse.ShowWindow( SW_HIDE );
		m_wndComboFuncKey.ShowWindow( SW_SHOW );
		break;
	case KeyTypeMouse:
		m_wndEdit.ShowWindow( SW_HIDE );
		m_wndComboFuncKey.ShowWindow( SW_HIDE );
		m_wndComboMouse.ShowWindow( SW_SHOW );
		break;
	}

	CString strFn;
	int nIdx;
	for( int i=1 ; i<=24 ; i++ )
	{
		strFn.Format( _T("F%d"), i );
		nIdx = m_wndComboFuncKey.AddString( strFn );
		UINT nScanCode = ::MapVirtualKey( VK_F1+i-1, 0 );
		BYTE cUsageId;
		if( FindUsageId( nScanCode, cUsageId ) )
		{
			m_wndComboFuncKey.SetItemData( nIdx, (DWORD_PTR)cUsageId );
		}
	}

	nIdx = m_wndComboMouse.AddString( _T("Left Button") );
	m_wndComboMouse.SetItemData( nIdx, LBUTTON );
	nIdx = m_wndComboMouse.AddString( _T("Right Button") );
	m_wndComboMouse.SetItemData( nIdx, RBUTTON );
	nIdx = m_wndComboMouse.AddString( _T("Middle Button") );
	m_wndComboMouse.SetItemData( nIdx, MBUTTON );

	CGdipStatic::PreSubclassWindow();
}

void CUserDefinedKeyCtrl::SetKeyType( KeyType eType )
{
	if( m_eKeyType == eType )
		return;

	m_eKeyType = eType;

	switch( m_eKeyType )
	{
	case KeyTypeNormal:
		m_wndComboMouse.ShowWindow( SW_HIDE );
		m_wndComboFuncKey.ShowWindow( SW_HIDE );
		m_wndEdit.ShowWindow( SW_SHOW );
		break;
	case KeyTypeFunction:
		m_wndEdit.ShowWindow( SW_HIDE );
		m_wndComboMouse.ShowWindow( SW_HIDE );
		m_wndComboFuncKey.ShowWindow( SW_SHOW );
		break;
	case KeyTypeMouse:
		m_wndEdit.ShowWindow( SW_HIDE );
		m_wndComboFuncKey.ShowWindow( SW_HIDE );
		m_wndComboMouse.ShowWindow( SW_SHOW );
		break;
	}
}

void CUserDefinedKeyCtrl::Reset()
{
	m_wKeyValue = 0x0000;
	switch( m_eKeyType )
	{
	case KeyTypeNormal:
		m_wndEdit.Reset();
		break;
	case KeyTypeFunction:
		m_wndComboFuncKey.SetCurSel( -1 );
		break;
	case KeyTypeMouse:
		m_wndComboMouse.SetCurSel( -1 );
		break;
	}
}

void CUserDefinedKeyCtrl::_SetFunctionKeyValue( WORD wKeyValue )
{
	WORD wFnKeyValue = (wKeyValue & 0x00FF);
	m_wndComboFuncKey.SetCurSel( -1 );
	for( int nF = 0 ; nF<m_wndComboFuncKey.GetCount() ; nF++ )
	{
		WORD wFnKey = (WORD)m_wndComboFuncKey.GetItemData( nF );
		if( wFnKey == wFnKeyValue )
		{
			m_wndComboFuncKey.SetCurSel( nF );
			break;
		}
	}
}

void CUserDefinedKeyCtrl::_SetMouseValue( WORD wKeyValue )
{
	WORD wMKeyValue = (wKeyValue & 0x00FF);
	m_wndComboMouse.SetCurSel( -1 );
	for( int nM = 0 ; nM<m_wndComboMouse.GetCount() ; nM++ )
	{
		WORD wMKey = (WORD)m_wndComboMouse.GetItemData( nM );
		if( wMKey == wMKeyValue )
		{
			m_wndComboMouse.SetCurSel( nM );
			break;
		}
	}
}

void CUserDefinedKeyCtrl::SetKeyValue( WORD wKeyValue )
{
	m_wKeyValue = wKeyValue;
	switch( m_eKeyType )
	{
	case KeyTypeNormal:
		m_wndEdit.SetKeyValue( wKeyValue );
		break;
	case KeyTypeFunction:
		_SetFunctionKeyValue( wKeyValue );
		break;
	case KeyTypeMouse:
		m_wKeyValue |= MM_MOUSE_KEY0_MOUSE<<8;
		_SetMouseValue( wKeyValue );
		break;
	}

	Invalidate();
}

WORD CUserDefinedKeyCtrl::GetKeyValue() 
{ 
	return m_wKeyValue; 
}

void CUserDefinedKeyCtrl::OnDraw( Graphics& g )
{
	CRect rc;
	GetClientRect( rc );

	BYTE cV0 = (BYTE)(m_wKeyValue >> 8);

	int nIconW = 0;
	int nIconH = m_pIconBtn[0]->GetHeight();
	int nIconX = 0;
	int nIconY = rc.top + (rc.Height()/2-nIconH) / 2;
	BOOL bDown;
	for( int i=0 ; i<4 ; i++ )
	{
		bDown = cV0 & m_wIconKeyValue[i] ? TRUE : FALSE;
		nIconW = m_pIconBtn[i]->GetWidth() / 2;
		m_rectIconBtn[i] = Rect(nIconX, nIconY, nIconW, nIconH);
		g.DrawImage( m_pIconBtn[i], m_rectIconBtn[i], bDown ? nIconW : 0, 0, nIconW, nIconH , UnitPixel);
		nIconX += nIconW+1;
	}

	m_rectIconClearBtn = Rect( rc.right - m_pIconClearBtn->GetWidth()/2, nIconY, nIconW, nIconH );
	g.DrawImage( m_pIconClearBtn, m_rectIconClearBtn, 0, 0, nIconW, nIconH, UnitPixel );
}

void CUserDefinedKeyCtrl::OnGdipEraseBkgnd( Graphics& g )
{
	CRect rc;
	GetClientRect( rc );

	Rect rectBody( rc.left, rc.top, rc.Width(), rc.Height() );

	DWORD dwBkColor = GetSysColor(COLOR_BTNFACE);

	Color clrBk;
	clrBk.SetFromCOLORREF( dwBkColor );
	g.FillRectangle( &SolidBrush( clrBk ), rectBody );
}

void CUserDefinedKeyCtrl::OnSize(UINT nType, int cx, int cy)
{
	CGdipStatic::OnSize(nType, cx, cy);

	//m_wndEdit.MoveWindow( m_nIconTotalWidth, 0, cx-m_nIconTotalWidth, cy );

	m_wndEdit.MoveWindow( 0, cy/2, cx, cy/2 );
}

LRESULT CUserDefinedKeyCtrl::OnKeyValueChange( WPARAM wParam, LPARAM lParam )
{
	WORD wKeyValue = (WORD)(wParam);

	m_wKeyValue = wKeyValue;
	if( m_eKeyType == KeyTypeMouse )
		m_wKeyValue |= MM_MOUSE_KEY0_MOUSE<<8;
	Invalidate();

	GetParent()->SendMessage( WM_KEYEDIT_CHANGE, (WPARAM)m_wKeyValue );
	return 0;
}

void CUserDefinedKeyCtrl::OnCbnSelchangeComboFuncKey()
{
	int nSel = m_wndComboFuncKey.GetCurSel();
	if( nSel < 0 )
		return;

	//m_wKeyValue = 

	WORD wKeyValue = (WORD)m_wndComboFuncKey.GetItemData( nSel );
	m_wKeyValue = ((m_wKeyValue & 0xFF00) | wKeyValue);

	GetParent()->SendMessage( WM_KEYEDIT_CHANGE, (WPARAM)m_wKeyValue );
}

void CUserDefinedKeyCtrl::OnCbnSelchangeComboMouse()
{
	int nSel = m_wndComboMouse.GetCurSel();
	if( nSel < 0 )
		return;

	//m_wKeyValue = 

	WORD wKeyValue = (WORD)m_wndComboMouse.GetItemData( nSel );
	m_wKeyValue = ((m_wKeyValue & 0xFF00) | wKeyValue);
	if( m_eKeyType == KeyTypeMouse )
		m_wKeyValue |= MM_MOUSE_KEY0_MOUSE<<8;

	GetParent()->SendMessage( WM_KEYEDIT_CHANGE, (WPARAM)m_wKeyValue );
}

void CUserDefinedKeyCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	Point pt(point.x, point.y);
	for( int i=0 ; i<4 ; i++ )
	{
		if( m_rectIconBtn[i].Contains(pt) )
		{
			BYTE cV0 = (BYTE)(m_wKeyValue >> 8);
			BYTE cV1 = (BYTE)(m_wKeyValue);
			cV0 ^= m_wIconKeyValue[i];

			m_wKeyValue = cV0 << 8 | cV1;
			switch( m_eKeyType )
			{
			case KeyTypeNormal:
				m_wndEdit.SetKeyValue( m_wKeyValue );
				m_wndEdit.SetFocus();
				break;
			case KeyTypeFunction:
				_SetFunctionKeyValue( m_wKeyValue );
				break;
			case KeyTypeMouse:
				m_wKeyValue |= MM_MOUSE_KEY0_MOUSE<<8;
				_SetMouseValue( m_wKeyValue );
				break;
			}
			Invalidate();

			GetParent()->SendMessage( WM_KEYEDIT_CHANGE, (WPARAM)m_wKeyValue );
		}
	}

	if( m_rectIconClearBtn.Contains(pt) )
	{
		m_wKeyValue = 0x00;
		switch( m_eKeyType )
		{
		case KeyTypeNormal:
			m_wndEdit.SetKeyValue( m_wKeyValue );
			m_wndEdit.SetFocus();
			break;
		case KeyTypeFunction:
			_SetFunctionKeyValue( m_wKeyValue );
			break;
		case KeyTypeMouse:
			m_wKeyValue |= MM_MOUSE_KEY0_MOUSE<<8;
			_SetMouseValue( m_wKeyValue );
			break;
		}
		Invalidate();

		GetParent()->SendMessage( WM_KEYEDIT_CHANGE, (WPARAM)m_wKeyValue );
	}

	CGdipStatic::OnLButtonDown(nFlags, point);
}

void CUserDefinedKeyCtrl::OnSetFocus(CWnd* pOldWnd)
{
	CGdipStatic::OnSetFocus(pOldWnd);

	if( m_eKeyType == KeyTypeNormal )
	{
		m_wndEdit.SetFocus();
	}
	else
	{
		//
	}
}

void CUserDefinedKeyCtrl::OnUpdateUIState(UINT nAction, UINT nUIElement)
{
	// 이 기능을 사용하려면 Windows 2000 이상이 필요합니다.
	// _WIN32_WINNT 및 WINVER 기호는 0x0500보다 크거나 같아야 합니다.
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	if ( nAction != UIS_SET )
		CGdipStatic::OnUpdateUIState(nAction, nUIElement);
}
