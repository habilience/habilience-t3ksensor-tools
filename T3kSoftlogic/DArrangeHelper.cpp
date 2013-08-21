// DArrangeHelper.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "T3kSoftlogic.h"
#include "DArrangeHelper.h"

#include "DesignCanvasWnd.h"

#include <afxpriv.h>
#include <float.h>
static void AFX_CDECL MyAfxTextIntFormat(CDataExchange* pDX, int nIDC,
	LPCTSTR lpszFormat, UINT nIDPrompt, ...)
	// only supports windows output formats - no floating point
{
	va_list pData;
	va_start(pData, nIDPrompt);

	HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
   ASSERT( hWndCtrl != NULL );

	const int SZT_SIZE = 64;
	TCHAR szT[SZT_SIZE];
	if (pDX->m_bSaveAndValidate)
	{
		void* pResult;

		pResult = va_arg( pData, void* );
		// the following works for %d, %u, %ld, %lu
		::GetWindowText(hWndCtrl, szT, _countof(szT));
		if (_sntscanf_s(szT, _countof(szT), lpszFormat, pResult) != 1)
		{
			AfxMessageBox(nIDPrompt);
			pDX->Fail();        // throws exception
		}
	}
	else
	{
		
		ATL_CRT_ERRORCHECK_SPRINTF(_vsntprintf_s(szT, _countof(szT), _countof(szT) - 1, lpszFormat, pData));
			// does not support floating point numbers - see dlgfloat.cpp
		AfxSetWindowText(hWndCtrl, szT);
	}

	va_end(pData);
}

static void AFXAPI MyAfxTextFloatFormat(CDataExchange* pDX, int nIDC,
	void* pData, double value, int nSizeGcvt)
{
	ASSERT(pData != NULL);

	pDX->PrepareEditCtrl(nIDC);
	HWND hWndCtrl;
	pDX->m_pDlgWnd->GetDlgItem(nIDC, &hWndCtrl);
	
	const int TEXT_BUFFER_SIZE = 400;
	TCHAR szBuffer[TEXT_BUFFER_SIZE];
	if (pDX->m_bSaveAndValidate)
	{
		::GetWindowText(hWndCtrl, szBuffer, _countof(szBuffer));
		double d;
		if (_sntscanf_s(szBuffer, _countof(szBuffer), _T("%lf"), &d) != 1)
		{
			AfxMessageBox(AFX_IDP_PARSE_REAL);
			pDX->Fail();            // throws exception
		}
		if (nSizeGcvt == FLT_DIG)
			*((float*)pData) = (float)d;
		else
			*((double*)pData) = d;
	}
	else
	{
		ATL_CRT_ERRORCHECK_SPRINTF(_sntprintf_s(szBuffer, _countof(szBuffer), _countof(szBuffer) -1, _T("%.1f"), value));
		AfxSetWindowText(hWndCtrl, szBuffer);
	}
}

void DArrangeHelper::MYDDX_Text(CDataExchange* pDX, int nIDC, double& dValue, BOOL bIsScaleX)
{
	double	dValueDouble = dValue;
	int		nValueInt;
	if (pDX->m_bSaveAndValidate)
	{
		switch ( m_eUnit )
		{
		case UnitRes:
			MyAfxTextIntFormat(pDX, nIDC, _T("%d"), AFX_IDP_PARSE_INT, &nValueInt);
			dValue = (double)nValueInt;
			break;
		case UnitMM:
			MyAfxTextFloatFormat(pDX, nIDC, &dValueDouble, dValueDouble, DBL_DIG);
			if ( bIsScaleX )
			{
				dValue = dValueDouble / m_dD2PScaleWidth;
			}
			else
			{
				dValue = dValueDouble / m_dD2PScaleHeight;
			}
			break;
		}
	}
	else
	{
		switch ( m_eUnit )
		{
		case UnitRes:
			nValueInt = int(dValue + .5f);
			MyAfxTextIntFormat(pDX, nIDC, _T("%d"), AFX_IDP_PARSE_INT, nValueInt);
			break;
		case UnitMM:
			if ( bIsScaleX )
			{
				dValueDouble = dValue * m_dD2PScaleWidth;
			}
			else
			{
				dValueDouble = dValue * m_dD2PScaleHeight;
			}
			MyAfxTextFloatFormat(pDX, nIDC, &dValueDouble, dValueDouble, DBL_DIG);
			break;
		}
	}
}

// DArrangeHelper 대화 상자입니다.

IMPLEMENT_DYNAMIC(DArrangeHelper, CDialog)

DArrangeHelper::DArrangeHelper(CWnd* pParent /*=NULL*/)
	: CDialog(DArrangeHelper::IDD, pParent)
	, m_nArrangeType(0)
	, m_dWidth(2000)
	, m_dHeight(1900)
	, m_dInterval(300)
	, m_nCount(8)
{
	m_eUnit = UnitRes;
	m_dD2PScaleWidth = m_dD2PScaleHeight = 0.0;
	m_pImageArrangePicture = GdipLoadImageFromRes( AfxGetResourceHandle(), _T("PNG"), PNG_PICTURE_AUTO_ARRANGE );
}

DArrangeHelper::~DArrangeHelper()
{
	if( m_pImageArrangePicture )
		delete m_pImageArrangePicture;
	m_pImageArrangePicture = NULL;
}

void DArrangeHelper::SetUnit( ScreenUnit eUnit, double dScaleWidth, double dScaleHeight )
{
	m_eUnit = eUnit;
	m_dD2PScaleWidth = dScaleWidth;
	m_dD2PScaleHeight = dScaleHeight;
}

void DArrangeHelper::UpdateUnit()
{
	switch ( m_eUnit )
	{
	case UnitRes:
		m_edtKeyWidth.SetFloatStyle( FALSE, FALSE );
		m_edtKeyHeight.SetFloatStyle( FALSE, FALSE );
		m_edtKeyInterval.SetFloatStyle( FALSE, FALSE );
		break;
	case UnitMM:
		m_edtKeyWidth.SetFloatStyle( TRUE, FALSE );
		m_edtKeyHeight.SetFloatStyle( TRUE, FALSE );
		m_edtKeyInterval.SetFloatStyle( TRUE, FALSE );
		break;
	}
	UpdateData( FALSE );
}

void DArrangeHelper::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_HORIZONTAL, m_nArrangeType);
	MYDDX_Text(pDX, IDC_EDIT_WIDTH, m_dWidth, TRUE);
	MYDDX_Text(pDX, IDC_EDIT_HEIGHT, m_dHeight, FALSE);
	MYDDX_Text(pDX, IDC_EDIT_INTERVAL, m_dInterval, m_nArrangeType == 0 ? TRUE : FALSE );
	DDX_Text(pDX, IDC_EDIT_KEY_COUNT, m_nCount);
	DDX_Control(pDX, IDC_EDIT_KEY_COUNT, m_edtKeyCount);
	DDX_Control(pDX, IDC_EDIT_WIDTH, m_edtKeyWidth);
	DDX_Control(pDX, IDC_EDIT_HEIGHT, m_edtKeyHeight);
	DDX_Control(pDX, IDC_EDIT_INTERVAL, m_edtKeyInterval);
}


BEGIN_MESSAGE_MAP(DArrangeHelper, CDialog)
	ON_BN_CLICKED(IDC_BTN_APPLY, &DArrangeHelper::OnBnClickedBtnApply)
	ON_BN_CLICKED(IDC_BTN_CANCEL, &DArrangeHelper::OnBnClickedBtnCancel)
	ON_BN_CLICKED(IDC_RADIO_HORIZONTAL, &DArrangeHelper::OnBnClickedRadioHorizontalVertical)
	ON_BN_CLICKED(IDC_RADIO_VERTICAL, &DArrangeHelper::OnBnClickedRadioHorizontalVertical)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// DArrangeHelper 메시지 처리기입니다.

void DArrangeHelper::OnBnClickedBtnApply()
{
	UpdateData( TRUE );

	CArray<CSoftkey*>& SelectKeys = m_pWndDesignCanvas->GetSelectKeys();
	BOOL bMake = FALSE;
	if( SelectKeys.GetCount() == 0 )
		bMake = TRUE;

	switch( m_nArrangeType )
	{
	case 0: // Horz
		if( bMake )
			m_pWndDesignCanvas->MakeArrangedKeys( CDesignCanvasWnd::KeyArrangeHorizontal, m_nCount, (int)m_dWidth, (int)m_dHeight, (int)m_dInterval );
		else
			m_pWndDesignCanvas->ArrangeSelectKeys( CDesignCanvasWnd::KeyArrangeHorizontal, (int)m_dWidth, (int)m_dHeight, (int)m_dInterval );
		break;
	case 1: // Vert
		if( bMake )
			m_pWndDesignCanvas->MakeArrangedKeys( CDesignCanvasWnd::KeyArrangeVertical, m_nCount, (int)m_dWidth, (int)m_dHeight, (int)m_dInterval );
		else
			m_pWndDesignCanvas->ArrangeSelectKeys( CDesignCanvasWnd::KeyArrangeVertical, (int)m_dWidth, (int)m_dHeight, (int)m_dInterval );
		break;
	}

	CDialog::OnOK();
}

void DArrangeHelper::OnBnClickedBtnCancel()
{
	CDialog::OnCancel();
}

void DArrangeHelper::OnBnClickedRadioHorizontalVertical()
{
	UpdateData( TRUE );

	CWnd* pWnd = GetDlgItem( IDC_STATIC_PIC );
	CRect rcPic;
	pWnd->GetWindowRect( rcPic );
	ScreenToClient( rcPic );
	InvalidateRect( rcPic );
}

INT_PTR  DArrangeHelper::DoModal( CWnd* pParentWnd )
{
	ASSERT( pParentWnd );
	m_pParentWnd = pParentWnd;

	m_pWndDesignCanvas = (CDesignCanvasWnd*) pParentWnd;

	return CDialog::DoModal();
}

BOOL DArrangeHelper::OnInitDialog()
{
	CDialog::OnInitDialog();

	CArray<CSoftkey*>& SelectKeys = m_pWndDesignCanvas->GetSelectKeys();
	BOOL bMake = FALSE;
	if( SelectKeys.GetCount() == 0 )
		bMake = TRUE;

	if( bMake )
	{
		GetDlgItem(IDC_STATIC_TITLE)->SetWindowText( _T("Make Arranged Keys") );
		GetDlgItem(IDC_EDIT_KEY_COUNT)->ShowWindow( SW_SHOW );
		GetDlgItem(IDC_STATIC_KEY_COUNT)->ShowWindow( SW_SHOW );
	}
	else
	{
		GetDlgItem(IDC_STATIC_TITLE)->SetWindowText( _T("Modify Arrangement") );
		GetDlgItem(IDC_EDIT_KEY_COUNT)->ShowWindow( SW_HIDE );
		GetDlgItem(IDC_STATIC_KEY_COUNT)->ShowWindow( SW_HIDE );
	}

	m_edtKeyCount.SetFloatStyle( FALSE );
	m_edtKeyCount.SetRange( 2, 30 );

	UpdateUnit();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void DArrangeHelper::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	using namespace Gdiplus;

	CRect rcBody;
	GetDlgItem(IDC_STATIC_PIC)->GetWindowRect( rcBody );
	ScreenToClient( rcBody );

	Graphics g( dc.GetSafeHdc() );

	if( m_pImageArrangePicture )
	{
		int nPicW = m_pImageArrangePicture->GetWidth() / 2;
		int nPicH = m_pImageArrangePicture->GetHeight();
		
		int nPicX = rcBody.left + (rcBody.Width()-nPicW) / 2;
		int nPicY = rcBody.top + (rcBody.Height()-nPicH) / 2;

		g.DrawImage( m_pImageArrangePicture, Rect(nPicX, nPicY, nPicW, nPicH), m_nArrangeType == 0 ? 0 : nPicW, 0, nPicW, nPicH , UnitPixel);
	}
}

BOOL DArrangeHelper::PreTranslateMessage(MSG* pMsg)
{
	CBorderStyleEdit* pEdit = NULL;
	CRect rcClient;
	int nDlgCtrlID = ::GetDlgCtrlID( pMsg->hwnd );

	pEdit = (CBorderStyleEdit*)GetDlgItem( nDlgCtrlID );

	if( !pEdit || !pEdit->IsKindOf( RUNTIME_CLASS(CBorderStyleEdit) ) ) 
		return CDialog::PreTranslateMessage(pMsg);

	if( pMsg->message == WM_KEYDOWN )
	{
		int nVirtKey = (int)pMsg->wParam;
		if( nVirtKey == VK_RETURN )
		{
			GetDlgItem(IDC_FOCUS_STEALER)->SetFocus();
		}
	}

	CWnd* pFocusWnd = GetFocus();
	if( pFocusWnd && pEdit && pMsg->message == WM_LBUTTONDOWN && pFocusWnd->m_hWnd != pEdit->m_hWnd )
	{   
		pEdit->SetSel(0,-1,TRUE);		
		pEdit->SetFocus();
		
		return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}
