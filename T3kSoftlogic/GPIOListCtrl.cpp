// GPIOListCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "T3kSoftlogic.h"
#include "GPIOListCtrl.h"

CGPIOListCtrl::CInPlaceCombo* CGPIOListCtrl::CInPlaceCombo::m_pInPlaceCombo = NULL; 

CGPIOListCtrl::CInPlaceCombo::CInPlaceCombo()
{
	m_iRowIndex = -1;
	m_iColumnIndex = -1;
	m_bESC = FALSE;
}

CGPIOListCtrl::CInPlaceCombo::~CInPlaceCombo()
{
}

BEGIN_MESSAGE_MAP(CGPIOListCtrl::CInPlaceCombo, CComboBox)
	ON_WM_CREATE()
	ON_WM_KILLFOCUS()
	ON_WM_CHAR()
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseup)
END_MESSAGE_MAP()

int CGPIOListCtrl::CInPlaceCombo::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
	
	CFont* pFont = GetParent()->GetFont();
	SetFont(pFont);
	
	SetFocus();

	ResetContent(); 
	for (POSITION Pos_ = m_DropDownList.GetHeadPosition(); Pos_ != NULL;)
	{
		AddString((LPCTSTR) (m_DropDownList.GetNext(Pos_)));
	}

	return 0;
}

BOOL CGPIOListCtrl::CInPlaceCombo::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		{
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			return TRUE;				
		}
	}
	
	return CComboBox::PreTranslateMessage(pMsg);
}

void CGPIOListCtrl::CInPlaceCombo::OnKillFocus(CWnd* pNewWnd) 
{
	CComboBox::OnKillFocus(pNewWnd);
	
	CString str;
	GetWindowText(str);

	LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = LVN_ENDLABELEDIT;

	dispinfo.item.mask = LVIF_TEXT;
	dispinfo.item.iItem = m_iRowIndex;
	dispinfo.item.iSubItem = m_iColumnIndex;
	dispinfo.item.pszText = m_bESC ? LPTSTR((LPCTSTR)m_strWindowText) : LPTSTR((LPCTSTR)str);
	dispinfo.item.cchTextMax = m_bESC ? m_strWindowText.GetLength() : str.GetLength();
	
	GetParent()->SendMessage(WM_NOTIFY, GetParent()->GetDlgCtrlID(), (LPARAM)&dispinfo);

	PostMessage(WM_CLOSE);
}

void CGPIOListCtrl::CInPlaceCombo::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_ESCAPE || nChar == VK_RETURN)
	{
		if (nChar == VK_ESCAPE)
		{
			m_bESC = TRUE;
		}

		GetParent()->SetFocus();
		return;
	}
	
	CComboBox::OnChar(nChar, nRepCnt, nFlags);
}

void CGPIOListCtrl::CInPlaceCombo::OnCloseup() 
{
	GetParent()->SetFocus();
}

CGPIOListCtrl::CInPlaceCombo* CGPIOListCtrl::CInPlaceCombo::GetInstance()
{
	if(m_pInPlaceCombo == NULL)
	{
		m_pInPlaceCombo = new CInPlaceCombo;
	}
	return m_pInPlaceCombo;
}

void CGPIOListCtrl::CInPlaceCombo::DeleteInstance()
{
	delete m_pInPlaceCombo;
	m_pInPlaceCombo = NULL;
}

BOOL CGPIOListCtrl::CInPlaceCombo::ShowComboCtrl(DWORD dwStyle, const CRect &rCellRect, CWnd* pParentWnd, UINT uiResourceID,
								  int iRowIndex, int iColumnIndex, CStringList* pDropDownList, 
								  CString strCurSelecetion /*= ""*/, int iCurSel /*= -1*/)
{

	m_iRowIndex = iRowIndex;
	m_iColumnIndex = iColumnIndex;
	m_bESC = FALSE;
	
	m_DropDownList.RemoveAll(); 
	m_DropDownList.AddTail(pDropDownList);

	BOOL bRetVal = TRUE;

	if (-1 != iCurSel)
	{
		GetLBText(iCurSel, m_strWindowText);
	}
	else if (!strCurSelecetion.IsEmpty()) 
	{
		m_strWindowText = strCurSelecetion;

		int nSel = 0;
		for ( POSITION Pos_ = m_DropDownList.GetHeadPosition() ; Pos_ != NULL ; nSel++ )
		{
			if ( m_strWindowText == m_DropDownList.GetNext(Pos_) )
			{
				iCurSel = nSel;
				break;
			}
		}
	}
	
	if (NULL == m_pInPlaceCombo->m_hWnd) 
	{
		bRetVal = m_pInPlaceCombo->Create(dwStyle, rCellRect, pParentWnd, uiResourceID); 
	}

	SetCurSel(iCurSel);
	ShowDropDown();

	return bRetVal;
}


// CGPIOListCtrl

IMPLEMENT_DYNAMIC(CGPIOListCtrl, CListCtrl)

CGPIOListCtrl::CGPIOListCtrl()
{

}

CGPIOListCtrl::~CGPIOListCtrl()
{
	CInPlaceCombo::DeleteInstance();
}


BEGIN_MESSAGE_MAP(CGPIOListCtrl, CListCtrl)
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndLabelEdit)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


BOOL CGPIOListCtrl::HitTestEx(CPoint &point, int* pRowIndex, int* pColumnIndex) const
{
	if (!pRowIndex || !pColumnIndex)
	{
		return FALSE;
	}

	*pRowIndex = HitTest(point, NULL);

	if (pColumnIndex)
	{
		*pColumnIndex = 0;
	}

	if ((GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT)
	{
		return FALSE;
	}

	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);

	int iColumnCount = pHeader->GetItemCount();

	CRect obCellRect;
	GetItemRect(*pRowIndex, &obCellRect, LVIR_BOUNDS);
	
	if (obCellRect.PtInRect(point))
	{
		for (*pColumnIndex = 0; *pColumnIndex < iColumnCount; (*pColumnIndex)++)
		{
			int iColWidth = GetColumnWidth(*pColumnIndex);
			
			if (point.x >= obCellRect.left && point.x <= (obCellRect.left + iColWidth))
			{
				return TRUE;
			}
			obCellRect.left += iColWidth;
		}
	}
	return FALSE;
}

void CGPIOListCtrl::CalculateCellRect(int iColumnIndex, int iRowIndex, CRect& robCellRect)
{
	GetItemRect(iRowIndex, &robCellRect, LVIR_BOUNDS);
	
	CRect rcClient;
	GetClientRect(&rcClient);

	if (robCellRect.right > rcClient.right) 
	{
		robCellRect.right = rcClient.right;
	}

	ScrollToView(iColumnIndex, robCellRect); 
}

void CGPIOListCtrl::ScrollToView(int iColumnIndex, CRect& robCellRect)
{
	CRect rcClient;
	GetClientRect(&rcClient);

	int iColumnWidth = GetColumnWidth(iColumnIndex);

	int iOffSet = 0;
	for (int iIndex = 0; iIndex < iColumnIndex; iIndex++)
	{
		iOffSet += GetColumnWidth(iIndex);
	}

	CSize obScrollSize(0, 0);

	if (((iOffSet + robCellRect.left) < rcClient.left) || 
		((iOffSet + robCellRect.left) > rcClient.right))
	{
		obScrollSize.cx = iOffSet + robCellRect.left;
	}
	else if ((iOffSet + robCellRect.left + iColumnWidth) > rcClient.right)
	{
		obScrollSize.cx = iOffSet + robCellRect.left + iColumnWidth - rcClient.right;
	}

	Scroll(obScrollSize);
	robCellRect.left -= obScrollSize.cx;
	
	robCellRect.left += iOffSet;
	robCellRect.right = robCellRect.left + iColumnWidth;
}

CGPIOListCtrl::CInPlaceCombo* CGPIOListCtrl::ShowInplaceCombo( int nRowIndex, int nColumnIndex, CString strCurSelection/*=_T("")*/, int nSel/*=-1*/ )
{
	if (!EnsureVisible(nRowIndex, TRUE))
	{
		return NULL;
	}

	CRect obCellRect(0, 0, 0, 0);
	CalculateCellRect(nColumnIndex, nRowIndex, obCellRect);

	int nHeight = obCellRect.Height();  
	int nCount = 3;

	obCellRect.bottom += nHeight * nCount;

	DWORD dwDropDownCtrlStyle = WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | 
							CBS_DROPDOWNLIST | CBS_DISABLENOSCROLL;

	CInPlaceCombo* pInPlaceCombo = CInPlaceCombo::GetInstance();
	CStringList rComboItemsList;
	if ( nColumnIndex == 1 )	// Enable
	{
		rComboItemsList.AddTail( _T("True") );
		rComboItemsList.AddTail( _T("False") );
	}
	else						// Input/Output
	{
		rComboItemsList.AddTail( _T("Input") );
		rComboItemsList.AddTail( _T("Output") );
	}
	pInPlaceCombo->ShowComboCtrl(dwDropDownCtrlStyle, obCellRect, this, 0, nRowIndex, nColumnIndex, &rComboItemsList, 
								 strCurSelection, nSel);

	return pInPlaceCombo;
}

void CGPIOListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	POSITION pos = GetFirstSelectedItemPosition();
	int nOldSel = -1;
	if ( pos )
	{
		nOldSel = GetNextSelectedItem(pos);
	}

	int nColumnIndex = -1;
	int nRowIndex = -1;

	if (!HitTestEx(point, &nRowIndex, &nColumnIndex))
	{
		return;
	}

	CListCtrl::OnLButtonDown(nFlags, point);

	if ( nOldSel != nRowIndex )
		return;

	CString strCurSelection = GetItemText(nRowIndex, nColumnIndex);
	
	if ( nRowIndex  >= 0 )
	{
		if ( nColumnIndex > 0 )	// combobox
		{
			ShowInplaceCombo( nRowIndex, nColumnIndex, strCurSelection );
		}
	}
}


void CGPIOListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	POSITION pos = GetFirstSelectedItemPosition();
	int nOldSel = -1;
	if ( pos )
	{
		nOldSel = GetNextSelectedItem(pos);
	}

	int nColumnIndex = -1;
	int nRowIndex = -1;

	if (!HitTestEx(point, &nRowIndex, &nColumnIndex))
	{
		return;
	}

	CListCtrl::OnLButtonDblClk(nFlags, point);

	if ( nOldSel != nRowIndex )
		return;

	CString strCurSelection = GetItemText(nRowIndex, nColumnIndex);
	
	if ( nRowIndex  >= 0 )
	{
		if ( nColumnIndex > 0 )	// combobox
		{
			ShowInplaceCombo( nRowIndex, nColumnIndex, strCurSelection );
		}
	}
}


void CGPIOListCtrl::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	
	SetItemText(pDispInfo->item.iItem, pDispInfo->item.iSubItem, pDispInfo->item.pszText);

	GetParent()->SendMessage(WM_VALIDATE_GPIO, GetDlgCtrlID(), (LPARAM)pDispInfo); 

	*pResult = 0;
}

BOOL CGPIOListCtrl::DeleteAllItems()
{
	CInPlaceCombo* pCombo = CInPlaceCombo::GetInstance();
	if ( pCombo && *pCombo )
	{
		if ( pCombo->IsWindowVisible() )
		{
			pCombo->SendMessage( WM_CLOSE );
		}
	}
	return CListCtrl::DeleteAllItems();
}

void CGPIOListCtrl::InsertGPIOItem( BOOL bEnable, BOOL bIsOutput )
{
	int nItemCnt = GetItemCount();
	CString strItemNumber;
	strItemNumber.Format( _T("%d"), nItemCnt+1 );
	int nIdx = InsertItem( nItemCnt, strItemNumber );
	SetItemText( nIdx, 1, bEnable ? _T("True") : _T("False") );
	SetItemText( nIdx, 2, bIsOutput ? _T("Output") : _T("Input") );
}

void CGPIOListCtrl::SetGPIOItem( int nItem, BOOL bEnable, BOOL bIsOutput )
{
	if ( GetItemCount() <= nItem )
		return;

	SetItemText( nItem, 1, bEnable ? _T("True") : _T("False") );
	SetItemText( nItem, 2, bIsOutput ? _T("Output") : _T("Input") );
}

BOOL CGPIOListCtrl::GetGPIOItem( int nItem, BOOL& bEnable, BOOL& bIsOutput )
{
	CString strEnable = GetItemText( nItem, 1 );
	CString strOutput = GetItemText( nItem, 2 );
	bEnable = (strEnable == _T("True")) ? TRUE : FALSE;
	bIsOutput = (strOutput == _T("Output")) ? TRUE : FALSE;
	return FALSE;
}
void CGPIOListCtrl::PreSubclassWindow()
{
	SetExtendedStyle(LVS_EX_FULLROWSELECT);

	CListCtrl::PreSubclassWindow();
}
