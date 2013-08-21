#include "stdafx.h"

#include "KeyMapStr.h"

#define MM_MOUSE_KEY0_CTRL    0x01
#define MM_MOUSE_KEY0_SHIFT   0x02
#define MM_MOUSE_KEY0_ALT     0x04
#define MM_MOUSE_KEY0_WINDOW  0x08
#define MM_MOUSE_KEY0_MOUSE   0x80
#define MM_MOUSE_KEY1_NOACTION             0x0
#define MM_MOUSE_KEY1_MOUSE_L_MOVE         0x01
#define MM_MOUSE_KEY1_MOUSE_L_CLICK        0x02
#define MM_MOUSE_KEY1_MOUSE_L_DOUBLECLICK  0x03
#define MM_MOUSE_KEY1_MOUSE_R_MOVE         0x04
#define MM_MOUSE_KEY1_MOUSE_R_CLICK        0x05
#define MM_MOUSE_KEY1_MOUSE_R_DOUBLECLICK  0x06
#define MM_MOUSE_KEY1_MOUSE_M_MOVE         0x07
#define MM_MOUSE_KEY1_MOUSE_M_CLICK        0x08
#define MM_MOUSE_KEY1_MOUSE_M_DOUBLECLICK  0x09
#define MM_MOUSE_KEY1_MOUSE_WHEEL          0xfe
#define MM_MOUSE_KEY1_MOUSE_MOVE           0xff

const short s_ScanCodeToHIDUsageID[] =
{	
	0x00, 0x29, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x2D, 0x2E, 0x2A, 0x2B, 0x14, 0x1A, 0x08, 0x15, 0x17, 0x1C,
	0x18, 0x0C, 0x12, 0x13, 0x2F, 0x30, 0x28, 0xE0, 0x04, 0x16, 0x07, 0x09, 0x0A, 0x0B, 0x0D, 0x0E, 0x0F, 0x33, 0x34, 0x35, 0xE1, 0x31,
	0x1D, 0x1B, 0x06, 0x19, 0x05, 0x11, 0x10, 0x36, 0x37, 0x38, 0xE5, 0x55, 0xE2, 0x2C, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x40, 0x41, 0x42, 0x43, 0x53, 0x47, 0x5F, 0x60, 0x61, 0x56, 0x5C, 0x5D, 0x5E, 0x57, 0x59, 0x5A, 0x5B, 0x62, 0x63, 0x46/*0x00*/, 0x00, 0x64,
	0x44, 0x45, 0x67, 0x00, 0x00, 0x8C, 0x65, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70,
	0x71, 0x72, 0x00, 0x88, 0x00, 0x00, 0x87, 0x00, 0x00, 0x73, 0x93, 0x92, 0x8A, 0x00, 0x8B, 0x00, 0x89, 0x85, 0x00  
};

BOOL FindUsageId( int nScanCode, BYTE& cUsageId )
{
	/*
	if( nVKey == VK_HANGUL )
	{
		cUsageId = 0x90;
		return TRUE;
	}
	else if( nVKey == VK_HANJA )
	{
		cUsageId = 0x91;
		return TRUE;
	}
	*/

	if( nScanCode >= 0 && nScanCode < sizeof(s_ScanCodeToHIDUsageID) / sizeof(short) )
	{
		cUsageId = (BYTE)s_ScanCodeToHIDUsageID[nScanCode];
		return TRUE;
	}

	return FALSE;
}

BOOL FindScanCode( BYTE cUsageId, int& nScanCode )
{
	switch( cUsageId )
	{
	case 0x50:
		nScanCode = MapVirtualKey( VK_LEFT, 0 );
		return TRUE;
	case 0x4F:
		nScanCode = MapVirtualKey( VK_RIGHT, 0 );
		return TRUE;
	case 0x52:
		nScanCode = MapVirtualKey( VK_UP, 0 );
		return TRUE;
	case 0x51:
		nScanCode = MapVirtualKey( VK_DOWN, 0 );
		return TRUE;
	case 0x49:
		nScanCode = MapVirtualKey( VK_INSERT, 0 );
		return TRUE;
	case 0x4A:
		nScanCode = MapVirtualKey( VK_HOME, 0 );
		return TRUE;
	case 0x4B:
		nScanCode = MapVirtualKey( VK_PRIOR, 0 );
		return TRUE;
	case 0x4C:
		nScanCode = MapVirtualKey( VK_DELETE, 0 );
		return TRUE;
	case 0x4D:
		nScanCode = MapVirtualKey( VK_END, 0 );
		return TRUE;
	case 0x4E:
		nScanCode = MapVirtualKey( VK_NEXT, 0);
		return TRUE;
	}

	for( int i=0 ; i<sizeof(s_ScanCodeToHIDUsageID)/sizeof(short) ; i++ )
	{
		if( s_ScanCodeToHIDUsageID[i] == cUsageId )
		{
			nScanCode = i;
			return TRUE;
		}
	}
	return FALSE;
}

CString CompositeKeyToString( BYTE cV )
{
	switch( cV )
	{
	case MM_MOUSE_KEY0_CTRL:
		return _T("Ctrl");
		break;
	case MM_MOUSE_KEY0_SHIFT:
		return _T("Shift");
		break;
	case MM_MOUSE_KEY0_ALT:
		return _T("Alt");
		break;
	case MM_MOUSE_KEY0_WINDOW:
		return _T("Win");
		break;
	}
	return _T("");
}

CString VirtualKeyToString( UINT nVk )
{
	switch( nVk )
	{
	case VK_BACK:
		return CString(_T("Backspace"));
	case VK_TAB:
		return CString(_T("Tab"));
	case VK_CLEAR:	// NumLock 꺼졌을때 숫자 패드의 5
		return CString(_T(""));
	case VK_PAUSE:
		return CString(_T("Pause"));
	case VK_HANGUL:
		return CString(_T("Lang"));
	case VK_HANJA:
		return CString(_T("Hanja"));
	case VK_ESCAPE:
		return CString(_T("ESC"));
	case VK_SPACE:
		return CString(_T("Space"));
	case VK_PRIOR:
		return CString(_T("PgUp"));
	case VK_NEXT:
		return CString(_T("PgDn"));
	case VK_END:
		return CString(_T("End"));
	case VK_HOME:
		return CString(_T("Home"));
	case VK_LEFT:
		return CString(_T("Left"));
	case VK_UP:
		return CString(_T("Up"));
	case VK_RIGHT:
		return CString(_T("Right"));
	case VK_DOWN:
		return CString(_T("Down"));
	case VK_PRINT:
		return CString(_T("Print"));
	case VK_SNAPSHOT:
		return CString(_T("Prt Scn"));
	case VK_INSERT:
		return CString(_T("Insert"));
	case VK_DELETE:
		return CString(_T("Delete"));
	case VK_RETURN:
		return CString(_T("Enter"));
	case VK_CAPITAL:
		return CString(_T("Caps Lock"));
	case VK_NUMLOCK:
		return CString(_T("Num Lock"));
	case VK_SCROLL:
		return CString(_T("Scrl Lock"));
	case VK_MULTIPLY:
		return CString(_T("Num *"));
	case VK_ADD:
		return CString(_T("Num +"));
	case VK_SEPARATOR:
		return CString(_T("Num S"));
	case VK_SUBTRACT:
		return CString(_T("Num -"));
	case VK_DIVIDE:
		return CString(_T("Num /"));
	case VK_DECIMAL:
		return CString(_T("Num D"));
	case VK_OEM_4:
		return CString(_T("["));
	case VK_OEM_6:
		return CString(_T("]"));
	case VK_OEM_5:
		return CString(_T("\\"));
	case VK_OEM_1:
		return CString(_T(";"));
	case VK_OEM_7:
		return CString(_T("'"));
	case VK_OEM_COMMA:
		return CString(_T(","));
	case VK_OEM_PERIOD:
		return CString(_T("."));
	case VK_OEM_2:
		return CString(_T("/"));
	case VK_OEM_3:
		return CString(_T("`"));
	//case VK_LWIN:
	//case VK_RWIN:
	//	return CString(_T("Win"));
	}
	if( nVk >= VK_F1 && nVk <= VK_F24 )
	{
		CString strK;
		strK.Format( _T("F%d"), nVk-VK_F1+1 );
		return strK;
	}
	if( nVk >= VK_NUMPAD0 && nVk <= VK_NUMPAD9 )
	{
		CString strK;
		strK.Format( _T("Num %d"), nVk-VK_NUMPAD0 );
		return strK;
	}

	if( (nVk >= '0' && nVk <= '9') ||
		(nVk >= 'A' && nVk <= 'Z') )
	{
		CString strK;
		strK.Format( _T("%C"), nVk );
		return strK;
	}

	return CString(_T(""));
}

CString MouseKeyToString( BYTE cMouseKey )
{
	switch( cMouseKey )
	{
	case MM_MOUSE_KEY1_NOACTION:
	default:
		return CString( _T("") );
		break;
	case MM_MOUSE_KEY1_MOUSE_L_MOVE:
		return CString( _T("Left\r\nDrag") );
	case MM_MOUSE_KEY1_MOUSE_L_CLICK:
		return CString( _T("Left\r\nClick") );
	case MM_MOUSE_KEY1_MOUSE_L_DOUBLECLICK:
		return CString( _T("Left\r\nDouble Click") );
	case MM_MOUSE_KEY1_MOUSE_R_MOVE:
		return CString( _T("Right\r\nDrag") );
	case MM_MOUSE_KEY1_MOUSE_R_CLICK:
		return CString( _T("Right\r\nClick") );
	case MM_MOUSE_KEY1_MOUSE_R_DOUBLECLICK:
		return CString( _T("Right\r\nDouble Click") );
	case MM_MOUSE_KEY1_MOUSE_M_MOVE:
		return CString( _T("Middle\r\nDrag") );
	case MM_MOUSE_KEY1_MOUSE_M_CLICK:
		return CString( _T("Middle\r\nClick") );
	case MM_MOUSE_KEY1_MOUSE_M_DOUBLECLICK:
		return CString( _T("Middle\r\nDouble Click") );
	case MM_MOUSE_KEY1_MOUSE_WHEEL:
		return CString( _T("Wheel") );
	case MM_MOUSE_KEY1_MOUSE_MOVE:
		return CString( _T("Move") );
	}
	return CString(_T(""));
}

CString GetMappingStr( BYTE cV0, BYTE cV1 )
{
	CString strText;
	CString strMouseKey;
	int nScanCode;

	if( cV0 & MM_MOUSE_KEY0_MOUSE )
	{
		strMouseKey = MouseKeyToString(cV1);
	}

	SHORT nCompositKey = cV0 & (~MM_MOUSE_KEY0_MOUSE);

	//switch( cV0 & (~MM_MOUSE_KEY0_MOUSE) )
	//case MM_MOUSE_KEY0_MOUSE:
	//	strText = MouseKeyToString(cV1);
	//	break;
	if( nCompositKey & MM_MOUSE_KEY0_CTRL )
	{
		if( strText.IsEmpty() )
			strText = _T("[");
		else
			strText += _T("+");
		strText += CompositeKeyToString( MM_MOUSE_KEY0_CTRL );
	}

	if( nCompositKey & MM_MOUSE_KEY0_ALT )
	{
		if( strText.IsEmpty() )
			strText = _T("[");
		else
			strText += _T("+");
		strText += CompositeKeyToString( MM_MOUSE_KEY0_ALT );
	}

	if( nCompositKey & MM_MOUSE_KEY0_SHIFT )
	{
		if( strText.IsEmpty() )
			strText = _T("[");
		else
			strText += _T("+");
		strText += CompositeKeyToString( MM_MOUSE_KEY0_SHIFT );
	}

	if( nCompositKey & MM_MOUSE_KEY0_WINDOW )
	{
		if( strText.IsEmpty() )
			strText = _T("[");
		else
			strText += _T("+");
		strText += CompositeKeyToString( MM_MOUSE_KEY0_WINDOW );
	}

	if( strMouseKey.IsEmpty() && FindScanCode(cV1, nScanCode) )
	{	
		//BYTE cKeyState;
		//WORD wChar;
		CString strK;
		int nVKey = ::MapVirtualKey(nScanCode, 1);

		strK = VirtualKeyToString( nVKey );

		if( !strK.IsEmpty() )
		{
			if( strText.IsEmpty() )
				strText = _T("[");
			else
				strText += _T("+");
			strText += strK;
			strText += _T("]");
		}
	}
	else
	{
		if( !strText.IsEmpty() )
			strText += _T("]");
	}

	if( !strMouseKey.IsEmpty() )
	{
		if( !strText.IsEmpty() )
		{
			strText += _T("+");
		}
		strText += strMouseKey;
	}

	return strText;
}
