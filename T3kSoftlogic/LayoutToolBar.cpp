// LayoutToolBar.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "T3kSoftlogic.h"
#include "LayoutToolBar.h"

#include "DesignCanvasWnd.h"
#include ".\layouttoolbar.h"

// DLayoutToolBar 대화 상자입니다.

IMPLEMENT_DYNAMIC(DLayoutToolBar, CDialog)
DLayoutToolBar::DLayoutToolBar(CWnd* pParent /*=NULL*/)
	: CDialog(DLayoutToolBar::IDD, pParent)
{
	m_pWndDesignCanvas = NULL;
	m_pWndDesignTool = NULL;
	m_eUnit = UnitRes;
}

DLayoutToolBar::~DLayoutToolBar()
{
}

void DLayoutToolBar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DLayoutToolBar, CDialog)
	ON_BN_CLICKED(IDC_BTN_ALIGN_LEFT, OnBnClickedBtnAlignLeft)
	ON_BN_CLICKED(IDC_BTN_ALIGN_CENTER, OnBnClickedBtnAlignCenter)
	ON_BN_CLICKED(IDC_BTN_ALIGN_RIGHT, OnBnClickedBtnAlignRight)
	ON_BN_CLICKED(IDC_BTN_ALIGN_TOP, OnBnClickedBtnAlignTop)
	ON_BN_CLICKED(IDC_BTN_ALIGN_MIDDLE, OnBnClickedBtnAlignMiddle)
	ON_BN_CLICKED(IDC_BTN_ALIGN_BOTTOM, OnBnClickedBtnAlignBottom)
	ON_BN_CLICKED(IDC_BTN_SAME_SIZE_WIDTH, OnBnClickedBtnSameSizeWidth)
	ON_BN_CLICKED(IDC_BTN_SAME_SIZE_HEIGHT, OnBnClickedBtnSameSizeHeight)
	ON_BN_CLICKED(IDC_BTN_SAME_SIZE_BOTH, OnBnClickedBtnSameSizeBoth)
	ON_BN_CLICKED(IDC_BTN_DISTRIB_HORZ_EQUAL_GAP, OnBnClickedBtnDistribHorzEqualGap)
	ON_BN_CLICKED(IDC_BTN_DISTRIB_VERT_EQUAL_GAP, OnBnClickedBtnDistribVertEqualGap)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_HELPER_REORDER, &DLayoutToolBar::OnBnClickedBtnHelperReorder)
	ON_BN_CLICKED(IDC_BTN_HELPER_ARRANGE, &DLayoutToolBar::OnBnClickedBtnHelperArrange)
END_MESSAGE_MAP()

BOOL DLayoutToolBar::Create(CDesignCanvasWnd* pDesignCanvasWnd, DSoftKeyDesignTool* pDesignTool)
{
	m_pWndDesignCanvas = pDesignCanvasWnd;
	m_pWndDesignTool = pDesignTool;

	ASSERT( m_pWndDesignCanvas && m_pWndDesignTool );

	return CDialog::Create(IDD, pDesignCanvasWnd);
}

// DLayoutToolBar 메시지 처리기입니다.
static BOOL s_bEnableLayout = TRUE;
void DLayoutToolBar::UpdateUIButtonState( int nSelectKeyCount )
{
	CWnd* pWnd;

	BOOL bEnableLayout;
	if( nSelectKeyCount > 1 )
		bEnableLayout = TRUE;
	else
		bEnableLayout = FALSE;

	//if( s_bEnableLayout != bEnableLayout )
	{
		pWnd = GetDlgItem(IDC_BTN_ALIGN_LEFT);
		if( !pWnd || !*pWnd ) return;

		pWnd->EnableWindow( bEnableLayout );

		pWnd = GetDlgItem(IDC_BTN_ALIGN_CENTER);
		pWnd->EnableWindow( bEnableLayout );
		pWnd = GetDlgItem(IDC_BTN_ALIGN_RIGHT);
		pWnd->EnableWindow( bEnableLayout );
		pWnd = GetDlgItem(IDC_BTN_ALIGN_TOP);
		pWnd->EnableWindow( bEnableLayout );
		pWnd = GetDlgItem(IDC_BTN_ALIGN_MIDDLE);
		pWnd->EnableWindow( bEnableLayout );
		pWnd = GetDlgItem(IDC_BTN_ALIGN_BOTTOM);
		pWnd->EnableWindow( bEnableLayout );

		pWnd = GetDlgItem(IDC_BTN_SAME_SIZE_WIDTH);
		pWnd->EnableWindow( bEnableLayout );
		pWnd = GetDlgItem(IDC_BTN_SAME_SIZE_HEIGHT);
		pWnd->EnableWindow( bEnableLayout );
		pWnd = GetDlgItem(IDC_BTN_SAME_SIZE_BOTH);
		pWnd->EnableWindow( bEnableLayout );

		pWnd = GetDlgItem(IDC_BTN_DISTRIB_HORZ_EQUAL_GAP);
		pWnd->EnableWindow( bEnableLayout );
		pWnd = GetDlgItem(IDC_BTN_DISTRIB_VERT_EQUAL_GAP);
		pWnd->EnableWindow( bEnableLayout );

		//pWnd = GetDlgItem(IDC_BTN_HELPER_ARRANGE);
		//pWnd->EnableWindow( bEnableLayout );
		//pWnd = GetDlgItem(IDC_BTN_HELPER_REORDER);
		//pWnd->EnableWindow( bEnableLayout );
		

		s_bEnableLayout = bEnableLayout;
	}
}

void DLayoutToolBar::SetUnit( ScreenUnit eUnit, double dScaleWidth, double dScaleHeight )
{
	m_eUnit = eUnit;
	m_dD2PScaleWidth = dScaleWidth;
	m_dD2PScaleHeight = dScaleHeight;
	m_wndArrangeHelperDlg.SetUnit( eUnit, dScaleWidth, dScaleHeight );
}

void DLayoutToolBar::OnBnClickedBtnAlignLeft()
{
	m_pWndDesignCanvas->AlignSelectKeys( CDesignCanvasWnd::KeyAlignLeft );
}

void DLayoutToolBar::OnBnClickedBtnAlignCenter()
{
	m_pWndDesignCanvas->AlignSelectKeys( CDesignCanvasWnd::KeyAlignCenter );
}

void DLayoutToolBar::OnBnClickedBtnAlignRight()
{
	m_pWndDesignCanvas->AlignSelectKeys( CDesignCanvasWnd::KeyAlignRight );
}

void DLayoutToolBar::OnBnClickedBtnAlignTop()
{
	m_pWndDesignCanvas->AlignSelectKeys( CDesignCanvasWnd::KeyAlignTop );
}

void DLayoutToolBar::OnBnClickedBtnAlignMiddle()
{
	m_pWndDesignCanvas->AlignSelectKeys( CDesignCanvasWnd::KeyAlignMiddle );
}

void DLayoutToolBar::OnBnClickedBtnAlignBottom()
{
	m_pWndDesignCanvas->AlignSelectKeys( CDesignCanvasWnd::KeyAlignBottom );
}

void DLayoutToolBar::OnBnClickedBtnSameSizeWidth()
{
	m_pWndDesignCanvas->AdjustSizeSelectKeys( CDesignCanvasWnd::AdjustSizeSameWidth );
}

void DLayoutToolBar::OnBnClickedBtnSameSizeHeight()
{
	m_pWndDesignCanvas->AdjustSizeSelectKeys( CDesignCanvasWnd::AdjustSizeSameHeight );
}

void DLayoutToolBar::OnBnClickedBtnSameSizeBoth()
{
	m_pWndDesignCanvas->AdjustSizeSelectKeys( CDesignCanvasWnd::AdjustSizeSameBoth );
}
void DLayoutToolBar::OnBnClickedBtnDistribHorzEqualGap()
{
	m_pWndDesignCanvas->DistribSelectKeys( CDesignCanvasWnd::DistribHorzEqualGap );
}

void DLayoutToolBar::OnBnClickedBtnDistribVertEqualGap()
{
	m_pWndDesignCanvas->DistribSelectKeys( CDesignCanvasWnd::DistribVertEqualGap );
}

int DLayoutToolBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	CString strPos = AfxGetApp()->GetProfileString(_T("Windows"), _T("LayoutToolBar_Pos"));
	if ( !strPos.IsEmpty() )
	{
		CRect rcWin;
		CString str;
		int nD;
		do
		{
			nD = strPos.Find(',');
			if ( nD <= 0 ) break;
			str = strPos.Left(nD); strPos = strPos.Mid(nD + 1);
			rcWin.left = _tcstol(str, NULL, 0);
			nD = strPos.Find(',');
			if ( nD <= 0 ) break;
			str = strPos.Left(nD); strPos = strPos.Mid(nD + 1);
			rcWin.top = _tcstol(str, NULL, 0);
			nD = strPos.Find(',');
			if ( nD <= 0 ) break;
			str = strPos.Left(nD); strPos = strPos.Mid(nD + 1);
			rcWin.right = _tcstol(str, NULL, 0);
			rcWin.bottom = _tcstol(strPos, NULL, 0);
			rcWin.NormalizeRect();

			CRect rcCaption = rcWin;
			rcCaption.DeflateRect(::GetSystemMetrics(SM_CXSIZEFRAME), ::GetSystemMetrics(SM_CYSIZEFRAME));
			rcCaption.bottom = rcCaption.top + ::GetSystemMetrics(SM_CYCAPTION);
			rcCaption.left += rcCaption.Height();
			rcCaption.right -= rcCaption.Height() * 4;

			void * pp[2];
			pp[0] = &rcCaption;
			pp[1] = FALSE;
			::EnumDisplayMonitors(NULL, NULL,
				MonitorEnumProc, (LPARAM)pp);
			if ( pp[1] )
				SetWindowPos( NULL, rcWin.left, rcWin.top, 0, 0, SWP_NOSIZE );
			else
				CenterWindow();
		}
		while ( FALSE );
	}
	else
	{
		CenterWindow();
	}

	return 0;
}

void DLayoutToolBar::OnDestroy()
{
	if( m_wndArrangeHelperDlg && m_wndArrangeHelperDlg.IsWindowVisible() )
	{
		m_wndArrangeHelperDlg.SendMessage( WM_CLOSE );
	}

	CDialog::OnDestroy();

	WINDOWPLACEMENT wp;
	GetWindowPlacement(&wp);
	CString str;
	str.Format(_T("%d,%d,%d,%d"),
		wp.rcNormalPosition.left, wp.rcNormalPosition.top,
		wp.rcNormalPosition.right, wp.rcNormalPosition.bottom);
	AfxGetApp()->WriteProfileString(_T("Windows"), _T("LayoutToolBar_Pos"), str);
}

void DLayoutToolBar::OnOK()
{
	ShowWindow( SW_HIDE );

	if( !m_pWndDesignTool )
		return;

	m_pWndDesignTool->UpdateLayoutButton();
}

void DLayoutToolBar::OnCancel()
{
	ShowWindow( SW_HIDE );

	if( !m_pWndDesignTool )
		return;

	m_pWndDesignTool->UpdateLayoutButton();

}

void DLayoutToolBar::OnBnClickedBtnHelperReorder()
{
	m_pWndDesignCanvas->ReOrderKeys();
}

void DLayoutToolBar::OnBnClickedBtnHelperArrange()
{
	m_pWndDesignTool->EnableWindow( FALSE );
	m_pWndDesignTool->ShowWindow( SW_HIDE );
	EnableWindow( FALSE );
	ShowWindow( SW_HIDE );
	m_wndArrangeHelperDlg.DoModal( m_pWndDesignCanvas );
	ShowWindow( SW_SHOW );
	EnableWindow( TRUE );
	m_pWndDesignTool->EnableWindow( TRUE );
	m_pWndDesignTool->ShowWindow( SW_SHOW );
}
