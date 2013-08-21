// DLogicGallery.cpp : implementation file
//

#include "stdafx.h"
#include "T3kSoftlogic.h"
#include "DLogicGallery.h"
#include "LogicDesignerWnd.h"
#include "T3kSoftlogicDlg.h"

#include "GdiplusExt.h"
// DLogicGallery dialog

IMPLEMENT_DYNAMIC(DLogicGallery, CDialog)

DLogicGallery::DLogicGallery(CWnd* pParent /*=NULL*/)
	: CDialog(DLogicGallery::IDD, pParent)
	, m_nGalleryItemType(0)
{
	m_portKey = CSoftlogic::epNoGate;
	m_ptMouse = CPoint(0,0);
}

DLogicGallery::~DLogicGallery()
{
}

void DLogicGallery::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_GALLERY_ITEM1, m_nGalleryItemType);
	DDX_Control(pDX, IDC_COMBO_SELECT_GPIO, m_cbSelectGPIO);
}


BEGIN_MESSAGE_MAP(DLogicGallery, CDialog)
	ON_BN_CLICKED(IDOK, &DLogicGallery::OnBnClickedOk)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_RADIO_GALLERY_ITEM1, &DLogicGallery::OnBnClickedRadioGalleryItem)
	ON_BN_CLICKED(IDC_RADIO_GALLERY_ITEM2, &DLogicGallery::OnBnClickedRadioGalleryItem)
	ON_BN_CLICKED(IDC_RADIO_GALLERY_ITEM3, &DLogicGallery::OnBnClickedRadioGalleryItem)
	ON_BN_CLICKED(IDC_RADIO_GALLERY_ITEM4, &DLogicGallery::OnBnClickedRadioGalleryItem)
	ON_BN_CLICKED(IDC_RADIO_GALLERY_ITEM5, &DLogicGallery::OnBnClickedRadioGalleryItem)
	ON_BN_CLICKED(IDC_RADIO_GALLERY_ITEM6, &DLogicGallery::OnBnClickedRadioGalleryItem)
END_MESSAGE_MAP()


// DLogicGallery message handlers

BOOL DLogicGallery::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetInfo(m_portKey, m_ptMouse);

	CT3kSoftlogicDlg* pDlg = (CT3kSoftlogicDlg*)AfxGetMainWnd();
	CSoftkeyArray& Keys = pDlg->GetKeys();

	for ( int i=0 ; i<Keys.GetGPIOCount() ; i++ )
	{
		GPIOInfo* pInfo = Keys.GetGPIOInfo(i);
		if ( pInfo )
		{
			if ( pInfo->bEnable )
			{
				CString strGPIO;
				strGPIO.Format( _T("GPIO %d"), i+1 );
				int nCbIdx = m_cbSelectGPIO.AddString( strGPIO );
				m_cbSelectGPIO.SetItemData( nCbIdx, (CSoftlogic::Port)(i+(int)CSoftlogic::epGpio0) );
			}
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void DLogicGallery::SetInfo( CSoftlogic::Port portKey, CPoint ptMouse )
{
	m_portKey = portKey;
	m_ptMouse = ptMouse;

	if ( GetSafeHwnd() )
	{
		GetDlgItem(IDC_RADIO_GALLERY_ITEM1)->EnableWindow( m_portKey == CSoftlogic::epNoGate ? FALSE : TRUE );
		GetDlgItem(IDC_RADIO_GALLERY_ITEM2)->EnableWindow( m_portKey == CSoftlogic::epNoGate ? FALSE : TRUE );
		GetDlgItem(IDC_RADIO_GALLERY_ITEM3)->EnableWindow( m_portKey == CSoftlogic::epNoGate ? FALSE : TRUE );
		GetDlgItem(IDC_RADIO_GALLERY_ITEM4)->EnableWindow( m_portKey != CSoftlogic::epNoGate ? FALSE : TRUE );
		GetDlgItem(IDC_RADIO_GALLERY_ITEM5)->EnableWindow( m_portKey != CSoftlogic::epNoGate ? FALSE : TRUE );
		GetDlgItem(IDC_RADIO_GALLERY_ITEM6)->EnableWindow( m_portKey != CSoftlogic::epNoGate ? FALSE : TRUE );

		GetDlgItem(IDC_COMBO_SELECT_GPIO)->EnableWindow( m_portKey != CSoftlogic::epNoGate ? FALSE : TRUE );
		GetDlgItem(IDC_STATIC_GPIO_TEXT)->EnableWindow( m_portKey != CSoftlogic::epNoGate ? FALSE : TRUE );
	}
}

void DLogicGallery::OnBnClickedOk()
{
	UpdateData(TRUE);

	CSoftlogic::Port port = m_portKey;

	if ( m_portKey == CSoftlogic::epNoGate )
	{
		int nCurSel = m_cbSelectGPIO.GetCurSel();
		if ( nCurSel < 0 )
		{
			::MessageBox( GetSafeHwnd(), _T("Please select GPIO."), _T("Error"), MB_OK|MB_ICONERROR );
			m_cbSelectGPIO.SetFocus();
			return;
		}

		port = (CSoftlogic::Port)m_cbSelectGPIO.GetItemData(nCurSel);
	}

	CLogicDesignerWnd* pLogicDesignerWnd = (CLogicDesignerWnd*)GetParent();
	pLogicDesignerWnd->AddLogicFromGallery( m_nGalleryItemType, port, m_ptMouse );

	OnOK();
}

void DLogicGallery::OnOK()
{
	ShowWindow(SW_HIDE);
}

void DLogicGallery::OnCancel()
{
	ShowWindow(SW_HIDE);
}

void DLogicGallery::OnPaint()
{
	CPaintDC dc(this); // device context for painting


	CRect rcPreview;
	GetDlgItem(IDC_STATIC_PREVIEW)->GetWindowRect(rcPreview);
	ScreenToClient(rcPreview);

	dc.FillSolidRect( rcPreview, RGB(0, 0, 0) );

	if ( m_nGalleryItemType < 0 )
	{
		DWORD dwOldTextColor = dc.SetTextColor( RGB(255, 255, 255) );
		int nOldBkMode = dc.SetBkMode( TRANSPARENT );

		CString strText = _T("<<  Select type");
		dc.DrawText( strText, rcPreview, DT_SINGLELINE|DT_VCENTER|DT_CENTER );

		dc.SetBkMode( nOldBkMode );
		dc.SetTextColor( dwOldTextColor );
		return;
	}

	Graphics g(dc.GetSafeHdc());
	Image* pImage = NULL;
	switch ( m_nGalleryItemType )
	{
	case 0:
		pImage = GdipLoadImageFromRes( AfxGetResourceHandle(), _T("PNG"), PNG_GALLERY_TOUCH_ONOFF );
		break;
	case 1:
		pImage = GdipLoadImageFromRes( AfxGetResourceHandle(), _T("PNG"), PNG_GALLERY_CALIBRATION );
		break;
	case 2:
		pImage = GdipLoadImageFromRes( AfxGetResourceHandle(), _T("PNG"), PNG_GALLERY_TASK_SWITCH );
		break;
	case 3:
		pImage = GdipLoadImageFromRes( AfxGetResourceHandle(), _T("PNG"), PNG_GALLERY_CALIBRATION_GPIO );
		break;
	case 4:
		pImage = GdipLoadImageFromRes( AfxGetResourceHandle(), _T("PNG"), PNG_GALLERY_TOUCHONOFF_GPIO );
		break;
	case 5:
		pImage = GdipLoadImageFromRes( AfxGetResourceHandle(), _T("PNG"), PNG_GALLERY_IRPEN_MODE_GPIO );
		break;
	}

	Rect rect;
	UINT nDstW = rcPreview.Width();
	UINT nDstH = rcPreview.Height();
	UINT nIW, nIH;

	if ( nDstW > pImage->GetWidth() &&
		nDstH > pImage->GetHeight() )
	{
		nIW = pImage->GetWidth();
		nIH = pImage->GetHeight();
	}
	else
	{
		if ( pImage->GetWidth() > pImage->GetHeight() )
		{
			if ( nDstW < pImage->GetWidth() )
			{
				nIW = nDstW;
				nIH = pImage->GetHeight() * nDstW / pImage->GetWidth();
			}
		}
		else
		{
			if ( nDstH < pImage->GetHeight() )
			{
				nIH = nDstH;
				nIW = pImage->GetWidth() * nDstH / pImage->GetHeight();
			}
		}
	}

	rect.X = rcPreview.left + (nDstW-nIW)/2;
	rect.Y = rcPreview.top + (nDstH-nIH)/2;
	rect.Width = nIW;
	rect.Height = nIH;

	g.DrawImage( pImage, rect, 0, 0, pImage->GetWidth(), pImage->GetHeight(), UnitPixel );

	if ( pImage )
		delete pImage;
}

int DLogicGallery::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	CString strPos = AfxGetApp()->GetProfileString(_T("Windows"), _T("LogicGallery_Pos"));
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

void DLogicGallery::OnDestroy()
{
	CDialog::OnDestroy();

	WINDOWPLACEMENT wp;
	GetWindowPlacement(&wp);
	CString str;
	str.Format(_T("%d,%d,%d,%d"),
		wp.rcNormalPosition.left, wp.rcNormalPosition.top,
		wp.rcNormalPosition.right, wp.rcNormalPosition.bottom);
	AfxGetApp()->WriteProfileString(_T("Windows"), _T("LogicGallery_Pos"), str);
}

void DLogicGallery::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if ( bShow )
	{
		m_nGalleryItemType = -1;
		UpdateData( FALSE );
		m_cbSelectGPIO.SetCurSel(-1);
		GetParent()->EnableWindow(FALSE);
	}
	else
	{
		GetParent()->EnableWindow(TRUE);
	}
}

void DLogicGallery::OnBnClickedRadioGalleryItem()
{
	UpdateData( TRUE );
	CRect rc;
	GetDlgItem(IDC_STATIC_PREVIEW)->GetWindowRect( rc );
	ScreenToClient( rc );
	InvalidateRect( rc );
}
