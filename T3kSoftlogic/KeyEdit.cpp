// KeyEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "T3kSoftlogic.h"
#include "KeyEdit.h"

#include "KeyMapStr.h"

// CKeyEdit

IMPLEMENT_DYNAMIC(CKeyEdit, CEdit)
CKeyEdit::CKeyEdit()
{
	m_wKeyValue = 0x00;
}

CKeyEdit::~CKeyEdit()
{
}


BEGIN_MESSAGE_MAP(CKeyEdit, CEdit)
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()



// CKeyEdit 메시지 처리기입니다.
void CKeyEdit::Reset()
{
	SetWindowText( _T("None") );
}

void CKeyEdit::SetKeyValue( WORD wKeyValue )
{
	BYTE cV0 = (BYTE)(wKeyValue >> 8);
	BYTE cV1 = (BYTE)(wKeyValue & 0xFF);

	/*
	if( cV0 & MM_MOUSE_KEY0_MOUSE )
	{
		SetWindowText( _T("None") );
		return;
	}
	*/

	m_wKeyValue = wKeyValue;

	CString strKey;
	if( cV0 & MM_MOUSE_KEY0_CTRL )
	{
		if( !strKey.IsEmpty() )
			strKey += _T("+");
		strKey += _T("Ctrl");
	}

	if( cV0 & MM_MOUSE_KEY0_ALT )
	{
		if( !strKey.IsEmpty() )
			strKey += _T("+");
		strKey += _T("Alt");
	}

	if( cV0 & MM_MOUSE_KEY0_SHIFT )
	{
		if( !strKey.IsEmpty() )
			strKey += _T("+");		
		strKey += _T("Shift");
	}	

	if( cV0 & MM_MOUSE_KEY0_WINDOW )
	{
		if( !strKey.IsEmpty() )
			strKey += _T("+");		
		strKey += _T("Win");
	}	
		
	if( !strKey.IsEmpty() )
		strKey += _T("+");

	int nScanCode;
	if( FindScanCode(cV1, nScanCode) )
	{	
		int nVKey = ::MapVirtualKey(nScanCode, 1);

		CString strK;
		strK = VirtualKeyToString( nVKey );

		strKey += strK;
	}

	SetWindowText( strKey );
}

BOOL CKeyEdit::PreTranslateMessage(MSG* pMsg)
{
	CString strKey;

	if( pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN )
	{
		BYTE cV0 = (BYTE)(m_wKeyValue >> 8);
		BYTE cV1;
		if( GetKeyState(VK_LCONTROL) < 0 )
		{
			cV0 ^= MM_MOUSE_KEY0_CTRL;
		}

		if( GetKeyState(VK_LMENU) < 0 )
		{
			cV0 ^= MM_MOUSE_KEY0_ALT;
		}

		if( GetKeyState(VK_LSHIFT) < 0 )
		{
			cV0 ^= MM_MOUSE_KEY0_SHIFT;
		}

		if( GetKeyState(VK_LWIN) < 0 )
		{
			cV0 ^= MM_MOUSE_KEY0_WINDOW;
		}

		if( cV0 & MM_MOUSE_KEY0_CTRL )
		{
			if( !strKey.IsEmpty() )
				strKey += _T("+");
			strKey += _T("Ctrl");
		}

		if( cV0 & MM_MOUSE_KEY0_ALT )
		{
			if( !strKey.IsEmpty() )
				strKey += _T("+");
			strKey += _T("Alt");
		}

		if( cV0 & MM_MOUSE_KEY0_SHIFT )
		{
			if( !strKey.IsEmpty() )
				strKey += _T("+");		
			strKey += _T("Shift");
		}	

		if( cV0 & MM_MOUSE_KEY0_WINDOW )
		{
			if( !strKey.IsEmpty() )
				strKey += _T("+");		
			strKey += _T("Win");
		}	
		
		if( !strKey.IsEmpty() )
			strKey += _T("+");

		UINT nVKey = (UINT)pMsg->wParam;
		TRACE( _T("VKEY: %d\r\n"), nVKey );
		int nScanCode = ::MapVirtualKey(nVKey, 0);

		if( nVKey == VK_HANGUL )
		{
			cV1 = 0x90;
		}
		else if( nVKey == VK_HANJA )
		{
			cV1 = 0x91;
		}
		else
		{
			if( !FindUsageId( nScanCode, cV1 ) )
			{
				cV1 = 0x00;
			}
		}

		if( pMsg->message == WM_KEYDOWN )
		{
			BOOL bExtendedKey = (pMsg->lParam & 0x100000) ? TRUE : FALSE;
			switch( nScanCode )
			{
				case 0x52:	// Insert
					cV1 = 0x49;
					break;
				case 0x47:	// Home
					cV1 = 0x4A;
					break;
				case 0x49:	// Page Up
					cV1 = 0x4B;
					break;
				case 0x53:	// Delete
					cV1 = 0x4C;
					break;
				case 0x4F:	// End
					cV1 = 0x4D;
					break;
				case 0x51:	// Page Down
					cV1 = 0x4E;
					break;
				case 0x4D:	// Right Arrow
					cV1 = 0x4F;
					break;
				case 0x4B:	// Left Arrow
					cV1 = 0x50;
					break;
				case 0x50:	// Down Arrow
					cV1 = 0x51;
					break;
				case 0x48:	// Up Arrow
					cV1 = 0x52;
					break;
			}
		}

		CString strK;
		strK = VirtualKeyToString( nVKey );
		if( strK.IsEmpty() )
		{
			cV1 = 0x00;
		}

		m_wKeyValue = cV0 << 8 | cV1;
		TRACE( _T("%04X\r\n"), m_wKeyValue );

		strKey += strK;

		if( strKey.IsEmpty() || m_wKeyValue == 0x0000 )
		{
			SetWindowText( _T("None") );
			GetParent()->SendMessage( WM_KEYEDIT_VALUE_CHANGE, (WPARAM)0x0000 );
		}
		else
		{
			SetWindowText( strKey );
			GetParent()->SendMessage( WM_KEYEDIT_VALUE_CHANGE, (WPARAM)m_wKeyValue );
		}

		return TRUE;
	}

	return CEdit::PreTranslateMessage(pMsg);
}

#include <imm.h>
#pragma comment(lib, "imm32.lib")
void CKeyEdit::OnSetFocus(CWnd* pOldWnd)
{
	CEdit::OnSetFocus(pOldWnd);

    HIMC himc;
    DWORD dwConversion, dwSentence;
    himc = ImmGetContext (GetSafeHwnd() );
    ImmGetConversionStatus( himc, &dwConversion, &dwSentence);
    ImmSetConversionStatus( himc, IME_CMODE_ALPHANUMERIC, dwSentence );
}
