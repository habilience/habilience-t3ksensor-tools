#pragma once


// CGPIOListCtrl

#define WM_VALIDATE_GPIO		(WM_USER+0x770)

class CGPIOListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CGPIOListCtrl)
protected:
	class CInPlaceCombo : public CComboBox
	{
	public:
		static CInPlaceCombo* GetInstance(); 
		static void DeleteInstance(); 

		BOOL ShowComboCtrl(DWORD dwStyle, const CRect& rCellRect, CWnd* pParentWnd, UINT uiResourceID,
			int iRowIndex, int iColumnIndex, CStringList* pDropDownList, CString strCurSelecetion = _T(""), int iCurSel = -1);

	public:
		virtual BOOL PreTranslateMessage(MSG* pMsg);

	protected:
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnKillFocus(CWnd* pNewWnd);
		afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
		afx_msg void OnCloseup();
		DECLARE_MESSAGE_MAP()

	private:
		CInPlaceCombo();
		CInPlaceCombo (CInPlaceCombo&) {}
		CInPlaceCombo& operator = (const CInPlaceCombo&) {}

		virtual ~CInPlaceCombo();

		int m_iRowIndex;
		int m_iColumnIndex;

		BOOL m_bESC;

		static CInPlaceCombo* m_pInPlaceCombo;

		CString m_strWindowText;
		CStringList m_DropDownList;
	};
protected:
	BOOL HitTestEx(CPoint &point, int* pRowIndex, int* pColumnIndex) const;
	CGPIOListCtrl::CInPlaceCombo* ShowInplaceCombo( int nRowIndex, int nColumnIndex, CString strCurSelection=_T(""), int nSel=-1 );
	void CalculateCellRect(int iColumnIndex, int iRowIndex, CRect& robCellRect);
	void ScrollToView(int iColumnIndex, CRect& robCellRect);
public:
	void InsertGPIOItem( BOOL bEnable, BOOL bIsOutput );
	void SetGPIOItem( int nItem, BOOL bEnable, BOOL bIsOutput );
	BOOL GetGPIOItem( int nItem, BOOL& bEnable, BOOL& bIsOutput );

	BOOL DeleteAllItems();

	CGPIOListCtrl();
	virtual ~CGPIOListCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
protected:
	virtual void PreSubclassWindow();
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};

