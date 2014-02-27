#include "EditGateWidget.h"
#include "ui_EditGateWidget.h"

//#include "LogicDesignerWnd.h"
#include "T3kCommonData.h"
#include "QKeyMapStr.h"

#include <QSettings>
#include <QPainter>
#include <QDesktopWidget>


QEditGateWidget::QEditGateWidget(QWidget* parent /*=NULL*/) :
    QDialog(parent),
    ui(new Ui::EditGateWidget)
{
    ui->setupUi(this);
	m_pSoftlogic = NULL;
    m_bShowLogic = true;

    m_nGateType = -1;

    ui->CBEnablePort->setCurrentIndex( -1 );
    ui->CBInputPort1->setCurrentIndex( -1 );
    ui->CBInputPort2->setCurrentIndex( -1 );
    ui->CBOutputType->setCurrentIndex( -1 );
    ui->CBOutputPort->setCurrentIndex( -1 );
    ui->CBTrigger->setCurrentIndex( -1 );
    ui->CBHidden->setCurrentIndex( -1 );

    ui->EditReport->setInputMethodHints( Qt::ImhPreferNumbers );

    connect( ui->ChkEnableInvertPort, &QCheckBox::clicked, this, &QEditGateWidget::onChkInvert );
    connect( ui->ChkInvertPort1, &QCheckBox::clicked, this, &QEditGateWidget::onChkInvert );
    connect( ui->ChkInvertPort2, &QCheckBox::clicked, this, &QEditGateWidget::onChkInvert );
    connect( ui->LBEditKey, &QUserDefinedKeyWidget::editChangedKey, this, &QEditGateWidget::onEditChangedKey );
    connect( ui->EditReport, &QLineEdit::editingFinished, this, &QEditGateWidget::onEditReportFinished );

    setWindowFlags( Qt::Tool );
    setWindowModality( Qt::NonModal );
    setModal( false );

    QDesktopWidget desktop;
    QRect rcScreen( desktop.screenGeometry( desktop.primaryScreen() ) );

    QSettings settings( "Habilience", "T3kSoftlogic" );
    settings.beginGroup( "Windows" );
    QString strPos = settings.value( "EditGate_Pos" ).toString();
    if ( !strPos.isEmpty() )
    {
        QRect rcWin;
        QString str;
        int nD;
        do
        {
            nD = strPos.indexOf(',');
            if ( nD <= 0 ) break;
            str = strPos.left(nD); strPos = strPos.mid(nD + 1);
            rcWin.setLeft( str.toLong() );
            nD = strPos.indexOf(',');
            if ( nD <= 0 ) break;
            str = strPos.left(nD); strPos = strPos.mid(nD + 1);
            rcWin.setTop( str.toLong() );
            nD = strPos.indexOf(',');
            if ( nD <= 0 ) break;
            str = strPos.left(nD); strPos = strPos.mid(nD + 1);
            rcWin.setRight( str.toLong() );
            rcWin.setBottom( strPos.toLong() );
            rcWin = rcWin.normalized();

            if ( rcScreen.intersects( rcWin ) )
                move( rcWin.left(), rcWin.top() );
            else
                move( rcScreen.center() - QRect(0,0,width(),height()).center() );
        }
        while ( false );
    }
    else
    {
        move( rcScreen.center() - QRect(0,0,width(),height()).center() );
    }

    ui->CBEnablePort->addItem( "(Empty)", CSoftlogic::epNoGate );
    ui->CBEnablePort->addItem( "Working", CSoftlogic::epStateWorking );
    ui->CBEnablePort->addItem( "Touching", CSoftlogic::epStateTouch );
    ui->CBEnablePort->addItem( "Calibration", CSoftlogic::epStateCalibration );
    ui->CBEnablePort->addItem( "TouchScreen Enable", CSoftlogic::epStateTouchScreen );
    ui->CBEnablePort->addItem( "Multi-touch Device", CSoftlogic::epStateMultiTouchDevice );
    ui->CBEnablePort->addItem( "Invert Detection", CSoftlogic::epStateInvertDetection );

    ui->CBInputPort1->addItem( "(Empty)", CSoftlogic::epNoGate );
    ui->CBInputPort1->addItem( "Working", CSoftlogic::epStateWorking );
    ui->CBInputPort1->addItem( "Touching", CSoftlogic::epStateTouch );
    ui->CBInputPort1->addItem( "Calibration", CSoftlogic::epStateCalibration );
    ui->CBInputPort1->addItem( "TouchScreen Enable", CSoftlogic::epStateTouchScreen );
    ui->CBInputPort1->addItem( "Multi-touch Device", CSoftlogic::epStateMultiTouchDevice );
    ui->CBInputPort1->addItem( "Invert Detection", CSoftlogic::epStateInvertDetection );
    ui->CBInputPort1->addItem( "All Softkey", CSoftlogic::epSoftkeyAll );

    ui->CBInputPort2->addItem( "(Empty)", CSoftlogic::epNoGate );
    ui->CBInputPort2->addItem( "Working", CSoftlogic::epStateWorking );
    ui->CBInputPort2->addItem( "Touching", CSoftlogic::epStateTouch );
    ui->CBInputPort2->addItem( "Calibration", CSoftlogic::epStateCalibration );
    ui->CBInputPort2->addItem( "TouchScreen Enable", CSoftlogic::epStateTouchScreen );
    ui->CBInputPort2->addItem( "Multi-touch Device", CSoftlogic::epStateMultiTouchDevice );
    ui->CBInputPort2->addItem( "Invert Detection", CSoftlogic::epStateInvertDetection );
    ui->CBInputPort2->addItem( "All Softkey", CSoftlogic::epSoftkeyAll );

    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

    for ( int i=0 ; i<Keys.getGPIOCount() ; i++ )
    {
        GPIOInfo* pInfo = Keys.getGPIOInfo(i);
        if ( pInfo )
        {
            if (pInfo->bEnable)
            {
                QString strGPIO = QString("GPIO %1").arg(i+1);
                ui->CBInputPort1->addItem( strGPIO, (CSoftlogic::Port)(i+(int)CSoftlogic::epGpio0) );
                ui->CBInputPort2->addItem( strGPIO, (CSoftlogic::Port)(i+(int)CSoftlogic::epGpio0) );
            }
        }
    }

    updateOutputPortType( m_nGateType );

    ui->CBTrigger->addItem( "(Not Used)", CSoftlogic::etNoTrigger );
    ui->CBTrigger->addItem( "Up  128 ms / Down  128 ms", CSoftlogic::etTrigger_U128_D128 );
    ui->CBTrigger->addItem( "Up  256 ms / Down  256 ms", CSoftlogic::etTrigger_U256_D256 );
    ui->CBTrigger->addItem( "Up  512 ms / Down  512 ms", CSoftlogic::etTrigger_U512_D512 );
    ui->CBTrigger->addItem( "Up 1024 ms / Down 1024 ms", CSoftlogic::etTrigger_U1024_D1024 );
    ui->CBTrigger->addItem( "Up 2048 ms / Down 2048 ms", CSoftlogic::etTrigger_U2048_D2048 );
    ui->CBTrigger->addItem( "Up  256 ms / Down    0 ms", CSoftlogic::etTrigger_U256_D0 );
    ui->CBTrigger->addItem( "Up  512 ms / Down    0 ms", CSoftlogic::etTrigger_U512_D0 );
    ui->CBTrigger->addItem( "Up 1024 ms / Down    0 ms", CSoftlogic::etTrigger_U1024_D0 );
    ui->CBTrigger->addItem( "Up 2048 ms / Down    0 ms", CSoftlogic::etTrigger_U2048_D0 );
    ui->CBTrigger->addItem( "Up 3072 ms / Down    0 ms", CSoftlogic::etTrigger_U3072_D0 );
    ui->CBTrigger->addItem( "Up  192 ms / Down   64 ms", CSoftlogic::etTrigger_U192_D64 );
    ui->CBTrigger->addItem( "Up  384 ms / Down  128 ms", CSoftlogic::etTrigger_U384_D128 );
    ui->CBTrigger->addItem( "Up  768 ms / Down  256 ms", CSoftlogic::etTrigger_U768_D256 );
    ui->CBTrigger->addItem( "Up 1536 ms / Down  512 ms", CSoftlogic::etTrigger_U1536_D512 );
    ui->CBTrigger->addItem( "Up 2048 ms / Down 1024 ms", CSoftlogic::etTrigger_U2048_D1024 );

    ui->LBEditKey->setKeyValue( 0x0000 );

    ui->CBHidden->addItem( "False" );
    ui->CBHidden->addItem( "True" );

    QRect rcEdit( ui->CBOutputPort->x(), ui->CBOutputPort->y(), ui->CBOutputPort->width(), ui->CBOutputPort->height() );
//    ScreenToClient( rcEdit );
    ui->EditReport->setGeometry( rcEdit );

    updateOutputPort( CSoftlogic::eptError );

    QFont fntDlg( font() );
    fntDlg.setFamily( "Lucida Console" );

    ui->CBTrigger->setFont( fntDlg );

//    ui->EditReport->.SetFloatStyle( FALSE );
//    m_edtReportId.SetRange( (double)0, (double)0x3F );
    ui->EditReport->setText( "0" );
}

QEditGateWidget::~QEditGateWidget()
{
    delete ui;
}

void QEditGateWidget::showEvent(QShowEvent *)
{
    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

    int nCbIdx = -1;
    while( (nCbIdx = ui->CBInputPort1->findText( "GPIO", Qt::MatchStartsWith )) >= 0 )
    {
        ui->CBInputPort1->removeItem( nCbIdx );
    }
    while( (nCbIdx = ui->CBInputPort2->findText( "GPIO", Qt::MatchStartsWith )) >= 0 )
    {
        ui->CBInputPort2->removeItem( nCbIdx );
    }

    for ( int i=0 ; i<Keys.getGPIOCount() ; i++ )
    {
        GPIOInfo* pInfo = Keys.getGPIOInfo(i);
        if ( pInfo )
        {
            if (pInfo->bEnable)
            {
                QString strGPIO = QString("GPIO %1").arg(i+1);
                ui->CBInputPort1->addItem( strGPIO, (CSoftlogic::Port)(i+(int)CSoftlogic::epGpio0) );
                ui->CBInputPort2->addItem( strGPIO, (CSoftlogic::Port)(i+(int)CSoftlogic::epGpio0) );
            }
        }
    }
}

void QEditGateWidget::closeEvent(QCloseEvent *)
{
    hide();

    m_pSoftlogic = NULL;

    QRect rc( x(), y(), width(), height() );
    QString str = QString("%1,%2,%3,%4").arg(rc.left()).arg(rc.top()).arg(rc.right()).arg(rc.bottom());
    QSettings settings( "Habilience", "T3kSoftlogic" );
    settings.beginGroup( "Windows" );
    settings.setValue( "EditGate_Pos", str );
    settings.endGroup();

    emit updateLogic();
}

void QEditGateWidget::applyItems()
{
	if( !m_pSoftlogic )
		return;

	int nSelIdx;

    if( (nSelIdx = ui->CBOutputType->currentIndex()) >= 0 )
	{
        CSoftlogic::PortType type = (CSoftlogic::PortType)ui->CBOutputType->itemData( nSelIdx ).toInt();

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
                nSelIdx = ui->CBOutputPort->currentIndex();
				switch( nSelIdx )
				{
				case 0: // user defined
					{
                        ui->LBEditKey->setKeyType( QUserDefinedKeyWidget::KeyTypeNormal );
                        ushort wKeyValue = ui->LBEditKey->getKeyValue();
                        ui->CBOutputPort->setItemData( 0, wKeyValue );
                        ui->CBOutputPort->setItemData( 1, wKeyValue );
					}
					break;
				case 1: // function key
					{
                        ui->LBEditKey->setKeyType( QUserDefinedKeyWidget::KeyTypeFunction );
                        ushort wKeyValue = ui->LBEditKey->getKeyValue();
                        ui->CBOutputPort->setItemData( 0, wKeyValue );
                        ui->CBOutputPort->setItemData( 1, wKeyValue );
					}
					break;
				}

                ushort wKeyValue = (ushort)ui->CBOutputPort->itemData( nSelIdx ).toUInt();
                uchar cCondition = wKeyValue >> 8;
                uchar cKey = wKeyValue & 0xFF;
				if( !m_pSoftlogic->setOutKey1( cCondition, cKey ) )
					goto goto_ReloadItems;
			}
			break;
		case CSoftlogic::eptMouse:
			{
                ui->LBEditKey->setKeyType( QUserDefinedKeyWidget::KeyTypeMouse );
                ushort wKeyValue = ui->LBEditKey->getKeyValue();
                uchar cCondition = wKeyValue >> 8;
                uchar cButton = wKeyValue & 0xFF;
                if( !m_pSoftlogic->setOutMouse( cCondition, cButton ) )
                    goto goto_ReloadItems;
			}
			break;
		case CSoftlogic::eptReport:
			{
                QString strReportId( ui->EditReport->text() );
                if( !m_pSoftlogic->setReportId( (uchar)strReportId.toInt() ) )
					goto goto_ReloadItems;
			}
			break;
		case CSoftlogic::eptState:
        case CSoftlogic::eptGPIO:
        case CSoftlogic::eptSoftkey:
			{
                if( (nSelIdx = ui->CBOutputPort->currentIndex()) >= 0 )
				{
                    CSoftlogic::Port port = (CSoftlogic::Port)ui->CBOutputPort->itemData(nSelIdx).toInt();
					if( !m_pSoftlogic->setOutPort( port ) )
						goto goto_ReloadItems;
				}
			}
			break;
		default:
			{
//                QString strText( ui->CBOutputPort->currentText() );
//                if( strText.compare( "Softlogic..." ) == 0 )
//				{
//				}
			}
			break;
		}
	}

    if( (nSelIdx = ui->CBOutputType->currentIndex()) >= 0 )
	{
        CSoftlogic::PortType type = (CSoftlogic::PortType)ui->CBOutputType->itemData( nSelIdx ).toInt();
		if ( type != CSoftlogic::eptKey && type != CSoftlogic::eptMouse )
		{
            if( (nSelIdx = ui->CBEnablePort->currentIndex()) >= 0 )
			{
                CSoftlogic::Port port = (CSoftlogic::Port)ui->CBEnablePort->itemData(nSelIdx).toInt();

				if( !m_pSoftlogic->setEnablePort( port ) )
					goto goto_ReloadItems;
			}

            if( !m_pSoftlogic->setEnableNot( ui->ChkEnableInvertPort->isChecked() ) )
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

    if( !m_pSoftlogic->setIn1Not( ui->ChkInvertPort1->isChecked() ) )
		goto goto_ReloadItems;
    if( !m_pSoftlogic->setIn2Not( ui->ChkInvertPort2->isChecked() ) )
		goto goto_ReloadItems;

    if( (nSelIdx = ui->CBInputPort1->currentIndex()) >= 0 )
	{
        CSoftlogic::Port port = (CSoftlogic::Port)ui->CBInputPort1->itemData( nSelIdx ).toInt();
		if( !m_pSoftlogic->setIn1Port( port ) )
			goto goto_ReloadItems;
	}
    if( (nSelIdx = ui->CBInputPort2->currentIndex()) >= 0 )
	{
        CSoftlogic::Port port = (CSoftlogic::Port)ui->CBInputPort2->itemData( nSelIdx ).toInt();
		if( !m_pSoftlogic->setIn2Port( port ) )
			goto goto_ReloadItems;
	}

    if( (nSelIdx = ui->CBTrigger->currentIndex()) >= 0 )
	{
        CSoftlogic::Trigger trigger = (CSoftlogic::Trigger)ui->CBTrigger->itemData(nSelIdx).toInt();

		if( !m_pSoftlogic->setLogicTrigger( trigger ) )
			goto goto_ReloadItems;

	}

    applyHiddenLogics();

	return;

goto_ReloadItems:
    updateItems();
}

void QEditGateWidget::applyHiddenLogics()
{
	if( !m_pSoftlogic )
		return;

    CSoftlogicArray& Logics = T3kCommonData::instance()->getLogics();
    int nIdx = Logics.indexFromSoftlogic( m_pSoftlogic );

    bool bShowLogic;
    if( ui->CBHidden->currentIndex() == 0 )
    {
        bShowLogic = true;
    }
    else
    {
        bShowLogic = false;
    }
	if( m_bShowLogic != bShowLogic )
	{
        Logics.hideSoftlogic( nIdx, !bShowLogic );
        nIdx = Logics.indexFromSoftlogic( m_pSoftlogic );
        m_bShowLogic = Logics.isHiddenLogic(nIdx);
	}
}

void QEditGateWidget::onUpdateKeyValue( ushort wKeyValue )
{
    displayKeyValue( wKeyValue );

    bool bFound = false;
	int nSelIndex = -1;
    for( int nI = 0 ; nI < ui->CBOutputPort->count() ; nI ++ )
	{
        ushort wItemValue = (ushort)ui->CBOutputPort->itemData(nI).toUInt();
		if( wItemValue == wKeyValue )
		{
            ui->CBOutputPort->setCurrentIndex( nI );
			nSelIndex = nI;
            bFound = true;
			break;
		}
	}

	if( !bFound )
	{
        bool bFunctionKey = false;
		if( !(wKeyValue >> 8 & 0x80) )
		{
			int nScanCode;
            if( findScanCode( wKeyValue & 0xFF, nScanCode ) )
			{
                int nVk = scanCodeToVirtualKey( nScanCode );
                if( nVk >= Qt::Key_F1 && nVk <= Qt::Key_F24 )
				{
                    bFunctionKey = true;
				}
			}
		}
		if( bFunctionKey )
		{
            ui->CBOutputPort->setCurrentIndex( 1 );
            ui->CBOutputPort->setItemData( 1, wKeyValue );

            setEditMode( ModeFunctionKey, wKeyValue );
		}
		else
		{
            ui->CBOutputPort->setCurrentIndex( 0 );
            ui->CBOutputPort->setItemData( 0, wKeyValue );

            setEditMode( ModeUserDefined, wKeyValue );
		}
	}
	else
	{
		if( nSelIndex == 1 )
		{
            ui->CBOutputPort->setItemData( 1, wKeyValue );

            setEditMode( ModeFunctionKey, wKeyValue );
		}
		else
		{
            setEditMode( ModeNone, wKeyValue );
		}
	}
}

void QEditGateWidget::setEditMode( EditMode Mode, ushort wKeyValue )
{
	switch( Mode )
	{
	case ModeNone:
        ui->LBEditKey->reset();
        ui->LBEditKey->hide();
		break;
	case ModeUserDefined:
        ui->LBEditKey->setKeyType( QUserDefinedKeyWidget::KeyTypeNormal );
        if( wKeyValue != 0xFFFF )
        {
            ui->LBEditKey->setKeyValue( wKeyValue );
        }
        ui->LBEditKey->show();
		break;
	case ModeFunctionKey:
        ui->LBEditKey->setKeyType( QUserDefinedKeyWidget::KeyTypeFunction );
        if( wKeyValue != 0xFFFF )
        {
            ui->LBEditKey->setKeyValue( wKeyValue );
        }
        ui->LBEditKey->show();
		break;
	}
}

void QEditGateWidget::updateInputCombo( CSoftlogic::Port port, QComboBox& cbInput )
{
	if ( port >= CSoftlogic::epSoftLogic0 )
	{
        int nCbIdx = cbInput.findText( "Softlogic", Qt::MatchStartsWith );
		if( nCbIdx >= 0 )
            cbInput.removeItem( nCbIdx );
        cbInput.addItem( "Softlogic...", port );
        cbInput.setCurrentIndex( cbInput.count()-1 );
	}
	else
	{
        int nCbIdx = cbInput.findText( "Softlogic", Qt::MatchStartsWith );
		if( nCbIdx >= 0 )
            cbInput.removeItem( nCbIdx );
        for( int i=0 ; i<cbInput.count() ; i++ )
		{
            if( port == (CSoftlogic::Port)cbInput.itemData(i).toInt() )
			{
                cbInput.setCurrentIndex( i );
				break;
			}
		}
	}
	if ( port >= CSoftlogic::epSoftkey0 && port < CSoftlogic::epSoftkeyAll )
	{
        int nCbIdx = cbInput.findText( "Softkey", Qt::MatchStartsWith );
		if( nCbIdx >= 0 )
            cbInput.removeItem( nCbIdx );
        QString strKey = QString("Softkey%1").arg((int)port-(int)(CSoftlogic::epSoftkey0)+1);
        cbInput.addItem( strKey, port );
        cbInput.setCurrentIndex( cbInput.count()-1 );
	}
	else
	{
        int nCbIdx = cbInput.findText( "Softkey", Qt::MatchStartsWith );
		if( nCbIdx >= 0 )
            cbInput.removeItem( nCbIdx );
        for( int i=0 ; i<cbInput.count() ; i++ )
		{
            if( port == (CSoftlogic::Port)cbInput.itemData(i).toInt() )
			{
                cbInput.setCurrentIndex( i );
				break;
			}
		}
	}
}

void QEditGateWidget::updateItems()
{
	if( m_pSoftlogic )
	{
        ui->CBEnablePort->setCurrentIndex( 0 );
        for( int i=0 ; i<ui->CBEnablePort->count() ; i++ )
		{
            if( m_pSoftlogic->getEnablePort() == (CSoftlogic::Port)ui->CBEnablePort->itemData(i).toInt() )
			{
                ui->CBEnablePort->setCurrentIndex( i );
				break;
			}
		}

        ui->ChkEnableInvertPort->setEnabled( ui->CBEnablePort->currentIndex() == 0 ? false : true );
        ui->ChkEnableInvertPort->setChecked( m_pSoftlogic->getEnableNot() );

		switch( m_pSoftlogic->getLogicType() )
		{
		case CSoftlogic::eltAND:
			m_nGateType = 0;
            ui->RBAND->setChecked( true );
			break; 
		case CSoftlogic::eltOR:
			m_nGateType = 2;
            ui->RBOR->setChecked( true );
			break;
		case CSoftlogic::eltXOR:
			m_nGateType = 3;
            ui->RBXOR->setChecked( true );
			break;
		case CSoftlogic::eltAND_RISINGEDGE:
			m_nGateType = 1;
            ui->RBANDRisingEdge->setChecked( true );
			break;
        default:
            Q_ASSERT( false );
            break;
		}

        ui->ChkInvertPort1->setChecked( m_pSoftlogic->getIn1Not() );
        ui->ChkInvertPort2->setChecked( m_pSoftlogic->getIn2Not() );

		CSoftlogic::Port port1 = m_pSoftlogic->getIn1Port();
        updateInputCombo( port1, *ui->CBInputPort1 );

		CSoftlogic::Port port2 = m_pSoftlogic->getIn2Port();
        updateInputCombo( port2, *ui->CBInputPort2 );


        for( int i=0 ; i<ui->CBTrigger->count() ; i++ )
		{
            if( m_pSoftlogic->getLogicTrigger() == (CSoftlogic::Trigger)ui->CBTrigger->itemData(i).toInt() )
			{
                ui->CBTrigger->setCurrentIndex( i );
				break;
			}
		}

		CSoftlogic::PortType type = m_pSoftlogic->getOutPortType();
        for( int i=0 ; i<ui->CBOutputType->count() ; i++ )
		{
            CSoftlogic::PortType t = (CSoftlogic::PortType)ui->CBOutputType->itemData(i).toInt();
            if( type == t )
			{
                /*m_nOutputTypeIndex = i;*/
                ui->CBOutputType->setCurrentIndex( i );
                updateOutputPort( type );
				break;
			}
		}

		switch( type )
		{
		case CSoftlogic::eptError:
            Q_ASSERT( false );
			// ??
			break;
		case CSoftlogic::eptKey:
			{
                uchar cKey = m_pSoftlogic->getOutKey1();
                uchar cCondition = m_pSoftlogic->getOutKey1ConditionKeys();


                onUpdateKeyValue( (ushort)(cCondition<<8)|cKey );
			}
			break;
		case CSoftlogic::eptMouse:
			{
                uchar cMouseButton = m_pSoftlogic->getOutMouse();
                uchar cCondition = m_pSoftlogic->getOutKey1ConditionKeys();

                ui->LBEditKey->setKeyType( QUserDefinedKeyWidget::KeyTypeMouse );
                ui->LBEditKey->setKeyValue( (ushort)((cCondition|MM_MOUSE_KEY0_MOUSE)<<8)|cMouseButton );
                ui->LBEditKey->show();
			}
			break;
		case CSoftlogic::eptReport:
			{
                ui->EditReport->setText( QString::number(m_pSoftlogic->getReportId() < 0 ? 0 : m_pSoftlogic->getReportId() ) );
			}
			break;
		case CSoftlogic::eptState:
			if( m_pSoftlogic->getOutPort() == 0x00 )
			{
                ui->CBOutputType->setCurrentIndex( 0 );
                ui->CBOutputPort->setCurrentIndex( -1 );
                ui->CBOutputPort->setEnabled( false );
				break;
			}
		default:
			{
				CSoftlogic::Port outPort = m_pSoftlogic->getOutPort();

                ui->CBOutputPort->setCurrentIndex( -1 );
//                m_nOutputPortIndex = -1;
				if ( outPort == CSoftlogic::epNoGate )
				{
					//m_cbOutputPort.SetCurSel(-1);
				}
				else if ( outPort >= CSoftlogic::epStateWorking && outPort <= CSoftlogic::epStateInvertDetection )
				{
                    for( int i=0 ; i<ui->CBOutputPort->count() ; i++ )
					{
                        if( outPort == (CSoftlogic::Port)ui->CBOutputPort->itemData(i).toInt() )
						{
                            ui->CBOutputPort->setCurrentIndex( i );
							break;
						}
					}
				}
				else if ( outPort >= CSoftlogic::epGpio0 && outPort < CSoftlogic::epSoftkey0 )
				{
                    for( int i=0 ; i<ui->CBOutputPort->count() ; i++ )
					{
                        if( outPort == (CSoftlogic::Port)ui->CBOutputPort->itemData(i).toInt() )
						{
                            ui->CBOutputPort->setCurrentIndex( i );
							break;
						}
					}
				}
				else if ( outPort >= CSoftlogic::epSoftkey0 && outPort < CSoftlogic::epSoftkeyAll )
				{
                    ui->CBOutputPort->setCurrentIndex( (int)outPort - (int)CSoftlogic::epSoftkey0 );
				}
				else if ( outPort >= CSoftlogic::epSoftLogic0 )
				{
                    ui->CBOutputPort->setItemText( -1, "Softlogic..." );
				}
			}
			break;
		}


        ui->CBHidden->setCurrentIndex( m_bShowLogic ? 0 : 1 );
//        int nIdx = m_nOutputTypeIndex;
//        m_nOutputTypeIndex = -1;
//        ui->CBOutputType->setCurrentIndex( nIdx );
	}
}

void QEditGateWidget::setLogic( CSoftlogic* pSoftlogic, bool bShow )
{
	m_pSoftlogic = pSoftlogic;
	m_bShowLogic = bShow;

    updateItems();
}

void QEditGateWidget::updateOutputPortType( int nGateType )
{
    int nCurSel = ui->CBOutputType->currentIndex();
	CSoftlogic::PortType oldPortType = CSoftlogic::eptError;
	if( nCurSel >= 0 )
	{
        oldPortType = (CSoftlogic::PortType)ui->CBOutputType->itemData(nCurSel).toInt();
	}

    ui->CBOutputType->clear();

    ui->CBOutputType->addItem( "(Empty)", CSoftlogic::eptError );
    ui->CBOutputType->addItem( "Keyboard", CSoftlogic::eptKey );

#ifdef USE_SOFTLOGIC_OUTPUT_MOUSE
	if( nGateType < 2 ) // AND, AND_RisingEdge
	{
        ui->CBOutputType->addItem( "Mouse", CSoftlogic::eptMouse );
	}
#endif
    ui->CBOutputType->addItem( "State", CSoftlogic::eptState );
    ui->CBOutputType->addItem( "GPIO", CSoftlogic::eptGPIO );
    ui->CBOutputType->addItem( "Report", CSoftlogic::eptReport );

    for( int i=0 ; i<ui->CBOutputType->count() ; i++ )
	{
        if( (CSoftlogic::PortType)ui->CBOutputType->itemData(i).toInt() == oldPortType )
		{
			if( oldPortType == CSoftlogic::eptError )
			{
                ui->CBOutputType->setCurrentIndex( -1 );
                ui->CBOutputPort->setCurrentIndex( -1 );
                updateOutputPort( CSoftlogic::eptError );
			}
			else
			{
                ui->CBOutputType->setCurrentIndex( i );
			}
			return;
		}
	}
    ui->CBOutputPort->setCurrentIndex( -1 );
    updateOutputPort( CSoftlogic::eptError );
}

void QEditGateWidget::updateOutputPort( CSoftlogic::PortType type )
{
    ui->CBOutputPort->clear();

	if( type == CSoftlogic::eptError ) // (Empty)
	{
        ui->EditReport->setVisible( false );
        ui->LBOutputKey->setVisible( false );
        ui->LBEditKey->hide();
        ui->CBOutputPort->setVisible( true );
        ui->CBOutputPort->setEnabled( false );
        ui->CBEnablePort->setEnabled( true );
		return;
	}

	if( type == CSoftlogic::eptReport ) // Report
	{
        ui->CBOutputPort->setVisible( false );
        ui->LBOutputKey->setVisible( false );
        ui->LBEditKey->hide();
        ui->EditReport->setVisible( true );
        ui->CBEnablePort->setEnabled( true );
		return;
	}

    ui->LBOutputKey->setVisible( false );
    ui->EditReport->setVisible( false );
    ui->LBEditKey->hide();
    ui->CBOutputPort->setEnabled( true );
    ui->CBEnablePort->setVisible( true );

	switch( type )
	{
	case CSoftlogic::eptKey:	// Keyboard
        ui->CBOutputPort->addItem( "(User Defined)" );
        ui->CBOutputPort->setItemData( ui->CBOutputPort->count()-1, 0xFFFF );
        ui->CBOutputPort->addItem( "(Function Key)" );
        ui->CBOutputPort->setItemData( ui->CBOutputPort->count()-1, 0xFFFF );
        ui->CBOutputPort->addItem( "Task Switcher" );
        ui->CBOutputPort->setItemData( ui->CBOutputPort->count()-1, 0x052B );
        ui->CBOutputPort->addItem( "Task Switcher(Aero)" );
        ui->CBOutputPort->setItemData( ui->CBOutputPort->count()-1, 0x092B );
        ui->CBOutputPort->addItem( "Task Manager" );
        ui->CBOutputPort->setItemData( ui->CBOutputPort->count()-1, 0x0329 );
        ui->CBOutputPort->addItem( "Explorer" );
        ui->CBOutputPort->setItemData( ui->CBOutputPort->count()-1, 0x0808 );
        ui->CBOutputPort->addItem( "Desktop" );
        ui->CBOutputPort->setItemData( ui->CBOutputPort->count()-1, 0x0807 );
        ui->CBOutputPort->addItem( "Escape" );
        ui->CBOutputPort->setItemData( ui->CBOutputPort->count()-1, 0x0029 );
#ifdef USE_SOFTLOGIC_OUTPUT_AUDIO_CONTROL
        ui->CBOutputPort->addItem( "Audio Volume Inc." );
        ui->CBOutputPort->setItemData( ui->CBOutputPort->count()-1, V_KEY_VOLUME_INC_VALUE );
        ui->CBOutputPort->addItem( "Audio Volume Dec." );
        ui->CBOutputPort->setItemData( ui->CBOutputPort->count()-1, V_KEY_VOLUME_DEC_VALUE );
        ui->CBOutputPort->addItem( "Audio Mute" );
        ui->CBOutputPort->setItemData( ui->CBOutputPort->count()-1, V_KEY_VOLUME_MUTE_VALUE );

        ui->CBOutputPort->addItem( "MM Play" );
        ui->CBOutputPort->setItemData( ui->CBOutputPort->count()-1, V_KEY_TRANSPORT_PLAY );
        ui->CBOutputPort->addItem( "MM Pause" );
        ui->CBOutputPort->setItemData( ui->CBOutputPort->count()-1, V_KEY_TRANSPORT_PAUSE );
        ui->CBOutputPort->addItem( "MM FForward" );
        ui->CBOutputPort->setItemData( ui->CBOutputPort->count()-1, V_KEY_TRANSPORT_FFORWARD );
        ui->CBOutputPort->addItem( "MM Rewind" );
        ui->CBOutputPort->setItemData( ui->CBOutputPort->count()-1, V_KEY_TRANSPORT_REWIND );
        ui->CBOutputPort->addItem( "MM Next Track" );
        ui->CBOutputPort->setItemData( ui->CBOutputPort->count()-1, V_KEY_TRANSPORT_NEXT_TRACK );
        ui->CBOutputPort->addItem( "MM Prev Track" );
        ui->CBOutputPort->setItemData( ui->CBOutputPort->count()-1, V_KEY_TRANSPORT_PREV_TRACK );
        ui->CBOutputPort->addItem( "MM Stop" );
        ui->CBOutputPort->setItemData( ui->CBOutputPort->count()-1, V_KEY_TRANSPORT_STOP );
        ui->CBOutputPort->addItem( "MM Eject" );
        ui->CBOutputPort->setItemData( ui->CBOutputPort->count()-1, V_KEY_TRANSPORT_EJECT );
        ui->CBOutputPort->addItem( "MM Play/Pause" );
        ui->CBOutputPort->setItemData( ui->CBOutputPort->count()-1, V_KEY_TRANSPORT_PLAYPAUSE );

        ui->CBOutputPort->addItem( "AL E-mail Viewer" );
        ui->CBOutputPort->setItemData( ui->CBOutputPort->count()-1, V_KEY_AL_EMAIL_VIEWER );
        ui->CBOutputPort->addItem( "AL Calculator" );
        ui->CBOutputPort->setItemData( ui->CBOutputPort->count()-1, V_KEY_AL_CALCULATOR );
        ui->CBOutputPort->addItem( "AL My Computer" );
        ui->CBOutputPort->setItemData( ui->CBOutputPort->count()-1, V_KEY_AL_MY_COMPUTER );
        ui->CBOutputPort->addItem( "AL Web Browser" );
        ui->CBOutputPort->setItemData( ui->CBOutputPort->count()-1, V_KEY_AL_WEB_BROWSER );
#endif

        ui->CBEnablePort->setEnabled( false );
        ui->LBOutputKey->setVisible( true );
        ui->CBOutputPort->setVisible( true );
        ui->LBEditKey->show();


        onUpdateKeyValue( (ushort)(0x00) );

		break;
	case CSoftlogic::eptMouse:	// Mouse
        ui->CBOutputPort->setVisible( false );
        ui->LBEditKey->reset();
        ui->LBEditKey->setKeyType( QUserDefinedKeyWidget::KeyTypeMouse );
        ui->LBEditKey->show();

        ui->CBEnablePort->setEnabled( false );
		break;
	case CSoftlogic::eptState:	// State
        ui->CBOutputPort->addItem( "Beep #1", CSoftlogic::epStateWorking );
        ui->CBOutputPort->addItem( "Beep #2", CSoftlogic::epStateTouch );
        ui->CBOutputPort->addItem( "Calibration", CSoftlogic::epStateCalibration );
        ui->CBOutputPort->addItem( "TouchScreen Enable", CSoftlogic::epStateTouchScreen );
        ui->CBOutputPort->addItem( "Multi-touch Device", CSoftlogic::epStateMultiTouchDevice );
        ui->CBOutputPort->addItem( "Invert Detection", CSoftlogic::epStateInvertDetection );
        ui->CBOutputPort->setEnabled( true );
        ui->CBOutputPort->setVisible( true );
		break;
	case CSoftlogic::eptGPIO:	// GPIO
		{
            CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

            bool bEnableOutputGPIO = false;
            for ( int i=0 ; i<Keys.getGPIOCount() ; i++ )
            {
                GPIOInfo* pInfo = Keys.getGPIOInfo(i);
				if ( pInfo )
				{
					if (pInfo->bEnable && pInfo->bOutput)
					{
                        QString strGPIO = QString("GPIO %1").arg(i+1);
                        ui->CBOutputPort->addItem( strGPIO );
                        ui->CBOutputPort->setItemData( ui->CBOutputPort->count()-1, (CSoftlogic::Port)(i+(int)CSoftlogic::epGpio0) );
                        bEnableOutputGPIO = true;
					}
				}
			}
            ui->CBOutputPort->setEnabled( bEnableOutputGPIO );
		}
        ui->CBEnablePort->setEnabled( true );
		break;
	case CSoftlogic::eptSoftkey: // Softkey
		for( int i=0 ; i<SOFT_KEY_MAX ; i++ )
		{
            QString strSoftkey = QString("Softkey %1").arg(i+1);
            ui->CBOutputPort->addItem( strSoftkey );
            ui->CBOutputPort->setItemData( ui->CBOutputPort->count()-1, (CSoftlogic::Port)(i+(int)CSoftlogic::epSoftkey0) );
		}
        ui->CBEnablePort->setEnabled( true );
		break;
    default:
        break;
	}

 //   m_nOutputPortIndex = -1;
    ui->CBOutputPort->setCurrentIndex( 0 );
}

void QEditGateWidget::displayKeyValue( ushort wKeyValue )
{
    uchar cV0 = (uchar)(wKeyValue >> 8);
    uchar cV1 = (uchar)(wKeyValue & 0xFF);

	if( (cV0 & MM_MOUSE_KEY0_MOUSE) || (wKeyValue == 0x0000) )
	{
        ui->LBOutputKey->setText( "" );
		return;
	}

    QString strKey;
	if( cV0 & MM_MOUSE_KEY0_CTRL )
	{
        if( !strKey.isEmpty() )
            strKey += "+";
		else
            strKey += "\"";
        strKey += "Ctrl";
	}

	if( cV0 & MM_MOUSE_KEY0_ALT )
	{
        if( !strKey.isEmpty() )
            strKey += "+";
		else
            strKey += "\"";
        strKey += "Alt";
	}

	if( cV0 & MM_MOUSE_KEY0_SHIFT )
	{
        if( !strKey.isEmpty() )
            strKey += "+";
		else
            strKey += "\"";
        strKey += "Shift";
	}	

	if( cV0 & MM_MOUSE_KEY0_WINDOW )
	{
        if( !strKey.isEmpty() )
            strKey += "+";
		else
            strKey += "\"";
        strKey += "Win";
	}	
		
    if( !strKey.isEmpty() )
        strKey += "+";
	else
        strKey += "\"";

	int nScanCode;
    if( findScanCode(cV1, nScanCode) )
        strKey += virtualKeyToString( scanCodeToVirtualKey( nScanCode ) );

    strKey += "\"";

    ui->LBOutputKey->setText( strKey );
}

void QEditGateWidget::paintEvent(QPaintEvent *evt)
{
    QDialog::paintEvent(evt);

    QRect rcGateBody( ui->GBInputPort->geometry().right(), ui->GBEnablePort->geometry().bottom(),
                      ui->GBOutputPort->geometry().left() - ui->GBInputPort->geometry().right(),
                      ui->LayoutGate->geometry().bottom() - ui->GBEnablePort->geometry().bottom() );
    rcGateBody.adjust( 10,10,10,10 );

    const int nPinLength = 15;

    QRect rcGate = rcGateBody.adjusted( nPinLength, nPinLength, -nPinLength, -nPinLength );

    int nOH = rcGate.height() / 4;

    QPainter painter;
    painter.begin( this );

    QPen penGate( Qt::SolidLine );
    penGate.setWidth( 2 );
    penGate.setColor( qRgb(50, 50, 50) );
    painter.setPen( penGate );
    painter.setBrush( Qt::white );

    painter.drawRect( rcGate );

    // draw pins
    painter.drawLine( rcGate.center().x(), rcGateBody.top(), rcGate.center().x(), rcGate.top() );
    painter.drawLine( rcGateBody.left(), rcGate.top()+nOH, rcGate.left(), rcGate.top()+nOH );
    painter.drawLine( rcGateBody.left(), rcGate.bottom()-nOH, rcGate.left(), rcGate.bottom()-nOH );
    painter.drawLine( rcGateBody.right(), rcGate.center().y(), rcGate.right(), rcGate.center().y() );

    painter.end();
}

void QEditGateWidget::on_CBEnablePort_activated(int index)
{
    ui->ChkEnableInvertPort->setEnabled( index == 0 ? false : true );

    onUpdateItem();
}

void QEditGateWidget::on_CBInputPort1_activated(int /*index*/)
{
    onUpdateItem();
}

void QEditGateWidget::on_CBInputPort2_activated(int /*index*/)
{
    onUpdateItem();
}

void QEditGateWidget::on_CBOutputType_activated(int index)
{
    if( index < 0 ) return;

    CSoftlogic::PortType type = (CSoftlogic::PortType)ui->CBOutputType->itemData( index ).toInt();

    updateOutputPort( type );

    onUpdateItem();
}

void QEditGateWidget::on_CBOutputPort_activated(int index)
{
    int nTypeIdex = ui->CBOutputType->currentIndex();

    if( nTypeIdex < 0 ) return;

    CSoftlogic::PortType type = (CSoftlogic::PortType)ui->CBOutputType->itemData( nTypeIdex ).toInt();

    if( type == CSoftlogic::eptKey )
    {
        if( index < 0 ) return;

        ushort wKeyValue = (ushort)ui->CBOutputPort->itemData( index ).toUInt();

        displayKeyValue( wKeyValue );

        if( index == 0 )
        {
            setEditMode( ModeUserDefined, wKeyValue );
        }
        else if( index == 1 )
        {
            setEditMode( ModeFunctionKey, wKeyValue );
        }
        else
        {
            setEditMode( ModeNone, wKeyValue );
        }
    }

    onUpdateItem();
}

void QEditGateWidget::on_CBTrigger_activated(int /*index*/)
{
    onUpdateItem();
}

void QEditGateWidget::on_CBHidden_activated(int /*index*/)
{
    applyHiddenLogics();

    emit updateLogic();
}

void QEditGateWidget::on_RBAND_clicked()
{
    switch( m_nGateType )
    {
    case 1:
        ui->RBANDRisingEdge->setChecked( false );
        break;
    case 2:
        ui->RBOR->setChecked( false );
        break;
    case 3:
        ui->RBXOR->setChecked( false );
        break;
    default:
        break;
    }

    m_nGateType = 0;

    onChangePortType();
}

void QEditGateWidget::on_RBANDRisingEdge_clicked()
{
    switch( m_nGateType )
    {
    case 0:
        ui->RBAND->setChecked( false );
        break;
    case 2:
        ui->RBOR->setChecked( false );
        break;
    case 3:
        ui->RBXOR->setChecked( false );
        break;
    default:
        break;
    }

    m_nGateType = 1;

    onChangePortType();
}

void QEditGateWidget::on_RBOR_clicked()
{
    switch( m_nGateType )
    {
    case 0:
        ui->RBAND->setChecked( false );
        break;
    case 1:
        ui->RBANDRisingEdge->setChecked( false );
        break;
    case 3:
        ui->RBXOR->setChecked( false );
        break;
    default:
        break;
    }

    m_nGateType = 2;

    onChangePortType();
}

void QEditGateWidget::on_RBXOR_clicked()
{
    switch( m_nGateType )
    {
    case 0:
        ui->RBAND->setChecked( false );
        break;
    case 1:
        ui->RBANDRisingEdge->setChecked( false );
        break;
    case 2:
        ui->RBOR->setChecked( false );
        break;
    default:
        break;
    }

    m_nGateType = 3;

    onChangePortType();
}

void QEditGateWidget::onChkInvert(bool /*bChecked*/)
{
    onUpdateItem();
}

void QEditGateWidget::onUpdateItem()
{
    applyItems();
    emit updateLogic();
}

void QEditGateWidget::onChangePortType()
{
//    UpdateData( TRUE );
    updateOutputPortType( m_nGateType );

    onUpdateItem();
}

void QEditGateWidget::onEditChangedKey(ushort wKeyValue)
{
    displayKeyValue( wKeyValue );

    onUpdateItem();
}

void QEditGateWidget::onEditReportFinished()
{
    onUpdateItem();
}

