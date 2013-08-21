// DEditGate.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "T3kSoftlogic.h"
#include "DEditGate.h"

#include "LogicDesignerWnd.h"
#include "T3kSoftlogicDlg.h"
#include "KeyMapStr.h"

// DEditGate 대화 상자입니다.

IMPLEMENT_DYNAMIC(DEditGate, CDialog)

DEditGate::DEditGate(CWnd* pParent /*=NULL*/)
	: CDialog(DEditGate::IDD, pParent)
	, m_nGateType(0)
	, m_bInvertInputPort1(FALSE)
	, m_bInvertInputPort2(FALSE)
	, m_bInvertControlPort(FALSE)
{
	m_pSoftlogic = NULL;
	m_bShowLogic = TRUE;
}

DEditGate::~DEditGate()
{
}

void DEditGate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_GATE_TYPE1, m_nGateType);
	DDX_Control(pDX, IDC_COMBO_CONTROL_PORT, m_cbControlPort);
	DDX_Control(pDX, IDC_COMBO_CONTROL_TRIGGER, m_cbTrigger);
	DDX_Check(pDX, IDC_CHECK_INVERT_INPUT_PORT1, m_bInvertInputPort1);
	DDX_Check(pDX, IDC_CHECK_INVERT_INPUT_PORT2, m_bInvertInputPort2);
	DDX_Control(pDX, IDC_COMBO_OUTPUT_TYPE, m_cbOutputPortType);
	DDX_Control(pDX, IDC_COMBO_OUTPUT_PORT, m_cbOutputPort);
	DDX_Check(pDX, IDC_CHECK_INVERT_CONTROL_PORT, m_bInvertControlPort);
	DDX_Control(pDX, IDC_COMBO_SHOWHIDE, m_cbLogicShowHide);
	DDX_Control(pDX, IDC_STATIC_EDIT_KEY, m_wndUserDefinedKey);
	DDX_Control(pDX, IDC_EDIT_VALUE, m_edtReportId);
	DDX_Control(pDX, IDC_COMBO_INPUT_PORT1, m_cbInputPort1);
	DDX_Control(pDX, IDC_COMBO_INPUT_PORT2, m_cbInputPort2);
}


BEGIN_MESSAGE_MAP(DEditGate, CDialog)
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
	ON_CBN_SELCHANGE(IDC_COMBO_CONTROL_PORT, &DEditGate::OnCbnSelchangeComboControlPort)
	ON_CBN_SELCHANGE(IDC_COMBO_CONTROL_TRIGGER, &DEditGate::OnCbnSelchangeComboControlTrigger)
	ON_CBN_SELCHANGE(IDC_COMBO_OUTPUT_PORT, &DEditGate::OnCbnSelchangeComboOutputPort)
	ON_CBN_SELCHANGE(IDC_COMBO_OUTPUT_TYPE, &DEditGate::OnCbnSelchangeComboOutputType)
	ON_BN_CLICKED(IDC_RADIO_GATE_TYPE1, &DEditGate::OnBnClickedRadioGateType)
	ON_BN_CLICKED(IDC_RADIO_GATE_TYPE2, &DEditGate::OnBnClickedRadioGateType)
	ON_BN_CLICKED(IDC_RADIO_GATE_TYPE3, &DEditGate::OnBnClickedRadioGateType)
	ON_BN_CLICKED(IDC_RADIO_GATE_TYPE4, &DEditGate::OnBnClickedRadioGateType)
	ON_BN_CLICKED(IDC_CHECK_INVERT_CONTROL_PORT, &DEditGate::OnBnClickedCheckInvert)
	ON_BN_CLICKED(IDC_CHECK_INVERT_INPUT_PORT1, &DEditGate::OnBnClickedCheckInvert)
	ON_BN_CLICKED(IDC_CHECK_INVERT_INPUT_PORT2, &DEditGate::OnBnClickedCheckInvert)
	
	ON_CBN_SELCHANGE(IDC_COMBO_SHOWHIDE, &DEditGate::OnCbnSelchangeComboShowhide)
	ON_MESSAGE(WM_KEYEDIT_CHANGE, &DEditGate::OnChangeKeyEdit)
	ON_MESSAGE(WM_EDIT_MODIFIED, &DEditGate::OnChangeReportIdEdit)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_CBN_SELCHANGE(IDC_COMBO_INPUT_PORT1, &DEditGate::OnCbnSelchangeComboInputPort1)
	ON_CBN_SELCHANGE(IDC_COMBO_INPUT_PORT2, &DEditGate::OnCbnSelchangeComboInputPort2)
END_MESSAGE_MAP()


// DEditGate 메시지 처리기입니다.
BOOL DEditGate::Create(CWnd* pParentWnd)
{
	m_pWndLogicCanvas = (CLogicDesignerWnd*)pParentWnd;

	return CDialog::Create(IDD, pParentWnd);
}

void DEditGate::OnClose()
{
	ShowWindow( SW_HIDE );
}

void DEditGate::ApplyItems()
{
	if( !m_pSoftlogic )
		return;

	UpdateData( TRUE );

	int nSelIdx;

	if( (nSelIdx = m_cbOutputPortType.GetCurSel()) >= 0 )
	{
		CSoftlogic::PortType type = (CSoftlogic::PortType)m_cbOutputPortType.GetItemData( nSelIdx );

		switch( type )
		{
		case CSoftlogic::eptError:
			m_pSoftlogic->setOutPort( CSoftlogic::epNoGate );
			//ASSERT( FALSE );
			// ??
		//	ASSERT( FALSE );
			break;
		case CSoftlogic::eptKey:
			{
				nSelIdx = m_cbOutputPort.GetCurSel();
				switch( nSelIdx )
				{
				case 0: // user defined
					{
						m_wndUserDefinedKey.SetKeyType( CUserDefinedKeyCtrl::KeyTypeNormal );
						WORD wKeyValue = m_wndUserDefinedKey.GetKeyValue();
						m_cbOutputPort.SetItemData( 0, (DWORD)wKeyValue );
						m_cbOutputPort.SetItemData( 1, (DWORD)wKeyValue );
					}
					break;
				case 1: // function key
					{
						m_wndUserDefinedKey.SetKeyType( CUserDefinedKeyCtrl::KeyTypeFunction );
						WORD wKeyValue = m_wndUserDefinedKey.GetKeyValue();
						m_cbOutputPort.SetItemData( 0, (DWORD)wKeyValue );
						m_cbOutputPort.SetItemData( 1, (DWORD)wKeyValue );
					}
					break;
				}

				WORD wKeyValue = (WORD)m_cbOutputPort.GetItemData( nSelIdx );
				//WORD wKey = m_wndUserDefinedKey.GetKeyValue();
				BYTE cCondition = wKeyValue >> 8;
				BYTE cKey = wKeyValue & 0xFF;
				if( !m_pSoftlogic->setOutKey1( cCondition, cKey ) )
					goto goto_ReloadItems;
			}
			break;
		case CSoftlogic::eptMouse:
			{
				m_wndUserDefinedKey.SetKeyType( CUserDefinedKeyCtrl::KeyTypeMouse );
				WORD wKeyValue = m_wndUserDefinedKey.GetKeyValue();
				BYTE cCondition = wKeyValue >> 8;
				BYTE cButton = wKeyValue & 0xFF;
				if( !m_pSoftlogic->setOutMouse( cCondition, cButton ) )
					goto goto_ReloadItems;
			}
			break;
		case CSoftlogic::eptReport:
			{
				CString strReportId;
				GetDlgItem(IDC_EDIT_VALUE)->GetWindowText( strReportId );
				BYTE cReportId = (BYTE)_tcstol(strReportId, NULL, 10 );
				if( !m_pSoftlogic->setReportId( cReportId ) )
					goto goto_ReloadItems;
			}
			break;
		case CSoftlogic::eptState:
			{
				if( (nSelIdx = m_cbOutputPort.GetCurSel()) >= 0 )
				{
					CSoftlogic::Port port = (CSoftlogic::Port)m_cbOutputPort.GetItemData(nSelIdx);
					if( !m_pSoftlogic->setOutPort( port ) )
						goto goto_ReloadItems;
				}
			}
			break;
		case CSoftlogic::eptGPIO:
			{
				if( (nSelIdx = m_cbOutputPort.GetCurSel()) >= 0 )
				{
					CSoftlogic::Port port = (CSoftlogic::Port)m_cbOutputPort.GetItemData(nSelIdx);
					if( !m_pSoftlogic->setOutPort( port ) )
						goto goto_ReloadItems;
				}
			}
			break;
		case CSoftlogic::eptSoftkey:
			{
				if( (nSelIdx = m_cbOutputPort.GetCurSel()) >= 0 )
				{
					CSoftlogic::Port port = (CSoftlogic::Port)m_cbOutputPort.GetItemData(nSelIdx);
					if( !m_pSoftlogic->setOutPort( port ) )
						goto goto_ReloadItems;
				}
			}
			break;
		default:
			{
				CString strText;
				m_cbOutputPort.GetWindowText( strText );
				if( strText.Compare( _T("Softlogic...") ) == 0 )
				{
				}
			}
			break;
		}
	}

	if( (nSelIdx = m_cbOutputPortType.GetCurSel()) >= 0 )
	{
		CSoftlogic::PortType type = (CSoftlogic::PortType)m_cbOutputPortType.GetItemData( nSelIdx );
		if ( type != CSoftlogic::eptKey && type != CSoftlogic::eptMouse )
		{
			if( (nSelIdx = m_cbControlPort.GetCurSel()) >= 0 )
			{
				CSoftlogic::Port port = (CSoftlogic::Port)m_cbControlPort.GetItemData(nSelIdx);

				if( !m_pSoftlogic->setEnablePort( port ) )
					goto goto_ReloadItems;
			}

			if( !m_pSoftlogic->setEnableNot( m_bInvertControlPort ) )
				goto goto_ReloadItems;
		}
	}

	switch( m_nGateType )
	{
	case 0:
		if( !m_pSoftlogic->setLogicType( CSoftlogic::eltAND ) )
			goto goto_ReloadItems;
		break; 
	case 2:
		if( !m_pSoftlogic->setLogicType( CSoftlogic::eltOR ) )
			goto goto_ReloadItems;
		break;
	case 3:
		if( !m_pSoftlogic->setLogicType( CSoftlogic::eltXOR ) )
			goto goto_ReloadItems;
		break;
	case 1:
		if( !m_pSoftlogic->setLogicType( CSoftlogic::eltAND_RISINGEDGE ) )
			goto goto_ReloadItems;
		break;
	}

	if( !m_pSoftlogic->setIn1Not( m_bInvertInputPort1 ) )
		goto goto_ReloadItems;
	if( !m_pSoftlogic->setIn2Not( m_bInvertInputPort2 ) )
		goto goto_ReloadItems;

	if( (nSelIdx = m_cbInputPort1.GetCurSel()) >= 0 )
	{
		CSoftlogic::Port port = (CSoftlogic::Port)m_cbInputPort1.GetItemData( nSelIdx );
		if( !m_pSoftlogic->setIn1Port( port ) )
			goto goto_ReloadItems;
	}
	if( (nSelIdx = m_cbInputPort2.GetCurSel()) >= 0 )
	{
		CSoftlogic::Port port = (CSoftlogic::Port)m_cbInputPort2.GetItemData( nSelIdx );
		if( !m_pSoftlogic->setIn2Port( port ) )
			goto goto_ReloadItems;
	}

	if( (nSelIdx = m_cbTrigger.GetCurSel()) >= 0 )
	{
		CSoftlogic::Trigger trigger = (CSoftlogic::Trigger)m_cbTrigger.GetItemData(nSelIdx);

		if( !m_pSoftlogic->setLogicTrigger( trigger ) )
			goto goto_ReloadItems;

	}

	ApplyHiddenLogics();

	return;

goto_ReloadItems:
	UpdateItems();
}

void DEditGate::ApplyHiddenLogics()
{
	if( !m_pSoftlogic )
		return;

	CT3kSoftlogicDlg* pDlg = (CT3kSoftlogicDlg*)AfxGetMainWnd();
	CSoftlogicArray& Logics = pDlg->GetLogics();
	int nIdx = Logics.IndexFromSoftlogic( m_pSoftlogic );

	BOOL bShowLogic;
	if( m_cbLogicShowHide.GetCurSel() == 0 )
	{
		bShowLogic = TRUE;
	}
	else
	{
		bShowLogic = FALSE;
	}
	if( m_bShowLogic != bShowLogic )
	{
		Logics.HideSoftlogic( nIdx, !bShowLogic );
		nIdx = Logics.IndexFromSoftlogic( m_pSoftlogic );
		m_bShowLogic = Logics.IsHiddenLogic(nIdx);
	}
}

void DEditGate::OnUpdateKeyValue( WORD wKeyValue )
{
	DisplayKeyValue( wKeyValue );

	BOOL bFound = FALSE;
	int nSelIndex = -1;
	for( int nI = 0 ; nI < m_cbOutputPort.GetCount() ; nI ++ )
	{
		WORD wItemValue = (WORD)m_cbOutputPort.GetItemData(nI);
		if( wItemValue == wKeyValue )
		{
			m_cbOutputPort.SetCurSel( nI );
			nSelIndex = nI;
			bFound = TRUE;
			break;
		}
	}

	if( !bFound )
	{
		BOOL bFunctionKey = FALSE;
		if( !(wKeyValue >> 8 & 0x80) )
		{
			int nScanCode;
			if( FindScanCode( wKeyValue & 0xFF, nScanCode ) )
			{
				int nVk = MapVirtualKey( nScanCode, 1 );
				if( nVk >= VK_F1 && nVk <= VK_F24 )
				{
					bFunctionKey = TRUE;
				}
			}
		}
		if( bFunctionKey )
		{
			m_cbOutputPort.SetCurSel( 1 );
			m_cbOutputPort.SetItemData( 1, (DWORD_PTR)wKeyValue );

			SetEditMode( ModeFunctionKey, wKeyValue );
		}
		else
		{
			m_cbOutputPort.SetCurSel( 0 );
			m_cbOutputPort.SetItemData( 0, (DWORD_PTR)wKeyValue );

			SetEditMode( ModeUserDefined, wKeyValue );
		}
	}
	else
	{
		if( nSelIndex == 1 )
		{
			m_cbOutputPort.SetItemData( 1, (DWORD_PTR)wKeyValue );

			SetEditMode( ModeFunctionKey, wKeyValue );
		}
		else
		{
			SetEditMode( ModeNone, wKeyValue );
		}
	}
}

void DEditGate::SetEditMode( EditMode Mode, WORD wKeyValue )
{
	switch( Mode )
	{
	case ModeNone:
		m_wndUserDefinedKey.Reset();
		m_wndUserDefinedKey.ShowWindow( SW_HIDE );
		break;
	case ModeUserDefined:
		m_wndUserDefinedKey.SetKeyType( CUserDefinedKeyCtrl::KeyTypeNormal );
		if( wKeyValue != 0xFFFF )
		{
			m_wndUserDefinedKey.SetKeyValue( wKeyValue );
		}
		m_wndUserDefinedKey.ShowWindow( SW_SHOW );
		break;
	case ModeFunctionKey:
		m_wndUserDefinedKey.SetKeyType( CUserDefinedKeyCtrl::KeyTypeFunction );
		if( wKeyValue != 0xFFFF )
		{
			m_wndUserDefinedKey.SetKeyValue( wKeyValue );
		}
		m_wndUserDefinedKey.ShowWindow( SW_SHOW );
		break;
	}
}

void DEditGate::UpdateInputCombo( CSoftlogic::Port port, CComboBox& cbInput )
{
	if ( port >= CSoftlogic::epSoftLogic0 )
	{
		int nCbIdx = cbInput.FindString( 0, _T("Softlogic") );
		if( nCbIdx >= 0 )
			cbInput.DeleteString( nCbIdx );
		nCbIdx = cbInput.AddString( _T("Softlogic...") );
		cbInput.SetItemData( nCbIdx, (DWORD_PTR)port );
		cbInput.SetCurSel( nCbIdx );
	}
	else
	{
		int nCbIdx = cbInput.FindString( 0, _T("Softlogic") );
		if( nCbIdx >= 0 )
			cbInput.DeleteString( nCbIdx );
		for( int i=0 ; i<cbInput.GetCount() ; i++ )
		{
			if( port == (CSoftlogic::Port)cbInput.GetItemData(i) )
			{
				cbInput.SetCurSel( i );
				break;
			}
		}
	}
	if ( port >= CSoftlogic::epSoftkey0 && port < CSoftlogic::epSoftkeyAll )
	{
		int nCbIdx = cbInput.FindString( 0, _T("Softkey") );
		if( nCbIdx >= 0 )
			cbInput.DeleteString( nCbIdx );
		CString strKey;
		strKey.Format( _T("Softkey%d"), (int)port-(int)(CSoftlogic::epSoftkey0)+1 );
		nCbIdx = cbInput.AddString( strKey );
		cbInput.SetItemData( nCbIdx, (DWORD_PTR)port );
		cbInput.SetCurSel( nCbIdx );
	}
	else
	{
		int nCbIdx = cbInput.FindString( 0, _T("Softkey") );
		if( nCbIdx >= 0 )
			cbInput.DeleteString( nCbIdx );
		for( int i=0 ; i<cbInput.GetCount() ; i++ )
		{
			if( port == (CSoftlogic::Port)cbInput.GetItemData(i) )
			{
				cbInput.SetCurSel( i );
				break;
			}
		}
	}
}

void DEditGate::UpdateItems()
{
	if( m_pSoftlogic )
	{
		m_cbControlPort.SetCurSel( 0 );
		for( int i=0 ; i<m_cbControlPort.GetCount() ; i++ )
		{
			if( m_pSoftlogic->getEnablePort() == (CSoftlogic::Port)m_cbControlPort.GetItemData(i) )
			{
				m_cbControlPort.SetCurSel( i );
				break;
			}
		}

		GetDlgItem(IDC_CHECK_INVERT_CONTROL_PORT)->EnableWindow( m_cbControlPort.GetCurSel() == 0 ? FALSE : TRUE );
		m_bInvertControlPort = m_pSoftlogic->getEnableNot();

		switch( m_pSoftlogic->getLogicType() )
		{
		case CSoftlogic::eltAND:
			m_nGateType = 0;
			break; 
		case CSoftlogic::eltOR:
			m_nGateType = 2;
			break;
		case CSoftlogic::eltXOR:
			m_nGateType = 3;
			break;
		case CSoftlogic::eltAND_RISINGEDGE:
			m_nGateType = 1;
			break;
		}

		m_bInvertInputPort1 = m_pSoftlogic->getIn1Not();
		m_bInvertInputPort2 = m_pSoftlogic->getIn2Not();

		CSoftlogic::Port port1 = m_pSoftlogic->getIn1Port();
		UpdateInputCombo( port1, m_cbInputPort1 );

		CSoftlogic::Port port2 = m_pSoftlogic->getIn2Port();
		UpdateInputCombo( port2, m_cbInputPort2 );


		for( int i=0 ; i<m_cbTrigger.GetCount() ; i++ )
		{
			if( m_pSoftlogic->getLogicTrigger() == (CSoftlogic::Trigger)m_cbTrigger.GetItemData(i) )
			{
				m_cbTrigger.SetCurSel( i );
				break;
			}
		}

		CSoftlogic::PortType type = m_pSoftlogic->getOutPortType();
		for( int i=0 ; i<m_cbOutputPortType.GetCount() ; i++ )
		{
			if( type == (CSoftlogic::PortType)m_cbOutputPortType.GetItemData(i) )
			{
				m_cbOutputPortType.SetCurSel( i );
				UpdateOutputPort( type );
				break;
			}
		}

		switch( type )
		{
		case CSoftlogic::eptError:
			ASSERT( FALSE );
			// ??
			break;
		case CSoftlogic::eptKey:
			{
				BYTE cKey = m_pSoftlogic->getOutKey1();
				BYTE cCondition = m_pSoftlogic->getOutKey1ConditionKeys();


				OnUpdateKeyValue( (WORD)(cCondition<<8)|cKey );
			}
			break;
		case CSoftlogic::eptMouse:
			{
				BYTE cMouseButton = m_pSoftlogic->getOutMouse();
				BYTE cCondition = m_pSoftlogic->getOutKey1ConditionKeys();

				m_wndUserDefinedKey.SetKeyType( CUserDefinedKeyCtrl::KeyTypeMouse );
				m_wndUserDefinedKey.SetKeyValue( (WORD)((cCondition|MM_MOUSE_KEY0_MOUSE)<<8)|cMouseButton );
				m_wndUserDefinedKey.ShowWindow( SW_SHOW );
				/*
				BYTE cMouseButton = m_pSoftlogic->getOutMouse();
				switch( cMouseButton )
				{
				case LBUTTON:
					m_cbOutputPort.SetCurSel( 0 );
					break;
				case RBUTTON:
					m_cbOutputPort.SetCurSel( 1 );
					break;
				case MBUTTON:
					m_cbOutputPort.SetCurSel( 2 );
					break;
				}
				*/
			}
			break;
		case CSoftlogic::eptReport:
			{
				BYTE cReportId = m_pSoftlogic->getReportId();
				CString strReportId;
				strReportId.Format( _T("%d"), cReportId );
				GetDlgItem(IDC_EDIT_VALUE)->SetWindowText( strReportId );
			}
			break;
		case CSoftlogic::eptState:
			if( m_pSoftlogic->getOutPort() == 0x00 )
			{
				m_cbOutputPortType.SetCurSel( 0 );
				m_cbOutputPort.SetCurSel( -1 );
				m_cbOutputPort.EnableWindow( FALSE );
				break;
			}
		default:
			{
				CSoftlogic::Port outPort = m_pSoftlogic->getOutPort();

				m_cbOutputPort.SetCurSel( -1 );
				if ( outPort == CSoftlogic::epNoGate )
				{
					//m_cbOutputPort.SetCurSel(-1);
				}
				else if ( outPort >= CSoftlogic::epStateWorking && outPort <= CSoftlogic::epStateInvertDetection )
				{
					for( int i=0 ; i<m_cbOutputPort.GetCount() ; i++ )
					{
						if( outPort == (CSoftlogic::Port)m_cbOutputPort.GetItemData(i) )
						{
							m_cbOutputPort.SetCurSel( i );
							break;
						}
					}
				}
				else if ( outPort >= CSoftlogic::epGpio0 && outPort < CSoftlogic::epSoftkey0 )
				{
					for( int i=0 ; i<m_cbOutputPort.GetCount() ; i++ )
					{
						if( outPort == (CSoftlogic::Port)m_cbOutputPort.GetItemData(i) )
						{
							m_cbOutputPort.SetCurSel( i );
							break;
						}
					}
				}
				else if ( outPort >= CSoftlogic::epSoftkey0 && outPort < CSoftlogic::epSoftkeyAll )
				{
					m_cbOutputPort.SetCurSel( (int)outPort - (int)CSoftlogic::epSoftkey0 );
					//m_cbOutputPort.SetWindowText( _T("Softkey...") );
				}
				else if ( outPort >= CSoftlogic::epSoftLogic0 )
				{
					m_cbOutputPort.SetWindowText( _T("Softlogic...") );
				}
			}
			break;
		}

		m_cbLogicShowHide.SetCurSel( m_bShowLogic ? 0 : 1 );


		UpdateData( FALSE );
	}
}

void DEditGate::SetLogic( CSoftlogic* pSoftlogic, BOOL bShow )
{
	m_pSoftlogic = pSoftlogic;
	m_bShowLogic = bShow;

	UpdateItems();
}

void DEditGate::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if( !bShow )
	{
		m_pSoftlogic = NULL;
		m_pWndLogicCanvas->Invalidate();
	}
}

BOOL DEditGate::OnInitDialog()
{
	CDialog::OnInitDialog();

	int nCbIdx;
	nCbIdx = m_cbControlPort.AddString( _T("(Empty)") );
	m_cbControlPort.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::epNoGate );
	nCbIdx = m_cbControlPort.AddString( _T("Working") );
	m_cbControlPort.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::epStateWorking );
	nCbIdx = m_cbControlPort.AddString( _T("Touching") );
	m_cbControlPort.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::epStateTouch );
	nCbIdx = m_cbControlPort.AddString( _T("Calibration") );
	m_cbControlPort.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::epStateCalibration );
	nCbIdx = m_cbControlPort.AddString( _T("TouchScreen Enable") );
	m_cbControlPort.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::epStateTouchScreen );
	nCbIdx = m_cbControlPort.AddString( _T("Multi-touch Device") );
	m_cbControlPort.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::epStateMultiTouchDevice );
	nCbIdx = m_cbControlPort.AddString( _T("Invert Detection") );
	m_cbControlPort.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::epStateInvertDetection );

	nCbIdx = m_cbInputPort1.AddString( _T("(Empty)") );
	m_cbInputPort1.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::epNoGate );
	nCbIdx = m_cbInputPort1.AddString( _T("Working") );
	m_cbInputPort1.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::epStateWorking );
	nCbIdx = m_cbInputPort1.AddString( _T("Touching") );
	m_cbInputPort1.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::epStateTouch );
	nCbIdx = m_cbInputPort1.AddString( _T("Calibration") );
	m_cbInputPort1.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::epStateCalibration );
	nCbIdx = m_cbInputPort1.AddString( _T("TouchScreen Enable") );
	m_cbInputPort1.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::epStateTouchScreen );
	nCbIdx = m_cbInputPort1.AddString( _T("Multi-touch Device") );
	m_cbInputPort1.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::epStateMultiTouchDevice );
	nCbIdx = m_cbInputPort1.AddString( _T("Invert Detection") );
	m_cbInputPort1.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::epStateInvertDetection );
	nCbIdx = m_cbInputPort1.AddString( _T("All Softkey") );
	m_cbInputPort1.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::epSoftkeyAll );

	nCbIdx = m_cbInputPort2.AddString( _T("(Empty)") );
	m_cbInputPort2.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::epNoGate );
	nCbIdx = m_cbInputPort2.AddString( _T("Working") );
	m_cbInputPort2.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::epStateWorking );
	nCbIdx = m_cbInputPort2.AddString( _T("Touching") );
	m_cbInputPort2.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::epStateTouch );
	nCbIdx = m_cbInputPort2.AddString( _T("Calibration") );
	m_cbInputPort2.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::epStateCalibration );
	nCbIdx = m_cbInputPort2.AddString( _T("TouchScreen Enable") );
	m_cbInputPort2.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::epStateTouchScreen );
	nCbIdx = m_cbInputPort2.AddString( _T("Multi-touch Device") );
	m_cbInputPort2.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::epStateMultiTouchDevice );
	nCbIdx = m_cbInputPort2.AddString( _T("Invert Detection") );
	m_cbInputPort2.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::epStateInvertDetection );
	nCbIdx = m_cbInputPort2.AddString( _T("All Softkey") );
	m_cbInputPort2.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::epSoftkeyAll );

	CT3kSoftlogicDlg* pDlg = (CT3kSoftlogicDlg*)AfxGetMainWnd();
	CSoftkeyArray& Keys = pDlg->GetKeys();

	for ( int i=0 ; i<Keys.GetGPIOCount() ; i++ )
	{
		GPIOInfo* pInfo = Keys.GetGPIOInfo(i);
		if ( pInfo )
		{
			if (pInfo->bEnable)
			{
				CString strGPIO;
				strGPIO.Format( _T("GPIO %d"), i+1 );
				nCbIdx = m_cbInputPort1.AddString( strGPIO );
				m_cbInputPort1.SetItemData( nCbIdx, (CSoftlogic::Port)(i+(int)CSoftlogic::epGpio0) );
				nCbIdx = m_cbInputPort2.AddString( strGPIO );
				m_cbInputPort2.SetItemData( nCbIdx, (CSoftlogic::Port)(i+(int)CSoftlogic::epGpio0) );
			}
		}
	}

	/*
	for( int i=0 ; i<Keys.GetSize() ; i++ )
	{
		CString strSoftkey;
		strSoftkey.Format( _T("Softkey %d"), i+1 );
		nCbIdx = m_cbInputPort1.AddString( strSoftkey );
		m_cbInputPort1.SetItemData( nCbIdx, (CSoftlogic::Port)(i+(int)CSoftlogic::epSoftkey0) );
		nCbIdx = m_cbInputPort2.AddString( strSoftkey );
		m_cbInputPort2.SetItemData( nCbIdx, (CSoftlogic::Port)(i+(int)CSoftlogic::epSoftkey0) );
	}
	*/

	UpdateOutputPortType( m_nGateType );

	nCbIdx = m_cbTrigger.AddString( _T("(Not Used)") );
	m_cbTrigger.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::etNoTrigger );
//	nCbIdx = m_cbTrigger.AddString( _T("Up  100 ms / Down  100 ms") );
	nCbIdx = m_cbTrigger.AddString( _T("Up  128 ms / Down  128 ms") );
	m_cbTrigger.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::etTrigger_U100_D100 );
//	nCbIdx = m_cbTrigger.AddString( _T("Up  250 ms / Down  250 ms") );
	nCbIdx = m_cbTrigger.AddString( _T("Up  256 ms / Down  256 ms") );
	m_cbTrigger.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::etTrigger_U250_D250 );
//	nCbIdx = m_cbTrigger.AddString( _T("Up  500 ms / Down  500 ms") );
	nCbIdx = m_cbTrigger.AddString( _T("Up  512 ms / Down  512 ms") );
	m_cbTrigger.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::etTrigger_U500_D500 );
//	nCbIdx = m_cbTrigger.AddString( _T("Up 1000 ms / Down 1000 ms") );
	nCbIdx = m_cbTrigger.AddString( _T("Up 1024 ms / Down 1024 ms") );
	m_cbTrigger.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::etTrigger_U1000_D1000 );
//	nCbIdx = m_cbTrigger.AddString( _T("Up 2000 ms / Down 2000 ms") );
	nCbIdx = m_cbTrigger.AddString( _T("Up 2048 ms / Down 2048 ms") );
	m_cbTrigger.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::etTrigger_U2000_D2000 );
//	nCbIdx = m_cbTrigger.AddString( _T("Up  250 ms / Down    0 ms") );
	nCbIdx = m_cbTrigger.AddString( _T("Up  256 ms / Down    0 ms") );
	m_cbTrigger.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::etTrigger_U250_D0 );
//	nCbIdx = m_cbTrigger.AddString( _T("Up  500 ms / Down    0 ms") );
	nCbIdx = m_cbTrigger.AddString( _T("Up  512 ms / Down    0 ms") );
	m_cbTrigger.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::etTrigger_U500_D0 );
//	nCbIdx = m_cbTrigger.AddString( _T("Up 1000 ms / Down    0 ms") );
	nCbIdx = m_cbTrigger.AddString( _T("Up 1024 ms / Down    0 ms") );
	m_cbTrigger.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::etTrigger_U1000_D0 );
//	nCbIdx = m_cbTrigger.AddString( _T("Up 2000 ms / Down    0 ms") );
	nCbIdx = m_cbTrigger.AddString( _T("Up 2048 ms / Down    0 ms") );
	m_cbTrigger.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::etTrigger_U2000_D0 );
//	nCbIdx = m_cbTrigger.AddString( _T("Up 3000 ms / Down    0 ms") );
	nCbIdx = m_cbTrigger.AddString( _T("Up 3072 ms / Down    0 ms") );
	m_cbTrigger.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::etTrigger_U3000_D0 );
//	nCbIdx = m_cbTrigger.AddString( _T("Up  200 ms / Down   50 ms") );
	nCbIdx = m_cbTrigger.AddString( _T("Up  192 ms / Down   64 ms") );
	m_cbTrigger.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::etTrigger_U200_D50 );
//	nCbIdx = m_cbTrigger.AddString( _T("Up  400 ms / Down  100 ms") );
	nCbIdx = m_cbTrigger.AddString( _T("Up  384 ms / Down  128 ms") );
	m_cbTrigger.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::etTrigger_U400_D100 );
//	nCbIdx = m_cbTrigger.AddString( _T("Up  750 ms / Down  250 ms") );
	nCbIdx = m_cbTrigger.AddString( _T("Up  768 ms / Down  256 ms") );
	m_cbTrigger.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::etTrigger_U750_D250 );
//	nCbIdx = m_cbTrigger.AddString( _T("Up 1500 ms / Down  500 ms") );
	nCbIdx = m_cbTrigger.AddString( _T("Up 1536 ms / Down  512 ms") );
	m_cbTrigger.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::etTrigger_U1500_D500 );
//	nCbIdx = m_cbTrigger.AddString( _T("Up 2000 ms / Down 1000 ms") );
	nCbIdx = m_cbTrigger.AddString( _T("Up 2048 ms / Down 1024 ms") );
	m_cbTrigger.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::etTrigger_U2000_D1000 );

	m_wndUserDefinedKey.SetKeyValue( 0x0000 );

	m_cbLogicShowHide.AddString( _T("False") );
	m_cbLogicShowHide.AddString( _T("True") );

	CRect rcEdit;
	GetDlgItem(IDC_COMBO_OUTPUT_PORT)->GetWindowRect( rcEdit );
	ScreenToClient( rcEdit );
	GetDlgItem(IDC_EDIT_VALUE)->MoveWindow( rcEdit );

	UpdateOutputPort( CSoftlogic::eptError );

	CFont* fntDlg = GetFont();
	LOGFONT logFont;
	fntDlg->GetLogFont( &logFont );

	if( !m_fntFixed.GetSafeHandle() )
	{
		m_fntFixed.CreateFont( logFont.lfHeight, 0, 
			0, 0, 0, 
			0, 0, 0, 
			0, 0, 0, 
			0, 0, _T("Lucida Console") );
	}
	m_cbTrigger.SetFont( &m_fntFixed );

	m_edtReportId.SetFloatStyle( FALSE );
	m_edtReportId.SetRange( (double)0, (double)0x3F );

	return TRUE;
}

void DEditGate::UpdateOutputPortType( int nGateType )
{
	int nCbIdx;
	int nCurSel = m_cbOutputPortType.GetCurSel();
	CSoftlogic::PortType oldPortType = CSoftlogic::eptError;
	if( nCurSel >= 0 )
	{
		oldPortType = (CSoftlogic::PortType)m_cbOutputPortType.GetItemData(nCurSel);
	}

	m_cbOutputPortType.ResetContent();

	nCbIdx = m_cbOutputPortType.AddString( _T("(Empty)") );
	m_cbOutputPortType.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::eptError );
	nCbIdx = m_cbOutputPortType.AddString( _T("Keyboard") );
	m_cbOutputPortType.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::eptKey );

#ifdef USE_SOFTLOGIC_OUTPUT_MOUSE
	if( nGateType < 2 ) // AND, AND_RisingEdge
	{
		nCbIdx = m_cbOutputPortType.AddString( _T("Mouse") );
		m_cbOutputPortType.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::eptMouse );
	}
#endif
	nCbIdx = m_cbOutputPortType.AddString( _T("State") );
	m_cbOutputPortType.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::eptState );
	nCbIdx = m_cbOutputPortType.AddString( _T("GPIO") );
	m_cbOutputPortType.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::eptGPIO );
	nCbIdx = m_cbOutputPortType.AddString( _T("Report") );
	m_cbOutputPortType.SetItemData( nCbIdx, (DWORD_PTR)CSoftlogic::eptReport );

	for( int i=0 ; i<m_cbOutputPortType.GetCount() ; i++ )
	{
		if( (CSoftlogic::PortType)m_cbOutputPortType.GetItemData(i) == oldPortType )
		{
			if( oldPortType == CSoftlogic::eptError )
			{
				m_cbOutputPortType.SetCurSel( -1 );
				m_cbOutputPort.SetCurSel( -1 );
				UpdateOutputPort( CSoftlogic::eptError );
			}
			else
			{
				m_cbOutputPortType.SetCurSel( i );
			}
			return;
		}
	}
	m_cbOutputPort.SetCurSel( -1 );
	UpdateOutputPort( CSoftlogic::eptError );
}

void DEditGate::UpdateOutputPort( CSoftlogic::PortType type )
{
	m_cbOutputPort.ResetContent();

	if( type == CSoftlogic::eptError ) // (Empty)
	{
		GetDlgItem(IDC_EDIT_VALUE)->ShowWindow( SW_HIDE );
		GetDlgItem(IDC_STATIC_KEY_VALUE)->ShowWindow( SW_HIDE );
		GetDlgItem(IDC_STATIC_EDIT_KEY)->ShowWindow( SW_HIDE );
		m_cbOutputPort.ShowWindow( SW_SHOW );
		m_cbOutputPort.EnableWindow(FALSE);
		m_cbControlPort.EnableWindow(TRUE);
		return;
	}

	/*
	if( type == CSoftlogic::eptKey ) // Keyboard
	{
		//m_cbOutputPort.ShowWindow( SW_HIDE );
		GetDlgItem(IDC_EDIT_VALUE)->ShowWindow( SW_HIDE );
		GetDlgItem(IDC_STATIC_EDIT_KEY)->ShowWindow( SW_HIDE );
		m_cbControlPort.EnableWindow(FALSE);
		return;
	}
	*/
	if( type == CSoftlogic::eptReport ) // Report
	{
		m_cbOutputPort.ShowWindow( SW_HIDE );
		GetDlgItem(IDC_STATIC_KEY_VALUE)->ShowWindow( SW_HIDE );
		GetDlgItem(IDC_STATIC_EDIT_KEY)->ShowWindow( SW_HIDE );
		GetDlgItem(IDC_EDIT_VALUE)->ShowWindow( SW_SHOW );
		m_cbControlPort.EnableWindow(TRUE);
		return;
	}

	GetDlgItem(IDC_STATIC_KEY_VALUE)->ShowWindow( SW_HIDE );
	GetDlgItem(IDC_EDIT_VALUE)->ShowWindow( SW_HIDE );
	GetDlgItem(IDC_STATIC_EDIT_KEY)->ShowWindow( SW_HIDE );
	m_cbOutputPort.EnableWindow(TRUE);
	m_cbOutputPort.ShowWindow( SW_SHOW );

	int nCbIdx;
	switch( type )
	{
	case CSoftlogic::eptKey:	// Keyboard
		nCbIdx = m_cbOutputPort.AddString( _T("(User Defined)") );
		m_cbOutputPort.SetItemData( nCbIdx, (DWORD_PTR)0xFFFF );
		nCbIdx = m_cbOutputPort.AddString( _T("(Function Key)") );
		m_cbOutputPort.SetItemData( nCbIdx, (DWORD_PTR)0xFFFF );
		nCbIdx = m_cbOutputPort.AddString( _T("Task Switcher") );
		m_cbOutputPort.SetItemData( nCbIdx, (DWORD_PTR)0x052B );
		nCbIdx = m_cbOutputPort.AddString( _T("Task Switcher(Aero)") );
		m_cbOutputPort.SetItemData( nCbIdx, (DWORD_PTR)0x092B );
		nCbIdx = m_cbOutputPort.AddString( _T("Task Manager") );
		m_cbOutputPort.SetItemData( nCbIdx, (DWORD_PTR)0x0329 );
		nCbIdx = m_cbOutputPort.AddString( _T("Explorer") );
		m_cbOutputPort.SetItemData( nCbIdx, (DWORD_PTR)0x0808 );
		nCbIdx = m_cbOutputPort.AddString( _T("Desktop") );
		m_cbOutputPort.SetItemData( nCbIdx, (DWORD_PTR)0x0807 );
		nCbIdx = m_cbOutputPort.AddString( _T("Escape") );
		m_cbOutputPort.SetItemData( nCbIdx, (DWORD_PTR)0x0029 );
#ifdef USE_SOFTLOGIC_OUTPUT_AUDIO_CONTROL
		nCbIdx = m_cbOutputPort.AddString( _T("Audio Volume Inc.") );
		m_cbOutputPort.SetItemData( nCbIdx, (DWORD_PTR)V_KEY_VOLUME_INC_VALUE );
		nCbIdx = m_cbOutputPort.AddString( _T("Audio Volume Dec.") );
		m_cbOutputPort.SetItemData( nCbIdx, (DWORD_PTR)V_KEY_VOLUME_DEC_VALUE );
		nCbIdx = m_cbOutputPort.AddString( _T("Audio Mute") );
		m_cbOutputPort.SetItemData( nCbIdx, (DWORD_PTR)V_KEY_VOLUME_MUTE_VALUE );

		nCbIdx = m_cbOutputPort.AddString( _T("MM Play") );
		m_cbOutputPort.SetItemData( nCbIdx, (DWORD_PTR)V_KEY_TRANSPORT_PLAY );
		nCbIdx = m_cbOutputPort.AddString( _T("MM Pause") );
		m_cbOutputPort.SetItemData( nCbIdx, (DWORD_PTR)V_KEY_TRANSPORT_PAUSE );
		nCbIdx = m_cbOutputPort.AddString( _T("MM FForward") );
		m_cbOutputPort.SetItemData( nCbIdx, (DWORD_PTR)V_KEY_TRANSPORT_FFORWARD );
		nCbIdx = m_cbOutputPort.AddString( _T("MM Rewind") );
		m_cbOutputPort.SetItemData( nCbIdx, (DWORD_PTR)V_KEY_TRANSPORT_REWIND );
		nCbIdx = m_cbOutputPort.AddString( _T("MM Next Track") );
		m_cbOutputPort.SetItemData( nCbIdx, (DWORD_PTR)V_KEY_TRANSPORT_NEXT_TRACK );
		nCbIdx = m_cbOutputPort.AddString( _T("MM Prev Track") );
		m_cbOutputPort.SetItemData( nCbIdx, (DWORD_PTR)V_KEY_TRANSPORT_PREV_TRACK );
		nCbIdx = m_cbOutputPort.AddString( _T("MM Stop") );
		m_cbOutputPort.SetItemData( nCbIdx, (DWORD_PTR)V_KEY_TRANSPORT_STOP );
		nCbIdx = m_cbOutputPort.AddString( _T("MM Eject") );
		m_cbOutputPort.SetItemData( nCbIdx, (DWORD_PTR)V_KEY_TRANSPORT_EJECT );
		nCbIdx = m_cbOutputPort.AddString( _T("MM Play/Pause") );
		m_cbOutputPort.SetItemData( nCbIdx, (DWORD_PTR)V_KEY_TRANSPORT_PLAYPAUSE );

		nCbIdx = m_cbOutputPort.AddString( _T("AL E-mail Viewer") );
		m_cbOutputPort.SetItemData( nCbIdx, (DWORD_PTR)V_KEY_AL_EMAIL_VIEWER );
		nCbIdx = m_cbOutputPort.AddString( _T("AL Calculator") );
		m_cbOutputPort.SetItemData( nCbIdx, (DWORD_PTR)V_KEY_AL_CALCULATOR );
		nCbIdx = m_cbOutputPort.AddString( _T("AL My Computer") );
		m_cbOutputPort.SetItemData( nCbIdx, (DWORD_PTR)V_KEY_AL_MY_COMPUTER );
		nCbIdx = m_cbOutputPort.AddString( _T("AL Web Browser") );
		m_cbOutputPort.SetItemData( nCbIdx, (DWORD_PTR)V_KEY_AL_WEB_BROWSER );		
#endif

		m_cbControlPort.EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_KEY_VALUE)->ShowWindow( SW_SHOW );

		OnUpdateKeyValue( (WORD)(0x00) );

		break;
	case CSoftlogic::eptMouse:	// Mouse
		m_cbOutputPort.ShowWindow( SW_HIDE );
		m_wndUserDefinedKey.Reset();
		m_wndUserDefinedKey.SetKeyType( CUserDefinedKeyCtrl::KeyTypeMouse );
		m_wndUserDefinedKey.ShowWindow( SW_SHOW );

		/*
		nCbIdx = m_cbOutputPort.AddString( _T("Left Button") );
		m_cbOutputPort.SetItemData( nCbIdx, LBUTTON );
		nCbIdx = m_cbOutputPort.AddString( _T("Right Button") );
		m_cbOutputPort.SetItemData( nCbIdx, RBUTTON );
		nCbIdx = m_cbOutputPort.AddString( _T("Middle Button") );
		m_cbOutputPort.SetItemData( nCbIdx, MBUTTON );
		*/
		m_cbControlPort.EnableWindow(FALSE);
		break;
	case CSoftlogic::eptState:	// State
		nCbIdx = m_cbOutputPort.AddString( _T("Beep #1") ); //Working
		m_cbOutputPort.SetItemData( nCbIdx, CSoftlogic::epStateWorking );
		nCbIdx = m_cbOutputPort.AddString( _T("Beep #2") ); //Touch
		m_cbOutputPort.SetItemData( nCbIdx, CSoftlogic::epStateTouch );
		nCbIdx = m_cbOutputPort.AddString( _T("Calibration") );
		m_cbOutputPort.SetItemData( nCbIdx, CSoftlogic::epStateCalibration );
		nCbIdx = m_cbOutputPort.AddString( _T("TouchScreen Enable") );
		m_cbOutputPort.SetItemData( nCbIdx, CSoftlogic::epStateTouchScreen );
		nCbIdx = m_cbOutputPort.AddString( _T("Multi-touch Device") );
		m_cbOutputPort.SetItemData( nCbIdx, CSoftlogic::epStateMultiTouchDevice );
		nCbIdx = m_cbOutputPort.AddString( _T("Invert Detection") );
		m_cbOutputPort.SetItemData( nCbIdx, CSoftlogic::epStateInvertDetection );
		m_cbControlPort.EnableWindow(TRUE);
		break;
	case CSoftlogic::eptGPIO:	// GPIO
		{
			CT3kSoftlogicDlg* pDlg = (CT3kSoftlogicDlg*)AfxGetMainWnd();
			CSoftkeyArray& Keys = pDlg->GetKeys();

			BOOL bEnableOutputGPIO = FALSE;
			for ( int i=0 ; i<Keys.GetGPIOCount() ; i++ )
			{
				GPIOInfo* pInfo = Keys.GetGPIOInfo(i);
				if ( pInfo )
				{
					if (pInfo->bEnable && pInfo->bOutput)
					{
						CString strGPIO;
						strGPIO.Format( _T("GPIO %d"), i+1 );
						nCbIdx = m_cbOutputPort.AddString( strGPIO );
						m_cbOutputPort.SetItemData( nCbIdx, (CSoftlogic::Port)(i+(int)CSoftlogic::epGpio0) );
						bEnableOutputGPIO = TRUE;
					}
				}
			}
			m_cbOutputPort.EnableWindow( bEnableOutputGPIO );
		}
		m_cbControlPort.EnableWindow(TRUE);
		break;
	case CSoftlogic::eptSoftkey: // Softkey
		for( int i=0 ; i<SOFT_KEY_MAX ; i++ )
		{
			CString strSoftkey;
			strSoftkey.Format( _T("Softkey %d"), i+1 );
			nCbIdx = m_cbOutputPort.AddString( strSoftkey );
			m_cbOutputPort.SetItemData( nCbIdx, (CSoftlogic::Port)(i+(int)CSoftlogic::epSoftkey0) );
		}
		m_cbControlPort.EnableWindow(TRUE);
		break;
	}

	m_cbOutputPort.SetCurSel( 0 );
}

void DEditGate::OnCbnSelchangeComboControlPort()
{
	GetDlgItem(IDC_CHECK_INVERT_CONTROL_PORT)->EnableWindow( m_cbControlPort.GetCurSel() == 0 ? FALSE : TRUE );
	ApplyItems();
	m_pWndLogicCanvas->Invalidate();
}

void DEditGate::OnCbnSelchangeComboControlTrigger()
{
	ApplyItems();
	m_pWndLogicCanvas->Invalidate();
}

void DEditGate::OnCbnSelchangeComboOutputType()
{
	int nPortType = m_cbOutputPortType.GetCurSel();

	if( nPortType < 0 ) return;

	CSoftlogic::PortType type = (CSoftlogic::PortType)m_cbOutputPortType.GetItemData(nPortType);

	UpdateOutputPort( type );
	ApplyItems();
	m_pWndLogicCanvas->Invalidate();
}

void DEditGate::OnCbnSelchangeComboInputPort1()
{
	ApplyItems();
	m_pWndLogicCanvas->Invalidate();
}

void DEditGate::OnCbnSelchangeComboInputPort2()
{
	ApplyItems();
	m_pWndLogicCanvas->Invalidate();
}

void DEditGate::DisplayKeyValue( WORD wKeyValue )
{
	BYTE cV0 = (BYTE)(wKeyValue >> 8);
	BYTE cV1 = (BYTE)(wKeyValue & 0xFF);

	if( (cV0 & MM_MOUSE_KEY0_MOUSE) || (wKeyValue == 0x0000) )
	{
		SetDlgItemText( IDC_STATIC_KEY_VALUE, _T("") );
		return;
	}

	CString strKey;
	if( cV0 & MM_MOUSE_KEY0_CTRL )
	{
		if( !strKey.IsEmpty() )
			strKey += _T("+");
		else
			strKey += _T("\"");
		strKey += _T("Ctrl");
	}

	if( cV0 & MM_MOUSE_KEY0_ALT )
	{
		if( !strKey.IsEmpty() )
			strKey += _T("+");
		else
			strKey += _T("\"");
		strKey += _T("Alt");
	}

	if( cV0 & MM_MOUSE_KEY0_SHIFT )
	{
		if( !strKey.IsEmpty() )
			strKey += _T("+");
		else
			strKey += _T("\"");
		strKey += _T("Shift");
	}	

	if( cV0 & MM_MOUSE_KEY0_WINDOW )
	{
		if( !strKey.IsEmpty() )
			strKey += _T("+");
		else
			strKey += _T("\"");
		strKey += _T("Win");
	}	
		
	if( !strKey.IsEmpty() )
		strKey += _T("+");
	else
		strKey += _T("\"");

	int nScanCode;
	if( FindScanCode(cV1, nScanCode) )
	{	
		int nVKey = ::MapVirtualKey(nScanCode, 1);

		CString strK;
		strK = VirtualKeyToString( nVKey );

		strKey += strK;
	}

	strKey += _T("\"");

	SetDlgItemText( IDC_STATIC_KEY_VALUE, strKey );
}

void DEditGate::OnCbnSelchangeComboOutputPort()
{
	int nPortType = m_cbOutputPortType.GetCurSel();

	if( nPortType < 0 ) return;

	CSoftlogic::PortType type = (CSoftlogic::PortType)m_cbOutputPortType.GetItemData(nPortType);

	if( type == CSoftlogic::eptKey )
	{
		int nSelKey = m_cbOutputPort.GetCurSel();
		if( nSelKey < 0 ) return;

		WORD wKeyValue = (WORD)m_cbOutputPort.GetItemData(nSelKey);

		DisplayKeyValue( wKeyValue );

		if( nSelKey == 0 )
		{
			SetEditMode( ModeUserDefined, wKeyValue );
		}
		else if( nSelKey == 1 )
		{
			SetEditMode( ModeFunctionKey, wKeyValue );
		}
		else
		{
			SetEditMode( ModeNone, wKeyValue );
		}
	}


	ApplyItems();
	m_pWndLogicCanvas->Invalidate();
}

void DEditGate::OnBnClickedRadioGateType()
{
	UpdateData( TRUE );
	UpdateOutputPortType( m_nGateType );

	ApplyItems();
	m_pWndLogicCanvas->Invalidate();
}

void DEditGate::OnBnClickedCheckInvert()
{
	ApplyItems();
	m_pWndLogicCanvas->Invalidate();
}

void DEditGate::OnCbnSelchangeComboShowhide()
{
	ApplyHiddenLogics();
	m_pWndLogicCanvas->Invalidate();
}

LRESULT DEditGate::OnChangeKeyEdit( WPARAM wParam, LPARAM lParam )
{
	DisplayKeyValue( (WORD)wParam );
	ApplyItems();
	m_pWndLogicCanvas->Invalidate();
	return 0;
}

LRESULT DEditGate::OnChangeReportIdEdit( WPARAM wParam, LPARAM lParam )
{
	ApplyItems();
	m_pWndLogicCanvas->Invalidate();
	return 0;
}

void DEditGate::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}

BOOL DEditGate::OnEraseBkgnd(CDC* pDC)
{
	CRect rcClient;
	GetClientRect( rcClient );

	//pDC->FillSolidRect( rcClient, RGB(255, 255, 255) );

	CDialog::OnEraseBkgnd(pDC);

	CRect rcGateBody;
	GetDlgItem(IDC_DRAW_GATE)->GetWindowRect( rcGateBody );
	ScreenToClient( rcGateBody );
	DrawGate( pDC, rcGateBody, m_nGateType );

	return TRUE;
}

void DEditGate::DrawGate( CDC* pDC, CRect rcBody, int nGateType )
{
	const int nPinLength = 15;

	CRect rcGate = rcBody;
	rcGate.DeflateRect( nPinLength, nPinLength, nPinLength, nPinLength );

	int nOH = rcGate.Height() / 4;

	CPen penGate( PS_SOLID, 2, RGB(50, 50, 50) );

	CPen* pOldPen = pDC->SelectObject( &penGate );

	pDC->Rectangle( rcGate );

	// draw pins
	pDC->MoveTo( rcGate.CenterPoint().x, rcBody.top );
	pDC->LineTo( rcGate.CenterPoint().x, rcGate.top );

	pDC->MoveTo( rcBody.left, rcGate.top+nOH );
	pDC->LineTo( rcGate.left, rcGate.top+nOH );

	pDC->MoveTo( rcBody.left, rcGate.bottom-nOH );
	pDC->LineTo( rcGate.left, rcGate.bottom-nOH );

	pDC->MoveTo( rcBody.right, rcGate.CenterPoint().y );
	pDC->LineTo( rcGate.right, rcGate.CenterPoint().y );

	pDC->SelectObject( pOldPen );
}

int DEditGate::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	CString strPos = AfxGetApp()->GetProfileString(_T("Windows"), _T("EditGate_Pos"));
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

void DEditGate::OnDestroy()
{
	CDialog::OnDestroy();

	WINDOWPLACEMENT wp;
	GetWindowPlacement(&wp);
	CString str;
	str.Format(_T("%d,%d,%d,%d"),
		wp.rcNormalPosition.left, wp.rcNormalPosition.top,
		wp.rcNormalPosition.right, wp.rcNormalPosition.bottom);
	AfxGetApp()->WriteProfileString(_T("Windows"), _T("EditGate_Pos"), str);
}

HBRUSH DEditGate::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	switch( (short)pWnd->GetDlgCtrlID() )
	{
	case IDC_STATIC:
	//case IDC_CHECK_INVERT_CONTROL_PORT:
	//case IDC_CHECK_INVERT_INPUT_PORT1:
	//case IDC_CHECK_INVERT_INPUT_PORT2:
		pDC->SetBkMode( TRANSPARENT );
		return (HBRUSH)GetStockObject(NULL_BRUSH);
		break;
	case IDC_RADIO_GATE_TYPE1:
	case IDC_RADIO_GATE_TYPE2:
	case IDC_RADIO_GATE_TYPE3:
	case IDC_RADIO_GATE_TYPE4:
		pDC->SetBkMode( TRANSPARENT );
		return (HBRUSH)GetStockObject(WHITE_BRUSH);
	}
	return hbr;
}
