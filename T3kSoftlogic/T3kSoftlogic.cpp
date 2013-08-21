// T3kSoftlogic.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include ".\T3kSoftlogic.h"
#include "T3kSoftlogicDlg.h"

#include "GdiplusInitializer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CT3kSoftlogicApp

BEGIN_MESSAGE_MAP(CT3kSoftlogicApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CT3kSoftlogicApp construction

CT3kSoftlogicApp::CT3kSoftlogicApp() : m_bScreenShotMode(FALSE)
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CT3kSoftlogicApp object

CT3kSoftlogicApp theApp;


// CT3kSoftlogicApp initialization

BOOL CT3kSoftlogicApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.

	if( !_GdiPlusInitializer.IsInitialize() )
	{
		::MessageBox( NULL, _T("Cannot initialize gdiplus\n"), _T("Error"), MB_OK );
		return FALSE;
	}

	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Habilience"));

	#define T3K_SOFTLOGIC_DIALOG_CLASSNAME			"Habilience T3k Softlogic Dialog"

	int nActiveTab = -1;
	CString strExecute;
	strExecute.Empty();
	CString strCmdLine = m_lpCmdLine;
	if( strCmdLine.Find( _T("/e") ) < 0 )
	{
		HWND hPrevAppWnd = ::FindWindow( _T(T3K_SOFTLOGIC_DIALOG_CLASSNAME), NULL );
		if( hPrevAppWnd )
		{
			COPYDATASTRUCT cds;
			cds.dwData = m_nCmdShow;
			cds.cbData = (DWORD)(_tcslen(m_lpCmdLine) + 1) * sizeof(TCHAR);
			cds.lpData = m_lpCmdLine;
			::SendMessage(hPrevAppWnd, WM_COPYDATA, 0, (LPARAM)&cds);

			::SetForegroundWindow( hPrevAppWnd );
			return FALSE;
		}

		if ( strCmdLine.Find( _T("/screenshot") ) >= 0 )
		{
			m_bScreenShotMode = TRUE;
		}
	}
	else
	{
		int nPos;
		if ( (nPos=strCmdLine.Find( _T("/tab") )) >= 0 )
		{
			if ( nPos+4+1+1 <= strCmdLine.GetLength() )
			{
				CString strT = strCmdLine.Mid( nPos+4+1, 1 );
				nActiveTab = _ttoi(strT);
			}
		}

		if ( (nPos=strCmdLine.Find( _T("/exe") )) >= 0 )
		{
			if ( nPos+4+1+1 <= strCmdLine.GetLength() )
			{
				int nE = strCmdLine.Find( _T(' '), nPos+4+1 );
				if ( nE >= 0 )
				{
					CString strT = strCmdLine.Mid( nPos+4+1, nE-(nPos+4+1) );
					strExecute = strT;
				}
				else
				{
					CString strT = strCmdLine.Mid( nPos+4+1, strCmdLine.GetLength()-(nPos+4+1) );
					strExecute = strT;
				}
			}
		}

		if ( strCmdLine.Find( _T("/screenshot") ) >= 0 )
		{
			m_bScreenShotMode = TRUE;
		}
	}

/*
	CString strTemp;
	LONG lSize = _MAX_PATH * 2;
	LONG lResult = ::RegQueryValue(HKEY_CLASSES_ROOT, _T(".brd"),
	strTemp.GetBuffer(lSize), &lSize);

	if( lResult != ERROR_SUCCESS && strTemp.IsEmpty() )
	{
		RegisterExt();
	}
*/

	WNDCLASS wc = {0};
	wc.style = CS_BYTEALIGNWINDOW|CS_SAVEBITS|CS_DBLCLKS;
	wc.lpfnWndProc = DefDlgProc;
	wc.cbWndExtra = DLGWINDOWEXTRA;
	wc.hInstance = m_hInstance;
	wc.hIcon = LoadIcon(IDR_MAINFRAME);
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = CreateSolidBrush( GetSysColor(COLOR_BTNFACE) );
	wc.lpszClassName = _T(T3K_SOFTLOGIC_DIALOG_CLASSNAME);

	ATOM cls = RegisterClass(&wc);

	CT3kSoftlogicDlg dlg;
	m_pMainWnd = &dlg;

	if( !strCmdLine.IsEmpty() )
	{
		strCmdLine.Remove( _T('\"') );
		dlg.SetDataFile( strCmdLine );
	}

	if ( !strExecute.IsEmpty() )
	{
		dlg.DoExecute( strExecute );
		return FALSE;
	}

	dlg.SetActiveTab( nActiveTab );

	dlg.DoModal();

	return FALSE;
}
/*
BOOL CT3kSoftlogicApp::IsOS64()
{
#if defined(_WIN64)
	return TRUE;
#elif defined(_WIN32)
	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

    LPFN_ISWOW64PROCESS 
    IsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress( GetModuleHandle( _T("kernel32") ), "IsWow64Process" );

	BOOL b64 = FALSE;
	return IsWow64Process( GetCurrentProcess(), &b64 ) && b64;
#else
	return FALSE;
#endif
}

#include <afxole.h>
void CT3kSoftlogicApp::RegisterExt()
{
	UINT nID = 0;

	if( IsOS64() )
		nID = IDR_EXE_REGEXT_X64;
	else
		nID = IDR_EXE_REGEXT;

	HRSRC hRsrc = ::FindResource( AfxGetResourceHandle(), MAKEINTRESOURCE(nID), _T("EXE") );
	if( !hRsrc )
		return FALSE;
	
	HGLOBAL hRes = ::LoadResource( AfxGetInstanceHandle(), hRsrc );
	if( !hRes )	
		return FALSE;

	LPSTR lpRes = (LPSTR)::LockResource( hRes );
	int nSize = ::SizeofResource( ::AfxGetInstanceHandle(), hRsrc );
	if( !nSize )
	{
		::UnlockResource( hRes );
		::FreeResource( hRes );
		return FALSE;
	}

	HGLOBAL hGlobal = ::GlobalAlloc( GMEM_MOVEABLE, nSize );
	if( !hGlobal ) 
	{
		::UnlockResource( hRes );
		::FreeResource( hRes );
		return FALSE;
	}

	LPVOID pvData = ::GlobalLock( hGlobal );
	CopyMemory( pvData, lpRes, nSize );
	::GlobalUnlock( hGlobal );	

	COleStreamFile StreamFile;
	LPSTREAM lpStream;
	TCHAR* szBuff = new TCHAR[nSize];
	HRESULT hr = ::CreateStreamOnHGlobal( hGlobal, FALSE, &lpStream );
	if( hr != S_OK )
	{
		delete[] szBuff;
		::GlobalFree( hGlobal );		
		return FALSE;
	}

	StreamFile.Attach( lpStream );
	StreamFile.Read( szBuff, nSize );

	TCHAR* szTempPath = new TCHAR[MAX_PATH];
	ZeroMemory( szTempPath, sizeof(szTempPath) );
	::GetTempPath( MAX_PATH, szTempPath );
	CString strFileName;
	strFileName.Format( _T("%sT3kSoftlogicRegExt.exe"), szTempPath );
	delete[] szTempPath;
	
	CFile ExeFile;
	if( !ExeFile.Open( strFileName, CFile::modeCreate | CFile::modeWrite ) )
	{
		delete[] szBuff;
		::GlobalFree( hGlobal );		
		return FALSE;
	}

	ExeFile.Write( szBuff, nSize );
	ExeFile.Close();

	delete[] szBuff;
	::GlobalFree( hGlobal );
	int n = ::UnlockResource( hRes );
	BOOL b = ::FreeResource( hRes );

	CString strParam;
	strParam.Format( _T("%s %s"), lpszRegKeyValue, lpszRegValue );
	SHELLEXECUTEINFO SEInfo;

	SEInfo.cbSize = sizeof(SEInfo);
	SEInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_DDEWAIT;
    SEInfo.hwnd = NULL;
	SEInfo.lpVerb = _T("open");
    SEInfo.lpFile = strFileName;
	SEInfo.lpParameters = strParam;
	SEInfo.lpDirectory = NULL;
    SEInfo.nShow = SW_HIDE;
	SEInfo.hInstApp = NULL;
	
	::ShellExecuteEx( &SEInfo );

	DWORD dwExitCode = 0;
	BOOL bDone = FALSE;
	MSG msg;
	while( !bDone )
	{
		::GetExitCodeProcess( SEInfo.hProcess, &dwExitCode );
		if( dwExitCode != STILL_ACTIVE )
			bDone = TRUE;
		else
		{
			Sleep(1);
			if( ::PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
			{
				::TranslateMessage( &msg );
				::DispatchMessage( &msg );
			}
		}
	}

	if( !::DeleteFile( strFileName ) )
		return FALSE;

	return TRUE;
}
*/
BOOL CT3kSoftlogicApp::PreTranslateMessage(MSG* pMsg)
{
	if( (pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_ESCAPE) )
	{
		TRACE( _T("ESCAPE\r\n") );
	}

	return CWinApp::PreTranslateMessage(pMsg);
}