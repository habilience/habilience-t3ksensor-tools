#include "QEditAction4WDWnd.h"
#include "ui_QEditAction4WDWnd.h"

#include "stdInclude.h"
#include "../common/T3kHandle.h"
#include "QMouseMappingTable.h"

#include "KeyMapStr.h"
#include "Common/nv.h"

#include "T3kPacketDef.h"

#include <QLabel>
#include <QComboBox>
#include <QShowEvent>

QEditAction4WDWnd::QEditAction4WDWnd(T3kHandle*& pHandle, QWidget *parent) :
    QDialog(parent), m_pTableWnd(NULL),
    ui(new Ui::QEditAction4WDWnd), m_pT3kHandle(pHandle)
{
    setWindowFlags( Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint );
    ui->setupUi(this);

#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
    genAdjustButtonWidgetForWinAndX11( this );
#endif

    setFont( parent->font() );

    ui->ChkLinkHorz->setVisible( false );
    ui->ChkLinkVert->setVisible( false );

    connect( ui->BtnCancel, SIGNAL(clicked()), this, SLOT(hide()) );
    connect( ui->LBUserDefKey, SIGNAL(KeyPressSignal(ushort)), this, SLOT(onKeyPress_Signal(ushort)) );
    connect( ui->LBUserDefKey, SIGNAL(FuncKey_activated(int)), this, SLOT(onFunctionKeyActivated(int)) );

    m_pTableWnd = (QMouseMappingTable*)parent;
    m_pWndActiveComboAction = NULL;

    m_bLinkLock = false;

    Init();

    setFixedSize( width(), height() );

    m_nLoad = 0x0000;
}

QEditAction4WDWnd::~QEditAction4WDWnd()
{
    delete ui;
}

void QEditAction4WDWnd::OnChangeLanguage()
{
    QLangRes& Res = QLangManager::GetPtr()->GetResource();

    setWindowTitle( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TITLE_CAPTION_EDIT_PROFILE_ITEM") ) );
    ui->TitleProfileItem->setText( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM") ) );
    ui->TitleAction->setText( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_ACTION") ) );
    ui->GBUserDefKey->setTitle( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_USER_DEFINE_KEY") ) );
    ui->LBKey->setText( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_KEY") ) );
    ui->BtnApply->setText( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("BTN_CAPTION_APPLY") ) );
    ui->BtnCancel->setText( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("BTN_CAPTION_CANCEL") ) );

    SetPredefineProfileTypes( *ui->CBActionLeft );
    SetPredefineProfileTypes( *ui->CBActionRight, true );
    SetPredefineProfileTypes( *ui->CBActionUp );
    SetPredefineProfileTypes( *ui->CBActionDown, true );

    if( Res.IsR2L() )
        ui->EditProfileItem->setAlignment( Qt::AlignRight );
    else
        ui->EditProfileItem->setAlignment( Qt::AlignLeft );

    ui->LBUserDefKey->SetRealGeometry();
}

void QEditAction4WDWnd::Init()
{
    ui->LinkIconWidget->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_4WD.png" );
    m_bLinkLock = false;

    ui->ChkLinkHorz->setIcon( QIcon(":/T3kCfgRes/Resources/PNG_ICON_LINK_HORZ.png") );
    ui->ChkLinkVert->setIcon( QIcon(":/T3kCfgRes/Resources/PNG_ICON_LINK_VERT.png") );

    OnChangeLanguage();
}

void QEditAction4WDWnd::SetPredefineProfileTypes( QComboBox& pWndComboAction, bool bAlter/*=FALSE*/ )
{
    QLangRes& Res = QLangManager::GetPtr()->GetResource();

    pWndComboAction.clear();

    pWndComboAction.addItem( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_DISABLED") ) );
    pWndComboAction.setItemData( pWndComboAction.count()-1, 0x0000 );
    pWndComboAction.addItem( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_USER_DEFINED") ) );
    pWndComboAction.setItemData( pWndComboAction.count()-1, 0xFFFF );
    pWndComboAction.addItem( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_FUNCTION_KEY") ) );
    pWndComboAction.setItemData( pWndComboAction.count()-1, 0xFFFF );

    pWndComboAction.addItem( "Left Drag" );
    pWndComboAction.setItemData( pWndComboAction.count()-1, 0x8001 );
    pWndComboAction.addItem( "Right Drag" );
    pWndComboAction.setItemData( pWndComboAction.count()-1, 0x8004 );
    pWndComboAction.addItem( "Middle Drag" );
    pWndComboAction.setItemData( pWndComboAction.count()-1, 0x8007 );
    pWndComboAction.addItem( "Wheel" );
    pWndComboAction.setItemData( pWndComboAction.count()-1, 0x80FE );
    pWndComboAction.addItem( "Ctrl+Wheel" );
    pWndComboAction.setItemData( pWndComboAction.count()-1, 0x81FE );

    if( !bAlter )
    {
        pWndComboAction.addItem( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_FORWARD") ) );
        pWndComboAction.setItemData( pWndComboAction.count()-1, 0x044F );
        pWndComboAction.addItem( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_BACK") ) );
        pWndComboAction.setItemData( pWndComboAction.count()-1, 0x0450 );
    }
    else
    {
        pWndComboAction.addItem( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_BACK") ) );
        pWndComboAction.setItemData( pWndComboAction.count()-1, 0x0450 );
        pWndComboAction.addItem( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_FORWARD") ) );
        pWndComboAction.setItemData( pWndComboAction.count()-1, 0x044F );
    }
}

void QEditAction4WDWnd::SetProfileInfo( int nProfileIndex, uchar cKey, ushort wKeyValue1, ushort wKeyValue2, ushort wKeyValue3, ushort wKeyValue4 )
{
    QLangRes& Res = QLangManager::GetPtr()->GetResource();

    SetPredefineProfileTypes( *ui->CBActionLeft );
    SetPredefineProfileTypes( *ui->CBActionRight, true );
    SetPredefineProfileTypes( *ui->CBActionUp );
    SetPredefineProfileTypes( *ui->CBActionDown, true );

    m_nProfileIndex = nProfileIndex;
    m_cProfileKey = cKey;
    m_wProfileValue[0] = wKeyValue1;		// left
    m_wProfileValue[1] = wKeyValue2;		// right
    m_wProfileValue[2] = wKeyValue3;		// up
    m_wProfileValue[3] = wKeyValue4;		// down

    QString strLoc;
    switch( cKey )
    {
    case MM_GESTURE_FINGERS_MOVE:
        strLoc = Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_FINGERS") ) + (Res.IsR2L() ? QString::fromUtf8(" < ") : QString::fromUtf8(" > ")) + Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_DRAG") );
        break;
    case MM_GESTURE_PALM_MOVE:
        strLoc = Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_PALM") ) + (Res.IsR2L() ? QString::fromUtf8(" < ") : QString::fromUtf8(" > ")) + Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_DRAG") );
        break;
    default:
        strLoc = "Unknown";
        break;
    }

    ui->EditProfileItem->setText( strLoc );

    SetActionWithProfileInfo( *ui->CBActionLeft, *ui->LBActionLeft, m_wProfileValue[0] );
    SetActionWithProfileInfo( *ui->CBActionRight, *ui->LBActionRight, m_wProfileValue[1] );
    SetActionWithProfileInfo( *ui->CBActionUp, *ui->LBActionUp, m_wProfileValue[2] );
    SetActionWithProfileInfo( *ui->CBActionDown, *ui->LBActionDown, m_wProfileValue[3] );

    ui->ChkLinkHorz->setChecked( false );
    ui->ChkLinkVert->setChecked( false );

    int nSel = ui->CBActionLeft->currentIndex();
    if( nSel < 0 ) return;

    m_bLinkLock = false;
    if( (nSel > 2) && (nSel < 6) )
        m_bLinkLock = true;

    if( !m_bLinkLock )
    {
        if( nSel > 2 )
        {
            ui->ChkLinkHorz->setChecked( true );
            if( ui->ChkLinkVert->isChecked() )
                ui->LinkIconWidget->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_4WD_LINK_ALL.png" );
            else
                ui->LinkIconWidget->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_4WD_LINK_LR.png" );
        }

        nSel = ui->CBActionUp->currentIndex();
        if( nSel < 0 ) return;

        if( nSel > 2 )
        {
            ui->ChkLinkVert->setChecked( true );
            if( ui->ChkLinkHorz->isChecked() )
                ui->LinkIconWidget->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_4WD_LINK_ALL.png" );
            else
                ui->LinkIconWidget->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_4WD_LINK_UD.png" );
        }
    }
    else
    {
        ui->LinkIconWidget->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_4WD_LINK_LOCK.png" );
    }
}

void QEditAction4WDWnd::SetActionWithProfileInfo( QComboBox& pWndComboAction, QLabel& pDetailLabel, ushort wKeyValue )
{
    pWndComboAction.setCurrentIndex(-1);
    pDetailLabel.setText( "" );

    QString strDetail = GetMappingStr(wKeyValue>>8, wKeyValue&0xFF);
    strDetail.replace( "\r\n", " " );
    strDetail.replace( "Click", "Button Click" );
    strDetail.replace( "Drag", "Button Drag" );

    bool bFound = false;
    int nSelIndex = -1;
    for( int nI = 0 ; nI < pWndComboAction.count() ; nI ++ )
    {
        ushort wItemValue = (ushort)pWndComboAction.itemData(nI).toInt();
        if( wItemValue == wKeyValue )
        {
            pWndComboAction.setCurrentIndex( nI );
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
            if( FindScanCode( wKeyValue & 0xFF, nScanCode ) )
            {
                int nVk = FromMapVirtualKey( nScanCode );
                if( nVk >= Qt::Key_F1 && nVk <= Qt::Key_F24 )
                {
                    bFunctionKey = true;
                }
            }
        }
        if( bFunctionKey )
        {
            pWndComboAction.setItemData( 2, wKeyValue );
            pWndComboAction.setCurrentIndex( 2 );

            SetEditMode( ModeFunctionKey, wKeyValue );
        }
        else
        {
            pWndComboAction.setItemData( 1, wKeyValue );
            pWndComboAction.setCurrentIndex( 1 );

            SetEditMode( ModeUserDefined, wKeyValue );
        }
    }
    else
    {
        if( nSelIndex == 2 )
        {
            pWndComboAction.setItemData( 2, wKeyValue );

            SetEditMode( ModeFunctionKey, wKeyValue );
        }
        else
        {
            SetEditMode( ModeNone, wKeyValue );
        }
    }

    pDetailLabel.setText( strDetail );

    if( &pWndComboAction == ui->CBActionLeft )
        m_nLoad |= 0x0001;
    else if( &pWndComboAction == ui->CBActionRight )
        m_nLoad |= 0x0010;
    else if( &pWndComboAction == ui->CBActionUp )
        m_nLoad |= 0x0100;
    else if( &pWndComboAction == ui->CBActionDown )
        m_nLoad |= 0x1000;
}

void QEditAction4WDWnd::SetEditMode( EditMode Mode, ushort wKeyValue )
{
    if( m_nLoad != 0x1111 )
    {
        Mode = ModeNone;
    }

    QLangRes& Res = QLangManager::GetPtr()->GetResource();

    switch( Mode )
    {
    case ModeNone:
        ui->GBUserDefKey->hide();
        ui->LBKey->hide();
        ui->LBUserDefKey->show();
        ui->LBUserDefKey->hide();
        ui->LBUserDefKey->Reset();
        break;
    case ModeUserDefined:
        if( wKeyValue == 0xFFFF )
            ui->LBUserDefKey->Reset();
        else
            ui->LBUserDefKey->SetKeyValue( wKeyValue );

        ui->GBUserDefKey->setTitle( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_USER_DEFINE_KEY") ) );
        ui->GBUserDefKey->show();
        ui->LBKey->show();
        ui->LBUserDefKey->SetUseFuncKey( false );
        ui->LBUserDefKey->show();
        ui->LBUserDefKey->setFocus();
        break;
    case ModeFunctionKey:
        ui->LBUserDefKey->hide();
        ui->LBUserDefKey->SetUseFuncKey( true );
        ui->LBUserDefKey->show();
        ui->GBUserDefKey->setTitle( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_FUNCTION_KEY") ) );
        ui->GBUserDefKey->show();
        ui->LBKey->show();

        ui->LBUserDefKey->Reset();
        if( wKeyValue == 0xFFFF )
            break;
        ui->LBUserDefKey->SetKeyValue( wKeyValue );

        ui->LBUserDefKey->SetFuncKeyCurrentIndex( -1 );
        for( int nF = 0 ; nF<ui->LBUserDefKey->GetFuncKeyCount(); nF++ )
        {
            ushort wFnKey = (ushort)ui->LBUserDefKey->GetItemData( nF ).toInt();
            if( wFnKey == (wKeyValue&0xFF) )
            {
                ui->LBUserDefKey->SetFuncKeyCurrentIndex( nF );
                break;
            }
        }
        break;
    }
}

void QEditAction4WDWnd::on_CBActionLeft_currentIndexChanged(int /*index*/)
{
    int nSel = ui->CBActionLeft->currentIndex();
    if( nSel < 0 ) return;

    bool bLinkLock = false;
    if( (nSel > 2) && (nSel < 6) )
        bLinkLock = true;

    ui->LinkIconWidget->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_4WD.png" );

    if( !bLinkLock )
    {
        if( m_bLinkLock )
        {
            m_bLinkLock = false;
            // reset
            //m_wndComboActionLeft.SetCurSel( 0 );
            ui->CBActionRight->setCurrentIndex( 0 );
            ui->CBActionUp->setCurrentIndex( 0 );
            ui->CBActionDown->setCurrentIndex( 0 );
        }

        if( nSel > 2 )
        {
            ui->ChkLinkHorz->setChecked( true );
            if( ui->ChkLinkVert->isChecked() )
                ui->LinkIconWidget->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_4WD_LINK_ALL.png" );
            else
                ui->LinkIconWidget->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_4WD_LINK_LR.png" );
        }
        else
        {
            if( ui->ChkLinkHorz->isChecked() )
            {
                ui->ChkLinkHorz->setChecked( false );
                ui->CBActionRight->setCurrentIndex( 0 );
                OnCbnSelchangeComboAction( ui->CBActionRight );

                if( ui->ChkLinkVert->isChecked() )
                    ui->LinkIconWidget->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_4WD_LINK_UD.png" );
            }
        }
    }
    else
    {
        ui->ChkLinkHorz->setChecked( false );
        ui->ChkLinkVert->setChecked( false );
        ui->LinkIconWidget->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_4WD_LINK_LOCK.png" );
    }

    if( !bLinkLock )
    {
        if( ui->ChkLinkHorz->isChecked() )
        {
            ui->CBActionRight->setCurrentIndex( nSel );
            OnCbnSelchangeComboAction( ui->CBActionRight );
        }

        OnCbnSelchangeComboAction( ui->CBActionLeft );
    }
    else
    {
        ui->CBActionRight->setCurrentIndex( nSel );
        OnCbnSelchangeComboAction( ui->CBActionRight );
        ui->CBActionUp->setCurrentIndex( nSel );
        OnCbnSelchangeComboAction( ui->CBActionUp );
        ui->CBActionDown->setCurrentIndex( nSel );
        OnCbnSelchangeComboAction( ui->CBActionDown );

        OnCbnSelchangeComboAction( ui->CBActionLeft );
    }

    m_bLinkLock = bLinkLock;
}

void QEditAction4WDWnd::on_CBActionRight_currentIndexChanged(int /*index*/)
{
    int nSel = ui->CBActionRight->currentIndex();
    if( nSel < 0 ) return;

    bool bLinkLock = false;
    if( (nSel > 2) && (nSel < 6) )
        bLinkLock = true;

    ui->LinkIconWidget->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_4WD.png" );

    if( !bLinkLock )
    {
        if( m_bLinkLock )
        {
            m_bLinkLock = false;
            // reset
            ui->CBActionLeft->setCurrentIndex( 0 );
            //m_wndComboActionRight.SetCurSel( 0 );
            ui->CBActionUp->setCurrentIndex( 0 );
            ui->CBActionDown->setCurrentIndex( 0 );
        }

        if( nSel > 2 )
        {
            ui->ChkLinkHorz->setChecked( true );
            if( ui->ChkLinkVert->isChecked() )
                ui->LinkIconWidget->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_4WD_LINK_ALL.png" );
            else
                ui->LinkIconWidget->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_4WD_LINK_LR.png" );
        }
        else
        {
            if( ui->ChkLinkHorz->isChecked() )
            {
                ui->ChkLinkHorz->setChecked( false );
                ui->CBActionLeft->setCurrentIndex( 0 );
                OnCbnSelchangeComboAction( ui->CBActionRight );

                if( ui->ChkLinkVert->isChecked() )
                    ui->LinkIconWidget->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_4WD_LINK_UD.png" );
            }
        }
    }
    else
    {
        ui->ChkLinkHorz->setChecked( false );
        ui->ChkLinkVert->setChecked( false );
        ui->LinkIconWidget->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_4WD_LINK_LOCK.png" );
    }

    if( !bLinkLock )
    {
        if( ui->ChkLinkHorz->isChecked() )
        {
            ui->CBActionLeft->setCurrentIndex( nSel );
            OnCbnSelchangeComboAction( ui->CBActionLeft );
        }

        OnCbnSelchangeComboAction( ui->CBActionRight );
    }
    else
    {
        ui->CBActionLeft->setCurrentIndex( nSel );
        OnCbnSelchangeComboAction( ui->CBActionLeft );
        ui->CBActionUp->setCurrentIndex( nSel );
        OnCbnSelchangeComboAction( ui->CBActionUp );
        ui->CBActionDown->setCurrentIndex( nSel );
        OnCbnSelchangeComboAction( ui->CBActionDown );

        OnCbnSelchangeComboAction( ui->CBActionRight );
    }

    m_bLinkLock = bLinkLock;
}

void QEditAction4WDWnd::on_CBActionUp_currentIndexChanged(int /*index*/)
{
    int nSel = ui->CBActionUp->currentIndex();
    if( nSel < 0 ) return;

    bool bLinkLock = false;
    if( (nSel > 2) && (nSel < 6) )
        bLinkLock = true;

    ui->LinkIconWidget->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_4WD.png" );

    if( !bLinkLock )
    {
        if( m_bLinkLock )
        {
            m_bLinkLock = false;
            // reset
            ui->CBActionLeft->setCurrentIndex( 0 );
            ui->CBActionRight->setCurrentIndex( 0 );
            //m_wndComboActionUp.SetCurSel( 0 );
            ui->CBActionDown->setCurrentIndex( 0 );
        }

        if( nSel > 2 )
        {
            ui->ChkLinkVert->setChecked( true );
            if( ui->ChkLinkHorz->isChecked() )
                ui->LinkIconWidget->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_4WD_LINK_ALL.png" );
            else
                ui->LinkIconWidget->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_4WD_LINK_UD.png" );
        }
        else
        {
            if( ui->ChkLinkVert->isChecked() )
            {
                ui->ChkLinkVert->setChecked( false );
                ui->CBActionDown->setCurrentIndex( 0 );
                OnCbnSelchangeComboAction( ui->CBActionDown );
                if( ui->ChkLinkHorz->isChecked() )
                    ui->LinkIconWidget->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_4WD_LINK_LR.png" );
            }
        }
    }
    else
    {
        ui->ChkLinkHorz->setChecked( false );
        ui->ChkLinkVert->setChecked( false );
        ui->LinkIconWidget->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_4WD_LINK_LOCK.png" );
    }

    if( !bLinkLock )
    {
        if( ui->ChkLinkVert->isChecked() )
        {
            ui->CBActionDown->setCurrentIndex( nSel );
            OnCbnSelchangeComboAction( ui->CBActionDown );
        }

        OnCbnSelchangeComboAction( ui->CBActionUp );
    }
    else
    {
        ui->CBActionLeft->setCurrentIndex( nSel );
        OnCbnSelchangeComboAction( ui->CBActionLeft );
        ui->CBActionRight->setCurrentIndex( nSel );
        OnCbnSelchangeComboAction( ui->CBActionRight );
        ui->CBActionDown->setCurrentIndex( nSel );
        OnCbnSelchangeComboAction( ui->CBActionDown );

        OnCbnSelchangeComboAction( ui->CBActionUp );
    }

    m_bLinkLock = bLinkLock;
}

void QEditAction4WDWnd::on_CBActionDown_currentIndexChanged(int /*index*/)
{
    int nSel = ui->CBActionDown->currentIndex();
    if( nSel < 0 ) return;

    bool bLinkLock = false;
    if( (nSel > 2) && (nSel < 6) )
        bLinkLock = true;

    ui->LinkIconWidget->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_4WD.png" );

    if( !bLinkLock )
    {
        if( m_bLinkLock )
        {
            m_bLinkLock = false;
            // reset
            ui->CBActionLeft->setCurrentIndex( 0 );
            ui->CBActionRight->setCurrentIndex( 0 );
            ui->CBActionUp->setCurrentIndex( 0 );
            //m_wndComboActionDown.SetCurSel( 0 );
        }

        if( nSel > 2 )
        {
            ui->ChkLinkVert->setChecked( true );
            if( ui->ChkLinkHorz->isChecked() )
                ui->LinkIconWidget->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_4WD_LINK_ALL.png" );
            else
                ui->LinkIconWidget->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_4WD_LINK_UD.png" );
        }
        else
        {
            if( ui->ChkLinkVert->isChecked() )
            {
                ui->ChkLinkVert->setChecked( false );
                ui->CBActionUp->setCurrentIndex( 0 );
                OnCbnSelchangeComboAction( ui->CBActionUp );

                if( ui->ChkLinkHorz->isChecked() )
                    ui->LinkIconWidget->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_4WD_LINK_LR.png" );
            }
        }
    }
    else
    {
        ui->ChkLinkHorz->setChecked( false );
        ui->ChkLinkVert->setChecked( false );
        ui->LinkIconWidget->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_4WD_LINK_LOCK.png" );
    }

    if( !bLinkLock )
    {


        if( ui->ChkLinkVert->isChecked() )
        {
            ui->CBActionUp->setCurrentIndex( nSel );
            OnCbnSelchangeComboAction( ui->CBActionUp );
        }

        OnCbnSelchangeComboAction( ui->CBActionDown );
    }
    else
    {
        ui->CBActionLeft->setCurrentIndex( nSel );
        OnCbnSelchangeComboAction( ui->CBActionLeft );
        ui->CBActionRight->setCurrentIndex( nSel );
        OnCbnSelchangeComboAction( ui->CBActionRight );
        ui->CBActionUp->setCurrentIndex( nSel );
        OnCbnSelchangeComboAction( ui->CBActionUp );

        OnCbnSelchangeComboAction( ui->CBActionDown );
    }

    m_bLinkLock = bLinkLock;
}

void QEditAction4WDWnd::OnCbnSelchangeComboAction( QComboBox* pWndComboAction )
{
    QLangRes& Res = QLangManager::GetPtr()->GetResource();

    int nSel = pWndComboAction->currentIndex();
    if( nSel < 0 ) return;

    QLabel* pLabel = NULL;
    if( pWndComboAction == ui->CBActionLeft )
        pLabel = ui->LBActionLeft;
    else if( pWndComboAction == ui->CBActionRight )
        pLabel = ui->LBActionRight;
    else if( pWndComboAction == ui->CBActionUp )
        pLabel = ui->LBActionUp;
    else if( pWndComboAction == ui->CBActionDown )
        pLabel = ui->LBActionDown;
    else return;

    ushort wKeyValue = (ushort)pWndComboAction->itemData(nSel).toInt();

    if( nSel == 1 )
    {
        pWndComboAction->setItemData( 2, 0x0000 );
        SetEditMode( ModeUserDefined, wKeyValue );
    }
    else if( nSel == 2 )
    {
        pWndComboAction->setItemData( 1, 0x0000 );
        SetEditMode( ModeFunctionKey, wKeyValue );
    }
    else
    {
        SetEditMode( ModeNone, wKeyValue );
    }

    if( wKeyValue == 0xFFFF )
    {
        pLabel->setText( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_NOT_DEFINED") ) );
        return;
    }

    QString strDetail( GetMappingStr(wKeyValue>>8, wKeyValue&0xFF) );
    strDetail.replace( "\r\n", " " );
    strDetail.replace( "Click", "Button Click" );
    strDetail.replace( "Drag", "Button Drag" );

    pLabel->setText( strDetail );
}

void QEditAction4WDWnd::showEvent(QShowEvent *evt)
{
    QDialog::showEvent(evt);

    if( evt->type() == QEvent::Show )
    {
        ui->LBUserDefKey->Reset();
        ui->LBUserDefKey->update();
    }
}

void QEditAction4WDWnd::hideEvent(QHideEvent *evt)
{
    QDialog::hideEvent(evt);

    if( evt->type() == QEvent::Hide )
    {
        m_pTableWnd->ResetSelect();
        m_nLoad = 0x0000;
    }
}

void QEditAction4WDWnd::on_BtnApply_clicked()
{
    ushort wKeyValue[4];
    int nSel;

    nSel = ui->CBActionLeft->currentIndex();
    if( nSel < 0 ) return;
    wKeyValue[0] = (ushort)ui->CBActionLeft->itemData(nSel).toInt();

    if( wKeyValue[0] == 0xFFFF )
    {
        hide();
        return;
    }

    nSel = ui->CBActionRight->currentIndex();
    if( nSel < 0 ) return;
    wKeyValue[1] = (ushort)ui->CBActionRight->itemData(nSel).toInt();

    if( wKeyValue[1] == 0xFFFF )
    {
        hide();
        return;
    }

    nSel = ui->CBActionUp->currentIndex();
    if( nSel < 0 ) return;
    wKeyValue[2] = (ushort)ui->CBActionUp->itemData(nSel).toInt();

    if( wKeyValue[2] == 0xFFFF )
    {
        hide();
        return;
    }

    nSel =ui->CBActionDown->currentIndex();
    if( nSel < 0 ) return;
    wKeyValue[3] = (ushort)ui->CBActionDown->itemData(nSel).toInt();

    if( wKeyValue[3] == 0xFFFF )
    {
        hide();
        return;
    }

    QString str;
    switch( m_nProfileIndex )
    {
    case 0:
        str = str.sprintf("%s%02X%04X%04X%04X%04X", cstrMouseProfile1, m_cProfileKey, wKeyValue[0], wKeyValue[1], wKeyValue[2], wKeyValue[3] );
        break;
    case 1:
        str = str.sprintf("%s%02X%04X%04X%04X%04X", cstrMouseProfile2, m_cProfileKey, wKeyValue[0], wKeyValue[1], wKeyValue[2], wKeyValue[3] );
        break;
    case 2:
        str = str.sprintf("%s%02X%04X%04X%04X%04X", cstrMouseProfile3, m_cProfileKey, wKeyValue[0], wKeyValue[1], wKeyValue[2], wKeyValue[3] );
        break;
    case 3:
        str = str.sprintf("%s%02X%04X%04X%04X%04X", cstrMouseProfile4, m_cProfileKey, wKeyValue[0], wKeyValue[1], wKeyValue[2], wKeyValue[3] );
        break;
    case 4:
        str = str.sprintf("%s%02X%04X%04X%04X%04X", cstrMouseProfile5, m_cProfileKey, wKeyValue[0], wKeyValue[1], wKeyValue[2], wKeyValue[3] );
        break;
    default:
        hide();
        return;
    }

    m_pT3kHandle->SendCommand( (const char*)str.toUtf8().data(), true );

    hide();
}

void QEditAction4WDWnd::OnKeyEditChange(ushort wKeyValue)
{
    if( m_pWndActiveComboAction == NULL )
        return;

    m_pWndActiveComboAction->setItemData( 1, wKeyValue );
    m_pWndActiveComboAction->setItemData( 2, wKeyValue );

    if( m_pWndActiveComboAction == ui->CBActionLeft ||
        m_pWndActiveComboAction == ui->CBActionRight )
    {
        if( ui->ChkLinkHorz->isChecked() )
        {
            if( m_pWndActiveComboAction == ui->CBActionLeft )
            {
                ui->CBActionRight->setItemData( 1, wKeyValue );
                OnCbnSelchangeComboAction( ui->CBActionRight );
            }
            else if( m_pWndActiveComboAction == ui->CBActionRight )
            {
                ui->CBActionLeft->setItemData( 1, wKeyValue );
                OnCbnSelchangeComboAction( ui->CBActionLeft );
            }
        }
    }
    if( m_pWndActiveComboAction == ui->CBActionUp ||
        m_pWndActiveComboAction == ui->CBActionDown )
    {
        if( ui->ChkLinkVert->isChecked() )
        {
            if( m_pWndActiveComboAction == ui->CBActionUp )
            {
                ui->CBActionDown->setItemData( 1, wKeyValue );
                OnCbnSelchangeComboAction( ui->CBActionDown );
            }
            else if( m_pWndActiveComboAction == ui->CBActionDown )
            {
                ui->CBActionUp->setItemData( 1, wKeyValue );
                OnCbnSelchangeComboAction( ui->CBActionUp );
            }
        }
    }

    OnCbnSelchangeComboAction( m_pWndActiveComboAction );
}

void QEditAction4WDWnd::onFunctionKeyActivated(int index)
{
    if( m_pWndActiveComboAction == NULL )
        return;

    int nFuncSelIndex = index;
    if( nFuncSelIndex < 0 )
        return;

    ushort wKeyValue = (ushort)ui->LBUserDefKey->GetKeyValue();//itemData( nFuncSelIndex ).toInt();
    m_pWndActiveComboAction->setItemData( 2, wKeyValue );

    if( m_pWndActiveComboAction == ui->CBActionLeft ||
        m_pWndActiveComboAction == ui->CBActionRight )
    {
        if( ui->ChkLinkHorz->isChecked() )
        {
            if( m_pWndActiveComboAction == ui->CBActionLeft )
            {
                ui->CBActionRight->setItemData( 2, wKeyValue );
                OnCbnSelchangeComboAction( ui->CBActionRight );
            }
            else if( m_pWndActiveComboAction == ui->CBActionRight )
            {
                ui->CBActionLeft->setItemData( 2, wKeyValue );
                OnCbnSelchangeComboAction( ui->CBActionLeft );
            }
        }
    }
    if( m_pWndActiveComboAction == ui->CBActionUp ||
        m_pWndActiveComboAction == ui->CBActionDown )
    {
        if( ui->ChkLinkVert->isChecked() )
        {
            if( m_pWndActiveComboAction == ui->CBActionUp )
            {
                ui->CBActionDown->setItemData( 2, wKeyValue );
                OnCbnSelchangeComboAction( ui->CBActionDown );
            }
            else if( m_pWndActiveComboAction == ui->CBActionDown )
            {
                ui->CBActionUp->setItemData( 2, wKeyValue );
                OnCbnSelchangeComboAction( ui->CBActionUp );
            }
        }
    }

    OnCbnSelchangeComboAction( m_pWndActiveComboAction );
}

void QEditAction4WDWnd::on_CBActionLeft_activated(int /*index*/)
{
    m_pWndActiveComboAction = ui->CBActionLeft;
    OnCbnSelchangeComboAction( ui->CBActionLeft );
}

void QEditAction4WDWnd::on_CBActionRight_activated(int /*index*/)
{
    m_pWndActiveComboAction = ui->CBActionRight;
    OnCbnSelchangeComboAction( ui->CBActionRight );
}

void QEditAction4WDWnd::on_CBActionUp_activated(int /*index*/)
{
    m_pWndActiveComboAction = ui->CBActionUp;
    OnCbnSelchangeComboAction( ui->CBActionUp );
}

void QEditAction4WDWnd::on_CBActionDown_activated(int /*index*/)
{
    m_pWndActiveComboAction = ui->CBActionDown;
    OnCbnSelchangeComboAction( ui->CBActionDown );
}

void QEditAction4WDWnd::on_ChkLinkVert_toggled(bool checked)
{
    if( checked )
    {
        if( ui->ChkLinkHorz->isChecked() )
            ui->LinkIconWidget->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_4WD_LINK_LR.png" );
        else
            ui->LinkIconWidget->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_4WD.png" );
    }
    else
    {
        if( ui->ChkLinkHorz->isChecked() )
            ui->LinkIconWidget->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_4WD_LINK_ALL.png" );
        else
            ui->LinkIconWidget->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_4WD_LINK_UD.png" );
    }
}

void QEditAction4WDWnd::on_ChkLinkHorz_toggled(bool checked)
{
    if( checked )
    {
        if( ui->ChkLinkVert->isChecked() )
            ui->LinkIconWidget->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_4WD_LINK_UD.png" );
        else
            ui->LinkIconWidget->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_4WD.png" );
    }
    else
    {
        if( ui->ChkLinkVert->isChecked() )
            ui->LinkIconWidget->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_4WD_LINK_ALL.png" );
        else
            ui->LinkIconWidget->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_4WD_LINK_LR.png" );
    }
}

void QEditAction4WDWnd::onKeyPress_Signal(ushort nValue)
{
    OnKeyEditChange( nValue );
}
