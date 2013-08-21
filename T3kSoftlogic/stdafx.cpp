// stdafx.cpp : source file that includes just the standard includes
// T30xSoftKey.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"


CString GetExecuteDirectory( void )
{
	TCHAR spBuf[_MAX_PATH];
	::GetModuleFileName(NULL, spBuf, sizeof(spBuf));

	CString strFullPath( spBuf ); 

	int nBackSlashPos = strFullPath.ReverseFind( _T('\\') );

	CString strPath = strFullPath.Left( nBackSlashPos );

	return strPath;
}

void MakeDirectory( LPCTSTR lpszDir )
{
	::SetLastError( 0 );
	CString strDir = lpszDir;
    if( strDir.Right(1) == _T("\\") )
		strDir = strDir.Left(strDir.GetLength()-1); 
	if( ::GetFileAttributes(strDir) != -1 )
		return;

	DWORD dwErr = ::GetLastError();
	if( !((dwErr == ERROR_PATH_NOT_FOUND) || (dwErr == ERROR_FILE_NOT_FOUND)) )
	{
		::SetLastError( dwErr );
		return;
	}

    int nFound = strDir.ReverseFind( _T('\\') );
    MakeDirectory( strDir.Left(nFound) );
    ::CreateDirectory( strDir, NULL );
}

BOOL CALLBACK MonitorEnumProc( HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData )
{
	CRect & rcCaption = *(CRect *)((void **)dwData)[0];

	MONITORINFO mi;
	memset(&mi, 0, sizeof(MONITORINFO));
	mi.cbSize = sizeof(MONITORINFO);
	::GetMonitorInfo(hMonitor, &mi);

	CRect rc;
	BOOL bRet = ::IntersectRect(&rc, &rcCaption, &mi.rcWork);
	((BOOL *)dwData)[1] |= bRet;

	return !bRet;
}

#pragma comment(lib, "Version.lib")
#include <memory>
CString GetProgVersion( void )
{
	CString version;

	TCHAR pModuleFileName[512];
	GetModuleFileName( NULL, pModuleFileName, 512 );
	CString strExe = (LPCTSTR)pModuleFileName;

	DWORD zero = 0;

	DWORD verSize = ::GetFileVersionInfoSize(strExe, &zero);

	if (verSize != 0)
	{
		std::auto_ptr<BYTE> spBuffer(new BYTE[verSize]);

		if (::GetFileVersionInfo(strExe, 0, verSize, spBuffer.get()))
		{
			LPTSTR pVersion = 0;
			UINT verLen = 0;

			CString strVerQuery;
			if ( ::VerQueryValue(spBuffer.get(), 
				const_cast<LPTSTR>(_T("\\VarFileInfo\\Translation")), 
				(LPVOID *)&pVersion, 
				&verLen) )
			{
				strVerQuery.Format( _T("\\StringFileInfo\\%04x%04x\\FileVersion"), LOWORD(*((UINT*)pVersion)), HIWORD(*((UINT*)pVersion)) );
			}
				
			if ( ::VerQueryValue(spBuffer.get(), 
				const_cast<LPTSTR>((LPCTSTR)strVerQuery), 
				(LPVOID *)&pVersion, 
				&verLen) )
			{
				version = pVersion;
			}
		}
	}

	return version;
}