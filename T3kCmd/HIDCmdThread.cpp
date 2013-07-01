
#include "StdAfx.h"

#include "resource.h"

#include ".\HIDCmdThread.h"
#include ".\T3000Cmd.h"

#include <process.h>

#define SYSTIME_PER_MSEC 10000

const char cszInstantMode[] = "instant_mode=";

const char cszIsNotConnected[] = "T3k series is not connected.\r\n";

const char cstrGetNv[] = "\
Displays the system values of T3k series.\r\n\
\r\n\
";
const char cszGetNv_End[] = "\r\n\
The end of a command.\r\n";

CHIDCmdThread::CHIDCmdThread(void)
{
	m_pT30xHandle = NULL;
	m_bIsConnect = FALSE;
	m_hExitEvent = ::CreateEvent( NULL, TRUE, FALSE, NULL );

	for ( int ni = 0; ni < PROMPT_MAX; ni++ )
	{
		m_prompt[ni] = promptNil;
	}

	m_ppPreCommands = NULL;
	m_nPreCommands = NULL;

	m_pFileGetNv = NULL;
	m_eGetNv = enFalse;

	m_tmStart = GetCurrentSystime();
	m_bTextOut = TRUE;
	m_bInstantMode = FALSE;

	m_szInstantMode[0] = '\0';

	m_dwTimeCheck = 0;

	m_nGetNvId = 0;

	::InitializeCriticalSection( &m_csTextOut );
}

CHIDCmdThread::~CHIDCmdThread(void)
{
	Stop();

	for ( int ni = 0; ni < m_nPreCommands; ni++ )
	{
		delete [] m_ppPreCommands[ni];
	}
	m_nPreCommands = 0;
	delete [] m_ppPreCommands;
	m_ppPreCommands = NULL;

	if ( m_pFileGetNv )
	{
		fclose(m_pFileGetNv);
		m_pFileGetNv = NULL;
	}

	::CloseHandle( m_hExitEvent );
	m_hExitEvent = NULL;

	::DeleteCriticalSection( &m_csTextOut );
}

void CHIDCmdThread::Start()
{
	if( m_hThread )
	{
		return;
	}

	::ResetEvent( m_hExitEvent );

	unsigned int nThreadID;
	m_hThread = (HANDLE)_beginthreadex( NULL, 0, _OnThread, (void *)this, 0, &nThreadID );
}

void CHIDCmdThread::Stop()
{
	if( !m_hThread )
	{
		return;
	}

	::SetEvent( m_hExitEvent );

	if( ::WaitForSingleObject( m_hThread, 1000 ) != WAIT_OBJECT_0 )
	{
		::TerminateThread( m_hThread, 0 );
	}

	::CloseHandle( m_hThread );

	m_hThread = NULL;
}

unsigned int __stdcall CHIDCmdThread::_OnThread( LPVOID lpParam )
{
	CHIDCmdThread* pThis = (CHIDCmdThread*)lpParam;
	pThis->OnThread();
	_endthreadex( 0 );
	return 0;
}

struct DEVICE_ID
{
	WORD nVID;
	WORD nPID;
	WORD nMI;
	int nDeviceIndex;
};

DEVICE_ID	g_DevId;

void ListView_SetItemData( HWND hWnd, int nItem, DWORD_PTR dwData )
{
	ASSERT(::IsWindow(hWnd));
	LVITEM lvi;
	memset(&lvi, 0, sizeof(LVITEM));
	lvi.iItem = nItem;
	lvi.mask = LVIF_PARAM;
	lvi.lParam = (LPARAM)dwData;
	VERIFY(::SendMessage(hWnd, LVM_SETITEM, 0, (LPARAM)&lvi));
}
DWORD_PTR ListView_GetItemData( HWND hWnd, int nItem )
{
	ASSERT(::IsWindow(hWnd));
	LVITEM lvi;
	memset(&lvi, 0, sizeof(LVITEM));
	lvi.iItem = nItem;
	lvi.mask = LVIF_PARAM;
	VERIFY(::SendMessage(hWnd, LVM_GETITEM, 0, (LPARAM)&lvi));
	return lvi.lParam;
}

void InsertListItem( HWND hListCtrl, LPCTSTR lpszModelName, int nDevCnt, WORD nVID, WORD nPID, WORD nMI, int nDeviceIndex )
{
	TCHAR szNum[10];
	LVITEM lvi;
	memset( &lvi, 0, sizeof(LVITEM) );
	lvi.mask = LVIF_TEXT;

	_stprintf( szNum, _T("%d"), nDevCnt );
	lvi.iItem = ListView_GetItemCount( hListCtrl );
	lvi.iSubItem = 0;
	lvi.pszText = szNum;
	int nIdx = ListView_InsertItem( hListCtrl, &lvi );
	lvi.iItem = nIdx;
	lvi.iSubItem = 1;
	lvi.pszText = (LPTSTR)lpszModelName;
	ListView_SetItem( hListCtrl, &lvi );
	lvi.iSubItem = 2;
	lvi.pszText = CT30xHandle::GetDevicePath( nVID, nPID, nMI, nDeviceIndex );
	ListView_SetItem( hListCtrl, &lvi );
}

void RefreshDeviceList( HWND hListCtrl )
{
	for ( int nI=0 ; nI<ListView_GetItemCount(hListCtrl) ; nI++ )
	{
		DEVICE_ID* pDevId = (DEVICE_ID*)ListView_GetItemData( hListCtrl, nI);
		delete pDevId;
	}

	ListView_DeleteAllItems( hListCtrl );

	int nOldT3000 = CT30xHandle::GetDeviceCount( 0xFFFF, 0x0000, 1 );
	int nDevCnt = 1;
	for( int i=0 ; i<nOldT3000 ; i++ )
	{
		InsertListItem( hListCtrl, _T("T3000"), nDevCnt++, 0xFFFF, 0x0000, 0x01, i );
	}

	int nT3000 = CT30xHandle::GetDeviceCount( 0x2200, 0x3000, 1 );
	for( int i=0 ; i<nT3000 ; i++ )
	{
		InsertListItem( hListCtrl, _T("T3000"), nDevCnt++, 0x2200, 0x3000, 0x01, i );
	}

	int nT3100 = CT30xHandle::GetDeviceCount( 0x2200, 0x3100, 1 );
	for( int i=0 ; i<nT3100 ; i++ )
	{
		InsertListItem( hListCtrl, _T("T3100"), nDevCnt++, 0x2200, 0x3100, 0x01, i );
	}
	// later model
	int nT3200 = CT30xHandle::GetDeviceCount( 0x2200, 0x3200, 1 );
	for( int i=0 ; i<nT3200 ; i++ )
	{
		InsertListItem( hListCtrl, _T("T3200"), nDevCnt++, 0x2200, 0x3200, 0x01, i );
	}
	
	int nT3500 = CT30xHandle::GetDeviceCount( 0x2200, 0x3500, 1 );
	for( int i=0 ; i<nT3500 ; i++ )
	{
		InsertListItem( hListCtrl, _T("T3k A"), nDevCnt++, 0x2200, 0x3500, 0x01, i );
	}

	int nT3900 = CT30xHandle::GetDeviceCount( 0x2200, 0x3900, 1 );
	for( int i=0 ; i<nT3900 ; i++ )
	{
		InsertListItem( hListCtrl, _T("T3900"), nDevCnt++, 0x2200, 0x3900, 0x01, i );
	}

	int nT3kVHID = CT30xHandle::GetDeviceCount( 0x2200, 0xFF02, 0 );
	for( int i=0 ; i<nT3kVHID ; i++ )
	{
		InsertListItem( hListCtrl, _T("T3900"), nDevCnt++, 0x2200, 0xFF02, 0, i );
	}
}

BOOL CenterWindow(HWND hWnd, HWND hWndCenter = NULL)
{
	// determine owner window to center against
	DWORD dwStyle = (DWORD)::GetWindowLong(hWnd, GWL_STYLE);
	if(hWndCenter == NULL)
	{
		if(dwStyle & WS_CHILD)
			hWndCenter = ::GetParent(hWnd);
		else
			hWndCenter = ::GetWindow(hWnd, GW_OWNER);
	}

	// get coordinates of the window relative to its parent
	RECT rcDlg;
	::GetWindowRect(hWnd, &rcDlg);
	RECT rcArea;
	RECT rcCenter;
	HWND hWndParent;
	if(!(dwStyle & WS_CHILD))
	{
		// don't center against invisible or minimized windows
		if(hWndCenter != NULL)
		{
			DWORD dwStyleCenter = ::GetWindowLong(hWndCenter, GWL_STYLE);
			if(!(dwStyleCenter & WS_VISIBLE) || (dwStyleCenter & WS_MINIMIZE))
				hWndCenter = NULL;
		}

		// center within screen coordinates
#if WINVER < 0x0500
		::SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcArea, NULL);
#else
		HMONITOR hMonitor = NULL;
		if(hWndCenter != NULL)
		{
			hMonitor = ::MonitorFromWindow(hWndCenter, MONITOR_DEFAULTTONEAREST);
		}
		else
		{
			hMonitor = ::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
		}

		MONITORINFO minfo;
		minfo.cbSize = sizeof(MONITORINFO);
		BOOL bResult = ::GetMonitorInfo(hMonitor, &minfo);

		rcArea = minfo.rcWork;
#endif
		if(hWndCenter == NULL)
			rcCenter = rcArea;
		else
			::GetWindowRect(hWndCenter, &rcCenter);
	}
	else
	{
		// center within parent client coordinates
		hWndParent = ::GetParent(hWnd);

		::GetClientRect(hWndParent, &rcArea);
		::GetClientRect(hWndCenter, &rcCenter);
		::MapWindowPoints(hWndCenter, hWndParent, (POINT*)&rcCenter, 2);
	}

	int DlgWidth = rcDlg.right - rcDlg.left;
	int DlgHeight = rcDlg.bottom - rcDlg.top;

	// find dialog's upper left based on rcCenter
	int xLeft = (rcCenter.left + rcCenter.right) / 2 - DlgWidth / 2;
	int yTop = (rcCenter.top + rcCenter.bottom) / 2 - DlgHeight / 2;

	// if the dialog is outside the screen, move it inside
	if(xLeft + DlgWidth > rcArea.right)
		xLeft = rcArea.right - DlgWidth;
	if(xLeft < rcArea.left)
		xLeft = rcArea.left;

	if(yTop + DlgHeight > rcArea.bottom)
		yTop = rcArea.bottom - DlgHeight;
	if(yTop < rcArea.top)
		yTop = rcArea.top;

	// map screen coordinates to child coordinates
	return ::SetWindowPos(hWnd, NULL, xLeft, yTop, -1, -1,
		SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

BOOL CALLBACK DeviceSelectDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	LVCOLUMN lvc;
	NMHDR* pNMHDR;
	switch(iMessage)
	{
	case WM_INITDIALOG:
		ListView_SetExtendedListViewStyle( GetDlgItem(hDlg, IDC_LIST_DEVICE), LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES );

		lvc.mask = LVCF_TEXT|LVCF_WIDTH;
		lvc.fmt = LVCFMT_LEFT;
		lvc.cx = 40;
		lvc.pszText = _T("No.");
		ListView_InsertColumn( GetDlgItem(hDlg, IDC_LIST_DEVICE), 0, &lvc );
		lvc.cx = 80;
		lvc.pszText = _T("Model");
		ListView_InsertColumn( GetDlgItem(hDlg, IDC_LIST_DEVICE), 1, &lvc );
		lvc.cx = 250;
		lvc.pszText = _T("Device Path");
		ListView_InsertColumn( GetDlgItem(hDlg, IDC_LIST_DEVICE), 2, &lvc );

		RefreshDeviceList( GetDlgItem(hDlg, IDC_LIST_DEVICE) );

		EnableWindow( GetDlgItem(hDlg, IDC_BTN_PLAY_BUZZER), FALSE );
		EnableWindow( GetDlgItem(hDlg, IDC_BTN_SELECT), FALSE );

		CenterWindow( hDlg );

		return TRUE;
	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
		case IDCANCEL:
			EndDialog(hDlg,IDCANCEL);
			return TRUE;
		case IDC_BTN_PLAY_BUZZER:
			{
				int nSelIdx = ListView_GetSelectionMark(GetDlgItem(hDlg, IDC_LIST_DEVICE));
				if( nSelIdx >= 0 )
				{
					DEVICE_ID* pDevId = (DEVICE_ID*)ListView_GetItemData( GetDlgItem(hDlg, IDC_LIST_DEVICE), nSelIdx );
					memcpy( &g_DevId, pDevId, sizeof(DEVICE_ID) );

					CT30xHandle handle;
					handle.Open( NULL, pDevId->nVID, pDevId->nPID, pDevId->nMI, pDevId->nDeviceIndex );

					handle.SendCommand( "buzzer_play=3,1", TRUE );

					handle.Close();
				}
			}
			return TRUE;
		case IDC_BTN_SELECT:
				{
					int nSelIdx = ListView_GetSelectionMark(GetDlgItem(hDlg, IDC_LIST_DEVICE));
					if( nSelIdx >= 0 )
					{
						DEVICE_ID* pDevId = (DEVICE_ID*)ListView_GetItemData( GetDlgItem(hDlg, IDC_LIST_DEVICE), nSelIdx );
						memcpy( &g_DevId, pDevId, sizeof(DEVICE_ID) );

						EndDialog( hDlg, IDOK );
					}
				}
			return TRUE;
		}
		break;
	case WM_DESTROY:
		{
			HWND hListCtrl = GetDlgItem(hDlg, IDC_LIST_DEVICE);
			for ( int nI=0 ; nI<ListView_GetItemCount(hListCtrl) ; nI++ )
			{
				DEVICE_ID* pDevId = (DEVICE_ID*)ListView_GetItemData( hListCtrl, nI);
				delete pDevId;
			}

			ListView_DeleteAllItems( hListCtrl );
		}
		break;
	case WM_NOTIFY:
		if( wParam == IDC_LIST_DEVICE )
		{
			pNMHDR = (LPNMHDR)lParam;
			switch( pNMHDR->code )
			{
			case NM_DBLCLK:
				{
					int nSelIdx = ListView_GetSelectionMark(GetDlgItem(hDlg, IDC_LIST_DEVICE));
					if( nSelIdx >= 0 )
					{
						DEVICE_ID* pDevId = (DEVICE_ID*)ListView_GetItemData( GetDlgItem(hDlg, IDC_LIST_DEVICE), nSelIdx );
						memcpy( &g_DevId, pDevId, sizeof(DEVICE_ID) );
						EndDialog( hDlg, IDOK );
					}
				}
				break;
			case LVN_ITEMCHANGED:
				{
					LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

					if( pNMLV->uNewState & LVIS_SELECTED )
					{
						EnableWindow( GetDlgItem( hDlg, IDC_BTN_PLAY_BUZZER ), TRUE );
						EnableWindow( GetDlgItem( hDlg, IDC_BTN_SELECT ), TRUE );
					}
					else
					{
						EnableWindow( GetDlgItem( hDlg, IDC_BTN_PLAY_BUZZER ), FALSE );
						EnableWindow( GetDlgItem( hDlg, IDC_BTN_SELECT ), FALSE );
					}
				}
				break;
			}
		}
		break;
	}
	return FALSE;
}


BOOL CHIDCmdThread::OpenT30xHandle()
{
	BOOL bRet = FALSE;

#if 0
	int nDevCnt =	CT30xHandle::EnumDevice( 0xFFFF, 0x0000, 1 ) +
					CT30xHandle::EnumDevice( 0x2200, 0x3000, 1 ) +
					CT30xHandle::EnumDevice( 0x2200, 0x3100, 1 ) +
					// later model
					CT30xHandle::EnumDevice( 0x2200, 0x3200, 1 ) +
					CT30xHandle::EnumDevice( 0x2200, 0x3500, 1 ) +
					CT30xHandle::EnumDevice( 0x2200, 0x3900, 1 ) +
					CT30xHandle::EnumDevice( 0x2200, 0xFF02, 0 );

	if( nDevCnt > 1 )
	{

		int nRet = DialogBox( GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SELECT_DEVICE), NULL, DeviceSelectDlgProc );
		if( nRet == IDCANCEL )
		{
			//exit( 0 );
		}
		else
		{
			return m_pT30xHandle->Open( this, g_szDevPath );
		}
	}
#endif

	do
	{
		int nOldT3000DetectCnt = CT30xHandle::GetDeviceCount( 0xFFFF, 0x0000, 1 );
		if( nOldT3000DetectCnt > 0 )
		{
			bRet = m_pT30xHandle->Open( this, 0xFFFF, 0x0000, 1, 0 );
			if( bRet ) break;
		}
		int nT3000DetectCnt = CT30xHandle::GetDeviceCount( 0x2200, 0x3000, 1 );
		if( nT3000DetectCnt > 0 )
		{
			bRet = m_pT30xHandle->Open( this, 0x2200, 0x3000, 1, 0 );
			if( bRet ) break;
		}
		int nT3100DetectCnt = CT30xHandle::GetDeviceCount( 0x2200, 0x3100, 1 );
		if( nT3100DetectCnt > 0 )
		{
			bRet = m_pT30xHandle->Open( this, 0x2200, 0x3100, 1, 0 );
			if( bRet ) break;
		}
		int nT3200DetectCnt = CT30xHandle::GetDeviceCount( 0x2200, 0x3200, 1 );
		if( nT3200DetectCnt > 0 )
		{
			bRet = m_pT30xHandle->Open( this, 0x2200, 0x3200, 1, 0 );
			if( bRet ) break;
		}
		int nT3500DetectCnt = CT30xHandle::GetDeviceCount( 0x2200, 0x3500, 1 );
		if( nT3500DetectCnt > 0 )
		{
			bRet = m_pT30xHandle->Open( this, 0x2200, 0x3500, 1, 0 );
			if( bRet ) break;
		}
		int nT3900DetectCnt = CT30xHandle::GetDeviceCount( 0x2200, 0x3900, 1 );
		if( nT3900DetectCnt > 0 )
		{
			bRet = m_pT30xHandle->Open( this, 0x2200, 0x3900, 1, 0 );
			if( bRet ) break;
		}
		int nT3kVHIDDetectCnt = CT30xHandle::GetDeviceCount( 0x2200, 0xFF02, 0 );
		if ( nT3kVHIDDetectCnt > 0 )
		{
			bRet = m_pT30xHandle->Open( this, 0x2200, 0xFF02, 0, 0 );
			break;
		}

	} while( FALSE );

	if( bRet )
	{
		m_dwTimeCheck = 0;
	}
	else
	{
		FILETIME f;
		::GetSystemTimeAsFileTime(&f);
		m_dwTimeCheck = (DWORD)((f.dwLowDateTime | (((LONGLONG)f.dwHighDateTime) << 32)) / SYSTIME_PER_MSEC);
	}

	return bRet;
}

void CHIDCmdThread::OnThread()
{
	m_pT30xHandle = new CT30xHandle();

	OpenT30xHandle();

	if ( !m_bIsConnect )
	{
		FlushPreCommand();
	}

	while( TRUE )
	{
		if( ::WaitForSingleObject( m_hExitEvent, 10 ) == WAIT_OBJECT_0 )
		{
			break;
		}

		if( !m_pT30xHandle )
			break;

		HWND hWnd = m_pT30xHandle->GetMsgWndHandle();
		if( hWnd && IsWindow(hWnd) )
		{
			MSG msg;
			while( ::PeekMessage( &msg, hWnd, 0, 0, PM_NOREMOVE ) )
			{
				if( ::GetMessage( &msg, hWnd, 0, 0 ) )
				{
					::TranslateMessage( &msg );
					::DispatchMessage( &msg );
				}
			}
		}

		if( !m_bIsConnect )
		{
			FILETIME f;
			::GetSystemTimeAsFileTime(&f);
			DWORD dwCurTimeCheck = (DWORD)((f.dwLowDateTime | (((LONGLONG)f.dwHighDateTime) << 32)) / SYSTIME_PER_MSEC);

			if( (dwCurTimeCheck - m_dwTimeCheck) > 2000 )		// 2s
			{
				OpenT30xHandle();
			}
		}
	}

	if( m_pT30xHandle )
	{
		m_pT30xHandle->Close();
		delete m_pT30xHandle;
		m_pT30xHandle = NULL;
	}
}

void CHIDCmdThread::OnDeviceConnected(T3K_DEVICE_INFO devInfo)
{
	m_bIsConnect = TRUE;

	WORD nVID = ::T3kGetDevInfoVendorID( devInfo );
	WORD nPID = ::T3kGetDevInfoProductID( devInfo );

	if ( nVID == 0xFFFF && nPID == 0x0000 )
	{
		TextOut(-1, _T("T3000 Open!\r\n"));
	}
	else
	{
		switch( nPID )
		{
		case 0x3000:
			TextOut(-1, _T("T3000 Open!\r\n"));
			break;
		case 0x3100:
			TextOut(-1, _T("T3100 Open!\r\n"));
			break;
		case 0x3200:
			TextOut(-1, _T("T3200 Open!\r\n"));
			break;
		case 0x3500:
			TextOut(-1, _T("T3k A Open!\r\n"));
			break;
		case 0x3900:
			TextOut(-1, _T("T3900 Open!\r\n"));
			break;
		default:
			TextOut(-1, _T("T3k series Open!\r\n"));
			break;
		}
	}

	m_bInstantMode = FALSE;
	m_pT30xHandle->SetInstantMode(T3K_HID_MODE_COMMAND, 5000, 0);
}

void CHIDCmdThread::OnDeviceDisconnected(T3K_DEVICE_INFO devInfo)
{
	WORD nVID = ::T3kGetDevInfoVendorID( devInfo );
	WORD nPID = ::T3kGetDevInfoProductID( devInfo );

	if ( nVID == 0xFFFF && nPID == 0x0000 )
	{
		TextOut(-1, _T("T3000 Close!\r\n"));
	}
	else
	{
		switch( nPID )
		{
		case 0x3000:
			TextOut(-1, _T("T3000 Close!\r\n"));
			break;
		case 0x3100:
			TextOut(-1, _T("T3100 Close!\r\n"));
			break;
		case 0x3200:
			TextOut(-1, _T("T3200 Close!\r\n"));
			break;
		case 0x3500:
			TextOut(-1, _T("T3k A Close!\r\n"));
			break;
		case 0x3900:
			TextOut(-1, _T("T3900 Close!\r\n"));
			break;
		default:
			TextOut(-1, _T("T3k series Close!\r\n"));
			break;
		}
	}
}


void CHIDCmdThread::AddPrompt( Prompt p )
{
	for ( int ni = 0; ni < PROMPT_MAX; ni++ )
	{
		if ( m_prompt[ni] == promptNil )
		{
			m_prompt[ni] = p;
			break;
		}
	}
}


void CHIDCmdThread::AddPreCommand( const char * szCmd )
{
	char ** ppPreCommands = new char *[m_nPreCommands + 1];
	if ( m_ppPreCommands )
	{
		memcpy(ppPreCommands, m_ppPreCommands, m_nPreCommands * sizeof(char *));
		delete [] m_ppPreCommands;
	}
	m_ppPreCommands = ppPreCommands;

	m_ppPreCommands[m_nPreCommands] = new char[strlen(szCmd) + 1];
	strcpy(m_ppPreCommands[m_nPreCommands], szCmd);
	m_nPreCommands++;
}


BOOL CHIDCmdThread::SendCommand( char * szCmd )
{
	if( !IsHIDConnect() )
	{
		TextOutRuntime(cszIsNotConnected, 0);
		return FALSE;
	}

	ASSERT( m_pT30xHandle );

	if ( strstr(szCmd, cszInstantMode) == szCmd )
	{
        // instant_mode=***, time -> 5000...
		char szBuffer[1024] = { 0 };
		szCmd += sizeof(cszInstantMode) - 1;
		int len = (int)strlen(szCmd);
		for ( int ni = 0; ni < len; ni++ )
		{
			if ( isspace(*szCmd) )
				szCmd++;
			else
				break;
		}
		char * pD = strchr(szCmd, ',');
		if ( pD == NULL )
			pD = szCmd + strlen(szCmd);
		strcpy(szBuffer, cszInstantMode);
		BOOL bCmd = FALSE;
		BOOL bQuery = FALSE;
		while ( szCmd < pD )
		{
			if ( *szCmd == 'c' || *szCmd == 'C' )
				bCmd = TRUE;
			if ( *szCmd == '*' || *szCmd == '!' || *szCmd == '?' )
			{
				bQuery = TRUE;
			}
			szBuffer[strlen(szBuffer)] = *szCmd++;
		}
		if ( !bQuery && !bCmd )
		{
			szBuffer[strlen(szBuffer)] = 'C';
		}
		strcat(szBuffer, pD);
		szCmd = szBuffer;

		m_bInstantMode = TRUE;
	}

	m_tmStart = GetCurrentSystime();
	m_pT30xHandle->SendCommand(szCmd);

	return TRUE;
}


void CHIDCmdThread::LockTextOut()
{
	::EnterCriticalSection( &m_csTextOut );
	m_bTextOut = FALSE;
	::LeaveCriticalSection( &m_csTextOut );
	TRACE("LockTextOut\r\n");
}

void CHIDCmdThread::UnlockTextOut()
{
	//@@ lock textout

	TRACE("UnlockTextOut\r\n");
	::EnterCriticalSection( &m_csTextOut );
	m_bTextOut = TRUE;
	::LeaveCriticalSection( &m_csTextOut );
}

void CHIDCmdThread::TextOut( DWORD ticktime, const char * szFormat, ... )
{
	va_list varpars;
	va_start(varpars, szFormat);

	char szCommand[1024];
	vsprintf(szCommand, szFormat, varpars);

	::EnterCriticalSection( &m_csTextOut );

	if ( m_bTextOut )
	{
		TextOutRuntime(szCommand, -1, ticktime);
	}
	else
	{
		//@@ lock textout
		szCommand;
	}

	::LeaveCriticalSection( &m_csTextOut );

	va_end(varpars);
}

void CHIDCmdThread::TextOutRuntime( const char * szCmd, UINT_PTR time, DWORD ticktime )
{
	if ( time == -1 )
	{
		time = GetCurrentSystime();
	}

	if ( time == 0 )
	{
		printf(szCmd);
	}
	else
	{
		char szOut[2048] = { 0 };
		char * pCur = szOut;

		// system time
		time -= m_tmStart;
		int sys_msec = (int)(time % 1000);
		int sys_sec = (int)((time / 1000) % 1000);

		// t3000 time
		int t_msec = (int)(ticktime % 1000);
		int t_sec = (int)((ticktime / 1000) % 1000);

		for ( int ni = 0; ni < PROMPT_MAX; ni++ )
		{
			switch ( m_prompt[ni] )
			{
			default:
			case promptNil:
				ni = PROMPT_MAX;
				break;
			case promptSystemTime:
				pCur += sprintf(pCur, "%03d.%03d ", sys_sec, sys_msec);
				break;
			case promptT3000Time:
				if ( ticktime != -1 )
					pCur += sprintf(pCur, "%03d.%03d ", t_sec, t_msec);
				else
				{
					strcat(pCur, "---.--- ");
					pCur += strlen(pCur);
				}
				break;
			}
		}

		strcat(pCur, szCmd);

		printf(szOut);
	}
}

void CHIDCmdThread::TPDP_OnConnected( T3K_DEVICE_INFO devInfo )
{
	OnDeviceConnected( devInfo );
}

void CHIDCmdThread::TPDP_OnDisconnected( T3K_DEVICE_INFO devInfo )
{
	OnDeviceDisconnected( devInfo );
	m_pT30xHandle->Close();
	m_bIsConnect = FALSE;
}

void CHIDCmdThread::TPDP_OnMSG( T3K_DEVICE_INFO devInfo, WORD ticktime, LPCSTR partid, const char * txt )
{
	TextOut(ticktime, "%s: %s\r\n", partid, txt);
}

void CHIDCmdThread::TPDP_OnVER( T3K_DEVICE_INFO devInfo, WORD ticktime, LPCSTR partid, t3kpacket::_body::_ver* ver )
{
	char szVersion[128] = { 0, };
	char date[T3K_VER_DATE_LEN + 1];
	char time[T3K_VER_TIME_LEN + 1];

	memcpy(date, ver->date, T3K_VER_DATE_LEN); date[T3K_VER_DATE_LEN] = 0;
	memcpy(time, ver->time, T3K_VER_TIME_LEN); time[T3K_VER_TIME_LEN] = 0;

	if ( ver->nv == 0 && ver->major == 0 && ver->minor == 0 )	// IAP
	{
		if ( partid[3] == 0 )
		{
			sprintf(szVersion, "Model: %X (IAP)", ver->model);
		}
		else
			sprintf(szVersion, "Model: T%X (IAP)", ver->model);
	}
	else
	{
		if ( partid[3] == 0 )
		{
			sprintf(szVersion, "NV: %d, Ver: %X.%X, Model: %X  %s %s", ver->nv, ver->major, ver->minor, ver->model, date, time);
		}
		else
			sprintf(szVersion, "NV: %d, Ver: %X.%X, Model: T%X  %s %s", ver->nv, ver->major, ver->minor, ver->model, date, time);
	}
	TextOut(ticktime, "%s: %s\r\n", partid, szVersion);
}

void CHIDCmdThread::TPDP_OnSTT( T3K_DEVICE_INFO devInfo, WORD ticktime, LPCSTR partid, const char * status )
{
	TextOut(ticktime, "%s: %s\r\n", partid, status);
}

void CHIDCmdThread::TPDP_OnRSP( T3K_DEVICE_INFO devInfo, WORD ticktime, LPCSTR partid, int id, BOOL bFinal, const char * cmd )
{
	BOOL bSend = FALSE;

	if ( strstr(cmd, cszInstantMode) == cmd )
	{
		if ( strcmp( m_szInstantMode, cmd ) != 0 )
		{
			strncpy( m_szInstantMode, cmd, 100 );
			if ( !m_bInstantMode )
			{
				TextOut(ticktime, "%s: %s\r\n", partid, cmd);
			}
		}
		if ( m_bInstantMode )
		{
			TextOut(ticktime, "%s: %s\r\n", partid, cmd);
			m_bInstantMode = FALSE;
		}
	}
	else
	{
		char szBuf[2048];

		if ( m_eGetNv > enFalse )
		{
			if ( m_nGetNvId != id )
				return;
			if ( strstr( cmd, "no cam" ) == cmd || strstr( cmd, "syntax error" ) == cmd )
			{
				bSend = GetNv_SendNext( TRUE );
			}
			else
			{
				if ( cmd[0] != 0 )
				{
					if ( m_nNvIdx < 0 )
						TextOut(ticktime, "%s: [%s]\r\n", partid, cmd);
					else
						TextOut(ticktime, "%s: %s\r\n", partid, cmd);

					if ( m_pFileGetNv )
					{
						if ( m_nNvIdx < 0 )
						{
							int nPartIdIdx = (int)strlen( partid )-1;

							if( partid[nPartIdIdx-1] == '-' )
							{
								nPartIdIdx -= 2;
							}
							int nCamSub = 0;

							switch( partid[nPartIdIdx] )
							{
							default:
							case 0:
								sprintf(szBuf, "[%s]\r\n", cmd);
								break;
							case '1':
								nCamSub = (int)(partid[nPartIdIdx + 2]-'0');
								if ( nCamSub > 0 )
									sprintf(szBuf, "[cam1/sub/%s]\r\n", cmd);
								else
									sprintf(szBuf, "[cam1/%s]\r\n", cmd);
								break;
							case '2':
								nCamSub = (int)(partid[nPartIdIdx + 2]-'0');
								if ( nCamSub > 0 )
									sprintf(szBuf, "[cam2/sub/%s]\r\n", cmd);
								else
									sprintf(szBuf, "[cam2/%s]\r\n", cmd);
								break;
							}
							fwrite(szBuf, sizeof(char), strlen(szBuf), m_pFileGetNv);
						}
						else
						{
							int nPartIdIdx = (int)strlen( partid )-1;

							if( partid[nPartIdIdx-1] == '-' )
							{
								nPartIdIdx -= 2;
							}
							int nCamSub = 0;

							switch( partid[nPartIdIdx] )
							{
							default:
							case 0:
								sprintf(szBuf, "%s\r\n", cmd);
								break;
							case '1':
								nCamSub = (int)(partid[nPartIdIdx + 2]-'0');
								if ( nCamSub > 0 )
									sprintf(szBuf, "cam1/sub/%s\r\n", cmd);
								else
									sprintf(szBuf, "cam1/%s\r\n", cmd);
								break;
							case '2':
								nCamSub = (int)(partid[nPartIdIdx + 2]-'0');
								if ( nCamSub > 0 )
									sprintf(szBuf, "cam2/sub/%s\r\n", cmd);
								else
									sprintf(szBuf, "cam2/%s\r\n", cmd);
								break;
							}
							fwrite(szBuf, sizeof(char), strlen(szBuf), m_pFileGetNv);
						}
					}
				}

				bSend = GetNv_SendNext(cmd[0] == 0);
			}
		}
		else if ( m_pFileGetNv )
		{
			switch ( partid[3] )
			{
			default:
			case 0:
				sprintf(szBuf, ";%s\r\n", cmd);
				break;
			case '1':
				sprintf(szBuf, ";cam1/%s\r\n", cmd);
				break;
			case '2':
				sprintf(szBuf, ";cam2/%s\r\n", cmd);
				break;
			}
			int strn = strlen(szBuf);
			TRACE( _T("[0x%02x][0x%02x]\r\n"), szBuf[strn-2], szBuf[strn-1] );
			fwrite(szBuf, sizeof(char), strlen(szBuf), m_pFileGetNv);
		}
		else
		{
			TextOut(ticktime, "%s: %s\r\n", partid, cmd);
		}
	}

	if ( !bSend )
	{
		FlushPreCommand();
	}
}

void CHIDCmdThread::FlushPreCommand()
{
	BOOL bSysCmd = FALSE;
	while ( m_nPreCommands > 0 )
	{
		BOOL bCmd = OnCommand(m_ppPreCommands[0], &bSysCmd);

		m_nPreCommands--;
		delete [] m_ppPreCommands[0];
		if ( m_nPreCommands == 0 )
		{
			delete [] m_ppPreCommands;
			m_ppPreCommands = NULL;
		}
		else
		{
			char ** ppPreCommands = new char *[m_nPreCommands];
			memcpy(ppPreCommands, m_ppPreCommands + 1, m_nPreCommands * sizeof(char *));
			delete [] m_ppPreCommands;
			m_ppPreCommands = ppPreCommands;
		}

		if ( !bCmd )
		{
			HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
			INPUT_RECORD ir;
			ir.EventType = KEY_EVENT;
			ir.Event.KeyEvent.bKeyDown = 1;
			ir.Event.KeyEvent.wRepeatCount = 1;
			ir.Event.KeyEvent.wVirtualKeyCode = VK_SPACE;
			ir.Event.KeyEvent.wVirtualScanCode = 0x39;
			ir.Event.KeyEvent.uChar.AsciiChar = ' ';
			ir.Event.KeyEvent.dwControlKeyState = 0x0;
			DWORD dwWrite;
			WriteConsoleInput(hIn, &ir, 1, &dwWrite);
			return;
		}
		if ( !bSysCmd )
			break;
	}
}


void CHIDCmdThread::GetNv( BOOL bFactory, const char * szFile )
{
	m_eGetNv = enFalse;
	if ( !m_bIsConnect )
	{
		TextOutRuntime(cszIsNotConnected, 0);
		return;
	}

	TextOutRuntime(cstrGetNv, 0);

	if ( szFile )
	{
		if ( m_pFileGetNv )
		{
			fclose(m_pFileGetNv);
		}
		m_pFileGetNv = fopen(szFile, "wb");
	}

	int nMode;
#if 1
	if ( m_pT30xHandle->QueryFirmwareVersion(PKT_ADDR_MM, &nMode) )
	{
		if ( nMode == MODE_MM_APP )
		{
			if ( m_pT30xHandle->QueryFirmwareVersion(PKT_ADDR_CM1, &nMode) )
			{
				//if ( nMode == MODE_CM_APP )
				m_pT30xHandle->QueryFirmwareVersion(PKT_ADDR_CM1 | PKT_ADDR_CM_SUB, &nMode);
			}
			if ( m_pT30xHandle->QueryFirmwareVersion(PKT_ADDR_CM2, &nMode) )
			{
				//if ( nMode == MODE_CM_APP )
				m_pT30xHandle->QueryFirmwareVersion(PKT_ADDR_CM2 | PKT_ADDR_CM_SUB, &nMode);
			}
		}
	}
#else
	m_pT30xHandle->SendCommand("firmware_version=?");
	m_pT30xHandle->SendCommand("cam1/firmware_version=?");
	m_pT30xHandle->SendCommand("cam1/sub/firmware_version=?");
	m_pT30xHandle->SendCommand("cam2/firmware_version=?");
	m_pT30xHandle->SendCommand("cam2/sub/firmware_version=?");
#endif

	// 
	m_bFactorialNv = bFactory;
	if ( m_bFactorialNv )
		m_nNvIdx = -1;
	else
		m_nNvIdx = 0;
	m_eGetNv = enCM1;
	m_tmStart = GetCurrentSystime();

	GetNv_SendNext(FALSE);
}

BOOL CHIDCmdThread::GetNv_SendNext( BOOL bEndOfGetNv )
{
	if ( m_eGetNv == enFalse )
		return FALSE;

	if ( bEndOfGetNv )
	{
		if ( m_nNvIdx < 0 )
			m_nNvIdx = 0;
		else
		{
			m_eGetNv = (tagGetNv)(m_eGetNv - 1);
			if ( m_eGetNv == enFalse )
			{
				if ( m_pFileGetNv )
				{
					fclose(m_pFileGetNv);
					m_pFileGetNv = NULL;
				}

				TextOutRuntime(cszGetNv_End, 0);
				return FALSE;
			}
			else
			{
				if ( m_bFactorialNv )
					m_nNvIdx = -1;
				else
					m_nNvIdx = 0;
			}
		}
	}

	char szCmd[80];
	switch ( m_eGetNv )
	{
	case enCM1:
		sprintf(szCmd, "cam1/get_nv=%d", m_nNvIdx);
		break;

	case enCM1c:
		sprintf(szCmd, "cam1/sub/get_nv=%d", m_nNvIdx);
		break;

	case enCM2:
		sprintf(szCmd, "cam2/get_nv=%d", m_nNvIdx);
		break;

	case enCM2c:
		sprintf(szCmd, "cam2/sub/get_nv=%d", m_nNvIdx);
		break;

	case enMM:
	default:
		sprintf(szCmd, "get_nv=%d", m_nNvIdx);
		break;
	}
	if ( m_nNvIdx < 0 )
		m_nNvIdx--;
	else
		m_nNvIdx++;

	int nId = m_pT30xHandle->SendCommand(szCmd, TRUE);
	m_nGetNvId = nId;
	//TRACE( "Send cmd[%d]: %s\r\n", nId, szCmd );

	return TRUE;
}
