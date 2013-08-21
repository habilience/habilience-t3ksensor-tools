// BorderStyleEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "BorderStyleEdit.h"
#include ".\borderstyleedit.h"

#include <float.h>

#define		ID_TIMER_WARNING		(100)

// CBorderStyleEdit

IMPLEMENT_DYNAMIC(CBorderStyleEdit, CEdit)
CBorderStyleEdit::CBorderStyleEdit() : m_dwBorderColor( RGB(150, 150, 150) ),
					 m_dwTextColor( RGB(0, 0, 0) ),
					 m_dwActiveTextColor( RGB(255, 0, 0) ),
					 m_dwBgColor( RGB(240, 240, 240) )

{
	m_BgBrush.CreateSolidBrush( m_dwBgColor );
	m_bShowBorder = TRUE;
	m_nBorderWidth = 2;

	m_bNowEditing = FALSE;

	m_dValueRangeMin = -DBL_MAX;
	m_dValueRangeMax = +DBL_MAX;
	m_strAllowString = _T("0123456789.-");
	m_bFloatStyle = TRUE;

	m_bModified = FALSE;

	m_nWarningTimer = 0;
	m_bBlink = FALSE;
	m_bWarning = FALSE;
	m_bTextStyle = FALSE;
}

CBorderStyleEdit::~CBorderStyleEdit()
{
}


BEGIN_MESSAGE_MAP(CBorderStyleEdit, CEdit)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_PAINT()
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
	ON_CONTROL_REFLECT(EN_SETFOCUS, OnSetfocus)
	ON_WM_TIMER()
	ON_WM_NCPAINT()
	ON_WM_UPDATEUISTATE()
END_MESSAGE_MAP()

void CBorderStyleEdit::SetFloatStyle( BOOL bFloatStyle, BOOL bAllowMinus/*=TRUE*/ )
{
	m_bFloatStyle = bFloatStyle;
	if ( bFloatStyle )
		m_strAllowString = _T("0123456789.");
	else
		m_strAllowString = _T("0123456789");
	if ( bAllowMinus )
		m_strAllowString += _T("-");

	m_strDeniedString.Empty();

	m_bTextStyle = FALSE;
}

void CBorderStyleEdit::SetPathNameStyle()
{
	m_strDeniedString = _T("\\/:*?\"<>|");
	m_strAllowString.Empty();

	m_bTextStyle = TRUE;
}

HBRUSH CBorderStyleEdit::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	DrawEditFrame( pDC );

	return m_BgBrush;
}

void CBorderStyleEdit::OnPaint()
{
	CEdit::OnPaint();
}

void CBorderStyleEdit::SetColor( COLORREF dwBorderColor, COLORREF dwBgColor )
{
	m_dwBgColor = dwBgColor;
	m_dwBorderColor = dwBorderColor;
	if( m_BgBrush.GetSafeHandle() )
		m_BgBrush.DeleteObject();

	m_BgBrush.CreateSolidBrush( dwBgColor );

	if( IsWindow(GetSafeHwnd()) )
		RedrawWindow();
}

void CBorderStyleEdit::DrawEditFrame( CDC* pDC )
{	
	CRect rcItem;
	GetClientRect( &rcItem );

	if( m_bNowEditing )
		pDC->SetTextColor( m_dwActiveTextColor );
	else
		pDC->SetTextColor( m_dwTextColor );

	pDC->SetBkColor( m_dwBgColor );

	if( m_bShowBorder )
	{
		DWORD dwBorderColor = m_dwBorderColor;
		if( m_bWarning )
		{
			dwBorderColor = (m_bBlink == TRUE ? RGB(255, 0, 0) : m_dwBorderColor);
		}

		rcItem.InflateRect( 1, 1, 1, 1 );
		CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject( NULL_BRUSH );
		CPen BorderPen;
		BorderPen.CreatePen( PS_SOLID, m_nBorderWidth, dwBorderColor );
		CPen* pOldPen = pDC->SelectObject( &BorderPen );

		//pDC->Rectangle( rcItem );

		pDC->SelectObject( pOldPen );
		pDC->SelectObject( pOldBrush );
	}
}

void CBorderStyleEdit::OnKillfocus() 
{
	CString strValue;
	GetWindowText( strValue );	
	if( m_bTextStyle )
	{
		if( m_strOldValue.Compare( strValue ) != 0 )
		{
			GetParent()->SendMessage( WM_EDIT_MODIFIED, (WPARAM)GetDlgCtrlID(), (LPARAM)(LPCTSTR)strValue );
		}
	}
	else
	{

		if( strValue.IsEmpty() )
		{
			if( m_bFloatStyle )
			{
				SetWindowText( _T("0.00") );
				strValue = _T("0.00");
			}
			else
			{
				SetWindowText( _T("0") );
				strValue = _T("0");
			}
		}
		
		double dValue = _tcstod( strValue, NULL );
		if( dValue < m_dValueRangeMin ) dValue = m_dValueRangeMin;
		if( dValue > m_dValueRangeMax ) dValue = m_dValueRangeMax;

		if( m_bFloatStyle )
			strValue.Format( _T("%.1f"), dValue );
		else
			strValue.Format( _T("%d"), (int)dValue );
		SetWindowText( strValue );

		if( m_dOldValue != dValue )
		{
			GetParent()->SendMessage( WM_EDIT_MODIFIED, (WPARAM)GetDlgCtrlID(), (LPARAM)&dValue );
		}
	}

	m_bNowEditing = FALSE;
	RedrawWindow();
}

void CBorderStyleEdit::OnSetfocus() 
{
	m_bNowEditing = TRUE;
	m_bModified = FALSE;

	CString strValue;
	GetWindowText( strValue );

	if( m_bTextStyle )
	{
		m_strOldValue = strValue;
	}
	else
	{
		if( strValue.IsEmpty() )
		{
			if( m_bFloatStyle )
			{
				SetWindowText( _T("0.0") );
				strValue = _T("0.0");
			}
			else
			{
				SetWindowText( _T("0") );
				strValue = _T("0");
			}
		}
		
		m_dOldValue = _tcstod( strValue, NULL );
	}

	RedrawWindow();
}

BOOL CBorderStyleEdit::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_CHAR )
	{
		if( !m_strAllowString.IsEmpty() )
		{
			TCHAR chCharCode = (TCHAR)pMsg->wParam;
			if( chCharCode == VK_BACK )
				return CEdit::PreTranslateMessage(pMsg);
			if( m_strAllowString.Find( chCharCode ) < 0 )
			{
				return TRUE;
			}
		}
		if( !m_strDeniedString.IsEmpty() )
		{
			TCHAR chCharCode = (TCHAR)pMsg->wParam;
			if( chCharCode == VK_BACK )
				return CEdit::PreTranslateMessage(pMsg);
			if( m_strDeniedString.Find( chCharCode ) >= 0 )
			{
				return TRUE;
			}
		}
	}

	return CEdit::PreTranslateMessage(pMsg);
}

void CBorderStyleEdit::SetWarning( BOOL bWarning )
{
	m_bWarning = bWarning;
	if( m_bWarning )
	{
		if( m_nWarningTimer )
			KillTimer( ID_TIMER_WARNING );
		SetTimer( ID_TIMER_WARNING, 500, NULL );
	}
	else
	{
		if( m_nWarningTimer )
			KillTimer( ID_TIMER_WARNING );
		m_nWarningTimer = 0;
	}
}

void CBorderStyleEdit::OnTimer(UINT nIDEvent)
{
	if( nIDEvent == ID_TIMER_WARNING )
	{
		m_bBlink = m_bBlink ? FALSE : TRUE;
		RedrawWindow();
	}

	CEdit::OnTimer(nIDEvent);
}

void CBorderStyleEdit::OnNcPaint()
{
	CEdit::OnNcPaint();
	//CWindowDC dc(this);

	//DrawEditFrame( &dc );
}

void CBorderStyleEdit::OnUpdateUIState(UINT nAction, UINT nUIElement)
{
	// 이 기능을 사용하려면 Windows 2000 이상이 필요합니다.
	// _WIN32_WINNT 및 WINVER 기호는 0x0500보다 크거나 같아야 합니다.
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	if ( nAction != UIS_SET )
		CEdit::OnUpdateUIState(nAction, nUIElement);
}
