// UnderlineTitleStatic.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "UnderlineTitleStatic.h"
#include "GdiplusExt.h"


// CUnderlineTitleStatic

IMPLEMENT_DYNAMIC(CUnderlineTitleStatic, CStatic)
CUnderlineTitleStatic::CUnderlineTitleStatic()
{
	m_pImageIcon = NULL;
}

CUnderlineTitleStatic::~CUnderlineTitleStatic()
{
	if( m_pImageIcon )
		delete m_pImageIcon;
	m_pImageIcon = NULL;
}


BEGIN_MESSAGE_MAP(CUnderlineTitleStatic, CStatic)
	ON_WM_PAINT()
	ON_WM_UPDATEUISTATE()
END_MESSAGE_MAP()



// CUnderlineTitleStatic 메시지 처리기입니다.
void CUnderlineTitleStatic::SetIconImage( HMODULE hModule, LPCTSTR lpszResType, UINT nIDResPNG )
{
	Image* pIcon = GdipLoadImageFromRes( hModule, lpszResType, nIDResPNG );
	if( pIcon )
	{
		if( m_pImageIcon ) delete m_pImageIcon;
		m_pImageIcon = pIcon;
	}
}

void CUnderlineTitleStatic::SetWindowText( LPCTSTR lpszNewText )
{
	CWnd::SetWindowText( lpszNewText );
	RedrawWindow();
}

void CUnderlineTitleStatic::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CString strText;
	GetWindowText( strText );

	CRect rcClient;
	GetClientRect( rcClient );

	dc.FillSolidRect( rcClient, RGB(255, 255, 255) );

	Graphics g( dc.GetSafeHdc() );

	g.SetTextRenderingHint( TextRenderingHintClearTypeGridFit );

	RectF rectText((float)rcClient.left, (float)rcClient.top, (float)rcClient.Width(), (float)rcClient.Height());
	int nIconW = 0;
	int nIconH = 0;
	int nIconX = 0;
	int nIconY = 0;

	if( m_pImageIcon )
	{
		nIconW = m_pImageIcon->GetWidth();
		nIconH = m_pImageIcon->GetHeight();
		const int nOffsetX = 4;
		
		nIconX = (int)rectText.X;
		nIconY = (int)(rectText.Y + (rectText.Height-nIconH) / 2);

		g.DrawImage( m_pImageIcon, Rect(nIconX, nIconY, nIconW, nIconH), 0, 0, nIconW, nIconH , UnitPixel);
		nIconW = 12;
		nIconH = 12;

		rectText.X = float(nIconX + nIconW + nOffsetX);
		rectText.Width -= float(nIconX + nIconW + nOffsetX);
	}

	NONCLIENTMETRICS ncm = { sizeof(NONCLIENTMETRICS) };
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);

	USES_CONVERSION;

	WCHAR* szFaceName;
	CString strX = (WCHAR*)T2W(ncm.lfMessageFont.lfFaceName);
	if ( strX.IsEmpty() )
		szFaceName = _T("Arial");
	else
		szFaceName = (WCHAR*)T2W(ncm.lfMessageFont.lfFaceName);

	FontFamily fntFamily( szFaceName );
	Font fntText( &fntFamily, 11.0f, FontStyleBold, UnitPixel );

	StringFormat stringFormat;
	stringFormat.SetLineAlignment( StringAlignmentNear );
	stringFormat.SetAlignment( StringAlignmentNear );

	BSTR bstrText = strText.AllocSysString();
	g.DrawString( bstrText, -1, &fntText, 
		rectText, 
		&stringFormat, &SolidBrush( Color(55, 100, 160) ) );
	RectF rectBBox;
	g.MeasureString( bstrText, -1, &fntText, RectF((float)rcClient.left, (float)rcClient.top, (float)rcClient.Width(), (float)rcClient.Height()),
		&rectBBox );
	::SysFreeString( bstrText );

	Rect rectUnderLine;
	rectUnderLine.X = rcClient.left + nIconX + nIconW + (m_pImageIcon ? 5 : 0);
	rectUnderLine.Y = (int)(rectBBox.GetBottom() + 1.f);
	rectUnderLine.Width = rcClient.Width();
	rectUnderLine.Height = 2;
	g.FillRectangle( &SolidBrush( Color(0x99, 0xBB, 0xE8) ), rectUnderLine );
}

void CUnderlineTitleStatic::OnUpdateUIState(UINT nAction, UINT nUIElement)
{
	// 이 기능을 사용하려면 Windows 2000 이상이 필요합니다.
	// _WIN32_WINNT 및 WINVER 기호는 0x0500보다 크거나 같아야 합니다.
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	if ( nAction != UIS_SET )
		CStatic::OnUpdateUIState(nAction, nUIElement);
}
