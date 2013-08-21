// DSelectModel.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "T3kSoftlogic.h"
#include "DSelectModel.h"

#include "T3kSoftlogicDlg.h"


// DSelectModel 대화 상자입니다.

IMPLEMENT_DYNAMIC(DSelectModel, CDialog)
DSelectModel::DSelectModel(CWnd* pParent /*=NULL*/)
	: CDialog(DSelectModel::IDD, pParent)
{
}

DSelectModel::~DSelectModel()
{
}

void DSelectModel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MODEL, m_lstModelName);
}


BEGIN_MESSAGE_MAP(DSelectModel, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_DELETE, OnBnClickedBtnDelete)
	ON_LBN_DBLCLK(IDC_LIST_MODEL, &DSelectModel::OnLbnDblclkListModel)
	ON_LBN_SELCHANGE(IDC_LIST_MODEL, &DSelectModel::OnLbnSelchangeListModel)
END_MESSAGE_MAP()


// DSelectModel 메시지 처리기입니다.

void DSelectModel::OnBnClickedOk()
{
	int nSelect = m_lstModelName.GetCurSel();
	if( nSelect < 0 )
	{
		::MessageBox( GetSafeHwnd(), _T("Select Model Name!"), _T("Error"), MB_OK|MB_ICONERROR );
		return;
	}

	CString strModelName;
	m_lstModelName.GetText( nSelect, strModelName );

	CString strDataPath = GetExecuteDirectory();
	strDataPath += _T("\\SoftlogicData");

	m_strPanelPathName.Format( _T("%s\\%s.hsk"), strDataPath, strModelName );

	OnOK();
}

void DSelectModel::OnBnClickedCancel()
{
	m_strPanelPathName.Empty();

	OnCancel();
}

void DSelectModel::OnBnClickedBtnDelete()
{
	int nCurSel = m_lstModelName.GetCurSel();
	if( nCurSel < 0 ) return;

	CString strItem;
	m_lstModelName.GetText( nCurSel, strItem );

	CString strMessage;
	strMessage.Format( _T("[%s] Item will be deleted! Are you sure?"), strItem );
	if( ::MessageBox( GetSafeHwnd(), strMessage, _T("Warning"), MB_YESNO|MB_ICONQUESTION ) == IDYES )
	{
		CT3kSoftlogicDlg* pDlg = (CT3kSoftlogicDlg*)AfxGetMainWnd();
		CSoftkeyArray& Keys = pDlg->GetKeys();
		if( strItem == Keys.GetPanelName() )
		{
			pDlg->OnBnClickedBtnNewModel();
		}

		CString strDataPath = GetExecuteDirectory();
		strDataPath += _T("\\SoftlogicData");

		CString strPanelPathName;
		strPanelPathName.Format( _T("%s\\%s.hsk"), strDataPath, strItem );
		DeleteFile( strPanelPathName );

		RefreshList();
	}
}

void DSelectModel::RefreshList()
{
	m_strPanelPathName.Empty();

	CString strDataPath = GetExecuteDirectory();
	strDataPath += _T("\\SoftlogicData");

	CString strFilter;
	strFilter.Format( _T("%s\\*.hsk"), strDataPath );

	m_lstModelName.ResetContent();

	int nSP;
	CFileFind  ff;
	if( ff.FindFile( strFilter ) )
	{
		BOOL bFind = TRUE;
		CString strModelName;
		while( bFind )
		{
			bFind = ff.FindNextFile();
			strModelName = ff.GetFileName();
			nSP = strModelName.Find( _T('.') );
			if( nSP >= 0 )
			{
				strModelName = strModelName.Left( nSP );
			}
			m_lstModelName.AddString( strModelName );
		}

		ff.Close();
	}
}

BOOL DSelectModel::OnInitDialog()
{
	CDialog::OnInitDialog();

	CFont* pFont = GetFont();
	LOGFONT logFont;
	pFont->GetLogFont( &logFont );

	logFont.lfHeight *= 2;
	logFont.lfQuality = ANTIALIASED_QUALITY;

	VERIFY( m_fntList.CreateFontIndirect(&logFont) );

	m_lstModelName.SetFont( &m_fntList );

	RefreshList();

	GetDlgItem(IDOK)->EnableWindow( FALSE );
	GetDlgItem(IDC_BTN_DELETE)->EnableWindow( FALSE );

	return TRUE;
}

LPCTSTR DSelectModel::GetPanelPathName()
{
	return m_strPanelPathName;
}

void DSelectModel::OnLbnDblclkListModel()
{
	OnBnClickedOk();
}

void DSelectModel::OnLbnSelchangeListModel()
{
	GetDlgItem(IDOK)->EnableWindow( TRUE );
	GetDlgItem(IDC_BTN_DELETE)->EnableWindow( TRUE );
}
