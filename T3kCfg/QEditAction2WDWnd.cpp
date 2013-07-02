#include "QEditAction2WDWnd.h"
#include "ui_QEditAction2WDWnd.h"

#include "stdInclude.h"
#include "../common/T3kHandle.h"
#include "QMouseMappingTable.h"

#include "KeyMapStr.h"
#include "Common/nv.h"

#include "T3kPacketDef.h"

#include <QShowEvent>
#include <QHideEvent>

QEditAction2WDWnd::QEditAction2WDWnd(T3kHandle*& pHandle, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QEditAction2WDWnd), m_pT3kHandle(pHandle), m_pTableWnd(NULL)
{
    setWindowFlags( Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint );
    ui->setupUi(this);

#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
    genAdjustButtonWidgetForWinAndX11( this );
#endif

    setFont( parent->font() );

    connect( ui->BtnCancel, SIGNAL(clicked()), this, SLOT(hide()) );
    connect( ui->LBUserDefKey, SIGNAL(KeyPressSignal(ushort)), this, SLOT(onKeyPress_Signal(ushort)) );
    connect( ui->LBUserDefKey, SIGNAL(FuncKey_activated(int)), this, SLOT(onFunctionKeyActivated(int)) );

    ui->BtnChkLink->setVisible( false );

    m_pTableWnd = (QMouseMappingTable*)parent;
    m_pWndActiveComboAction = NULL;

    Init();

    setFixedSize( width(), height() );

    m_nLoad = 0x0000;
}

QEditAction2WDWnd::~QEditAction2WDWnd()
{
    delete ui;
}

void QEditAction2WDWnd::OnChangeLanguage()
{
    QLangRes& Res = QLangManager::GetPtr()->GetResource();

    setWindowTitle( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TITLE_CAPTION_EDIT_PROFILE_ITEM") ) );
    ui->TitleProfileItem->setText( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM") ) );
    ui->TitleAction->setText( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_ACTION") ) );
    ui->LBUserDefKey->setText( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_USER_DEFINE_KEY") ) );
    ui->LBKey->setText( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_KEY") ) );
    ui->BtnApply->setText( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("BTN_CAPTION_APPLY") ) );
    ui->BtnCancel->setText( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("BTN_CAPTION_CANCEL") ) );

    SetPredefineProfileTypes( ui->CBActionZoomOut );
    SetPredefineProfileTypes( ui->CBActionZoomIn );

    if( Res.IsR2L() )
        ui->EditProfileItem->setAlignment( Qt::AlignRight );
    else
        ui->EditProfileItem->setAlignment( Qt::AlignLeft );

    ui->LBUserDefKey->SetRealGeometry();
}

void QEditAction2WDWnd::Init()
{
    ui->LinkIconWidget->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_2WD.png" );

    ui->BtnChkLink->setIcon( QIcon(":/T3kCfgRes/Resources/PNG_ICON_LINK_HORZ.png") );

    OnChangeLanguage();
}

void QEditAction2WDWnd::SetPredefineProfileTypes( QComboBox* pWndComboAction )
{
    QLangRes& Res = QLangManager::GetPtr()->GetResource();

    pWndComboAction->clear();

    pWndComboAction->addItem( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_DISABLED") ) );
    pWndComboAction->setItemData( pWndComboAction->count()-1, 0x0000 );
    pWndComboAction->addItem( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_USER_DEFINED") ) );
    pWndComboAction->setItemData( pWndComboAction->count()-1, 0xFFFF );
    pWndComboAction->addItem( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_PROFILE_ITEM_FUNCTION_KEY") ) );
    pWndComboAction->setItemData( pWndComboAction->count()-1, 0xFFFF );

    pWndComboAction->addItem( "Wheel" );
    pWndComboAction->setItemData( pWndComboAction->count()-1, 0x80FE );
    pWndComboAction->addItem( "Ctrl+Wheel" );
    pWndComboAction->setItemData( pWndComboAction->count()-1, 0x81FE );
}

void QEditAction2WDWnd::SetProfileInfo( int nProfileIndex, uchar cKey, ushort wKeyValue1, ushort wKeyValue2 )
{
    QLangRes& Res = QLangManager::GetPtr()->GetResource();

    SetPredefineProfileTypes( ui->CBActionZoomOut );
    SetPredefineProfileTypes( ui->CBActionZoomIn );

    m_nProfileIndex = nProfileIndex;
    m_cProfileKey = cKey;
    m_wProfileValue[0] = wKeyValue1;		// Zoom Out
    m_wProfileValue[1] = wKeyValue2;		// Zoom In

    QString strLoc;
    switch( cKey )
    {
    case MM_GESTURE_ZOOM:
        strLoc = Res.GetResString( QString::fromUtf8("MOUSE SETTING"), QString::fromUtf8("TEXT_ZOOM") );
        break;
    default:
        strLoc = "Unknown";
        break;
    }

    ui->EditProfileItem->setText( strLoc );

    SetActionWithProfileInfo( ui->CBActionZoomOut, ui->LBZoomOut, m_wProfileValue[0] );
    SetActionWithProfileInfo( ui->CBActionZoomIn, ui->LBZoomIn, m_wProfileValue[1] );

    ui->BtnChkLink->setChecked( false );

    int nSel = ui->CBActionZoomIn->currentIndex();
    if( nSel < 0 ) return;

    if( nSel > 2 )
    {
        ui->BtnChkLink->setChecked( true );
        ui->LinkIconWidget->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_2WD_LINK.png" );
    }
}

void QEditAction2WDWnd::SetActionWithProfileInfo( QComboBox* pWndComboAction, QLabel* pDetailLabel, ushort wKeyValue )
{
    pWndComboAction->setCurrentIndex(-1);
    pDetailLabel->setText( ("") );

    QString strDetail = GetMappingStr(wKeyValue>>8, wKeyValue&0xFF);
    strDetail.replace( "\r\n", " " );
    strDetail.replace( "Click", "Button Click" );
    strDetail.replace( "Drag", "Button Drag" );

    bool bFound = false;
    int nSelIndex = -1;
    for( int nI = 0 ; nI < pWndComboAction->count() ; nI ++ )
    {
        ushort wItemValue = (ushort)pWndComboAction->itemData(nI).toInt();
        if( wItemValue == wKeyValue )
        {
            pWndComboAction->setCurrentIndex( nI );
            nSelIndex = nI;
            bFound = true;
            break;
        }
    }

    pDetailLabel->setText( strDetail );

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
            pWndComboAction->setCurrentIndex( 2 );
            pWndComboAction->setItemData( 2, wKeyValue );

            SetEditMode( ModeFunctionKey, wKeyValue );
        }
        else
        {
            pWndComboAction->setCurrentIndex( 1 );
            pWndComboAction->setItemData( 1, wKeyValue );

            SetEditMode( ModeUserDefined, wKeyValue );
        }
    }
    else
    {
        if( nSelIndex == 2 )
        {
            pWndComboAction->setItemData( 2, wKeyValue );

            SetEditMode( ModeFunctionKey, wKeyValue );
        }
        else
        {
            SetEditMode( ModeNone, wKeyValue );
        }
    }

    if( pWndComboAction == ui->CBActionZoomIn )
        m_nLoad |= 0x0001;
    else if( pWndComboAction == ui->CBActionZoomOut )
        m_nLoad |= 0x0010;
}

void QEditAction2WDWnd::SetEditMode( EditMode Mode, ushort wKeyValue )
{
    if( m_nLoad != 0x0011 )
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

void QEditAction2WDWnd::on_CBActionZoomOut_activated(int /*index*/)
{
    m_pWndActiveComboAction = ui->CBActionZoomOut;
    int nSel = ui->CBActionZoomOut->currentIndex();
    if( nSel < 0 ) return;

    if( nSel > 2 )
    {
        ui->BtnChkLink->setChecked( true );
        ui->LinkIconWidget->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_2WD_LINK.png" );
    }
    else
    {
        if( ui->BtnChkLink->isChecked() )
        {
            ui->CBActionZoomIn->setCurrentIndex( 0 );
            OnCbnSelchangeComboAction( ui->CBActionZoomIn );
            ui->BtnChkLink->setChecked( false );
            ui->LinkIconWidget->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_2WD.png" );
        }
    }

    if( ui->BtnChkLink->isChecked() )
    {
        ui->CBActionZoomIn->setCurrentIndex( nSel );
        OnCbnSelchangeComboAction( ui->CBActionZoomIn );
    }
    OnCbnSelchangeComboAction( ui->CBActionZoomOut );
}

void QEditAction2WDWnd::on_CBActionZoomIn_activated(int /*index*/)
{
    m_pWndActiveComboAction = ui->CBActionZoomIn;
    int nSel = ui->CBActionZoomIn->currentIndex();
    if( nSel < 0 ) return;

    if( nSel > 2 )
    {
        ui->BtnChkLink->setChecked( true );
        ui->LinkIconWidget->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_2WD_LINK.png" );
    }
    else
    {
        if( ui->BtnChkLink->isChecked() )
        {
            ui->CBActionZoomOut->setCurrentIndex( 0 );
            OnCbnSelchangeComboAction( ui->CBActionZoomOut );
            ui->BtnChkLink->setChecked( false );
            ui->LinkIconWidget->SetIconImage( ":/T3kCfgRes/Resources/PNG_ICON_2WD.png" );
        }
    }

    if( ui->BtnChkLink->isChecked() )
    {
        ui->CBActionZoomOut->setCurrentIndex( nSel );
        OnCbnSelchangeComboAction( ui->CBActionZoomOut );
    }
    OnCbnSelchangeComboAction( ui->CBActionZoomIn );
}

void QEditAction2WDWnd::OnCbnSelchangeComboAction( QComboBox* pWndComboAction )
{
    QLangRes& Res = QLangManager::GetPtr()->GetResource();

    int nSel = pWndComboAction->currentIndex();
    if( nSel < 0 ) return;

    QLabel* pDetailLabel;
    if( pWndComboAction == ui->CBActionZoomOut )
        pDetailLabel = ui->LBZoomOut;
    else if( pWndComboAction == ui->CBActionZoomIn )
        pDetailLabel = ui->LBZoomIn;
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
        pDetailLabel->setText( Res.GetResString( QString::fromUtf8("EDIT PROFILE ITEM"), QString::fromUtf8("TEXT_NOT_DEFINED") ) );
        return;
    }

    QString strDetail( GetMappingStr(wKeyValue>>8, wKeyValue&0xFF) );
    strDetail.replace( "\r\n", " " );
    strDetail.replace( "Click", "Button Click" );
    strDetail.replace( "Drag", "Button Drag" );

    pDetailLabel->setText( strDetail );
}

void QEditAction2WDWnd::showEvent(QShowEvent *evt)
{
    QDialog::showEvent(evt);

    if( evt->type() == QEvent::Show )
    {
        ui->LBUserDefKey->Reset();
        ui->LBUserDefKey->update();
    }
}

void QEditAction2WDWnd::hideEvent(QHideEvent *evt)
{
    QDialog::hideEvent(evt);

    if( evt->type() == QEvent::Hide )
    {
        m_pTableWnd->ResetSelect();
        m_nLoad = 0x0000;
    }
}

void QEditAction2WDWnd::on_BtnApply_clicked()
{
    ushort wKeyValue[4];
    int nSel;

    nSel = ui->CBActionZoomOut->currentIndex();
    if( nSel < 0 ) return;
    wKeyValue[0] = (ushort)ui->CBActionZoomOut->itemData(nSel).toInt();

    if( wKeyValue[0] == 0xFFFF )
    {
        hide();
        return;
    }

    nSel = ui->CBActionZoomIn->currentIndex();
    if( nSel < 0 ) return;
    wKeyValue[1] = (ushort)ui->CBActionZoomIn->itemData(nSel).toInt();

    if( wKeyValue[1] == 0xFFFF )
    {
        show();
        return;
    }

    QString str;
    switch( m_nProfileIndex )
    {
    case 0:
        str = str.sprintf( "%s%02X%04X%04X", cstrMouseProfile1, m_cProfileKey, wKeyValue[0], wKeyValue[1] );
        break;
    case 1:
        str = str.sprintf( "%s%02X%04X%04X", cstrMouseProfile2, m_cProfileKey, wKeyValue[0], wKeyValue[1] );
        break;
    case 2:
        str = str.sprintf( "%s%02X%04X%04X", cstrMouseProfile3, m_cProfileKey, wKeyValue[0], wKeyValue[1] );
        break;
    case 3:
        str = str.sprintf( "%s%02X%04X%04X", cstrMouseProfile4, m_cProfileKey, wKeyValue[0], wKeyValue[1] );
        break;
    case 4:
        str = str.sprintf( "%s%02X%04X%04X", cstrMouseProfile5, m_cProfileKey, wKeyValue[0], wKeyValue[1] );
        break;
    default:
        hide();
        return;
    }

    m_pT3kHandle->SendCommand( (const char*)str.toUtf8().data(), true );

    hide();
}

void QEditAction2WDWnd::OnKeyEditChange(ushort wKeyValue)
{
    if( m_pWndActiveComboAction == NULL )
        return;

    m_pWndActiveComboAction->setItemData( 1, wKeyValue );
    m_pWndActiveComboAction->setItemData( 2, wKeyValue );

    if( ui->BtnChkLink->isChecked() )
    {
        if( m_pWndActiveComboAction == ui->CBActionZoomOut )
        {
           ui->CBActionZoomIn->setItemData( 1, wKeyValue );
            OnCbnSelchangeComboAction( ui->CBActionZoomIn );
        }
        else if( m_pWndActiveComboAction == ui->CBActionZoomIn )
        {
            ui->CBActionZoomOut->setItemData( 1, wKeyValue );
            OnCbnSelchangeComboAction( ui->CBActionZoomOut );
        }
    }

    OnCbnSelchangeComboAction( m_pWndActiveComboAction );
}

void QEditAction2WDWnd::onFunctionKeyActivated(int /*index*/)
{
    if( m_pWndActiveComboAction == NULL )
        return;

    int nFuncSelIndex = ui->LBUserDefKey->GetFuncKeyCurrentIndex();
    if( nFuncSelIndex < 0 )
        return;

    ushort wKeyValue = (ushort)ui->LBUserDefKey->GetKeyValue();//GetItemData( nFuncSelIndex ).toInt();
    m_pWndActiveComboAction->setItemData( 2, wKeyValue );

    if( ui->BtnChkLink->isChecked() )
    {
        if( m_pWndActiveComboAction == ui->CBActionZoomOut )
        {
            ui->CBActionZoomIn->setItemData( 2, wKeyValue );
            OnCbnSelchangeComboAction( ui->CBActionZoomIn );
        }
        else if( m_pWndActiveComboAction == ui->CBActionZoomIn )
        {
            ui->CBActionZoomOut->setItemData( 2, wKeyValue );
            OnCbnSelchangeComboAction( ui->CBActionZoomOut );
        }
    }

    OnCbnSelchangeComboAction( m_pWndActiveComboAction );
}

void QEditAction2WDWnd::on_BtnChkLink_toggled(bool /*checked*/)
{
    if( ui->BtnChkLink->isChecked() )
        ui->LinkIconWidget->SetIconImage( tr(":/T3kCfgRes/Resources/PNG_ICON_2WD.png") );
    else
        ui->LinkIconWidget->SetIconImage( tr(":/T3kCfgRes/Resources/PNG_ICON_2WD_LINK.png") );
}

void QEditAction2WDWnd::onKeyPress_Signal(ushort nValue)
{
    OnKeyEditChange( nValue );
}
